#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "Webserv.hpp"
class queue{
    private:
        int     _fd;
        request _req;
        int     _contentLent;
        int     _contentSent;
        size_t     _contentRead;
        std::string _resString;
        bool        _isChunked;
    public:
        bool    _isDone;
        std::string _reqString;
        queue(std::vector<ServerData>);
        queue    initQueueElm(int  fd, request req);
        int     getFD() const;
        void     setFD(int fd);
        request     &getReq();
        int         getcontentLent() const;
        size_t         getcontentRead() const;
        void         setcontentLent(int );
        void         setcontentRead(size_t );
        int         &getReqSent();
        const std::string &getResponse() const;
        int        updateReqSent(int size);
        bool        isBodyDone();
        void    parseReq();
        void    setReq(request &_req);
        void     reqCheack();
        bool    isChunked(void);
        bool    isDone(void);
        void    chunkParser();
        void    setcontentSent(int size);
        ~queue();
};

#endif /* QUEUE_HPP */
