#ifndef IOHANDLER_HPP
#define IOHANDLER_HPP
#include  "Webserv.hpp"

class IOhandler{
    private:
        std::vector<ServerData>     _data;
		bool						_close;
        std::vector<int>            _master_fds;
        std::vector<struct pollfd>  _pollfd_list;
        int                         _fdNum;
        struct sockaddr_in         _address;
        size_t                      _addrlen;
        std::vector<queue>          _reqQueue;
        size_t                      _outSize;
        bool                        _busy;
    public:
        // IOhandler();
        IOhandler(std::vector<ServerData>     data);
        void    addPollFd(struct pollfd &pollfd);
        void    set_pollList(std::vector<struct pollfd> &pollList);
        void    set_masterFdlist(std::vector<int> &list);
        void    newElem(int fd);
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
