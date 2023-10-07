#pragma once
#include "helper/Poller.h"
#include <memory>
#include <queue>
#include <mutex>
class Scheduler {

public:

	Scheduler() {
		EpollPoller *epoll_poller = new EpollPoller();
		poller_ = std::unique_ptr<Poller>(epoll_poller);
	}
	~Scheduler() {}

	bool addTriggerEvent(std::shared_ptr<TriggerEvent> event) { 
		std::lock_guard<std::mutex> guard(latch_);
		trigger_events_.push(event);
		return true;
	}

	bool addIOEvent(std::shared_ptr<IOEvent> event){return poller_->AddIOEvent(event); }// reentrant
	bool updateIOEvent(std::shared_ptr<IOEvent> event){ return poller_->UpdateIOEvent(event);}
	bool removeIOEvent(std::shared_ptr<IOEvent> event) { return poller_->RemoveIOEvent(event); }

	void loop() { 

		while (1) {
			handleTriggerEvent();
			poller_->Poll();
		}
	}

private:
	void handleTriggerEvent() {
		std::lock_guard<std::mutex> guard(latch_);
		while (!trigger_events_.empty()) {
			std::shared_ptr<TriggerEvent> event = trigger_events_.front();
			trigger_events_.pop();
			event->handleEvent();
		}
	}

	std::mutex latch_;

	std::unique_ptr<Poller> poller_;
	std::queue<std::shared_ptr<TriggerEvent>> trigger_events_;

	


};
