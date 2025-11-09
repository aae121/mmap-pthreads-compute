# mmap & Pthreads Performance Analysis

This project demonstrates the use of **Inter-Process Communication (IPC)** via **shared memory (mmap)** and **POSIX threads** to perform parallel computation.  
It compares performance across four computation strategies using multiple processes and threads.
---

## 🚀 Features
- Shared memory parallelization using **mmap**
- Multi-threaded computation with **pthread**
- Classic inter-process communication via **pipes**
- Sequential baseline for comparison
- Automatic workload distribution and timing
- CSV/XLSX output for easy graph generation

---

## 🧠 Concept Overview

### 1. mmap-based Parallelism
`mmap()` creates a shared memory segment accessible to multiple processes.  
Each process reads from the same region and writes its computed result, synchronized through shared memory.

### 2. Thread-based Parallelism
`pthread_create()` spawns lightweight threads sharing the same address space.  
Mutexes ensure proper synchronization for critical sections to avoid race conditions.

### 3. Pipes and Sequential Baselines
`pipes_compute()` represents process-level parallelism using pipes, while `sequential_compute()` serves as the single-threaded reference implementation.

---

## ⚙️ Build Instructions

### Prerequisites
- GCC or Clang compiler
- Linux or Unix-based OS
- POSIX thread support (`pthread`)
- `make` utility

### Build the Project
```bash
make all
