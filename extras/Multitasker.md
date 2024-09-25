# Multitasking with Arduino

I have been searching the web to find multitasking libraries for Arduino AVR, but there wasn't much available. I prefer something simple that is easy to use and understand, and not so automated that you don't understand what is happening. It's fine if it has some features, as long as they work independently and you can select what you want. The idea with this library is to implement a simple task switcher and see how it can be used. It is possible to add queues, locks, semaphores, scheduling, and a lot of other stuff, but that will be future development. This library is tested with Arduino Nano and ATmega4809. This multitasker works with small chips too, such as the Arduino Nano, but not with as many tasks. In most cases, it works with only 128 bytes of stack, and 4 tasks take up a total of 512 bytes of stack. My examples take 96 bytes of stack.

In this tutorial, we will learn how to:

1.  **Create a new task.**
2.  **Manually switch between tasks.**
3.  **Return to the main task using the `yield()` function and understand the difference compared to procedures and methods.**
4.  **Ways to create and destroy a task, including tasks on the heap and on the stack.**
5.  **Implement multitasking with a task switcher.**
6.  **Use an alternative task for the task switcher instead of `main`.**
7.  **Optimize multitasking using `yield()` and `delay()`.**
8.  **Conclusion.**
9.  **Contact.**

----------

# Create a New Task

Let us see how to create a task with a 128-byte stack:
```cpp
#include "Multitasker.h"                                // Include headers

// Define a specific task by inheriting from CreateSimpleTask and implementing the task() method
class Task1: public CreateSimpleTask<128> {             // Define Task1 with 128 bytes stack
  void task() override;                                 // Overrides task()
} task1;                                                // Create an instance task1 in global datasegment

void Task1::task() {                                    // Code for task1 here
  // code
}
```
We insert our code for the task in `void task() {}`.


----------

# Manual Switch Between Tasks

To show simple task switching, we create three tasks and do some manual task switching first.
```cpp
#include "Multitasker.h"                                // Include headers

auto& Stdout = Serial;                                  // Uses Serial as Stdout

// Define a specific task by inheriting from CreateSimpleTask and implementing the task() method
class Task1: public CreateSimpleTask<128> {             // Define Task1 with 128 bytes stack
  void task() override;                                 // Overrides task()
} task1;                                                // Create an instance task1

class Task2: public CreateSimpleTask<128> {             // Define Task2 with 128 bytes stack
  void task() override;                                 // Overrides task()
} task2;                                                // Create an instance task2

class Task3: public CreateSimpleTask<128> {             // Define Task3 with 128 bytes stack
  void task() override;                                 // Overrides task()
} task3;                                                // Create an instance task3

void Task1::task() {                                    // Code for task1 here
  Stdout.println("This is task 1");
  Stdout.println("This is task 1 selecting task 2");
  task2.go();                                           // Select task 2
  Stdout.println("This is task 1 selecting task 3");
  task3.go();                                           // Select task 3
  Stdout.println("Task 1 finished. Selecting main");
  maintask();                                           // Select main
}

void Task2::task() {                                    // Code for task2 here
  Stdout.println("This is task 2");
  Stdout.println("This is task 2 selecting task 3");
  task3.go();                                           // Select task 3
  Stdout.println("Task 2 finished. Selecting main");
  maintask();                                           // Select main
}

void Task3::task() {                                    // Code for task3 here
  Stdout.println("This is task 3");
  Stdout.println("This is task 3 selecting task 2");
  task2.go();                                           // Select task 2
  Stdout.println("Task 3 finished. Selecting main");
  maintask();                                           // Select main
}

void setup() {                                          // Main program
  Stdout.begin(115200);                                 // Initialize Print and wait
  while (!Stdout) Stdout.flush();

  Stdout.println("Main is running...");                 // Main task is running
  Stdout.println("Main calls task1, 1. time");
  task1.go();                                           // Select task 1
  Stdout.println("Main calls task1, 2. time");
  task1.go();                                           // Select task 1
  Stdout.println("Main calls task1, 3. time");
  task1.go();                                           // Select task 1
  Stdout.println("Main calls task1, 4. time");
  task1.go();                                           // Select task 1
  Stdout.println("Main ended");
};

void loop() {
}
```
**Output:**
```
Main is running...
Main calls task1, 1. time
This is task 1
This is task 1 selecting task 2
This is task 2
This is task 2 selecting task 3
This is task 3
This is task 3 selecting task 2
Task 2 finished. Selecting main
Main calls task1, 2. time
This is task 1 selecting task 3
Task 3 finished. Selecting main
Main calls task1, 3. time
Task 1 finished. Selecting main
Main calls task1, 4. time
This is task 1
This is task 1 selecting task 2
This is task 2
This is task 2 selecting task 3
This is task 3
This is task 3 selecting task 2
Task 2 finished. Selecting main
Main ended
```


