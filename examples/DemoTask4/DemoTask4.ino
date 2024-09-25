//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                      //
//  Arduino Multitasking                                                                                                                //
//  Copyright (C) 2019-2024 Jens Dyekjær Madsen                                                                                         //
//                                                                                                                                      //
//  Filename: DemoTask4.ino                                                                                                             //
//                                                                                                                                      //
//  Description:                                                                                                                        //
//  Demo example using Task class to illustrate task switching.                                                                         //
//  This demo shows how delays executes other tasks while pausing. The tasks are executed in time order to match the pause in delay.    //
//  As long yield() function is called in loops, time is used to other tasks, if example waiting on hardware, inputs, or interrupts.    //
//  As demo3 it uses a task switching class instead of main task. Main task might be used for other purposes.                           //
//                                                                                                                                      //
//  This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License    //
//  as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.             //
//                                                                                                                                      //
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of      //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.                   //
//  You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the              //
//  Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA                                            //
//                                                                                                                                      //
//  It is not allowed to change any license or copyright statements, but feel free to modify, change, and add your own copyrights       //
//  below this line only !                                                                                                              //
//  ----------------------------------------------------------------------------------------------------------------------------------  //
//                                                                                                                                      //
//  Testet on AVR architecture: Arduino Nano and ATMEGA4809.                                                                            //
//                                                                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include "Multitasker.h"                                                        // Include headers

auto& Stdout = Serial;                                                          // Uses Serial as Stdout

// Define a specific task by inheriting from CreateTask and implementing the task() method
class Multitasker: public CreateTask<128> {                                     // Task switcher running trough the tasks (Scheduler), 128 bytes stack
  void task() override;                                                         // Overrides task()
} multitasker;                                                                  // Create an instance multitask


class Task1: public CreateTask<128> {                                           // Define Task1 with 128 bytes stack
  void task() override;                                                         // Overrides task()
  void yield() override;                                                        // Overrides yield() funktion for Task1, executes task switcher
} task1;                                                                        // Create an instance task1

class Task2: public CreateTask<128> {                                           // Define Task2 with 128 bytes stack
  void task() override;                                                         // Overrides task()
  void yield() override;                                                        // Overrides yield() funktion for Task3, executes task switcher
} task2;                                                                        // Create an instance task3

class Task3: public CreateTask<128> {                                           // Define Task3 with 128 bytes stack
  void task() override;                                                         // Overrides task()
  void yield() override;                                                        // Overrides yield() funktion for Task4, executes task switcher
} task3;                                                                        // Create an instance task4


void Task1::yield() {                                                           // Implements yield() funktion for Task1
  multitasker.go();                                                             // Execute task switcher
}

void Task2::yield() {                                                           // Implements yield() funktion for Task2
  multitasker.go();                                                             // Execute task switcher
}

void Task3::yield() {                                                           // Implements yield() funktion for Task3
  multitasker.go();                                                             // Execute task switcher
}


void Task1::task() {                                                            // Code for task1 here
  Stdout.println("0000 ms: This is task 1 part 1");
  delay(1000);                                                                  // delay(1000) releases task, other tasks runs
  Stdout.println("1000 ms: This is task 1 part 2");
  delay(2000);                                                                  // delay(2000) releases task, other tasks runs
  Stdout.println("3000 ms: This is task 1 part 3");
  delay(3000);                                                                  // delay(3000) releases task, other tasks runs
  Stdout.println("6000 ms: This is task 1 part 4");
  delay(4000);                                                                  // delay(4000) releases task, other tasks runs
  Stdout.println("10000ms: This is task 1 part 5, Task 1 has finished.");
  while (true) yield();                                                         // Stop task
}                                                        // Stop task

void Task2::task() {                                                            // Code for task2 here
  Stdout.println("0000 ms: This is task 2 part 1");
  delay(200);                                                                   // delay(100) releases task, other tasks runs
  Stdout.println("0200 ms: This is task 2 part 2");
  delay(200);                                                                   // delay(100) releases task, other tasks runs
  Stdout.println("0400 ms: This is task 2 part 3");
  delay(200);                                                                   // delay(100) releases task, other tasks runs
  Stdout.println("0600 ms: This is task 2 part 4");
  delay(200);                                                                   // delay(100) releases task, other tasks runs
  Stdout.println("0800 ms: This is task 2 part 5, task 2 has finished.");
  while (true) yield();                                                         // Stop task
}

void Task3::task() {                                                            // Code for task3 here
  Stdout.println("0000 ms: This is task 3 part 1");
  delay(1000);                                                                  // delay(1000) releases task, other tasks runs
  Stdout.println("1000 ms: This is task 3 part 2");
  delay(1000);                                                                  // delay(1000) releases task, other tasks runs
  Stdout.println("2000 ms: This is task 3 part 3");
  delay(1000);                                                                  // delay(1000) releases task, other tasks runs
  Stdout.println("3000 ms: This is task 3 part 4");
  delay(1000);                                                                  // delay(1000) releases task, other tasks runs
  Stdout.println("4000 ms: This is task 3 part 5, task 3 has finished.");
  while (true) yield();                                                         // Stop task
}

void Multitasker::task() {                                                      // Code for multitasker/scheduler here, loops
  task1.go();                                                                   // Time to task1
  task2.go();                                                                   // Time to task2
  task1.go();                                                                   // Double time to task1
  task3.go();                                                                   // Time to task3
  maintask();                                                                   // Also give time to main task
}                                                                               // A thread always loops after finished

void setup() {                                                                  // Main program
  Stdout.begin(115200);                                                         // Initialize Print and wait
  while (!Stdout) Stdout.flush();

  Stdout.println("Main running...   Main selects tasker first time");           // Main task is running
  multitasker.go();
};

void loop() {
  multitasker.go();                                                             // Execute tasks until they are finished
}
