#include "helper/Poller.h"
#include "helper/LOG.h"
#include <unistd.h>



EpollPoller::EpollPoller() :thread_pool_(8), epoll_fd_(epoll_create1(EPOLL_CLOEXEC)), events_(16) {
	if (epoll_fd_ < 0) {
		LOG_ERROR("epoll_create error");
	}

}

EpollPoller::~EpollPoller() {
	close(epoll_fd_);
}

bool EpollPoller::AddIOEvent(std::shared_ptr<IOEvent> io_event) {
	
	int fd = io_event->getFd();
	struct epoll_event event;

	if (io_event->isReadHandling()) {
		event.events |= EPOLLIN;
	}
	if (io_event->isWriteHandling()) {
		event.events |= EPOLLOUT;
	}
	if(io_event->isErrorHandling()) {
		event.events |= EPOLLERR;
	}
	event.data.fd = fd;
	event.events |= EPOLLET;
	int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);
	if(ret < 0) {
		LOG_ERROR("epoll_ctl error");
		return false;
	}
	//latch_.lock();
	fd2event_[fd] = io_event;
	//latch_.unlock();
	return true;
}

bool EpollPoller::RemoveIOEvent(std::shared_ptr<IOEvent> io_event) {
	int fd = io_event->getFd();
	struct epoll_event event;
	event.data.fd = fd;
	int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &event);
	if(ret < 0) {
		LOG_ERROR("epoll_ctl error");
		return false;
	}
	//latch_.lock();
	fd2event_.erase(fd);
	//latch_.unlock();
	return true;
}

bool EpollPoller::UpdateIOEvent(std::shared_ptr<IOEvent> io_event) {
	int fd = io_event->getFd();
	struct epoll_event event;
	event.data.fd = fd;
	if (io_event->isReadHandling()) {
		event.events |= EPOLLIN;
	}
	if (io_event->isWriteHandling()) {
		event.events |= EPOLLOUT;
	}
	if(io_event->isErrorHandling()) {
		event.events |= EPOLLERR;
	}
	event.data.fd = fd;
	event.events |= EPOLLET;
	int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &event);
	if(ret < 0) {
		LOG_ERROR("epoll_ctl error");
		return false;
	}
	return true;
}



bool EpollPoller::AddTimerEvent(std::shared_ptr<TimerEvent> timer_event) {

	int fd = timer_event->getFd();
	struct epoll_event event;

	event.events |= EPOLLIN;
	event.data.fd = fd;

	int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);
	if (ret < 0) {
		LOG_ERROR("epoll_ctl error");
		return false;
	}
	//latch_.lock();
	fd2timer_[fd] = timer_event;
	//latch_.unlock();
	return true;

}

bool EpollPoller::RemoveTimerEvent(std::shared_ptr<TimerEvent> timer_event) {
	int fd = timer_event->getFd();
	struct epoll_event event;
	event.data.fd = fd;
	int ret = epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &event);
	if (ret < 0) {
		LOG_ERROR("epoll_ctl error");
		return false;
	}
	//latch_.lock();
	fd2timer_.erase(fd);
	//latch_.unlock();
	return true;
}


void EpollPoller::Poll() {
	int timeout = 1000;

	int cnt = epoll_wait(epoll_fd_, &events_[0], events_.size(), timeout);
	if (cnt < 0) {
		LOG_ERROR("epoll_wait error");
		return;
	}
	else if (cnt == 0) {
		LOG_INFO("epoll_wait timeout");
		return;
	}
	else {
		//LOG_INFO("epoll_wait return %d", cnt);
		
	}
	//latch_.lock();
	for (int i = 0; i < cnt; ++i) {
		int fd = events_[i].data.fd;

		auto iter = fd2event_.find(fd);
		
		if (iter != fd2event_.end()) {
			std::shared_ptr<IOEvent> event = iter->second;
			
			LOG_INFO("handle event");
			event->handleEvent();
			
			
		}
		else {
			auto iter_timer = fd2timer_.find(fd);
			uint64_t buf;
			read(fd, &buf, sizeof(buf));
			std::shared_ptr<TimerEvent> event = iter_timer->second;
			
			//LOG_INFO("handle timer");
			event->handleEvent();
				
		}
	}
	if ((int)events_.size() == cnt) {
		events_.resize(events_.size() * 2);
	}
	//latch_.unlock();

}