----------

# Return to Main Task Using `yield()` Function

Until now, we have not seen much we can't do with function calls or methods. Using the `yield()` function, we get some real multitasking. We are now able to suspend a task, let it continue with other tasks, and go back to our task when the time has elapsed or an event has occurred. `yield()` is often also used in wait and delay loops, and luckily for us, there is a built-in `delay` function in the library.

Let us take a very simple example using the `yield()` function:
```cpp
#include "Multitasker.h"                                // Include headers

auto& Stdout = Serial;                                  // Uses Serial as Stdout

// Define a specific task by inheriting from CreateTask and implementing the task() method
class Task1: public CreateTask<128> {                   // Define Task1 with 128 bytes stack
  void task() override;                                 // Overrides task()
} task1;                                                // Create an instance task1

void Task1::task() {                                    // Code for task1 here
  Stdout.println("This is task 1 part 1");
  yield();                                              // maintask() releases task, to be continued
  Stdout.println("This is task 1 part 2");
  yield();                                              // maintask() releases task, to be continued
  Stdout.println("This is task 1 part 3");
  yield();                                              // maintask() releases task, to be continued
  Stdout.println("This is task 1 part 4");
  yield();                                              // maintask() releases task, to be continued
  Stdout.println("This is task 1 part 5");
  Stdout.println("Task 1 has finished, repeats");       // All threads repeats when they reach bottum
}

void setup() {                                          // Main program
  Stdout.begin(115200);                                 // Initialize Print and wait
  while (!Stdout) Stdout.flush();
  Stdout.println("Main running...");                    // Main task is running
};

void loop() {
  Stdout.println("Main selects task1");
  task1.go();                                           // Select task 1
}
```
**Output:**
```
Main running...
Main selects task1
This is task 1 part 1
Main selects task1
This is task 1 part 2
Main selects task1
This is task 1 part 3
Main selects task1
This is task 1 part 4
Main selects task1
This is task 1 part 5
Task 1 has finished, repeats
This is task 1 part 1
Main selects task1
This is task 1 part 2
Main selects task1
This is task 1 part 3
Main selects task1
This is task 1 part 4
Main selects task1
This is task 1 part 5
Task 1 has finished, repeats
This is task 1 part 1
Main selects task1
This is task 1 part 2
...
```
As we see, the `yield()` command lets us continue from where we left in task 1.


----------

# How to Create and Destroy a Task

The tasks are implemented as a normal class, and you might use class variables as well as local variables in the task.

Typical the instance are placed in the global datasegment. This allows compiletime memory check. But there is no way to remove it.

If you place the instance on the heap, you are able to remove it and might use the space for other purposes, but memory is not checked at compiletime.

It is possible to place the instance on the stack as well, but it takes up a lot of space since it includes the stack for the task. The benefit is that it is easy to remove if you put it in a scope. If you create an instance on the stack, it is typically in the main task. However, it could also be done in another task, but you need to make sure there is enough stack space where you include the other task stored on the stack.

