#ifndef WORKER_H
#define WORKER_H

#include <vector>
#include <windows.h>
#include <atomic>
#include "utils.h"
#include "config.h"



struct WorkerPipe {
    HANDLE to_worker[2];   // Pipe for sending to worker
    HANDLE from_worker[2]; // Pipe for receiving from worker
    PROCESS_INFORMATION pi; // Process info for worker
    int type;              // Worker type
    std::atomic<bool> available;

    // Constructor
    WorkerPipe() : 
        to_worker{INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE},
        from_worker{INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE},
        type(0),
        available(false) 
    {
        ZeroMemory(&pi, sizeof(pi));
    }

    // Move constructor
    WorkerPipe(WorkerPipe&& other) noexcept :
        to_worker{other.to_worker[0], other.to_worker[1]},
        from_worker{other.from_worker[0], other.from_worker[1]},
        pi(other.pi),
        type(other.type),
        available(other.available.load())
    {
        // Invalidate the moved-from object
        other.to_worker[0] = other.to_worker[1] = INVALID_HANDLE_VALUE;
        other.from_worker[0] = other.from_worker[1] = INVALID_HANDLE_VALUE;
        ZeroMemory(&other.pi, sizeof(other.pi));
    }

    // Move assignment
    WorkerPipe& operator=(WorkerPipe&& other) noexcept {
        if (this != &other) {
            // Clean up existing resources
            safe_close(to_worker[1]);
            safe_close(from_worker[0]);

            // Transfer resources
            to_worker[0] = other.to_worker[0];
            to_worker[1] = other.to_worker[1];
            from_worker[0] = other.from_worker[0];
            from_worker[1] = other.from_worker[1];
            pi = other.pi;
            type = other.type;
            available = other.available.load();

            // Invalidate the moved-from object
            other.to_worker[0] = other.to_worker[1] = INVALID_HANDLE_VALUE;
            other.from_worker[0] = other.from_worker[1] = INVALID_HANDLE_VALUE;
            ZeroMemory(&other.pi, sizeof(other.pi));
        }
        return *this;
    }

    // Destructor
    ~WorkerPipe() {
        safe_close(to_worker[1]);
        safe_close(from_worker[0]);
    }

    // Delete copy operations
    WorkerPipe(const WorkerPipe&) = delete;
    WorkerPipe& operator=(const WorkerPipe&) = delete;
};

void workerProcess(int worker_type, HANDLE to_worker, HANDLE from_worker);
extern std::vector<WorkerPipe> createWorkers(const std::vector<WorkerConfig>& configs);
WorkerPipe* findAvailableWorker(std::vector<WorkerPipe>& workers, int job_type);
void cleanupWorkers(std::vector<WorkerPipe>& workers);
int worker_main(int argc, char* argv[]);
#endif