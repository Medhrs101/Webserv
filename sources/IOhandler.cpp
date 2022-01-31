#include "../includes/Webserv.hpp"

// IOhandler::IOhandler():_fdNum(0){};

IOhandler::IOhandler(request &req):_fdNum(0), _req(req){
        _addrlen = sizeof(_address);
};

void    IOhandler::addPollFd(struct pollfd &pollfd){
    this->_pollfd_list.push_back(pollfd);
}
void    IOhandler::set_pollList(std::vector<struct pollfd> &pollList){
    this->_pollfd_list = pollList;
};

void    IOhandler::set_masterFdlist(std::vector<int> &list){
    this->_master_fds = list;
    _fdNum = _master_fds.size();
};

void    IOhandler::inputEvent(int fd, int index){
    int     new_socket;
    int     ret = 0;
    std::string req_string;
    struct pollfd   evPoll;

    if (std::find(_master_fds.begin(), _master_fds.end(), fd) != _master_fds.end()){
        new_socket = accept(fd, (struct sockaddr *) &_address, (socklen_t *) &_addrlen);
        if (new_socket < 0){
            perror("vv  ");
            throw Socketexeption("accept");
        }
        fcntl(new_socket, F_SETFL, O_NONBLOCK);
        std::cout << "new connecticon " << new_socket << "----------------"  << std::endl;
        evPoll.fd = new_socket;
        evPoll.events = POLLIN;
        evPoll.revents = 0;
        _pollfd_list.push_back(evPoll);
        _fdNum++;
    }
    else{
        req_string = readReq(fd, &ret);
        if (ret == 0)
            return ;
        if (ret > 0){
            _req.initialize();
            _req.requestParser(req_string);
            _pollfd_list[index].events = POLLIN | POLLOUT;
        }
    }
};

void    IOhandler::outputEvent(int fd, int index){
    std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    int sen = send(fd, hello.c_str(), hello.length(), 0);
    std::cout  << " hello sent " << std::endl;
    std::cout <<"conection header "<< _req.getHeaderOf("Connection").first << std::endl;
    if (_req.getHeaderOf("Connection").first == false || (_req.getHeaderOf("Connection").first == true && _req.getHeaderOf("Connection").second->second == "close")){
        this->deleteS(index);
        return;
    }
    _pollfd_list[index].events = POLLIN;
    _pollfd_list[index].revents = 0;
};

void    IOhandler::deleteS(int index){
    close(_pollfd_list[index].fd);
    _pollfd_list.erase(_pollfd_list.begin() + index);
    _fdNum--;
};

void    IOhandler::IOwatch(){
    int     ret = 0;
    int     size = 0;
    int     fd = -1;
    while (true)
    {
            std::cout << "im here \n";
        ret = poll(&(_pollfd_list[0]), _fdNum, -1);
        if (ret < 0){
            std::cerr << "FATAL error -poll-: " << strerror(errno);
        }
        size = _fdNum;
        for (int i = 0; i < size; i++)
        {
            if (_pollfd_list[i].revents == 0)
                continue ;
            fd = _pollfd_list[i].fd;
            if (_pollfd_list[i].revents & POLLIN){
                inputEvent(fd, i);
            }
            if (_pollfd_list[i].revents & POLLOUT){
                //TODO: write to fd
                outputEvent(fd, i);
            }
            if (_pollfd_list[i].revents & POLLHUP){
                //TODO: Connectio to be close;
                this->deleteS(i);
            }
        }
    }
};
IOhandler::~IOhandler(){};

 std::string IOhandler::readReq(int fd, int *n){
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