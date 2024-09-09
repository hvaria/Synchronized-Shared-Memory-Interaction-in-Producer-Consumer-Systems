
# Synchronized Shared Memory Interaction in Producer-Consumer Systems

## Overview

This project implements a synchronized shared memory interaction mechanism in a producer-consumer system. The producer-consumer problem is a classic example of multi-process synchronization, where the producer generates data and the consumer processes it. The two processes share a buffer, and this implementation uses shared memory and synchronization techniques to ensure data integrity.

## Project Structure

- **consumer.c**: Contains the implementation of the consumer, which reads data from the shared memory and processes it.
- **producer.c**: Contains the implementation of the producer, which generates data and writes it to the shared memory.
- **consumer/** and **producer/**: Additional resources or extensions for the consumer and producer processes, respectively.

## Features

- **Shared Memory**: Utilizes shared memory to enable data exchange between the producer and consumer.
- **Synchronization**: Implements synchronization techniques to prevent race conditions and ensure that the producer and consumer access shared memory in a controlled manner.
- **Minimal and Efficient**: Designed to be simple while ensuring efficient data exchange and synchronization.

## Requirements

- **GCC**: The code is written in C and can be compiled using `gcc`.
- **POSIX Shared Memory and Semaphores**: The system relies on POSIX shared memory and semaphores for synchronization.

## Compilation and Execution

1. To compile the producer and consumer programs, run:
    ```bash
    gcc producer.c -o producer
    gcc consumer.c -o consumer
    ```

2. First, start the consumer in one terminal:
    ```bash
    ./consumer
    ```

3. Then, in another terminal, run the producer:
    ```bash
    ./producer
    ```

+-----------------+     +-------------------+     +-----------------+
|                 |     |                   |     |                 |
|     Producer    |---->|  Shared Memory     |---->|    Consumer     |
|                 |     |   (Buffer)         |     |                 |
+-----------------+     |                   |     +-----------------+
        |               +-------------------+              ^
        |                        |                         |
        v                        v                         |
+----------------+        +------------------+        +----------------+
|   Semaphore    |<------>|   Synchronization |<------>|   Semaphore    |
|   (Controls)   |        |   Mechanism       |        |   (Controls)   |
+----------------+        +------------------+        +----------------+


## License

This project is open-source and can be modified or distributed under the terms of the MIT License.
