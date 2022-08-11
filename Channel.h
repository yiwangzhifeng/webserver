#pragma once
#include <functional>
#include <memory>
#include <sys/epoll.h>
#include "HttpData.h"

class Channel
{
public:
    typedef std::function<void()> CallBack;
    Channel(EventLoop *loop)
        : loop_(loop), events_(0), lastEvents_(0), fd_(0) {}
    Channel(EventLoop *loop, int fd)
        : loop_(loop), fd_(fd), events_(0), lastEvents_(0) {}
    ~Channel() {}
    int getFd() { return fd_; }
    void setFd(int fd) { fd_ = fd; }
    void setHolder(std::shared_ptr<HttpData> holder) { holder_ = holder; }
    std::shared_ptr<HttpData> getHolder()
    {
        std::shared_ptr<HttpData> ret(holder_.lock());
        return ret;
    }
    void setReadHandler(CallBack &&readHndler)
    {
        readHandler_ = readHndler;
    }
    void setWriteHandler(CallBack &&writeHandler)
    {
        writeHandler_ = writeHandler;
    }
    void setErrorHandler(CallBack &&errorHandler)
    {
        errorHandler_ = errorHandler;
    }
    void setConnHandler(CallBack &&connHandler)
    {
        connHandler_ = connHandler;
    }
    void handleEvents()
    {
        events_ = 0;
        if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
        {
            events_ = 0;
            return;
        }
        if (revents_ & EPOLLERR)
        {
            if (errorHandler_)
                errorHandler_();
            events_ = 0;
            return;
        }
        if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
        {
            handleRead();
        }
        if (revents_ & EPOLLOUT)
        {
            handleWrite();
        }
        handleConn();
    }
    void handleRead()
    {
        if (readHandler_)
        {
            readHandler_();
        }
    }
    void handleWrite()
    {
        if (writeHandler_)
        {
            writeHandler_();
        }
    }
    void handleError(int fd, int err_num, std::string short_msg);
    void handleConn()
    {
        if (connHandler_)
        {
            connHandler_;
        }
    }
    void setRevents(__uint32_t ev) { revents_ = ev; }
    void setEvents(__uint32_t ev) { events_ = ev; }
    __uint32_t &getEvents() { return events_; }
    bool EqualAndUpdaateLastEvents()
    {
        bool ret = (lastEvents_ == events_);
        lastEvents_ = events_;
        return ret;
    }
    __uint32_t getLastEvents() { return lastEvents_; }

private:
    EventLoop *loop_;
    int fd_;
    __uint32_t events_;
    __uint32_t revents_;
    __uint32_t lastEvents_;
    std::weak_ptr<HttpData> holder_;
    CallBack readHandler_;
    CallBack writeHandler_;
    CallBack errorHandler_;
    CallBack connHandler_;
};
typedef std::shared_ptr<Channel> SP_Channel;