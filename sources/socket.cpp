#include "../includes/Webserv.hpp"

    __socket::__socket(struct sockaddr_in &address):_address(address){
        // if ((_sockFD = socket(AF_INET, SOCK_STREAM,0)) < 0){
        //     // std::cerr << "Error at creation :" << strerror(errno);
        //     throw Socketexeption("socket Creation");
        // }
        // _addrlen = sizeof(_address);
    };
    int     __socket::init(int port, std::string host){
        if ((_sockFD = socket(AF_INET, SOCK_STREAM,0)) < 0){
            // std::cerr << "Error at creation :" << strerror(errno);
            throw Socketexeption("socket Creation");
        }
        _addrlen = sizeof(_address);
        bindSock(port, host);
        return  _sockFD;
    }
    void    __socket::bindSock(int port, std::string interface){
        int opt = true;
        _address.sin_family = AF_INET;
        _address.sin_port = htons(port);
        // _address.sin_addr.s_addr = inet_addr(interface.c_str());
        _address.sin_addr.s_addr = htonl(INADDR_ANY);
        setsockopt(_sockFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        int res = bind(_sockFD, (struct sockaddr *)&_address, _addrlen);
        if (res < 0){
            throw Socketexeption(strerror(errno));
            // throw Socketexeption("socket Bind");
        }
    }
    int const &__socket::getsocket() const{
        return _sockFD;
    };
    __socket::~__socket(){};  