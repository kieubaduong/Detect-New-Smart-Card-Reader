# Smart Card Reader Detection

This repository contains a C++ program that monitors for the insertion of new smart card readers into a system. The main logic is implemented in the `main.cpp` file.

## How It Works

The application continuously monitors the system for any newly inserted smart card readers. When a new reader is detected, the application will output a message indicating the detection.

The program first establishes a context for the smart card using `SCardEstablishContext()`. It then retrieves a list of smart card readers using `SCardListReaders()`. The program counts the number of readers and initializes a vector to store the state of each reader. 

An additional reader state is added to the vector to receive notifications when a new reader is inserted. The program then enters a loop where it waits for a change in the state of any of the readers using `SCardGetStatusChange()`. If a change is detected, it outputs a message. If a new reader is detected, it also outputs a message.
