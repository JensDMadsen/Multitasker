//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                      //
//  Arduino Multitasking                                                                                                                //
//  Copyright (C) 2019-2024 Jens Dyekjær Madsen                                                                                         //
//                                                                                                                                      //
//  Filename: DemoTask5A.ino                                                                                                            //
//                                                                                                                                      //
//  Description:                                                                                                                        //
//  Demo example using Task class to illustrate task switching.                                                                         //
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
class Task1: public CreateTask<128> {                                           // Define Task1 with 128 bytes stack
  void task() override;                                                         // Create an instance task1
};

class Task2: public CreateTask<128> {                                           // Define Task2 with 128 bytes stack
  void task() override;                                                         // Create an instance task1
};

void Task1::task() {                                                            // Code for task1 here
  Stdout.println("This is task 1 part 1");
  yield();                                                                      // release to main task
  Stdout.println("This is task 1 part 2");
  yield();                                                                      // release to main task
  Stdout.println("This is task 1 part 3");
  yield();                                                                      // release to main task
  Stdout.println("This is task 1 part 4");
  yield();                                                                      // release to main task
  Stdout.println("This is task 1 part 5");
  Stdout.println("Task 1 has finished, repeats");                               // All threads repeats when they reach bottum
}

void Task2::task() {                                                            // Code for task1 here
  Stdout.println("This is task 2 part 1");
  yield();                                                                      // release to main task
  Stdout.println("This is task 2 part 2");
  yield();                                                                      // release to main task
  Stdout.println("This is task 2 part 3");
  yield();                                                                      // release to main task
  Stdout.println("This is task 2 part 4");
  yield();                                                                      // release to main task
  Stdout.println("This is task 2 part 5");
  yield();                                                                      // release to main task (extra)
  Stdout.println("Task 2 has finished, repeats");                               // All threads repeats when they reach bottum
}

Task1 task1;                                                                    // Place Task1 in global datasegment. Compiletime memory check.
Task2 task2;                                                                    // Place Task2 in global datasegment. Compiletime memory check.

void setup() {                                                                  // Main program
  Stdout.begin(115200);                                                         // Initialize Print and wait
  while (!Stdout) Stdout.flush();
  Stdout.println("Main running...");                                            // Main task is running

  Stdout.println("Task 1 test:");                                               // Task 1 test
  for (int i = 0; i < 11; i++) task1.go();                                      // Select task 1, 11 times

  Stdout.println("Task 2 test:");                                               // Task 2 test
  for (int i = 0; i < 11; i++) task2.go();                                      // Select task 2, 11 times

  Stdout.println("Program ended");                                              // End Program
};

void loop() {
}