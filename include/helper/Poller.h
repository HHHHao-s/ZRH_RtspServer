#pragma once
#include <unordered_map>
#include "helper/Event.h"
#include <memory>
#include <sys/epoll.h>
#define MAX_EVENTS 32

class Poller
{
public:
	Poller() {}
	virtual ~Poller() {}
	virtual bool AddIOEvent(std::shared_ptr<IOEvent> io_event) = 0;
	virtual bool RemoveIOEvent(std::shared_ptr<IOEvent> io_event) = 0;
	virtual bool UpdateIOEvent(std::shared_ptr<IOEvent> io_event) = 0;

	virtual void Poll() = 0;

protected:
	std::unordered_map<int, std::shared_ptr<IOEvent>> fd2event_;


};


class EpollPoller : public Poller
{
public:
	EpollPoller();
	virtual ~EpollPoller();
	virtual bool AddIOEvent(std::shared_ptr<IOEvent> io_event);
	virtual bool RemoveIOEvent(std::shared_ptr<IOEvent> io_event);
	virtual bool UpdateIOEvent(std::shared_ptr<IOEvent> io_event);
	virtual void Poll();


private:
	int epoll_fd_;
	struct epoll_event active_events_[MAX_EVENTS]; // epoll_wait返回的活跃事件
};