Let us define two tasks:
```cpp
#include "Multitasker.h"                                // Include headers

auto& Stdout = Serial;                                  // Uses Serial as Stdout

// Define a specific task by inheriting from CreateTask and implementing the task() method
class Task1: public CreateTask<128> {                   // Define Task1 with 128 bytes stack
  void task() override;                                 // Create an instance task1
};

class Task2: public CreateTask<128> {                   // Define Task2 with 128 bytes stack
  void task() override;                                 // Create an instance task1
};

void Task1::task() {                                    // Code for task1 here
  Stdout.println("This is task 1 part 1");
  yield();                                              // release to main task
  Stdout.println("This is task 1 part 2");
  yield();                                              // release to main task
  Stdout.println("This is task 1 part 3");
  yield();                                              // release to main task
  Stdout.println("This is task 1 part 4");
  yield();                                              // release to main task
  Stdout.println("This is task 1 part 5");
  Stdout.println("Task 1 has finished, repeats");       // All threads repeats when they reach bottum
}

void Task2::task() {                                    // Code for task1 here
  Stdout.println("This is task 2 part 1");
  yield();                                              // release to main task
  Stdout.println("This is task 2 part 2");
  yield();                                              // release to main task
  Stdout.println("This is task 2 part 3");
  yield();                                              // release to main task
  Stdout.println("This is task 2 part 4");
  yield();                                              // release to main task
  Stdout.println("This is task 2 part 5");
  yield();                                              // release to main task (extra)
  Stdout.println("Task 2 has finished, repeats");       // All threads repeats when they reach bottum
}
```

-   **Creating Tasks Globally:**
```cpp
Task1 task1;                                            // Place task1 in global datasegment. Compiletime memory check.
Task2 task2;                                            // Place task2 in global datasegment. Compiletime memory check.

void setup() {                                          // Main program
  Stdout.begin(115200);                                 // Initialize Print and wait
  while (!Stdout) Stdout.flush();
  Stdout.println("Main running...");                    // Main task is running

  Stdout.println("Task 1 test:");                       // Task 1 test
  for (int i = 0; i < 11; i++) task1.go();              // Select task 1, 11 times

  Stdout.println("Task 2 test:");                       // Task 2 test
  for (int i = 0; i < 11; i++) task2.go();              // Select task 2, 11 times

  Stdout.println("Program ended");                      // End Program
};

void loop() {
}
```

-   **Creating Tasks on the Stack:**

Remember that tasks created on the stack consume more space, as they include the stack for the task itself.

```cpp
void setup() {                                          // Main program
  Stdout.begin(115200);                                 // Initialize Print and wait
  while (!Stdout) Stdout.flush();
  Stdout.println("Main running...");                    // Main task is running

  Stdout.println("Task 1 test:");                       // Task 1 test
  {                                                     // Start scope
    Task1 task1;                                        //   Place Task1 on stack. No compiletime memory check.
    for (int i = 0; i < 11; i++) task1.go();            //   Select task 1, 11 times
  }                                                     // End scope, Task 1 memory released

  Stdout.println("Task 2 test:");                       // Task 2 test
  {                                                     // Start scope
    Task2 task2;                                        //   Place Task2 on stack. No compiletime memory check.
    for (int i = 0; i < 11; i++) task2.go();            //   Select task 2, 11 times
  }                                                     // End scope, Task 2 memory released

  Stdout.println("Program ended");                      // End Program
};

void loop() {
}
```

-   **Creating Tasks on the Heap:**

```cpp
void setup() {                                          // Main program
  Stdout.begin(115200);                                 // Initialize Print and wait
  while (!Stdout) Stdout.flush();
  Stdout.println("Main running...");                    // Main task is running

  // Reserve memory for Task 1, switch to it, and release memory again
  Stdout.println("Task 1 test:");                       // Task 1 test
  Task1* task1ptr = new Task1;                          // Allocate memory on heap for Task 1
  for (int i = 0; i < 11; i++) task1ptr->go();          // Select task 1, 11 times
  delete task1ptr;                                      // Task 1 memory released

  // Reserve memory for Task 2, switch to it, and release memory again
  Stdout.println("Task 2 test:");                       // Task 2 test
  Task2* task2ptr = new Task2;                          // Allocate memory on heap for Task 2
  for (int i = 0; i < 11; i++) task2ptr->go();          // Select task 2, 11 times
  delete task2ptr;                                      // Task 2 memory released

  Stdout.println("Program ended");                      // End Program
};

void loop() {
}
```


