#include "helper/Poller.h"
#include "helper/LOG.h"
#include <unistd.h>

EpollPoller::EpollPoller() : epoll_fd_(epoll_create1(EPOLL_CLOEXEC)){
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
	fd2event_[fd] = io_event;
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
	fd2event_.erase(fd);
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

void EpollPoller::Poll() {
	int timeout = 1000;

	int ret = epoll_wait(epoll_fd_, active_events_, MAX_EVENTS, timeout);
	if (ret < 0) {
		LOG_ERROR("epoll_wait error");
	}
	else if (ret == 0) {
		LOG_INFO("epoll_wait timeout");
	}
	else {
		LOG_INFO("epoll_wait return %d", ret);
	}
	for (int i = 0; i < ret; ++i) {
		int fd = active_events_[i].data.fd;
		auto iter = fd2event_.find(fd);
		if (iter != fd2event_.end()) {
			iter->second->handleEvent();
		}
	}

}