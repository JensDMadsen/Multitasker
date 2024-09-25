//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                      //
//  Arduino Multitasking                                                                                                                //
//  Copyright (C) 2019-2024 Jens Dyekjær Madsen                                                                                         //
//                                                                                                                                      //
//  Filename: SimpleTask.ino                                                                                                            //
//                                                                                                                                      //
//  Description:                                                                                                                        //
//  Demo example using SimpleTask class to illustrate task switching.                                                                   //
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

// Define a specific task by inheriting from CreateSimpleTask and implementing the task() method
class Task1: public CreateSimpleTask<128> {                                     // Define Task1 with 128 bytes stack
  void task() override;                                                         // Overrides task()
} task1;                                                                        // Create an instance task1

class Task2: public CreateSimpleTask<128> {                                     // Define Task2 with 128 bytes stack
  void task() override;                                                         // Overrides task()
} task2;                                                                        // Create an instance task2

class Task3: public CreateSimpleTask<128> {                                     // Define Task3 with 128 bytes stack
  void task() override;                                                         // Overrides task()
} task3;                                                                        // Create an instance task3

void Task1::task() {                                                            // Code for task1 here
  Stdout.println("This is task 1");
  Stdout.println("This is task 1 selecting task 2");
  task2.go();                                                                   // Select task 2
  Stdout.println("This is task 1 selecting task 3");
  task3.go();                                                                   // Select task 3
  Stdout.println("Task 1 finished. Selecting main");
  maintask();                                                                   // Select main
}

void Task2::task() {                                                            // Code for task2 here
  Stdout.println("This is task 2");
  Stdout.println("This is task 2 selecting task 3");
  task3.go();                                                                   // Select task 3
  Stdout.println("Task 2 finished. Selecting main");
  maintask();                                                                   // Select main
}

void Task3::task() {                                                            // Code for task3 here
  Stdout.println("This is task 3");
  Stdout.println("This is task 3 selecting task 2");
  task2.go();                                                                   // Select task 2
  Stdout.println("Task 3 finished. Selecting main");
  maintask();                                                                   // Select main
}

void setup() {                                                                  // Main program
  Stdout.begin(115200);                                                         // Initialize Print and wait
  while (!Stdout) Stdout.flush();

  Stdout.println("Main is running...");                                         // Main task is running
  Stdout.println("Main calls task1, 1. time");
  task1.go();                                                                   // Select task 1
  Stdout.println("Main calls task1, 2. time");
  task1.go();                                                                   // Select task 1
  Stdout.println("Main calls task1, 3. time");
  task1.go();                                                                   // Select task 1
  Stdout.println("Main calls task1, 4. time");
  task1.go();                                                                   // Select task 1
  Stdout.println("Main ended");
};

void loop() {
}