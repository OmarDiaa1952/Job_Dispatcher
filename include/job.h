#ifndef JOB_H
#define JOB_H

#include <string>

struct Job {
    int type;
    int duration;
    std::string description; // Additional job info
    
    // Validation function
    bool isValid() const;
};

#endif