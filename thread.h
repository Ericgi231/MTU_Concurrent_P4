#include "ThreadClass.h"

class LordThread : public Thread {
	public:
		LordThread(int m, int n) : m(m), n(n) {};
	private:
        int m; //number of times landlord can check room
        int n; //number of students allowed in room, no more than
		void ThreadFunc();
        void CheckRoom(int i);
};

class StudentThread : public Thread {
	public:
		StudentThread(int index, int k) : index(index), k(k) {};
	private:
		int index;
		int k; //total number of students
		void ThreadFunc();
        void GoToParty();
};