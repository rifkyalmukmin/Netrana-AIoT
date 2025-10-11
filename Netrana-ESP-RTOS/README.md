# Netrana ESP-RTOS

This repository contains the firmware for the Netrana AIoT device, now refactored to use a modular programming approach and a Real-Time Operating System (RTOS).

## Folder Structure

```
.
├── include/      # Global header files
├── lib/          # Libraries and modules
│   ├── Config/   # Configuration management
│   ├── Modules/  # Hardware/feature modules
│   └── Task/     # RTOS tasks
├── src/          # Main application source code
└── test/         # Test files
```

### Folder Descriptions

*   **`include/`**: This directory contains global header files that are used across the entire project.
*   **`lib/`**: This directory contains the core logic of the application, organized into subdirectories:
    *   **`Config/`**: Manages configuration settings for the device.
    *   **`Modules/`**: Contains modules for specific hardware components or features, such as the Camera and UART.
    *   **`Task/`**: Implements the various RTOS tasks, such as capturing images, managing connections, and sending data.
*   **`src/`**: The main source code for the application, including `main.cpp`.
*   **`test/`**: Contains tests for the various components of the firmware.

## Modular Programming

The codebase is organized into modules to improve maintainability and scalability. Each module encapsulates a specific functionality:

-   **Camera:** Manages the camera hardware and image capture.
-   **UART:** Handles serial communication.
-   **Dummy:** A template module for creating new modules.

## Real-Time Operating System (RTOS)

The firmware now utilizes an RTOS to manage concurrent tasks efficiently. The main tasks are:

-   **Capture:** Captures images from the camera.
-   **Connection:** Manages network connectivity.
-   **Control:** Handles control logic.
-   **SendData:** Sends data to the cloud.

This new architecture allows for better resource management and more complex, real-time applications.

## Adding New Features

To add a new feature to the firmware, follow these steps:

1.  **Create a New Module:**
    *   Create a new subdirectory in `lib/Modules/` for your feature (e.g., `lib/Modules/MyNewFeature/`).
    *   Inside the new directory, create the header (`MyNewFeature.h`) and source (`MyNewFeature.cpp`) files.
    *   Implement the logic for your feature within this module. You can use the `Dummy` module as a template.

2.  **Create a New Task:**
    *   Create a new subdirectory in `lib/Task/` for the corresponding task (e.g., `lib/Task/MyNewTask/`).
    *   Create the header (`MyNewTask.h`) and source (`MyNewTask.cpp`) files.
    *   In the source file, implement the task function that will use your new module. This function will be passed to `xTaskCreatePinnedToCore`.

3.  **Integrate into the Main Application:**
    *   Open `src/main.cpp`.
    *   Include the header file for your new task (e.g., `#include "MyNewTask.h"`).
    *   In the `app_main()` function, create an instance of your new task and start it using `xTaskCreatePinnedToCore`.

### RTOS Best Practices

When adding new tasks, it's important to consider the following RTOS concepts:

*   **Task Priority:**
    *   Assign priorities to tasks based on their importance and real-time requirements.
    *   Higher priority numbers indicate higher priority tasks.
    *   Use `configMAX_PRIORITIES - 1` for the highest priority.
    *   Be mindful of priority inversion. If a high-priority task is waiting for a resource held by a low-priority task, consider using priority inheritance.

*   **Memory Management:**
    *   When creating a task, you need to allocate a stack size for it.
    *   The stack size should be large enough to accommodate the task's local variables, function call overhead, and interrupt context.
    *   Use the `uxTaskGetStackHighWaterMark()` function during development to determine the minimum required stack size.
    *   Avoid dynamic memory allocation (`malloc`, `free`) within tasks if possible. Use static allocation or pre-allocated memory pools instead.

*   **Inter-Task Communication:**
    *   Use queues, semaphores, and mutexes for communication and synchronization between tasks.
    *   **Queues:** For passing data between tasks.
    *   **Semaphores:** For signaling and synchronization.
    *   **Mutexes:** For protecting shared resources and preventing race conditions.

*   **Critical Sections:**
    *   Protect critical sections of code that access shared resources using mutexes or by suspending the scheduler (`vTaskSuspendAll()`, `xTaskResumeAll()`).
    *   Keep critical sections as short as possible to minimize their impact on system responsiveness.

By following this structure and these best practices, you can easily extend the functionality of the firmware while maintaining a clean, organized, and efficient codebase.