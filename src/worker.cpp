#include "../include/worker.h"
#include "../include/utils.h"
#include <iostream>
#include <sstream>
#include <windows.h>
#include <memory>  // For unique_ptr

// RAII wrapper for Windows handles
struct HandleDeleter {
    void operator()(HANDLE h) const {
        if (h != INVALID_HANDLE_VALUE && h != NULL) {
            CloseHandle(h);
        }
    }
};
using HandlePtr = std::unique_ptr<void, HandleDeleter>;

#include "../include/worker.h"
#include "../include/utils.h"
#include <iostream>
#include <sstream>
#include <windows.h>

int worker_main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: worker.exe <type> <input_pipe> <output_pipe>\n";
        return EXIT_FAILURE;
    }

    try {
        int worker_type = std::stoi(argv[1]);
        HANDLE input_pipe = reinterpret_cast<HANDLE>(std::stoull(argv[2]));
        HANDLE output_pipe = reinterpret_cast<HANDLE>(std::stoull(argv[3]));

        // Create new console for this worker
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        
        std::cout << "Worker " << worker_type << " ready (PID: " << GetCurrentProcessId() << ")\n";

        while (true) {
            int duration;
            DWORD bytes_read;
            
            if (!ReadFile(input_pipe, &duration, sizeof(duration), &bytes_read, NULL)) {
                DWORD err = GetLastError();
                if (err != ERROR_BROKEN_PIPE) {
                    std::cerr << "Read error: " << err << "\n";
                }
                break;
            }

            std::cout << "Worker " << worker_type << " processing job (" << duration << "s)\n";
            Sleep(duration * 1000);
            std::cout << "Worker " << worker_type << " job completed\n";

            DWORD bytes_written;
            if (!WriteFile(output_pipe, &worker_type, sizeof(worker_type), &bytes_written, NULL)) {
                std::cerr << "Write error: " << GetLastError() << "\n";
                break;
            }
        }

        CloseHandle(input_pipe);
        CloseHandle(output_pipe);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

std::vector<WorkerPipe> createWorkers(const std::vector<WorkerConfig>& configs) {
    std::vector<WorkerPipe> workers;
    
    for (const auto& config : configs) {
        for (int i = 0; i < config.count; i++) {
            WorkerPipe wp;
            wp.type = config.type;
            wp.available = true;
            
            // Create pipes with RAII guards for cleanup
            try {
                create_pipe(wp.to_worker, "Input pipe creation failed");
                create_pipe(wp.from_worker, "Output pipe creation failed");

                // Temporary RAII guards in case of failure
                HandlePtr to_reader(wp.to_worker[0]);
                HandlePtr from_writer(wp.from_worker[1]);

                // Prepare process creation
                STARTUPINFOW si = { sizeof(si) };
                PROCESS_INFORMATION pi = {0};
                std::wstringstream cmd;
                cmd << L"worker.exe "
                    << config.type << L" "
                    << reinterpret_cast<uintptr_t>(wp.to_worker[0]) << L" "
                    << reinterpret_cast<uintptr_t>(wp.from_worker[1]);

                std::wstring cmd_str = cmd.str();
                if (!CreateProcessW(
                    NULL, 
                    &cmd_str[0],
                    NULL, NULL, TRUE, 
                    CREATE_NEW_CONSOLE, 
                    NULL, NULL, &si, &pi
                )) {
                    DWORD err = GetLastError();
                    throw std::runtime_error(
                        "CreateProcess failed (Error " + 
                        std::to_string(err) + ")"
                    );
                }

                wp.pi = pi;
                workers.push_back(std::move(wp));
                
                // Release the guards for the handles we're keeping
                to_reader.release();
                from_writer.release();

                // Close handles we don't need in main process
                CloseHandle(wp.to_worker[0]);
                CloseHandle(wp.from_worker[1]);

            } catch (const std::exception& e) {
                // Cleanup any created resources
                if (wp.to_worker[0] != INVALID_HANDLE_VALUE) CloseHandle(wp.to_worker[0]);
                if (wp.to_worker[1] != INVALID_HANDLE_VALUE) CloseHandle(wp.to_worker[1]);
                if (wp.from_worker[0] != INVALID_HANDLE_VALUE) CloseHandle(wp.from_worker[0]);
                if (wp.from_worker[1] != INVALID_HANDLE_VALUE) CloseHandle(wp.from_worker[1]);
                throw;
            }
        }
    }
    return workers;
}

WorkerPipe* findAvailableWorker(std::vector<WorkerPipe>& workers, int job_type) {
    for (auto& worker : workers) {
        if (worker.type == job_type && worker.available.load()) {
            return &worker;
        }
    }
    return nullptr;
}

void cleanupWorkers(std::vector<WorkerPipe>& workers) {
    for (auto& worker : workers) {
        // Close pipe handles
        if (worker.to_worker[1] != INVALID_HANDLE_VALUE) 
            CloseHandle(worker.to_worker[1]);
        if (worker.from_worker[0] != INVALID_HANDLE_VALUE) 
            CloseHandle(worker.from_worker[0]);
        
        // Terminate worker process
        if (worker.pi.hProcess) {
            TerminateProcess(worker.pi.hProcess, 0);
            CloseHandle(worker.pi.hProcess);
            CloseHandle(worker.pi.hThread);
        }
    }
}