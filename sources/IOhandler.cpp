#include "../includes/Webserv.hpp"

// IOhandler::IOhandler():_fdNum(0){};

IOhandler::IOhandler(std::vector<ServerData>  data):_fdNum(0),_data(data) {
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
        if (_reqQueue[i].getFD() == elm.getFD() && _reqQueue[i]._isDone){
            _reqQueue[i].getReq().initialize();
            _reqQueue[i].initQueueElm(elm.getFD(), elm.getReq());
            return ;
        }
    }
    std::cout << "add new elemnt" << std::endl;
    _reqQueue.push_back(elm);
};

void    IOhandler::newElem(int fd){
    this->_reqQueue.push_back(queue(_data));
    this->_reqQueue[_reqQueue.size() - 1].setFD(fd);
}
void    IOhandler::removeQueue(int fd){
    for (size_t i = 0; i < _reqQueue.size(); i++)
    {
        if (_reqQueue[i].getFD() == fd){
            _reqQueue.erase(_reqQueue.begin() + i);
            return ;
        }
    }
};
bool   isReqDone(std::string req){
    std::string Content_Type;
    bool        boundary;
    std::string boundaryString;

    size_t i = 0;
    i = req.find("Content-Type");
    if (i != std::string::npos){
        i = req.find(':', i);
        Content_Type = req.substr(i, req.find("\r\n", i));
        i = Content_Type.find("boundary");
        if (i != std::string::npos){
            boundaryString = Content_Type.substr(Content_Type.find("=", i));
            std::cout << "<<<<<<<<<<<<<<<<<" <<  boundaryString <<std::endl;
        }
        return true;
    }
    return true;
}

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
        std::cout << WHT <<  "[ NEW connection from " << inet_ntoa(_address.sin_addr) << ":" << ntohs(_address.sin_port) << " ---------------- ]" << RESET << std::endl;
        evPoll.fd = new_socket;
        evPoll.events = POLLIN;
        evPoll.revents = 0;
        _pollfd_list.push_back(evPoll);
        this->newElem(new_socket);
        _fdNum++;
    }
    else{
        req_string = readReq(fd, &ret);
        std::cout << req_string << std::endl;
        if (ret > 0){
            if (req_string[0] != '\r' || req_string.length()){
                (*this)[fd]->setcontentRead(ret);
                if ((*this)[fd] == this->_reqQueue.end()){
                    (*this)[fd]->_reqString.append(req_string);
                }else{
                    if ((*this)[fd]->_isDone){
                        (*this)[fd]->_isDone = false;
                        (*this)[fd]->getReq().initialize();
                        (*this)[fd]->setcontentSent(0);
                        (*this)[fd]->setcontentRead(0);
                        (*this)[fd]->_reqString.clear();
                    }
                    (*this)[fd]->_reqString.append(req_string);
                }
            }
            _pollfd_list[index].events = POLLIN | POLLOUT;
        }
    }
}

std::vector<queue>::iterator   IOhandler::operator[](int n){
    for (int i = 0; i < _reqQueue.size(); i++)
    {
        if (_reqQueue[i].getFD() == n)
            return _reqQueue.begin() + i;
    }
    return _reqQueue.end();
}

void    IOhandler::outputEvent(int fd, int index){
    std::vector<queue>::iterator it = (*this)[fd];
    queue &current = *it;

    std::ofstream file("out.txt");
    file << current._reqString;
    // system("clear");

    current.reqCheack();
    std::cout << "---- is Done : " << std::boolalpha  << current._isDone << std::endl;
    if (current._isDone){
        if (current.getReqSent() == 0)
            current.parseReq();
        const std::string hello = current.getResponse();
        int  reqSize = hello.size();
        int  reqSent = current.getReqSent();
        int sen = send(fd, hello.c_str() + reqSent, reqSize - reqSent, 0);
        std::cout << " <<< sent" << sen << std::endl;
        if (reqSent < reqSize){
            if (sen == -1){
                throw Socketexeption(strerror(errno));
            }
            reqSent = current.updateReqSent(sen);
            if (reqSent < reqSize){
                _pollfd_list[index].events = POLLOUT;
                _pollfd_list[index].revents = 0;
                return ;
            }
        }
        std::cout <<" |> response sent <|" << std::endl;
        if (current.getReq().getHeaderOf("Connection").first == false || (current.getReq().getHeaderOf("Connection").first == true && current.getReq().getHeaderOf("Connection").second->second == "close")){
            this->deleteS(index);
            std::cout << "---------------- close debug ---------------- " <<_pollfd_list[index].fd <<"\n";
            return;
        }
    }
    _pollfd_list[index].events = POLLIN;
    _pollfd_list[index].revents = 0;
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
                TODO: std::cout << "---------------- input\n";
                inputEvent(fd, i);
            }
            if (_pollfd_list[i].revents & POLLOUT){
                //TODO: write to fd
                std::cout << "---------------- output\n";
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
        int temp;
        char buff[1024] = {0};
        bzero(buff,1024);
        std::string ret;
        res = recv(fd, buff, sizeof(buff) - 1, 0);
        std::cout << "---------------- read ----------------  " << res << std::endl;
        if (res == 0){
            std::cout << "---------------- Close Connetion at read ----------------"  << std::endl;
            this->deleteS(i);
            *n = 0;
        }
        else if (res < 0){
            *n = -1;
            throw Socketexeption(strerror(errno));
        }
        else{
            ret.append(buff, res);
            *n += res;
            // while (res > 0)
            // {
            //     *n += res;
            //     temp += res;
            //     if (res < 1023)
            //         break;
            //     res = recv(fd, buff, sizeof(buff) - 1, 0);
            //     if (res < 0)
            //         throw Socketexeption(strerror(errno));
            //     // buff[res] = '\0';
            //     if (res > 0){
            //         ret.append(buff, res);
            //     }
            //     bzero(buff,1024);
            // }
            std::cout << " ---------------- read ----------------  " << *n << std::endl;
        }
        return ret;
    }