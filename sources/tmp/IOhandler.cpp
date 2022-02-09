#include "../includes/Webserv.hpp"

// IOhandler::IOhandler():_fdNum(0){};

IOhandler::IOhandler(request &req):_fdNum(0), _req(req){
        _addrlen = sizeof(_address);
        _outSize = 0;
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
    int     ret = index;
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
    std::string hello = _req.getResponse();
    int  reqSize = hello.size();
    reqSize -= _outSize;
    if (reqSize > 0){
        int sen = send(fd, hello.c_str() + _outSize, reqSize, 0);
        if (sen == -1){
            throw Socketexeption(strerror(errno));
        }
        _outSize += sen;
        _pollfd_list[index].events = POLLOUT;
        std::cout << reqSize << std::endl;
        _pollfd_list[index].revents = 0;
        return ;
    }
    std::cout  << " hello sent " << std::endl;
    std::cout <<"conection header "<< _req.getHeaderOf("Connection").first << std::endl;
    if (_req.getHeaderOf("Connection").first == false || (_req.getHeaderOf("Connection").first == true && _req.getHeaderOf("Connection").second->second == "close")){
        this->deleteS(index);
        return;
    }
    _pollfd_list[index].events = POLLIN;
    _pollfd_list[index].revents = 0;
    _outSize = 0;
};

void    IOhandler::deleteS(int index){
    if (index < _pollfd_list.size()){
        close(_pollfd_list[index].fd);
        std::cout << "---------------- close " <<_pollfd_list[index].fd <<"\n";
        _pollfd_list.erase(_pollfd_list.begin() + index);
        _fdNum--;
    }
};

void    IOhandler::IOwatch(){
    int     ret = 0;
    int     size = 0;
    int     fd = -1;
    while (true)
    {
        std::cout << "---------------- Poll\n";
        ret = poll(&(_pollfd_list[0]), _fdNum, -1);
        if (ret < 0){
            std::cerr << "FATAL error -poll-: " << strerror(errno);
        }
        size = _fdNum;
        for (int i = 0; i < size; i++)
        {
            if (_pollfd_list[i].revents == 0)
                continue ;
            if ((_pollfd_list[i].revents & POLLERR) || (_pollfd_list[i].revents & POLLNVAL)){
                this->deleteS(i);
                continue ;
            }
            fd = _pollfd_list[i].fd;
            if (_pollfd_list[i].revents & POLLIN){
                // TODO: std::cout << "---------------- input\n";
                inputEvent(fd, i);
            }
            if (_pollfd_list[i].revents & POLLOUT){
                //TODO: write to fd
                // std::cout << "---------------- output\n";
                outputEvent(fd, i);
            }
            if (_pollfd_list[i].revents & POLLHUP){
                //TODO: Connectio to be close;
                // std::cout << "---------------- POLLHUB\n";
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
        if (res == 0){
            std::cout << "---------------- Close Connetion at read" << i  <<"----------------"  << std::endl;
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