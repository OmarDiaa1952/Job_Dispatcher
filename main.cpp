// main.cpp
#include "include/config.h"
#include "include/dispatcher.h"
#include "include/worker.h"
#include <iostream>
#include <windows.h>
#include <sstream> // For std::ostringstream

void EnableConsoleDebug() {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    SetConsoleTitleW(L"Dispatcher Debug Console");
}

int main() {
    EnableConsoleDebug(); // Force console window to appear

    try {
        std::cout << "=== Dispatcher Starting ===" << std::endl;

        // 1. Parse configuration
        std::cout << "Reading worker configuration..." << std::endl;
        std::vector<WorkerConfig> configs = parseWorkerConfig();
        std::cout << "Configured " << configs.size() << " worker types\n";

        // Generate worker summary string
        std::ostringstream summary;
        for (const auto& config : configs) {
            summary << config.count << " worker(s) of type " << config.type << "\n";
        }

        // 2. Create workers
        std::cout << "Launching worker processes..." << std::endl;
        std::vector<WorkerPipe> workers = createWorkers(configs);

        // Print worker summary
        std::cout << "\nWorker configuration summary:\n" << summary.str() << std::endl;

        // 3. Run dispatcher
        std::cout << "\nEnter jobs (type duration, one per line):\n";
        std::cout << "Example: '1 5' for type=1 duration=5\n";
        std::cout << "Press Ctrl+Z then Enter to finish\n" << std::endl;

        Dispatcher dispatcher(workers);
        dispatcher.run();

        // 4. Cleanup
        std::cout << "Shutting down workers..." << std::endl;
        cleanupWorkers(workers);
        std::cout << "=== Clean Exit ===" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "\n!!! CRASH: " << e.what() << " !!!" << std::endl;
        return EXIT_FAILURE;
    }

    // Keep console open
    std::cout << "\nPress Enter to exit...";
    std::cin.ignore();
    return EXIT_SUCCESS;
}