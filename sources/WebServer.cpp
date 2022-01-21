#include "../includes/Webserv.hpp"
#include <sys/ioctl.h>

    server::server(std::vector<ServerData> __data):sock(_address), _data(__data), req(__data)
    {
        _addrlen = sizeof(_address);
    }

    std::string server::readReq(int fd, size_t _size){
        int res = 0;
        char buff[_size + 1];
        std::string ret;
        res = read(fd, buff, sizeof(buff) - 1);
        buff[res] = '\0';
        if (res == 0){
            std::cout << "---------------- Close Connetion" << "----------------"  << std::endl;
            close(fd);
        }
        else if (res < 0){
            throw Socketexeption("cant read Req");
        }
        else{
            ret = buff;
        }
        return ret;
    }
    void    server::init(){
        // struct kevent listning_event;
        // struct kevent ch_event;
        int     kq;
        int     epollfd, n;
        int     kv; 
        int     new_socket;
        std::string req_string;
        std::vector<int>::iterator it;
        struct epoll_event event;
        // create kqueue
        epollfd = epoll_create1(0);
        // kq = kqueue();
        for (int i = 0; i != _data.size(); i++)
        {
            master_socket = sock.init( _data[i].getPort(), _data[i].getHost());
            
            fcntl(master_socket, F_SETFL, O_NONBLOCK);
            if (listen(master_socket, 3) < 0){
                throw Socketexeption("listen");
            }
            master_fds.push_back(master_socket);
            event.events = EPOLLIN;
            event.data.fd = master_socket;
            // add file descriptor to queue
            // EV_SET(&listning_event, master_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, master_socket, &event) == -1){
                perror("epoll_ctl: listen_sock");
                exit(EXIT_FAILURE);
            }
            // add event to queue
            // if (kevent(kq, &listning_event, 1, NULL, 0, NULL) < 0){
            //     throw Socketexeption("add to queue");
            // }
            std::cout << "server  " << event.data.fd << std::endl;
        }
        
        std::cout << "server listening" << std::endl;


        while (true)
        {
            std::cout << "---------------- waiting for EVENT ----------------" << std::endl;
            // kv = kevent(kq, NULL, 0, &ch_event, 1, NULL);
            kv = epoll_wait(epollfd, &event, 1, -1);
            if ( kv == -1)
                throw Socketexeption("kevent");
            else{
                // int event_fd = ch_event.ident;
                int event_fd = event.data.fd;
                // if (ch_event.flags & EV_EOF){
                //     std::cout << "---------------- Close Connetion" << "----------------"  << std::endl;
                //     close(event_fd);
                // }
                /*else */if ((it = std::find(master_fds.begin(), master_fds.end(), event_fd)) != master_fds.end()){
                         new_socket = accept(*it, (struct sockaddr *) &_address, (socklen_t *) &_addrlen);
                        if (new_socket < 0){
                            throw Socketexeption("accept");
                        }
                        std::cout << "---------------- new connecticon " <<  new_socket << "----------------"  << std::endl;
                        event.events = EPOLLIN | EPOLLET;
                        event.data.fd = new_socket;
                        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, new_socket, &event) == -1) {
                            perror("epoll_ctl: conn_sock");
                            exit(EXIT_FAILURE);
                        }
                        // EV_SET(&listning_event, new_socket, EVFILT_READ, EV_ADD, 0, 0, 0);
                        // if (kevent(kq, &listning_event, 1, NULL, 0, 0) < 0){
                        //     throw Socketexeption("add to queue");
                        // }
                }
                else// if (ch_event.filter & EVFILT_READ)
                {
                        if(ioctl(event_fd, FIONREAD, &n) < 0) { //file descriptor, call, unsigned int
                            perror("ioctl");
                            exit(EXIT_FAILURE);
                        }
                        req_string = readReq(event_fd, n);
                        req.requestParser(req_string);

                        char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
                        send(event_fd, hello, strlen(hello), 0);
                        // close(event_fd);
                        if (req.getBody() == "exit"){
                            break;
                        }
                        // EV_SET(&listning_event, new_socket, EVFILT_READ, EV_DELETE, 0, 0, 0);
                        std::cout << " mesage sent" << std::endl;
                }
            }
        }

        // int client_nb = master_socket;
        // int new_socket;
        // fd_set  working_set = _read_fds;
        // char buff[1024];
        // std::string req;
        // fcntl(master_socket, F_SETFL, O_NONBLOCK);
        // if (listen(master_socket, 3) < 0){
        //     throw Socketexeption("listen");
        // }
        // std::cout << "server listening" << std::endl;
        // FD_ZERO(&_read_fds);
        // FD_SET(master_socket, &_read_fds);

        // while (1)
        // {
        //     working_set = _read_fds;
        //     std::cout << "---------------- waiting for select ----------------" << std::endl;
        //     if (select(client_nb + 1, &working_set, NULL, NULL, NULL) <= 0){
        //         throw Socketexeption("select");
        //     }
        //     for (int i = 0; i < client_nb + 1;i++){
        //         if (FD_ISSET(i, &working_set)){
        //             if (i == master_socket){
        //                  new_socket = accept(master_socket, (struct sockaddr *) &_address, (socklen_t *) &_addrlen);
        //                 if (new_socket < 0){
        //                     throw Socketexeption("accept");
        //                 }
        //                 std::cout << "---------------- new connecticon " <<  new_socket << "----------------"  << std::endl;
        //                 FD_SET(new_socket, &_read_fds);
        //                 if (client_nb < new_socket)
        //                     client_nb = new_socket;
        //             }
        //             else{
        //                 req = readReq(i);
        //                 std::cout << req << std::endl;
        //                 char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
        //                 send(i, hello, strlen(hello), 0);
        //                 std::cout << " mesage sent" << std::endl;
        //             }
        //         }
        //     }
        // }
    }
    server::~server(){
        for (size_t i = 0; i < master_fds.size(); i++)
        {
            std::cout << "closed" << master_fds[i] << std::endl;
            close(master_fds[i]);
        }
    }