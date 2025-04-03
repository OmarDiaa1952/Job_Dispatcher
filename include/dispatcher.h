#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <vector>
#include <windows.h>
#include "worker.h"
#include "job.h"

class Dispatcher {
public:
    explicit Dispatcher(std::vector<WorkerPipe>& workers);
    ~Dispatcher();
    
    void run();
    
private:
    std::vector<WorkerPipe>& workers_;
    std::vector<Job> pending_jobs_;
    
    void processJob(const Job& job);
    void checkCompletedJobs();
    void handleWorkerCompletion(HANDLE worker_fd);
};

#endif