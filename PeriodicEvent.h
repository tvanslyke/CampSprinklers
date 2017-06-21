/*
 * PeriodicEvent.h
 *
 *  Created on: Jun 19, 2017
 *      Author: tim
 */

#ifndef PERIODICEVENT_H_
#define PERIODICEVENT_H_
#include <chrono>
#include <mutex>
#include <thread>
template <class F>
class PeriodicEvent {
public:
	using clock_t = typename std::chrono::system_clock;
	using time_point = typename clock_t::time_point;
	using duration = typename clock_t::duration;
	using mutex_t = std::mutex;
	using lock_t = std::lock_guard<mutex_t>;
	PeriodicEvent(const F & event, duration period):
		event_{event}, period_{period}, next_event_{clock_t::now() + period_}
	{

	}

	void wait()
	{
		lock_t lock(event_mutex_);
		while(next_event_ > clock_t::now())
		{
			std::this_thread::sleep_for(next_event_ - clock_t::now());
		}
	}

	void reset_event_time()
	{
		lock_t lock(event_mutex_);
		next_event_ += period_;
	}

	template <class ... T>
	decltype(auto) event(T && ... args)
	{
		lock_t lock(event_mutex_);
		return event_(std::forward<T>(args) ...);
	}

	template <class ... T>
	void run_continuously(T ... args)
	{
		while(true)
		{
			wait();
			reset_event_time();
			event(args...);
		}
	}
private:
	F event_;
	duration period_;
	time_point next_event_;
	std::mutex event_mutex_;
};

template <class F, class ... T>
auto make_periodic_event(const F & f, T && ... args)
{
	return PeriodicEvent<F>(f, std::forward<T>(args)...);
}


#endif /* PERIODICEVENT_H_ */
