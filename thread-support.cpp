// ----------------------------------------------------------- 
// NAME : Eric Grant                    User ID: ewgrant 
// DUE DATE : 03/31/2021                                       
// Party Room Problem                             
// FILE NAME : thread-support.cpp        
// PROGRAM PURPOSE :                                           
//      simulate party room problem using semaphores and threads
//      students use a social room, a landlord checks it at random
//      if too many students are present the landlord clears it out
//      the landlord eventually retires
// ----------------------------------------------------------- 

#include "thread.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//static file variables
//
static int lordPresent = 0;
static int lordRetired = 0;
static int studentsInRoom = 0;
static int dead = 0;

static Semaphore door("door",1);
static Semaphore inQueue("inQueue",1);
static Semaphore outQueue("outQueue",1);
static Semaphore counter("counter",1);
static Semaphore clear("clear",1);
static Semaphore inBlocked("inBlocked",1);
static Semaphore outBlocked("outBlocked",1);
static Semaphore retire("retire",0);

// ----------------------------------------------------------- 
// FUNCTION  CheckRoom :                         
//    landlord tells all students to stop what they are doing
//    then enters the door and checks the room
//    if the room has a fair amount of students in it he leaves
//    if there are too many students, he steps out of the door and
//    allows students to leave, once all students are gone he allows more to enter
//    if he is ready to retire he allows students to leave and signals
//    that any students attempting to enter should die
// PARAMETER USAGE :                                           
//    i - current count of times room has been checked   
// ----------------------------------------------------------- 
void LordThread::CheckRoom(int i){
    //pre
    char buf[1000];
    inBlocked.Wait(); //block students from entering
    outBlocked.Wait(); //block students from exiting
    lordPresent = 1; //declare that the lord is in the room
    door.Wait(); //wait for student to step asside in door
    counter.Wait(); //assure number of students does not change
    
    if (i == 1) {
        sprintf(buf,"The landlord checks the room the %dst time\n", i);
        write(1, buf, strlen(buf));
    } else if (i == 2) {
        sprintf(buf,"The landlord checks the room the %dnd time\n", i);
        write(1, buf, strlen(buf));
    } else if (i == 3) {
        sprintf(buf,"The landlord checks the room the %drd time\n", i);
        write(1, buf, strlen(buf));
    } else {
        sprintf(buf,"The landlord checks the room the %dth time\n", i);
        write(1, buf, strlen(buf));
    }
    

    if (studentsInRoom > n && i != m) { //too many students
        sprintf(buf,"The landlord finds there are %d students in the room and breaks up the party\n", studentsInRoom);
        write(1, buf, strlen(buf));
    }

    //delay
    for (int j = 0; j < 100; j++) {
        Delay();
    }
    
    //post
    if (i == m) //time to retire
    { 
        lordRetired = 1; //declare lord is ready to retire
        counter.Signal(); //open counter
        door.Signal(); //open door
        outBlocked.Signal(); //allow students to exit
        inBlocked.Signal(); //allow students to enter, they should die uppon trying
        retire.Wait(); //wait till able to retire (all students dead)
        
        sprintf(buf,"After checking the room %d times, the landlord retires and is on vacation!\n", i);
        write(1, buf, strlen(buf));

        lordPresent = 0;

        Exit();
    } 
    else if (studentsInRoom > n) //too many students
    { 
        sprintf(buf,"The landlord finishes checking and forces everyone to leave\n");
        write(1, buf, strlen(buf));

        counter.Signal(); //open counter
        door.Signal(); //open door
        outBlocked.Signal(); //allow students to exit
        clear.Wait(); //wait till room is clear
        clear.Signal(); //rest clear
        
        sprintf(buf,"The landlord leaves the room and the room is empty\n");
        write(1, buf, strlen(buf));

        inBlocked.Signal(); //let students enter again
        lordPresent = 0;
    } 
    else if (studentsInRoom == 0) //no students in room
    { 
        sprintf(buf,"The landlord finds the room has no students and leaves\n");
        write(1, buf, strlen(buf));
        counter.Signal(); //open counter
        door.Signal(); //open door
        outBlocked.Signal(); //allow students to exit
        inBlocked.Signal(); //let students enter again
        lordPresent = 0;
    } 
    else //safe number of students
    { 
        sprintf(buf,"The landlord finds there are %d students in the room (condition being good) and leaves\n", studentsInRoom);
        write(1, buf, strlen(buf));
        counter.Signal(); //open counter
        door.Signal(); //open door
        outBlocked.Signal(); //allow students to exit
        inBlocked.Signal(); //let students enter again
        lordPresent = 0;
    }
}

