#include "timer.h"

using namespace std::chrono;

void Timer::init(HANDLE h_cp)
{
	m_hiocp = h_cp;

	std::priority_queue<Timer_event> empty_queue;
	std::swap(m_Timer_queue, empty_queue);

	m_isRun = TRUE;
}

void Timer::push_event(int key, EVENT_TYPE event_type, int delaytime_ms, char* message)
{
	std::lock_guard <std::mutex> lg{ m_timer_lock };
	Timer_event te;
	te.key = key;
	te.OE_Type = event_type;
	te.start_time = system_clock::now() + milliseconds(delaytime_ms);
	memcpy(te.event_message, message, message[0]);
	m_Timer_queue.push(te);
}

void Timer::Timer_main()
{
	while (m_isRun) {
		m_timer_lock.lock();
		if ((m_Timer_queue.empty() == FALSE)
			&& (m_Timer_queue.top().start_time <= system_clock::now())) {
			Timer_event te = m_Timer_queue.top();
			m_Timer_queue.pop();
			m_timer_lock.unlock();

			OVER_EXP* over_exp = new OVER_EXP;
			over_exp->target_id = te.key;
			over_exp->_comp_type = OP_EVENT;
			memcpy(over_exp->_send_buf, reinterpret_cast<char*>(&te), sizeof(te));

			PostQueuedCompletionStatus(m_hiocp, 1, te.key, (LPOVERLAPPED)&over_exp->_over);
		}
		else {
			m_timer_lock.unlock();
			std::this_thread::sleep_for(100ms);
		}
	}
}