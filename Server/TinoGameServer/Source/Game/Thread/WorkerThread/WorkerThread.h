#pragma once
#include "../TinoThread.h"
#include "../../Utility/Utility.h"
class WorkerThread :public TinoThread
{
public:
	WorkerThread(class MainServer* Server);
	virtual ~WorkerThread();


	virtual void doThread() override;

protected:
	void matchTimerType(const eEventType eventType);

private:
	class MainServer* mMainServer;	//Weak
};

