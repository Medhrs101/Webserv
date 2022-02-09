#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "Webserv.hpp"
class queue{
    private:
        int     _fd;
        request _req;
        int     _contentLent;
        int     _contentSent;
    public:
        queue();
        queue    initQueueElm(int  fd, request req);
        int     getFD() const;
        request     getReq() const;
        int         getReqLent() const;
        int         &getReqSent();
        void        updateReqSent(int size);
        void    setReq(request &_req);
        ~queue();
};

#endif /* QUEUE_HPP */
