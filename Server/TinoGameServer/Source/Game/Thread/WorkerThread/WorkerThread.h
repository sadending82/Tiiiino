#pragma once
#include "../TinoThread.h"
class WorkerThread :public TinoThread
{
public:
	WorkerThread(class MainServer* Server);
	~WorkerThread();


	virtual void doThread() override;


private:
	class MainServer* mMainServer;	//Weak
};

