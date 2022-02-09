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

void    IOhandler::addToQueue(queue elm){
    for (size_t i = 0; i < _reqQueue.size(); i++)
    {
        if (_reqQueue[i].getFD() == elm.getFD()){
            _reqQueue[i].initQueueElm(elm.getFD(), elm.getReq());
            return ;
        }
    }
    _reqQueue.push_back(elm);
};

void    IOhandler::removeQueue(int fd){
    for (size_t i = 0; i < _reqQueue.size(); i++)
    {
        if (_reqQueue[i].getFD() == fd){
            _reqQueue.erase(_reqQueue.begin() + i);
            return ;
        }
    }
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
        std::cout << "[ ---------- Accept connection from " << inet_ntoa(_address.sin_addr) << ":" << ntohs(_address.sin_port) << " ---------------- ]"  << std::endl;
        evPoll.fd = new_socket;
        evPoll.events = POLLIN;
        evPoll.revents = 0;
        _pollfd_list.push_back(evPoll);
        _fdNum++;
    }
    else{
        req_string = readReq(fd, &ret);
        std::cout << ret << std::endl;
        if (ret == 0)
            return ;
        if (ret > 0){
            queue elm;
            _req.initialize();
            _req.requestParser(req_string);
            elm.initQueueElm(fd, _req);
            this->addToQueue(elm);
            _pollfd_list[index].events = POLLIN | POLLOUT;
        }
    }
};

queue   &IOhandler::operator[](int n){
    for (int i = 0; i < _reqQueue.size(); i++)
    {
        if (_reqQueue[i].getFD() == n)
            return _reqQueue[i];
    }
    return *(_reqQueue.end());
}

void    IOhandler::outputEvent(int fd, int index){
    queue &current = (*this)[fd];

    // current = (*this)[fd];
    std::string hello = current.getReq().getResponse();
    int  reqSize = hello.size();
    int  reqSent = current.getReqSent();

    if (reqSent < reqSize){
        int sen = send(fd, hello.c_str() + reqSent, reqSize - reqSent, 0);
        if (sen == -1){
            throw Socketexeption(strerror(errno));
        }
        current.updateReqSent(sen);
        std::cout << current.getReqSent() << " "  << reqSize << std::endl;
        if (reqSent < reqSize){
            _pollfd_list[index].events = POLLOUT;
            _pollfd_list[index].revents = 0;
            return ;
        }
    }
    std::cout  << " response Sent " << std::endl;
    if (_req.getHeaderOf("Connection").first == false || (_req.getHeaderOf("Connection").first == true && _req.getHeaderOf("Connection").second->second == "close")){
        this->deleteS(index);
        std::cout << "---------------- close debug ---------------- " <<_pollfd_list[index].fd <<"\n";
        return;
    }
    _pollfd_list[index].events = POLLIN;
    _pollfd_list[index].revents = 0;
    _outSize = 0;
};

void    IOhandler::deleteS(int index){
    if (index < _pollfd_list.size()){
        int fd = _pollfd_list[index].fd;
        close(fd);
        std::cout << "---------------- close connection ---------------- " <<_pollfd_list[index].fd <<"\n";
        _pollfd_list.erase(_pollfd_list.begin() + index);
        removeQueue(fd);
        _fdNum--;
    }
};

void    IOhandler::IOwatch(){
    int     ret = 0;
    int     size = 0;
    int     fd = -1;
    while (true)
    {
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
        *n = 0;
        char buff[1024] = {0};
        std::string ret;
        res = recv(fd, buff, sizeof(buff) - 1, 0);
        if (res == 0){
            std::cout << "---------------- Close Connetion at read ----------------"  << std::endl;
            this->deleteS(i);
            *n = 0;
        }
        else if (res < 0){
            *n = -1;
            throw Socketexeption("cant read Req");
        }
        else{
            ret = buff;
            while (res > 0)
            {
                *n += res;
                if (res < 1023)
                    break;
                res = recv(fd, buff, sizeof(buff) - 1, 0);
                if (res > 0)
                    ret += buff;
            }
        }
        return ret;
    }