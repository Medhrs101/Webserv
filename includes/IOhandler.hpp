#ifndef IOHANDLER_HPP
#define IOHANDLER_HPP
#include  "Webserv.hpp"

class IOhandler{
    private:
		bool						_close;
        std::vector<int>            _master_fds;
        std::vector<struct pollfd>  _pollfd_list;
        int                         _fdNum;
        struct sockaddr_in         _address;
        size_t                      _addrlen;
        request                     &_req;
        std::vector<queue>          _reqQueue;
        size_t                      _outSize;
        bool                        _busy;
    public:
        // IOhandler();
        IOhandler(request &req);
        void    addPollFd(struct pollfd &pollfd);
        void    set_pollList(std::vector<struct pollfd> &pollList);
        void    set_masterFdlist(std::vector<int> &list);
        void    newElem();
        void    addToQueue(queue elm);
        void    removeQueue(int fd);
        void    IOwatch();
        void    inputEvent(int fd, int index);
        void    outputEvent(int fd, int index);
        std::string    readReq(int fd, int *n);
        int     reqCheack(std::string req, queue &que);
        // queue   &operator[](int n);
        std::vector<queue>::iterator   operator[](int n);
        void    deleteS(int index);
        ~IOhandler();
};

#endif