----------

# Multitasking with a Task Switcher/Scheduler

A task switcher/scheduler is simple code that activates the tasks one by one. If you need more resources for a task, you simply switch to it more times in the task switcher loop, distributing the calls over the loop. A task switcher is the most efficient way to automatically switch between tasks, and it has its own stack, which makes it very efficient. In some cases, you might write your own code that decides which tasks are active and their execution order if you like to controle it.

In most operating systems, there is not much control over which tasks are running. Instead, the `yield()` function is used in a loop that is suspended, and all tasks, even the suspended ones, are actually running. But since they release quickly after polling a port or checking time, only very few resources are used by the suspended tasks. Here, you have control yourself, and you can write code such as state machines that control which tasks run and in what order, use priority queues, or whatever you like. There are no limits to your creativity. Operating systems have a very simple way of handling it, and although they may contain more complicated functions such as priority queues, you are actually more constrained compared to coding everything yourself.

Let us see an example:
```cpp
#include "Multitasker.h"                                // Include headers

auto& Stdout = Serial;                                  // Uses Serial as Stdout

// Define a specific task by inheriting from CreateTask and implementing the task() method
class Task1: public CreateTask<128> {                   // Define Task1 with 128 bytes stack
  void task() override;                                 // Overrides task()
} task1;                                                // Create an instance task1

class Task2: public CreateTask<128> {                   // Define Task2 with 128 bytes stack
  void task() override;                                 // Overrides task()
} task2;                                                // Create an instance task2

class Task3: public CreateTask<128> {                   // Define Task3 with 128 bytes stack
  void task() override;                                 // Overrides task()
} task3;                                                // Create an instance task3

class Task4: public CreateTask<128> {                   // Define Task4 with 128 bytes stack
  void task() override;                                 // Overrides task()
} task4;                                                // Create an instance task4

void Task1::task() {                                    // Code for task1 here
  Stdout.println("This is task 1 part 1");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 1 part 2");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 1 part 3");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 1 part 4");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 1 part 5");
  Stdout.println("Task 1 has finished.");
  while (true) yield();                                 // Stop task, but continue running yield()
}

void Task2::task() {                                    // Code for task2 here
  Stdout.println("This is task 2 part 1");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 2 part 2");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 2 part 3");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 2 part 4");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 2 part 5");
  Stdout.println("Task 2 has finished.");
  while (true) yield();                                 // Stop task
}

void Task3::task() {                                    // Code for task3 here
  Stdout.println("This is task 3 part 1");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 3 part 2");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 3 part 3");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 3 part 4");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 3 part 5");
  Stdout.println("Task 3 has finished.");
  while (true) yield();                                 // Stop task
}

void Task4::task() {                                    // Code for task4 here
  Stdout.println("This is task 4 part 1");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 4 part 2");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 4 part 3");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 4 part 4");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 4 part 5");
  Stdout.println("Task 4 has finished.");
  while (true) yield();                                 // Stop task
}

void setup() {                                          // Main program
  Stdout.begin(115200);                                 // Initialize Print and wait
  while (!Stdout) Stdout.flush();

  Stdout.println("Main running...");                    // Main task is running
};

void loop() {                                           // Main task is scheduler, executing task 1,2,3,1,2,4 in loop. (Half time to task 3,4)
  task1.go();                                           // Invoke time to task1
  task2.go();                                           // Invoke time to task2
  task3.go();                                           // Invoke time to task3
  task1.go();                                           // Invoke double time to task 1
  task2.go();                                           // and double time to task 2
  task4.go();                                           // Invoke time to task4
}
```
**Output:**
```
Main running...
This is task 1 part 1
This is task 2 part 1
This is task 3 part 1
This is task 1 part 2
This is task 2 part 2
This is task 4 part 1
This is task 1 part 3
This is task 2 part 3
This is task 3 part 2
This is task 1 part 4
This is task 2 part 4
This is task 4 part 2
This is task 1 part 5
Task 1 has finished.
This is task 2 part 5
Task 2 has finished.
This is task 3 part 3
This is task 4 part 3
This is task 3 part 4
This is task 4 part 4
This is task 3 part 5
Task 3 has finished.
This is task 4 part 5
Task 4 has finished.
```
As we see, a task is divided into small parts, and `yield()` suspends the task and continues execution in another task.


