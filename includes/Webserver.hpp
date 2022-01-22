#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP
#include "socket.hpp"
#include "../includes/Webserv.hpp"
// #include <sys/event.h>

    class server{
        private:
            std::vector<ServerData> _data;
            std::vector<__socket> _socket_list;
            struct sockaddr_in _address;
            size_t      _addrlen;
            std::vector<int>    master_fds;
            request     req;
            // std::vector<struct epoll_event> _event_list TODO: event list
            struct epoll_event _event;
            int     _poll;
        public:
            server(std::vector<ServerData> __data);
            void    init();
            void    initial_sockets();
            char    *req_string();
            std::string readReq(int fd, size_t _size);
            ~server();
    };
#endif
