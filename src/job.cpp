#include "../include/job.h"

bool Job::isValid() const {
    return (type >= 1 && type <= 5) && (duration >= 1 && duration <= 10);
}
