#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP
// #include "socket.hpp"
// #include "../includes/Webserv.hpp"
#include <sys/ioctl.h>
// #include <sys/event.h>
    class server{
        private:
            std::vector<ServerData> _data;
            std::vector<__socket>   _socket_list;
            struct sockaddr_in      _address;
            size_t                  _addrlen;
            std::vector<int>        master_fds;
            request     req;
            IOhandler   _queue;
            std::vector<struct pollfd>  _pollfd_list;
            int                         _fdNum;
            int     _poll;
        public:
            server(std::vector<ServerData> __data);
            void    init();
            void    initial_sockets();
            int     pollList();
            void    deleteS(int );
            char    *req_string();
            std::string readReq(int fd, int *ret);
            ~server();
    };
#endif