----------

# Alternative Task for Task Switcher Instead of `main`

It is possible to use an alternative task as schedule the tasks instead of main task. Often main task might be needed for other purpose.
Let us see an example:
```cpp
#include "Multitasker.h"                                // Include headers

auto& Stdout = Serial;                                  // Uses Serial as Stdout

// Define a specific task by inheriting from CreateTask and implementing the task() method
class Multitasker: public CreateTask<128> {             // Task switcher running trough the tasks (Scheduler), 128 bytes stack
  void task() override;                                 // Overrides task()
} multitasker;                                          // Create an instance multitask


class Task1: public CreateTask<128> {                   // Define Task1 with 128 bytes stack
  void task() override;                                 // Overrides task()
  void yield() override;                                // Overrides yield() funktion for Task1, executes task switcher
} task1;                                                // Create an instance task1

class Task2: public CreateTask<128> {                   // Define Task2 with 128 bytes stack
  void task() override;                                 // Overrides task()
  void yield() override;                                // Overrides yield() funktion for Task2, executes task switcher
} task2;                                                // Create an instance task2

class Task3: public CreateTask<128> {                   // Define Task3 with 128 bytes stack
  void task() override;                                 // Overrides task()
  void yield() override;                                // Overrides yield() funktion for Task3, executes task switcher
} task3;                                                // Create an instance task3

class Task4: public CreateTask<128> {                   // Define Task4 with 128 bytes stack
  void task() override;                                 // Overrides task()
  void yield() override;                                // Overrides yield() funktion for Task4, executes task switcher
} task4;                                                // Create an instance task4


void Task1::yield() {                                   // Implements yield() funktion for Task1
  multitasker.go();                                     // Execute task switcher
}

void Task2::yield() {                                   // Implements yield() funktion for Task2
  multitasker.go();                                     // Execute task switcher
}

void Task3::yield() {                                   // Implements yield() funktion for Task3
  multitasker.go();                                     // Execute task switcher
}

void Task4::yield() {                                   // Implements yield() funktion for Task4
  multitasker.go();                                     // Execute task switcher
}

void Task1::task() {                                    // Code for task1 here
  Stdout.println("This is task 1 part 1");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 1 part 2");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 1 part 3");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 1 part 4");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 1 part 5");
  Stdout.println("Task 1 has finished.");
  while (true) yield();                                 // Stop task, but continue running yield()
}

void Task2::task() {                                    // Code for task2 here
  Stdout.println("This is task 2 part 1");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 2 part 2");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 2 part 3");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 2 part 4");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 2 part 5");
  Stdout.println("Task 2 has finished.");
  while (true) yield();                                 // Stop task
}

void Task3::task() {                                    // Code for task3 here
  Stdout.println("This is task 3 part 1");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 3 part 2");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 3 part 3");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 3 part 4");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 3 part 5");
  Stdout.println("Task 3 has finished.");
  while (true) yield();                                 // Stop task
}

void Task4::task() {                                    // Code for task4 here
  Stdout.println("This is task 4 part 1");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 4 part 2");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 4 part 3");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 4 part 4");
  yield();                                              // yield() releases task, to be continued
  Stdout.println("This is task 4 part 5");
  Stdout.println("Task 4 has finished.");
  while (true) yield();                                 // Stop task
}

void Multitasker::task() {                              // Code for multitasker/scheduler here, loops
  task1.go();                                           // Invoke time to task1
  task2.go();                                           // Invoke time to task2
  task3.go();                                           // Invoke time to task3
  task1.go();                                           // Invoke double time to task 1
  task2.go();                                           // and double time to task 2
  task4.go();                                           // Invoke time to task4
  maintask();                                           // Also time to main task
}                                                       // A task always loops after finished

void setup() {                                          // Main program
  Stdout.begin(115200);                                 // Initialize Print and wait
  while (!Stdout) Stdout.flush();

  Stdout.println("Main running...");                    // Main task is running
  Stdout.println("Main selects multitasker first time");
  multitasker.go();
};

void loop() {
  multitasker.go();                                     // Execute tasks until they are finished
}
```
**Output:**
```
Main running...
Main selects multitasker first time
This is task 1 part 1
This is task 2 part 1
This is task 3 part 1
This is task 1 part 2
This is task 2 part 2
This is task 4 part 1
This is task 1 part 3
This is task 2 part 3
This is task 3 part 2
This is task 1 part 4
This is task 2 part 4
This is task 4 part 2
This is task 1 part 5
Task 1 has finished.
This is task 2 part 5
Task 2 has finished.
This is task 3 part 3
This is task 4 part 3
This is task 3 part 4
This is task 4 part 4
This is task 3 part 5
Task 3 has finished.
This is task 4 part 5
Task 4 has finished.
```


