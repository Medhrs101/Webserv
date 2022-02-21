#include "../includes/Webserv.hpp"

    server::server(std::vector<ServerData> __data):_fdNum(0), _data(__data), req(__data), _queue(_data)
    {
        _addrlen = sizeof(_address);
    }


    void    server::initial_sockets(){
        for(int i = 0; i < _data.size();i++){
            _socket_list.push_back(__socket());
            if (_socket_list[i].init(_data[i].getPort(), _data[i].getHost()) == -1)
                continue ;
            if (_socket_list[i].listen_socket() < 0){
                std::cerr <<  RED << " Error : skiping server -listen- " << _data[i].getHost() << ":" << _data[i].getPort() << RESET <<  std::endl;
                shutdown(_socket_list[i].getsocket(), SHUT_RDWR);
            }
            else{
                std::cout <<  CYN << "LISTENING ON  " << _data[i].getHost() << _data[i].getPort() << std::endl;
            }
        }
    };

    int    server::pollList(){
        int     sockId = 0;
        int     ret = 0;
        struct pollfd   evPoll;

        for (int i = 0; i != _socket_list.size(); i++)
        {
            sockId = _socket_list[i].getsocket();
            if (sockId == -1)
                continue ;
            master_fds.push_back(sockId);
            evPoll.fd = sockId;
            evPoll.events = POLLIN;
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
        int     kq;
        int     sockId;
        int     n = 0;
        int     rc;
        int     new_socket;
        std::string req_string;
        struct pollfd   evPoll;

        std::cout << GREEN << "[>>>>>>>>> Web Server Started <<<<<<<<<]" << RESET << std::endl;
        initial_sockets();
        if (pollList() == 0)
        {
            std::cerr << RED << "fatal Error : Cannot creat any vertual server" << RESET << std::endl;
            exit(EXIT_FAILURE);
        }
        _queue.IOwatch();
        // while (true)
        // {
        //     std::cout << "---------------- waiting for EVENT ----------------" << std::endl;
        //     rc = poll(&(_pollfd_list[0]), _fdNum, -1);
        //     if ( rc == -1)
        //         throw Socketexeption("kevent");
        //     int _size = _fdNum;
        //     for(int i = 0; i < _size; i++){
        //         if (_pollfd_list[i].revents == 0)
		// 		    continue ;
        //         int event_fd = _pollfd_list[i].fd;
        //         if ((_pollfd_list[i].revents & POLLERR) || (_pollfd_list[i].revents & POLLNVAL)){
        //             this->deleteS(i);
        //             continue ;
        //         }
        //         else if (_pollfd_list[i].revents & POLLIN){
        //             if (std::find(master_fds.begin(), master_fds.end(), _pollfd_list[i].fd) != master_fds.end()){
        //                 new_socket = accept(_pollfd_list[i].fd, (struct sockaddr *) &_address, (socklen_t *) &_addrlen);
        //                 if (new_socket < 0){
        //                     throw Socketexeption("accept");
        //                 }
        //                 fcntl(new_socket, F_SETFL, O_NONBLOCK);
        //                 std::cout << "---------------- new connecticon " <<  new_socket << "----------------"  << std::endl;
        //                 evPoll.fd = new_socket;
        //                 evPoll.events = POLLIN;
        //                 evPoll.revents = 0;
        //                 _pollfd_list.push_back(evPoll);
        //                 _fdNum++;
        //             }
        //             else{
        //                 n = i;
        //                 req_string = readReq(_pollfd_list[i].fd, &n);
        //                 if (n == 0)
        //                     continue;
        //                 if (n > 0){
        //                     req.initialize();
        //                     req.requestParser(req_string);
        //                     _pollfd_list[i].events = POLLIN | POLLOUT;
        //                 }
        //             }
        //         }
        //     }
        // }
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