// ----------------------------------------------------------- 
// FUNCTION  GoToParty :                         
//    Student gets in line to attend party, once at the front of
//    the line they enter the door then into the room
//    After they are bored they get in line to leave the party,
//    once at front of line they enter the door then out of the room
//    if at any point the landlord wants in a student must step out
//    of the door and wait for the landlord to do his job
//    a student may only use the door as dictated by the landlord
//    when the landlord is present
// ----------------------------------------------------------- 
void StudentThread::GoToParty(){
    //pre
    char buf[1000];
    if (lordRetired) {
        sprintf(buf,"     Student %d terminates\n", index);
        write(1, buf, strlen(buf));
        dead++;
        if (dead == k) {
            retire.Signal();
        }
        Exit();
    }

    sprintf(buf,"     Student %d waits to enter the room\n", index);
    write(1, buf, strlen(buf));

    inQueue.Wait(); //get in queue to enter room
    door.Wait(); //attempt to enter door when your turn

    if (lordPresent) { //when lord is present, blocked is 0
        door.Signal(); //let lord in door
        inBlocked.Wait(); //wait until not blocked by lord
        inBlocked.Signal(); //free another student behind you
        door.Wait(); //attempt to get back in door once lord is gone
    } 
    
    if (lordRetired) { //lord is ready to retire
        sprintf(buf,"     Student %d terminates\n", index);
        write(1, buf, strlen(buf));
        door.Signal(); //step out of door
        inQueue.Signal(); //step out of queue
        dead++; //die
        if (dead == k) {
            retire.Signal(); //if last dead, let lord know
        }
        Exit();
    }

    counter.Wait(); //only one student may change room counter at a time
    studentsInRoom++;
    sprintf(buf,"     Student %d enters the room (%d students in the room)\n", index, studentsInRoom);
    write(1, buf, strlen(buf));
    if (studentsInRoom == 1) {
        clear.Wait(); //signal that you are first in the room and it is no longer clear
    }
    counter.Signal(); //tell other students you are done with counter

    door.Signal(); //tell other students you are done with door
    inQueue.Signal(); //tell other students you are done in queue

    //delay
    for (int j = 0; j < 100; j++) {
        Delay();
    }

    //post
    
    sprintf(buf,"     Student %d waits to leave the room\n", index);
    write(1, buf, strlen(buf));

    outQueue.Wait(); //enter queue of students exiting
    door.Wait(); //attempt to enter door when your turn

    if (lordPresent) { //when lord is present and not clearing, exitBlocked is 0
        door.Signal();
        outBlocked.Wait(); //wait until not blocked by lord
        outBlocked.Signal(); //free another student behind you
        door.Wait();
    }

    counter.Wait(); //only one student may change room counter at a time
    studentsInRoom--;

    sprintf(buf,"     Student %d leaves the room (%d students in the room)\n", index, studentsInRoom);
    write(1, buf, strlen(buf));

    if (studentsInRoom == 0) {
        clear.Signal(); //Signal that room is clear of students
    }
    counter.Signal(); //tell other students you are done with counter

    door.Signal(); //tell other students you are done with door
    outQueue.Signal(); //tell other students you are done with out queue

    return;
}