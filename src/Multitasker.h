//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                      //
//  Arduino Multitasking                                                                                                                //
//  Copyright (C) 2019-2024 Jens Dyekjær Madsen                                                                                         //
//                                                                                                                                      //
//  Filename: Multitasker.h                                                                                                             //
//                                                                                                                                      //
//  Taskswitcher and multitasker v1.0 for AVR architecture. Tested with Arduino Nano. Copyright (C) 2019-2024 Jens Dyekjær Madsen       //
//                                                                                                                                      //
//  Description:                                                                                                                        //
//  Allows taskswitching and multitasking between more tasks and main program. Use yield() and delay() in loops to do multitasking.     //
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



#ifndef MULTITASKER_H
#define MULTITASKER_H

#include <Arduino.h>                                                                                            // Include header


class SimpleTask {                                                                                              // SimpleTask - simple task switching class.
private:                                                                                                        // SimpleTask only contains fundemental task switching. Do not modify.
  virtual void task() __attribute__((noinline)) = 0;                                                            // Implements the task, pure virtual
protected:
  SimpleTask(uint8_t *stack,uint16_t stacksize):tsp(stack+stacksize-1),newtask(true) {};                        // Constructor, setup stackpointer to end of stack, set newtask
public:
  void tasksw(bool run = true) volatile __attribute__((noinline));                                              // No inline accepted, needs call to return to swap to new task
  inline void go() __attribute__((always_inline))                                   { tasksw(true);           } // Activate task, short form, enables interrupt
  inline void maintask() __attribute__((always_inline))                             { tasksw(false);          } // Select main task, short form, enables interrupt
  inline bool isTaskRunning() const __attribute__((always_inline))                  { return taskPtr==this;   } // Returns true if actual task is running
  static inline bool isMainRunning() __attribute__((always_inline))                 { return taskPtr==NULL;   } // Returns true if main task is running
  static inline SimpleTask* runningTaskPtr() __attribute__((always_inline))         { return taskPtr;         } // Returns running task pointer, NULL if main is running
private:                                                                                                        // Private, do not enter this region:
  volatile bool newtask;                                                                                        //   New task is started first time task switcher is activated
  volatile uint8_t* volatile tsp;                                                                               //   Task stack pointer, volatile: stack data can be changed
  static volatile uint8_t* volatile msp;                                                                        //   Main stack pointer, volatile: stack data can be changed
  static SimpleTask* volatile taskPtr;                                                                          //   Pointer to running task, main if NULL
};

template<int STACK_SIZE>                                                                                        // Template to create a SimpleTask with stack
class CreateSimpleTask: public SimpleTask {                                                                     // SimpleTask Class
  uint8_t stack[STACK_SIZE];                                                                                    // Declaire space for stack
public:
  CreateSimpleTask():SimpleTask(stack,sizeof(stack)) {}                                                         // Setup stack
};



class Task: public SimpleTask {                                                                                 // Contains example of extended features, yield() and multitasking
protected:                                                                                                      // You might extend it with more advanced features
  Task(uint8_t *stack,uint16_t stacksize):SimpleTask(stack,stacksize) {};                                       // Constructor setups stack
public:
  virtual void yield()                                                              { maintask();             } // yield(), executes scheduler task, maintask() chosen default
  virtual void delay(uint16_t ms);                                                                              // delay(ms) suspends task in ms milliseconds and runs yield()
};

template<int STACK_SIZE>                                                                                        // Template to create a Task with stack
class CreateTask: public Task {
  uint8_t stack[STACK_SIZE];                                                                                    // Declaire space for stack
public:
  CreateTask():Task(stack,sizeof(stack)) {}                                                                     // Setup stack
};

#endif
