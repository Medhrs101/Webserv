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
        bool        _isChunked;
        bool    _isDone;
        std::string _resString;
        std::string _reqString;
    public:
        queue(std::vector<ServerData>);
        void    reset();
        queue    initQueueElm(int  fd, request req);
        
        void    reqCheack();
        bool    isChunked(void);
        bool    isDone(void);
        void    chunkParser();

        void    setcontentSent(int size);
        void	setcontentLent(int );
        void	setcontentRead(size_t );

        const std::string	&getResponse() const;
		std::string			&getReqString();
		void    appendReq(std::string &str);
        request &getReq();
        int     getFD() const;
        void    setFD(int fd);
        int     getcontentLent() const;
        size_t	getcontentRead() const;
        int		&getReqSent();
        int		updateReqSent(int size);
        bool	isBodyDone();
        void    parseReq();
        void    setReq(request &_req);
        ~queue();
};

#endif /* QUEUE_HPP */
