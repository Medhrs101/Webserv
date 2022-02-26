#include "../includes/Webserv.hpp"

    server::server(std::vector<ServerData> __data):_data(__data), req(__data), _queue(_data),_fdNum(0)
    {
        _addrlen = sizeof(_address);
    }


    void    server::initial_sockets(){
        std::vector<int> Ports;
        for(size_t i = 0; i < _data.size();i++){
            if (std::find(Ports.begin(),Ports.end(), _data[i].getPort()) != Ports.end())
                continue ;
            _socket_list.push_back(__socket());
            if (_socket_list[i].init(_data[i].getPort(), _data[i].getHost()) == -1)
                continue ;
            if (_socket_list[i].listen_socket() < 0){
                std::cerr <<  RED << " Error : skiping server -listen- " << _data[i].getHost() << ":" << _data[i].getPort() << RESET <<  std::endl;
                shutdown(_socket_list[i].getsocket(), SHUT_RDWR);
            }
            else{
                std::cout <<  CYN << "LISTENING ON  " << _data[i].getHost() << ':' << _data[i].getPort() << std::endl;
                Ports.push_back(_data[i].getPort());
            }
        }
    };

    int    server::pollList(){
        int     sockId = 0;
        int     ret = 0;
        struct pollfd   evPoll;

        for (size_t i = 0; i != _socket_list.size(); i++)
        {
            sockId = _socket_list[i].getsocket();
            if (sockId == -1)
                continue ;
            master_fds.push_back(sockId);
            evPoll.fd = sockId;
            evPoll.events = POLLIN | POLLOUT;
            evPoll.revents = 0;
            _pollfd_list.push_back(evPoll);
            _queue.addPollFd(evPoll);
            this->_fdNum++;
            ret++;
        }
        _queue.set_masterFdlist(master_fds);
        return ret;
    };
    void    server::init(){
        std::string req_string;

        std::cout << GREEN << "\e[1m" << "Web Server Started" << RESET << std::endl;
        initial_sockets();
        if (pollList() == 0)
        {
            std::cerr << RED << "Fatal Error : Cannot creat any Virtual server" << RESET << std::endl;
            exit(EXIT_FAILURE);
        }
        _queue.IOwatch();
    }

    void    server::deleteS(int index){
        close(_pollfd_list[index].fd);
        _pollfd_list.erase(_pollfd_list.begin() + index);
        _fdNum--;
    };
    server::~server(){
        // for (size_t i = 0; i < master_fds.size(); i++)
        // {
        //     std::cout << "closed" << master_fds[i] << std::endl;
        //     close(master_fds[i]);
        // }
    }