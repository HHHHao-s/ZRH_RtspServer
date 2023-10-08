#include "helper/Event.h"
#include <stdio.h>
#include "helper/LOG.h"
#include "helper/SocketHelper.h"

TriggerEvent::TriggerEvent(void* arg) : arg_(arg), trigger_call_back_(NULL) {
}
TriggerEvent::~TriggerEvent() {


}

void TriggerEvent::handleEvent() {
    if (trigger_call_back_)
        trigger_call_back_(arg_);
}


TimerEvent::TimerEvent(void* arg,uint64_t time_ms) : arg_(arg),
timeout_call_back_(NULL),
stop_(false),
interval_ms_(time_ms)
{
    time_fd_ = timerfd_create(CLOCK_MONOTONIC,  TFD_CLOEXEC);
    if (time_fd_ < 0) {
        LOG_ERROR("timerfd_create error");
    }



    struct itimerspec ts;
    ts.it_value.tv_sec = 1;// expire after 1 second
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = time_ms * 1000000; // expire every time_ms

    timerfd_settime(time_fd_, 0, &ts, NULL);


}
TimerEvent::~TimerEvent() {
    stop();
}

bool TimerEvent::handleEvent()
{
    if (stop_) {
        return stop_;
    }

    if (timeout_call_back_)
        timeout_call_back_(arg_);

    return stop_;
}
void TimerEvent::stop() {
    stop_ = true;
    if (time_fd_ > 0) {
        
        Close(time_fd_);
        time_fd_ = 0;
    }
    
}


IOEvent::IOEvent(int fd, void* arg) :
    fd_(fd),
    arg_(arg),
    event_(EVENT_NONE),
    read_call_back_(NULL),
    write_call_back_(NULL),
    error_call_back_(NULL) {

    LOG_INFO("IOEvent() fd=%d", fd_);
}
IOEvent::~IOEvent() {
    LOG_INFO("~IOEvent() fd=%d", fd_);
}

void IOEvent::handleEvent()
{
    if (read_call_back_ && (event_ & EVENT_READ))
    {
        read_call_back_(arg_);
    }

    if (write_call_back_ && (event_ & EVENT_WRITE))
    {
        write_call_back_(arg_);
    }

    if (error_call_back_ && (event_ & EVENT_ERROR))
    {
        error_call_back_(arg_);
    }
};
