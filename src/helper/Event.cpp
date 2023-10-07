#include "helper/Event.h"
#include <stdio.h>
#include "helper/LOG.h"

TriggerEvent::TriggerEvent(void* arg) : arg_(arg), trigger_call_back_(NULL) {
}
TriggerEvent::~TriggerEvent() {


}

void TriggerEvent::handleEvent() {
    if (trigger_call_back_)
        trigger_call_back_(arg_);
}


TimerEvent::TimerEvent(void* arg) : arg_(arg),
timeout_call_back_(NULL),
stop_(false) {

}
TimerEvent::~TimerEvent() {

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
