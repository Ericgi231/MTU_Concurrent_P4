// ----------------------------------------------------------- 
// NAME : Eric Grant                    User ID: ewgrant 
// DUE DATE : 03/31/2021                                       
// Party Room Problem                             
// FILE NAME : thread.cpp        
// PROGRAM PURPOSE :                                           
//      simulate party room problem using semaphores and threads
//      students use a social room, a landlord checks it at random
//      if too many students are present the landlord clears it out
//      the landlord eventually retires
// -----------------------------------------------------------

#include "thread.h"
#include <iostream>
#include <string.h>
#include <stdio.h>

// ----------------------------------------------------------- 
// FUNCTION  LordThread::ThreadFunc :
//      tell a lord to check the room after random delays
// ----------------------------------------------------------- 
void LordThread::ThreadFunc()
{
    Thread::ThreadFunc();

    char buf[1000];

    for (int i = 1; i < m+1; i++) {
        for (int j = 0; j < 10; j++) {
            Delay();
        }
        CheckRoom(i);
        for (int j = 0; j < 10; j++) {
            Delay();
        }
    }

    //code below should never be reached, left in as emergency
    //
    sprintf(buf, "Landlord terminates\n");
    write(1, buf, strlen(buf));

    Exit();
}

// ----------------------------------------------------------- 
// FUNCTION  StudentThread::ThreadFunc :
//      tell a student to attend parties forever with delay between
// ----------------------------------------------------------- 
void StudentThread::ThreadFunc()
{
	Thread::ThreadFunc();

    char buf[1000];

    sprintf(buf, "     Student %d starts\n", index);
    write(1, buf, strlen(buf));

    while (1) {
        Delay();
        GoToParty();
        Delay();
    }
}