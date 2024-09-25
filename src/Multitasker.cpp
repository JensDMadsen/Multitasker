//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                      //
//  Arduino Multitasking                                                                                                                //
//  Copyright (C) 2019-2024 Jens Dyekjær Madsen                                                                                         //
//                                                                                                                                      //
//  Filename: Multitasker.cpp                                                                                                           //
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



#include "Multitasker.h"                                                                                        // Include headers

// Initialize static variables
volatile uint8_t* volatile SimpleTask::msp;                                                                     // Instantiate main stack pointer
SimpleTask* volatile SimpleTask::taskPtr = NULL;                                                                // Set running taskPtr to main

// Task tasksw(bool run) runs task if run is true, if run is false it leaves task and returns to main task
void SimpleTask::tasksw(bool run) volatile {                                                                    // Taskswitcher, enables interrupts
  asm volatile (                                                                                                // push call saved registers R2–R17, R28, R29
    "push r2\n  push r3\n  push r4\n  push r5\n  push r6\n  push r7\n "                                         // - other registers are no need to save
    "push r8\n  push r9\n  push r10\n push r11\n push r12\n push r13\n"
    "push r14\n push r15\n push r16\n push r17\n push r28\n push r29\n" ::: "memory"                            // No reorder across memory barrier
  );
  cli();                                                                                                        // No interrupts allowed at switching task
  if (taskPtr!=NULL) taskPtr->tsp=SP; else msp=SP;                                                              // Store SP in running task stackpointer
  if (run) {                                                                                                    // Run task if run is true:
    taskPtr=this;                                                                                               //   Setup this as running task
    SP=tsp;                                                                                                     //   Setup stackpointer for this task
    if (newtask) { newtask = false; sei(); while (true) task(); }                                               //   Check for new task and run forever, until destroyed
  } else {                                                                                                      // Leave task and run main task if run is false:
    taskPtr=NULL;                                                                                               //   Setup main as running task
    SP=msp;                                                                                                     //   Setup stackpointer for main task
  }
  sei();                                                                                                        // Interrupts back on
  asm volatile (                                                                                                // pop registers to switch to new task
    "pop r29\n  pop r28\n  pop r17\n  pop r16\n  pop r15\n  pop r14\n"                                          // pop call saved registers R2–R17, R28, R29
    "pop r13\n  pop r12\n  pop r11\n  pop r10\n  pop r9\n   pop r8\n "
    "pop r7\n   pop r6\n   pop r5\n   pop r4\n   pop r3\n   pop r2\n" ::: "memory"                              // No reorder across memory barrier
  );
}__attribute__((noinline));                                                                                     // No inline accepted, needs call return to swap to new task


void Task::delay(uint16_t ms) {                                                                                 // delay(ms) suspends task in ms milliseconds and runs yield()
  unsigned long time = micros();                                                                                // Get time in micros, to get high accuracy
  time+=(unsigned long)ms *1000;                                                                                // Calculate end time in microseconds
  while ((signed long)(time-micros())>0) yield();                                                               // Execute yield() until time elapsed
}