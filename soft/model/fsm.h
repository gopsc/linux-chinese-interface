#pragma once
#include<mutex>
#include<condition_variable>
#include"../../type.h"
namespace qing{

	enum Stat{
	    SSHUT = -1,
	    SSTOP = 0,
	    SSTART = 1,
	    SRUNNING = 2
	};//Status

	class Fsm:Unit{//FiniteStateMachine
	public:
	    Fsm();
	    enum Stat chk();
	    void wake();
	    void stop();
	    void close();
	    void run();
	protected:
	    void suspend();
	private:
	    std::mutex lock;
	    std::condition_variable cv;
	    enum Stat stat;
	};//StateMachine	

}//qing
