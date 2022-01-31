#ifndef IOHANDLER_HPP
#define IOHANDLER_HPP
#include  "Webserv.hpp"

class IOhandler{
    private:
            std::vector<int>            _master_fds;
            std::vector<struct pollfd>  _pollfd_list;
            int                         _fdNum;
            struct sockaddr_in         _address;
            size_t                      _addrlen;
            request                     &_req;
    public:
        // IOhandler();
        IOhandler(request &req);
        void    addPollFd(struct pollfd &pollfd);
        void    set_pollList(std::vector<struct pollfd> &pollList);
        void    set_masterFdlist(std::vector<int> &list);
        void    IOwatch();
        void    inputEvent(int fd, int index);
        void    outputEvent(int fd, int index);
        std::string    readReq(int fd, int *n);
        void    deleteS(int index);
        ~IOhandler();
};

#endif
