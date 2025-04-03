#ifndef CONFIG_H
#define CONFIG_H

#include <vector>
#include <string>

struct WorkerConfig {
    int type;
    int count;
};

std::vector<WorkerConfig> parseWorkerConfig();

#endif