# Job Dispatcher System

This project implements a job dispatcher system where a main process distributes incoming jobs to a pool of worker processes. Each worker process is specialized to handle a specific job type, and jobs are processed concurrently. Communication between the main process and worker processes is established using pipes, and the `select`-like functionality (`WaitForMultipleObjects` in Windows) is used in the main process to manage communication efficiently.

## Project Idea and Logic

The core idea is to simulate a distributed task processing system. The main dispatcher process receives job configurations and job execution requests. It then creates and manages worker processes, each responsible for a specific job type. The dispatcher distributes jobs to available workers, and workers simulate job execution by sleeping for the specified duration.

**Key Components:**

- **Dispatcher (main.cpp):**
  - Reads worker configurations from standard input.
  - Creates worker processes based on the configuration.
  - Manages inter-process communication using pipes.
  - Distributes jobs to available workers.
  - Uses `WaitForMultipleObjects` to monitor worker process communication.
- **Worker (worker.cpp):**
  - Receives job duration from the dispatcher via pipes.
  - Simulates job execution by sleeping.
  - Communicates job completion status back to the dispatcher.

## System Calls and Windows API Functions Used

- **`CreatePipe()`:** Creates anonymous pipes for inter-process communication.
- **`CreateProcessW()`:** Creates new worker processes.
- **`ReadFile()`:** Reads data from pipes.
- **`WriteFile()`:** Writes data to pipes.
- **`CloseHandle()`:** Closes handles to pipes and processes.
- **`TerminateProcess()`:** Terminates worker processes.
- **`WaitForMultipleObjects()`:** Waits for multiple handles to become signaled (used for `select`-like functionality).
- **`AllocConsole()`:** creates a new console for the worker processes.
- **`freopen()`:** redirects the standard output and error to the new console.
- **`GetCurrentProcessId()`:** retrieves the current process ID.
- **`Sleep()`:** pauses the execution of the thread.

## Building the Project

To build the project, you'll need a C++ compiler (e.g., g++) and a Windows environment.

1.  **Clone the repository:**

    ```bash
    git clone <repository_url>
    cd job_dispatcher
    ```

2.  **Build using Makefile (recommended):**

    ```bash
    make
    ```

    - This will compile `main.cpp`, `worker.cpp`, `dispatcher.cpp`, and other source files, creating `dispatcher.exe` and `worker.exe` in the `bin` directory.

3.  **Manual Compilation (if Makefile is not used):**

    ```bash
    g++ -std=c++17 -Wall -Wextra -I./include -g -static -Wno-missing-field-initializers -MMD -MP src/worker.cpp src/utils.cpp src/dispatcher.cpp src/config.cpp src/job.cpp main.cpp -o bin/dispatcher.exe -lkernel32 -luser32 -Wl,-subsystem,console
    g++ -std=c++17 -Wall -Wextra -I./include -g -static -Wno-missing-field-initializers -MMD -MP worker_main.cpp src/utils.cpp src/worker.cpp -o bin/worker.exe -lkernel32 -luser32 -Wl,-subsystem,console
    ```

## Testing the Project

1.  **Run the dispatcher:**

    ```bash
    bin/dispatcher.exe
    ```

2.  **Provide worker configurations:**

    - The dispatcher will prompt you to enter worker configurations. Enter 5 lines, each in the format `<worker_type> <worker_count>`. For example:

      ```
      1 2
      2 1
      3 3
      4 1
      5 2
      ```

3.  **Provide job inputs:**

    - After configuring workers, enter jobs in the format `<job_type> <job_duration>`. For example:

      ```
      1 5
      3 2
      2 8
      ```

4.  **Observe worker activity:**

    - Each worker process will open its own console window and display messages indicating job processing.
    - The dispatcher console will show job input and worker summary.

5.  **Terminate the dispatcher:**

    - Press Ctrl+Z and Enter in the dispatcher console to gracefully shut down the system.

## Project Structure

job_dispatcher/
├── bin/ # Executables (dispatcher.exe, worker.exe)
├── include/ # Header files
│ ├── config.h
│ ├── dispatcher.h
│ ├── job.h
│ ├── utils.h
│ └── worker.h
├── src/ # Source files
│ ├── config.cpp
│ ├── dispatcher.cpp
│ ├── job.cpp
│ ├── utils.cpp
│ └── worker.cpp
├── main.cpp # Main dispatcher program
├── worker_main.cpp # worker entry point.
└── Makefile # Build instructions
└── README.md # Project documentation
