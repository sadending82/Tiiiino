#pragma once

class TinoThread
{
public:
	virtual ~TinoThread();


	virtual void doThread() = 0;
};

