#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/socket.h>
#include <netinet/in.h>
// #include <sys/epoll.h>
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>
#include <exception>
#include "../includes/Webserv.hpp"

    class Socketexeption : public std::exception{
        private:
            std::string _msg;
        public:
            Socketexeption(std::string msg):_msg(msg){};
            const char * what () const throw (){
                return _msg.c_str();
            }
            virtual ~Socketexeption() _NOEXCEPT{}
    };
    class __socket
    {
        public:
        private:
            int _sockFD;
            struct sockaddr_in _address;
            size_t     _addrlen;
        public:
            __socket();
            int     init(int port, std::string host);
            void bindSock(int port, std::string interface);
            int    listen_socket();
            int const   &getsocket() const;
            struct sockaddr_in  getAddress();
            ~__socket();
    };


#endif
