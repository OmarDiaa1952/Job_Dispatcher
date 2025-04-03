#include <windows.h>
#include <iostream>
#include "include/worker.h"

void EnableWorkerConsole() {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    SetConsoleTitleW(L"Worker Process");
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    EnableWorkerConsole();
    std::cout << "Worker process started\n";
    return worker_main(__argc, __argv);
}