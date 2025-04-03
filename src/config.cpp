#include "../include/config.h"
#include "../include/utils.h"
#include <sstream>
#include <iostream>
#include <limits>

std::vector<WorkerConfig> parseWorkerConfig() {
    std::vector<WorkerConfig> configs;
    std::string line;
    
    for (int i = 0; i < 5; i++) {
        if (!std::getline(std::cin, line)) {
            handle_error("Failed to read worker configuration");
        }
        
        std::istringstream iss(line);
        WorkerConfig config;
        
        if (!(iss >> config.type >> config.count)) {
            handle_error("Invalid worker configuration format");
        }
        
        // Validate input
        if (config.type < 1 || config.type > 5) {
            handle_error("Worker type must be between 1 and 5");
        }
        
        if (config.count < 1) {
            handle_error("Worker count must be positive");
        }
        
        configs.push_back(config);
        
        // Clear any remaining input in case of extra values
        iss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
    return configs;
}