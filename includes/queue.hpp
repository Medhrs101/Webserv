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
        bool    _isDone;
        std::string _reqString;
        queue();
        queue    initQueueElm(int  fd, request req);
        int     getFD() const;
        request     &getReq();
        int         getcontentLent() const;
        void         setcontentLent(int );
        int         &getReqSent();
        int        updateReqSent(int size);
        bool        isBodyDone();
        void    parseReq();
        void    setReq(request &_req);
        void     reqCheack();
        ~queue();
};

#endif /* QUEUE_HPP */