----------

# Using `yield()` and `delay()` to Optimize Multitasking

The library has its own `delay` function that includes calling the `yield()` function. If you use delays, the time of the delay is used by other tasks. You can easily add message queues if you like, but remember to use `yield()` or `delay()` in the waiting loop. Then you allow the other tasks to receive or send messages to the task.

Let us see an example:
```cpp
#include "Multitasker.h"                                // Include headers

auto& Stdout = Serial;                                  // Uses Serial as Stdout

// Define a specific task by inheriting from CreateTask and implementing the task() method
class Multitasker: public CreateTask<128> {             // Task switcher running trough the tasks (Scheduler), 128 bytes stack
  void task() override;                                 // Overrides task()
} multitasker;                                          // Create an instance multitask


class Task1: public CreateTask<128> {                   // Define Task1 with 128 bytes stack
  void task() override;                                 // Overrides task()
  void yield() override;                                // Overrides yield() funktion for Task1, executes task switcher
} task1;                                                // Create an instance task1

class Task2: public CreateTask<128> {                   // Define Task2 with 128 bytes stack
  void task() override;                                 // Overrides task()
  void yield() override;                                // Overrides yield() funktion for Task3, executes task switcher
} task2;                                                // Create an instance task3

class Task3: public CreateTask<128> {                   // Define Task3 with 128 bytes stack
  void task() override;                                 // Overrides task()
  void yield() override;                                // Overrides yield() funktion for Task4, executes task switcher
} task3;                                                // Create an instance task4


void Task1::yield() {                                   // Implements yield() funktion for Task1
  multitasker.go();                                     // Execute task switcher
}

void Task2::yield() {                                   // Implements yield() funktion for Task2
  multitasker.go();                                     // Execute task switcher
}

void Task3::yield() {                                   // Implements yield() funktion for Task3
  multitasker.go();                                     // Execute task switcher
}


void Task1::task() {                                    // Code for task1 here
  Stdout.println("0000 ms: This is task 1 part 1");
  delay(1000);                                          // delay(1000) releases task, other tasks runs
  Stdout.println("1000 ms: This is task 1 part 2");
  delay(2000);                                          // delay(2000) releases task, other tasks runs
  Stdout.println("3000 ms: This is task 1 part 3");
  delay(3000);                                          // delay(3000) releases task, other tasks runs
  Stdout.println("6000 ms: This is task 1 part 4");
  delay(4000);                                          // delay(4000) releases task, other tasks runs
  Stdout.println("10000ms: This is task 1 part 5, Task 1 has finished.");
  while (true) yield();                                 // Stop task
}                                // Stop task

void Task2::task() {                                    // Code for task2 here
  Stdout.println("0000 ms: This is task 2 part 1");
  delay(200);                                           // delay(100) releases task, other tasks runs
  Stdout.println("0200 ms: This is task 2 part 2");
  delay(200);                                           // delay(100) releases task, other tasks runs
  Stdout.println("0400 ms: This is task 2 part 3");
  delay(200);                                           // delay(100) releases task, other tasks runs
  Stdout.println("0600 ms: This is task 2 part 4");
  delay(200);                                           // delay(100) releases task, other tasks runs
  Stdout.println("0800 ms: This is task 2 part 5, task 2 has finished.");
  while (true) yield();                                 // Stop task
}

void Task3::task() {                                    // Code for task3 here
  Stdout.println("0000 ms: This is task 3 part 1");
  delay(1000);                                          // delay(1000) releases task, other tasks runs
  Stdout.println("1000 ms: This is task 3 part 2");
  delay(1000);                                          // delay(1000) releases task, other tasks runs
  Stdout.println("2000 ms: This is task 3 part 3");
  delay(1000);                                          // delay(1000) releases task, other tasks runs
  Stdout.println("3000 ms: This is task 3 part 4");
  delay(1000);                                          // delay(1000) releases task, other tasks runs
  Stdout.println("4000 ms: This is task 3 part 5, task 3 has finished.");
  while (true) yield();                                 // Stop task
}

void Multitasker::task() {                              // Code for multitasker/scheduler here, loops
  task1.go();                                           // Time to task1
  task2.go();                                           // Time to task2
  task1.go();                                           // Double time to task1
  task3.go();                                           // Time to task3
  maintask();                                           // Also give time to main task
}                                                       // A thread always loops after finished

void setup() {                                          // Main program
  Stdout.begin(115200);                                 // Initialize Print and wait
  while (!Stdout) Stdout.flush();

  Stdout.println("Main running...   Main selects tasker first time");   // Main task is running
  multitasker.go();
};

void loop() {
  multitasker.go();                                     // Execute tasks until they are finished
}
```
**Output:**
```
Main running...   Main selects tasker first time
0000 ms: This is task 1 part 1
0000 ms: This is task 2 part 1
0000 ms: This is task 3 part 1
0200 ms: This is task 2 part 2
0400 ms: This is task 2 part 3
0600 ms: This is task 2 part 4
0800 ms: This is task 2 part 5, task 2 has finished.
1000 ms: This is task 1 part 2
1000 ms: This is task 3 part 2
2000 ms: This is task 3 part 3
3000 ms: This is task 1 part 3
3000 ms: This is task 3 part 4
4000 ms: This is task 3 part 5, task 3 has finished.
6000 ms: This is task 1 part 4
10000ms: This is task 1 part 5, Task 1 has finished.
```

