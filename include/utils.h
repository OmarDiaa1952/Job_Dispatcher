#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <string>
#include <stdexcept>

// ===== ERROR HANDLING UTILITIES =====
void handle_error(const std::string& msg, bool exit_program = true);


void create_pipe(HANDLE pipefd[2], const std::string& error_msg);

// ===== SAFE HANDLE OPERATIONS =====
void safe_close(HANDLE& fd);
void safe_dup2(HANDLE oldfd, HANDLE newfd, const std::string& error_msg);

#endif