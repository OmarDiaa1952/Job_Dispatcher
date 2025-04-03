// dispatcher.cpp
#include "../include/dispatcher.h"
#include <iostream>
#include <sstream>
#include <windows.h>

// Constructor implementation
Dispatcher::Dispatcher(std::vector<WorkerPipe>& workers)
    : workers_(workers) {
    // Initialize any necessary members
}

// Destructor implementation
Dispatcher::~Dispatcher() {
    // Clean up any resources
    std::cout << "Dispatcher shutting down...\n";
}

void Dispatcher::processJob(const Job& job) {
    if (!job.isValid()) {
        throw std::runtime_error(
            "Invalid job: type=" + std::to_string(job.type) +
            " duration=" + std::to_string(job.duration)
        );
    }

    WorkerPipe* worker = findAvailableWorker(workers_, job.type);
    if (!worker) {
        pending_jobs_.push_back(job);
        return;
    }

    DWORD bytes_written;
    if (!WriteFile(
        worker->to_worker[1],
        &job.duration,
        sizeof(job.duration),
        &bytes_written,
        NULL
    )) {
        DWORD err = GetLastError();
        worker->available = false;
        throw std::runtime_error(
            "Failed to send job to worker: " +
            std::to_string(err)
        );
    }
    worker->available = false;
}

void Dispatcher::checkCompletedJobs() {
    std::vector<HANDLE> pipes;
    for (const auto& worker : workers_) {
        if (!worker.available) {
            pipes.push_back(worker.from_worker[0]);
        }
    }

    if (pipes.empty()) return;

    DWORD result = WaitForMultipleObjects(
        static_cast<DWORD>(pipes.size()),
        pipes.data(),
        FALSE,
        100 // 100ms timeout
    );

    if (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + pipes.size()) {
        size_t idx = result - WAIT_OBJECT_0;
        workers_[idx].available = true;

        // Process pending jobs for this worker type
        for (auto it = pending_jobs_.begin(); it != pending_jobs_.end(); ) {
            if (it->type == workers_[idx].type) {
                processJob(*it);
                it = pending_jobs_.erase(it);
                break;
            } else {
                ++it;
            }
        }
    }
}

void Dispatcher::run() {
    std::cout << "Enter jobs (type duration, one per line):\n";
    std::cout << "Example: '1 5' for type=1 duration=5\n";
    std::cout << "Press Ctrl+Z then Enter to finish\n";

    std::string line;
    while (std::getline(std::cin, line)) {
        try {
            std::istringstream iss(line);
            Job job;
            if (!(iss >> job.type >> job.duration)) {
                std::cerr << "Invalid input format\n";
                continue;
            }

            processJob(job);
            checkCompletedJobs();
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    // Process remaining jobs
    while (!pending_jobs_.empty()) {
        checkCompletedJobs();
        Sleep(100);
    }
}