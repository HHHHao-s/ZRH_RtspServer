#pragma once
#include <sys/timerfd.h>
#include <stdint.h>

typedef void (*EventCallback)(void*);

class TriggerEvent {
public:


    TriggerEvent(void* arg);
    ~TriggerEvent();

    void setArg(void* arg) { arg_ = arg; }
    void setTriggerCallback(EventCallback cb) { trigger_call_back_ = cb; }
    void handleEvent();

private:
    void* arg_;
    EventCallback trigger_call_back_;
};

class TimerEvent {
public:

    TimerEvent(void* arg, uint64_t time_ms);
    ~TimerEvent();
    int getFd() const { return time_fd_; }
    void setArg(void* arg) { arg_ = arg; }
    void setTimeoutCallback(EventCallback cb) { timeout_call_back_ = cb; }
    bool handleEvent();

    void stop();

private:
    
    void* arg_;
    EventCallback timeout_call_back_;
    bool stop_;
    uint64_t interval_ms_;
    int time_fd_;
    
};

class IOEvent {
public:
    enum IOEventType
    {
        EVENT_NONE = 0,
        EVENT_READ = 1,
        EVENT_WRITE = 2,
        EVENT_ERROR = 4,
    };


    IOEvent(int fd, void* arg);
    ~IOEvent();

    int getFd() const { return fd_; }
    int getEvent() const { return event_; }
    //void setREvent(int event) { r_event_ = event; }
    void setArg(void* arg) { arg_ = arg; }

    void setReadCallback(EventCallback cb) { read_call_back_ = cb; };
    void setWriteCallback(EventCallback cb) { write_call_back_ = cb; };
    void setErrorCallback(EventCallback cb) { error_call_back_ = cb; };

    void enableReadHandling() { event_ |= EVENT_READ; }
    void enableWriteHandling() { event_ |= EVENT_WRITE; }
    void enableErrorHandling() { event_ |= EVENT_ERROR; }
    void disableReadeHandling() { event_ &= ~EVENT_READ; }
    void disableWriteHandling() { event_ &= ~EVENT_WRITE; }
    void disableErrorHandling() { event_ &= ~EVENT_ERROR; }

    bool isNoneHandling() const { return event_ == EVENT_NONE; }
    bool isReadHandling() const { return (event_ & EVENT_READ) != 0; }
    bool isWriteHandling() const { return (event_ & EVENT_WRITE) != 0; }
    bool isErrorHandling() const { return (event_ & EVENT_ERROR) != 0; };

    void handleEvent();

private:
    int fd_;
    void* arg_;
    int event_;
    //int r_event_;
    EventCallback read_call_back_;
    EventCallback write_call_back_;
    EventCallback error_call_back_;
};