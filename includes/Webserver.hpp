#ifndef WEBSERV_20COPY_HPP
#define WEBSERV_20COPY_HPP
#include "socket.hpp"
#include "../includes/Webserv.hpp"
#include <sys/event.h>

    class server{
        private:
            typedef int sock_fd;
            std::vector<ServerData> _data;
            __socket    sock;
            struct sockaddr_in _address;
            sock_fd     master_socket;
            size_t      _addrlen;
            std::vector<int>    master_fds;
            request     req;  // TODO:
        public:
            server(std::vector<ServerData> __data);
            void    init();
            char    *req_string();
            std::string readReq(int fd, size_t _size);
            ~server();
    };
#endif /* WEBSERV_20COPY_HPP */
