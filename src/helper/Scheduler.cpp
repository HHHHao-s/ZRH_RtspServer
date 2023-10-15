#include "helper/Scheduler.h"

timer_id_t TimerManager::StartTimer(EventCallback cb, void* arg, uint64_t time_ms) {
	std::lock_guard<std::mutex> guard(latch_);
	timer_id_t timer_id = ++cur_timer_id_;
	std::shared_ptr<TimerEvent> timer_event = std::make_shared<TimerEvent>(cb, arg, time_ms);

	timer_events_[timer_id] = timer_event;



	return timer_id;
}

bool TimerManager::StopTimer(timer_id_t timer_id) {
	std::lock_guard<std::mutex> guard(latch_);
	if (timer_events_.count(timer_id) == 0) {
		return false;
	}
	
	timer_events_[timer_id]->stop();
	timer_events_.erase(timer_id);
	
	return true;
}

std::shared_ptr<TimerEvent> TimerManager::GetTimer(timer_id_t timer_id) {
	std::lock_guard<std::mutex> guard(latch_);
	if (timer_events_.count(timer_id) == 0) {
		return {};
	}
	else {
		return timer_events_[timer_id];
	}
	
}

timer_id_t Scheduler::StartTimer(EventCallback cb, void* arg, uint64_t time_ms) {
	auto timer_id = timer_manager_->StartTimer(cb, arg, time_ms);

	auto timer_event = timer_manager_->GetTimer(timer_id);
	
	if (timer_event.get() == nullptr) {
		return 0;
	}

	poller_->AddTimerEvent(timer_event);

	return timer_id;
}

bool Scheduler::StopTimer(timer_id_t timer_id) {
	auto timer_event = timer_manager_->GetTimer(timer_id);

	if (timer_event.get() == nullptr) {
		return false;
	}

	poller_->RemoveTimerEvent(timer_event);
	timer_manager_->StopTimer(timer_id);
	return true;
}