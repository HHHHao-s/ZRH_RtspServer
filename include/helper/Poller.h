#pragma once
#include <unordered_map>
#include "helper/Event.h"
#include "helper/ThreadPool.h"
#include <memory>
#include <sys/epoll.h>
#include <mutex>
#include <vector>



class Poller
{
public:

	Poller() {}
	virtual ~Poller() {}
	virtual bool AddIOEvent(std::shared_ptr<IOEvent> io_event) = 0;
	virtual bool RemoveIOEvent(std::shared_ptr<IOEvent> io_event) = 0;
	virtual bool UpdateIOEvent(std::shared_ptr<IOEvent> io_event) = 0;
	virtual bool AddTimerEvent(std::shared_ptr<TimerEvent> timer_event) = 0;
	virtual bool RemoveTimerEvent(std::shared_ptr<TimerEvent> timer_event) = 0;
	virtual void Poll() = 0;

protected:

	std::mutex latch_;

	std::unordered_map<int, std::shared_ptr<IOEvent>> fd2event_;
	std::unordered_map<int, std::shared_ptr<TimerEvent>> fd2timer_;


};


class EpollPoller : public Poller
{
public:

	EpollPoller();
	virtual ~EpollPoller();
	virtual bool AddIOEvent(std::shared_ptr<IOEvent> io_event);
	virtual bool RemoveIOEvent(std::shared_ptr<IOEvent> io_event);
	virtual bool UpdateIOEvent(std::shared_ptr<IOEvent> io_event);
	virtual bool AddTimerEvent(std::shared_ptr<TimerEvent> timer_event);
	virtual bool RemoveTimerEvent(std::shared_ptr<TimerEvent> timer_event);
	virtual void Poll();


private:

	ThreadPool thread_pool_;

	int epoll_fd_;
	std::vector<struct epoll_event> events_;
};

