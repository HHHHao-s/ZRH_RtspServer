#pragma once
#include "helper/Poller.h"
#include <memory>
#include <queue>
#include <mutex>
#include <map>
typedef uint64_t timer_id_t;
class RtspContext;
class Scheduler;


class TimerManager {

	friend Scheduler;
public:
	TimerManager() {}
	~TimerManager() {}

private:
	
	

	std::shared_ptr<TimerEvent> GetTimer(timer_id_t timer_id);

	timer_id_t StartTimer(EventCallback cb, void* arg, uint64_t time_ms);

	bool StopTimer(timer_id_t timer_id);
	std::mutex latch_;
	std::map<timer_id_t, std::shared_ptr<TimerEvent>> timer_events_;
	timer_id_t cur_timer_id_{ 0 };

};


class Scheduler {

public:

	Scheduler() {
		Poller * p = new EpollPoller();
		poller_ = std::unique_ptr<Poller>(p);
		TimerManager* tp = new TimerManager();
		timer_manager_ = std::unique_ptr<TimerManager>(tp);
	}
	~Scheduler() {}

	bool addTriggerEvent(std::shared_ptr<TriggerEvent> event) { 
		std::lock_guard<std::mutex> guard(latch_);
		trigger_events_.push(event);
		return true;
	}

	bool addIOEvent(std::shared_ptr<IOEvent> event){return poller_->AddIOEvent(event); }// multi-thread
	bool updateIOEvent(std::shared_ptr<IOEvent> event){ return poller_->UpdateIOEvent(event);}
	bool removeIOEvent(std::shared_ptr<IOEvent> event) { return poller_->RemoveIOEvent(event); }
	
	timer_id_t StartTimer(EventCallback cb, void* arg, uint64_t time_ms);

	bool StopTimer(timer_id_t timer_id);
	

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

	std::unique_ptr<TimerManager> timer_manager_;

};
