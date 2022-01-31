#include "../includes/Webserv.hpp"

    server::server(std::vector<ServerData> __data):_fdNum(0), _data(__data), req(__data), _queue(req)
    {
        _addrlen = sizeof(_address);
    }

    std::string server::readReq(int fd, int *n){
        int res = 0;
        int i = *n;
        char buff[1024] = {0};
        std::string ret;
        res = recv(fd, buff, sizeof(buff) - 1, 0);
        // res = read(fd, buff, sizeof(buff) - 1);
        // buff[res] = '\0';
        if (res == 0){
            std::cout << "---------------- Close Connetion at read" << "----------------"  << std::endl;
            this->deleteS(i);
                    // close(_pollfd_list[i].fd);
                    // _pollfd_list.erase(_pollfd_list.begin() + i);
                    // _fdNum--;
            *n = 0;
        }
        else if (res < 0){
            *n = -1;
            throw Socketexeption("cant read Req");
        }
        else{
            *n = res;
            ret = buff;
            // while (res > 0)
            // {
            //     if (res < 1024)
            //         break;
            //     res = recv(fd, buff, sizeof(buff) - 1, 0);
            //     if (res > 0)
            //         ret += buff;
            // }
        }
        return ret;
    }

    void    server::initial_sockets(){
        for(int i = 0; i < _data.size();i++){
            _socket_list.push_back(__socket());
            if (_socket_list[i].init(_data[i].getPort(), _data[i].getHost()) == -1)
                continue ;
            if (_socket_list[i].listen_socket() < 0){
                std::cerr << " Error : skiping server -listen- " << _data[i].getHost() << ":" << _data[i].getPort() << std::endl;
                shutdown(_socket_list[i].getsocket(), SHUT_RDWR);
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
            // add file descriptor to queue
            // EV_SET(&listning_event, master_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
            // add event to queue
            // if (kevent(kq, &listning_event, 1, NULL, 0, NULL) < 0){
            //     throw Socketexeption("add to queue");
            // }
            this->_fdNum++;
            ret++;
            std::cout << "server  " << evPoll.fd << std::endl;
        }
        _queue.set_masterFdlist(master_fds);
        return ret;
    };
    void    server::init(){
        // struct kevent listning_event;
        // struct kevent ch_event;
        int     kq;
        int     sockId;
        int     n = 0;
        int     rc;
        int     new_socket;
        std::string req_string;
        struct pollfd   evPoll;

        // create kqueue
        // kq = kqueue();
        initial_sockets();
        if (pollList() == 0)
        {
            std::cerr << "fatal Error : Cannot creat any vertual server";
            exit(EXIT_FAILURE);
        }
        std::cout << "Web Server Started" << std::endl;
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
        //         if (_pollfd_list[i].revents & POLLOUT){
        //             std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
        //             int sen = send(_pollfd_list[i].fd, hello.c_str(), hello.length(), 0);
        //             std::cout  << " hello sent " << std::endl;
        //             std::cout <<"conection header "<<req.getHeaderOf("Connection").first << std::endl;
        //             if (req.getHeaderOf("Connection").first == false || (req.getHeaderOf("Connection").first == true && req.getHeaderOf("Connection").second->second == "close")){
        //                 this->deleteS(i);
        //                 continue;
        //             }
        //             _pollfd_list[i].events = POLLIN;
        //             _pollfd_list[i].revents = 0;
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