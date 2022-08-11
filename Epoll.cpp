#include "Epoll.h"
#include "base/Logging.h"
#include <memory>
#include <errno.h>
#include <cstdio>

const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;

typedef std::shared_ptr<Channel> SP_Channel;

Epoll::Epoll() : epollFd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTSNUM) {}
Epoll::~Epoll() {}
void Epoll::epoll_add(SP_Channel request, int timeout)
{
    int fd = request->getFd();
    if (timeout > 0)
    {
        add_timer(request, timeout);
        fd2http_[fd] = request->getHolder();
    }
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();
    request->EqualAndUpdaateLastEvents();
    fd2chan_[fd] = request;
    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        perror("epoll_add error");
        fd2chan_[fd].reset();
    }
}
void Epoll::epoll_mod(SP_Channel request, int timeout)
{
    if (timeout > 0)
        add_timer(request, timeout);
    int fd = request->getFd();
    if (!request->EqualAndUpdaateLastEvents())
    {
        struct epoll_event event;
        event.data.fd = fd;
        event.events = request->getEvents();
        if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0)
        {
            perror("epoll_add error");
            fd2chan_[fd].reset();
        }
    }
}
void Epoll::epoll_del(SP_Channel request)
{
    int fd = request->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getLastEvents();
    if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0)
    {
        perror("epoll_del error");
    }
    fd2chan_[fd].reset();
    fd2http_[fd].reset();
}
std::vector<SP_Channel> Epoll::poll()
{
    while (true)
    {
        int event_count = epoll_wait(epollFd_, &*events_.begin(), events_.size(), EPOLLWAIT_TIME);
        if (event_count < 0)
            perror("epoll wait error");
        std::vector<SP_Channel> req_data = getEventsRequest(event_count);
        if (req_data.size() > 0)
            return req_data;
    }
}
void Epoll::handleExpired()
{
    timerManager_.handleExpiredEvent();
}
std::vector<SP_Channel> Epoll::getEventsRequest(int events_num)
{
    std::vector<SP_Channel> req_data;
    for (int i = 0; i < events_num; ++i)
    {
        int fd = events_[i].data.fd;
        SP_Channel cur_req = fd2chan_[fd];
        if (cur_req)
        {
            cur_req->setRevents(events_[i].events);
            cur_req->setEvents(0);
            req_data.push_back(cur_req);
        }
        else
        {
            LOG << "SP cur_req is invalid";
        }
    }
    return req_data;
}
void Epoll::add_timer(SP_Channel request_data, int timeout)
{
    std::shared_ptr<HttpData> t = request_data->getHolder();
    if (t)
        timerManager_.addTimer(t, timeout);
    else
        LOG << "timer add fail";
}