As we see, the tasks are performed in an order determined by the delays. All delays start at the same time and run concurrently. When the time has elapsed, they continue. By adding `yield()` in any loop or where you need to wait (as is done in the `delay()` function), other tasks are performed while waiting.


----------

# Conclusion

In this tutorial, we've explored how to implement simple multitasking on an Arduino using a custom task switcher. We began by creating new tasks and manually switching between them to understand the basics of task management. By introducing the `yield()` function, we achieved cooperative multitasking, allowing tasks to suspend and resume efficiently.

We also discussed various methods for creating and destroying tasks, whether on the heap or the stack, highlighting the benefits and considerations of each approach. Implementing a task switcher or scheduler enabled us to automate task execution, improving efficiency and control over task prioritization.

By using `yield()` and `delay()` strategically, we optimized multitasking to ensure that tasks run concurrently and processor time is utilized effectively. This approach allows for more responsive and efficient programs, even on resource-constrained devices like the Arduino Nano.

Remember that multitasking on microcontrollers requires careful management of resources. Always monitor your memory usage and ensure that tasks are efficiently coded to prevent issues like stack overflows or resource conflicts. In the examples I use 128 bytes stack for tasks. With 96 bytes for the basic stuff, it leaves 32 butes free.

By mastering these multitasking techniques, you can significantly enhance the capabilities of your Arduino projects and tackle more advanced challenges in embedded systems development.

----------

# Contact

If you have any questions or feedback, feel free to reach out using the [contact form](https://www.arduino.one/contact.html) on my site.
