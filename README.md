
# Dining Philosophers Problem - C Implementations

## Overview
This project contains **six implementations** of the Dining Philosophers problem - a classic synchronization problem in computer science that illustrates the challenges of concurrent programming and resource sharing in a system, solving it using both **threads** and **processes** in C.  
Each approach has three versions:
1. **Deadlock Possible**  
2. **Starvation Possible**  
3. **No Deadlock or Starvation (Optimized Solution)**  

## **Project Structure**
The project is divided into two main folders:  

```
📂 using-threads/
 ├── dpp-deadlock.c                      # Thread-based (Deadlock Possible)
 ├── dpp-starvation.c                    # Thread-based (Starvation Possible)
 ├── dpp-nostarvation-nodeadlock.c       # Thread-based (No Deadlock & No Starvation)
 ├── dpp-deadlock.out                    # Linux executable
 ├── dpp-starvation.out                  # Linux executable
 ├── dpp-nostarvation-nodeadlock.out     # Linux executable
 ├── dpp-deadlock.exe                    # Windows executable
 ├── dpp-starvation.exe                  # Windows executable
 ├── dpp-no-starvation-nodeadlock.exe    # Windows executable

📂 using-processes/
 ├── dpp-deadlock.c                      # Process-based (Deadlock Possible)
 ├── dpp-starvation.c                    # Process-based (Starvation Possible)
 ├── dpp-nostarvation-nodeadlock.c       # Process-based (No Deadlock & No Starvation)
 ├── dpp-deadlock.out                    # Linux executable
 ├── dpp-starvation.out                  # Linux executable
 ├── dpp-nostarvation-nodeadlock.out     # Linux executable
```

## **How to Compile (if you want) and Run**
### **On Linux**
To compile and run a program:  
```sh
gcc -o dpp-deadlock.out dpp-deadlock.c -pthread
./dpp-deadlock.out
```

### **On Windows**
To compile and run a thread-based program:
```sh
x86_64-w64-mingw32-gcc -o dpp-deadlock.exe dpp-deadlock.c -lpthread
./dpp-deadlock.exe
```
For process-based programs, MinGW might not fully support them. You may need **Cygwin** or **WSL**.

## **Explanations of the Implementations**
### **1. Deadlock Possible**
- Philosophers may hold one fork and wait indefinitely for the second, leading to a deadlock.
- No strategy is implemented to prevent circular waiting.

### **2. Starvation Possible**
- Philosophers may keep getting skipped if others continuously take forks before them.
- Some philosophers might never get a chance to eat.

### **3. No Deadlock & No Starvation**
- Every philosopher has an equal chance to eat and deadlock is impossible to occur.
---
