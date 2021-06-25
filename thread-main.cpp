// ----------------------------------------------------------- 
// NAME : Eric Grant                    User ID: ewgrant 
// DUE DATE : 03/31/2021                                       
// Party Room Problem                             
// FILE NAME : thread-main.cpp        
// PROGRAM PURPOSE :                                           
//      simulate party room problem using semaphores and threads
//      students use a social room, a landlord checks it at random
//      if too many students are present the landlord clears it out
//      the landlord eventually retires
// ----------------------------------------------------------- 

#include <iostream>
#include "thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

// ----------------------------------------------------------- 
// FUNCTION  main :                         
//    spawns students and landlord then waits for them to die
// PARAMETER USAGE :                                           
//    argv, argc    
//    argc contains:
//    m, number of times landlord checks room
//    n, number of students allowed in room
//    k, number of students that exist
// ----------------------------------------------------------- 
int main(int argv, char* argc[])
{
    //vars
    //
    if (argv != 4) {
        return 0;
    }

    int m = atoi(argc[1]);
    int n = atoi(argc[2]);
    int k = atoi(argc[3]);

    if (m == 0) {
        m = 5;
    }

    if (n == 0) {
        n = 5;
    }

    if (k == 0) {
        k = 10;
    }

    LordThread* lord;
    StudentThread* students[k];

    //spawn land lord
    lord = new LordThread(m, n);
    lord -> Begin();

    //spawn students
    for (int i = 0; i < k; i++) {
        students[i] = new StudentThread(i, k);
        students[i] -> Begin();
    }

    //wait for death
    lord -> Join();
    for (int i = 0; i < k; i++) {
        students[i] -> Join();
    }
}