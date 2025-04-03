#include "../include/utils.h"
#include <iostream>

void handle_error(const std::string& msg, bool exit_program) {
    DWORD err = GetLastError();
    std::cerr << "ERROR: " << msg 
              << " (WinError: " << err << ")\n";
    if (exit_program) {
        ExitProcess(EXIT_FAILURE);
    }
}

void create_pipe(HANDLE pipefd[2], const std::string& error_msg) {
    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
    if (!CreatePipe(&pipefd[0], &pipefd[1], &sa, 0)) {
        DWORD err = GetLastError();
        throw std::runtime_error(error_msg + " (Error " + std::to_string(err) + ")");
    }
}

void safe_close(HANDLE& fd) {
    if (fd != INVALID_HANDLE_VALUE) {
        CloseHandle(fd);
        fd = INVALID_HANDLE_VALUE;
    }
}

void safe_dup2(HANDLE oldfd, HANDLE newfd, const std::string& error_msg) {
    if (!DuplicateHandle(
        GetCurrentProcess(), oldfd,
        GetCurrentProcess(), &newfd,
        0, TRUE, DUPLICATE_SAME_ACCESS
    )) {
        throw std::runtime_error(error_msg);
    }
}