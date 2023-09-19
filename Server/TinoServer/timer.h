#pragma once
#include "stdafx.h"

constexpr int EVENTBUFSIZE = 100;

enum EVENT_TYPE { EV_MOVE, EV_ATTACK };

struct Timer_event {
	int key;
	std::chrono::system_clock::time_point start_time;
	EVENT_TYPE OE_Type;
	char event_message[EVENTBUFSIZE + 1];

	constexpr bool operator< (const Timer_event& other) const {
		return start_time > other.start_time;
	}
};

class Timer {
public:
	std::priority_queue<Timer_event> m_Timer_queue;

	void init(HANDLE h_cp);
	void push_event(int key, EVENT_TYPE event_type, int delaystart_time, char* message);
	void Timer_main();

	void Set_isRun(bool value) { m_isRun = value; }

private:
	int m_isRun;
	HANDLE m_hiocp;
	std::mutex m_timer_lock;
};