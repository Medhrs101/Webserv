#include "../includes/Webserv.hpp"

// IOhandler::IOhandler():_fdNum(0){};

IOhandler::IOhandler(std::vector<ServerData>  data):_data(data),_fdNum(0) {
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
        if (_reqQueue[i].getFD() == elm.getFD() && _reqQueue[i].isDone()){
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
    std::string boundaryString;

    size_t i = 0;
    i = req.find("Content-Type");
    if (i != std::string::npos){
        i = req.find(':', i);
        Content_Type = req.substr(i, req.find("\r\n", i));
        i = Content_Type.find("boundary");
        if (i != std::string::npos){
            boundaryString = Content_Type.substr(Content_Type.find("=", i));
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
            std::cerr << RED <<  "Cant accept Connection :" << inet_ntoa(_address.sin_addr) << strerror(errno) << RESET << std::endl;
            return ;
        }
        fcntl(new_socket, F_SETFL, O_NONBLOCK);
        std::cout << GRN << "\e[1m" <<  "✔ NEW connection from " << inet_ntoa(_address.sin_addr) << RESET << std::endl;
        evPoll.fd = new_socket;
        evPoll.events = POLLIN | POLLOUT;
        evPoll.revents = 0;
        _pollfd_list.push_back(evPoll);
        this->newElem(new_socket);
        _fdNum++;
    }
    else{
        req_string = readReq(fd, &ret);

        if (ret > 0){
            if (req_string[0] != '\r' || req_string.length()){
                (*this)[fd]->setcontentRead(ret);
                {
                    if ((*this)[fd]->isDone()){
                        (*this)[fd]->reset();
                    }
                    (*this)[fd]->appendReq(req_string);
                }
            }
            _pollfd_list[index].events = POLLIN | POLLOUT;
        }
    }
}

std::vector<queue>::iterator   IOhandler::operator[](int n){
    for (size_t i = 0; i < _reqQueue.size(); i++)
    {
        if (_reqQueue[i].getFD() == n)
            return _reqQueue.begin() + i;
    }
    return _reqQueue.end();
}

void    IOhandler::outputEvent(int fd, int index){
    std::vector<queue>::iterator it = (*this)[fd];
    queue &current = *it;

    if (!current.isDone())
        current.reqCheack();
    if (current.isDone()){
        if(current.isChunked()){
            current.chunkParser();
        }
        if (current.getReqSent() == 0)
            current.parseReq();
        const std::string hello = current.getResponse();
        int  reqSize = hello.size();
        int  reqSent = current.getReqSent();
        int sen = send(fd, hello.c_str() + current.getReqSent(), reqSize - current.getReqSent(), 0);
        if (sen == 0)
                return ;
        if (sen == -1){
            _pollfd_list[index].revents = POLLHUP;
            // throw Socketexeption(strerror(errno));
            return ;
        }
        reqSent = current.updateReqSent(sen);
        if (current.getReqSent() < reqSize){
            return ;
        }
        std::cout << GREEN << "/e[1m<<" << " Response sent :" << sen << RESET << std::endl;
        if (current.getReq().getHeaderOf("Connection").first == false || (current.getReq().getHeaderOf("Connection").first == true && current.getReq().getHeaderOf("Connection").second->second == "close")){
            this->deleteS(index);
            return;
        }
    }
    _pollfd_list[index].events = POLLIN;
    _pollfd_list[index].revents = 0;
};

void    IOhandler::deleteS(int index){
    if (static_cast<size_t>(index) < _pollfd_list.size()){
        int fd = _pollfd_list[index].fd;
        close(fd);
        _pollfd_list.erase(_pollfd_list.begin() + index);
        removeQueue(fd);
        _fdNum--;
        std::cout << RED << "\e[1m" << "Connection Closed With Client" << RESET << std::endl;
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
        for (int i = 0; i < _pollfd_list.size(); i++)
        {
            if (_pollfd_list[i].revents == 0)
                continue ;
            if ((_pollfd_list[i].revents & POLLERR) || (_pollfd_list[i].revents & POLLNVAL)){
                this->deleteS(i);
                continue ;
            }
            std::cout << BLU << _pollfd_list.size() << RESET << std::endl;
            fd = _pollfd_list[i].fd;
            if (_pollfd_list[i].revents & POLLIN){
                inputEvent(fd, i);
            }
            if (_pollfd_list[i].revents & POLLOUT){
                outputEvent(fd, i);
            }
            if (_pollfd_list[i].revents & POLLHUP){
                this->deleteS(i);
            }
            size = _fdNum;
        }
    }
};

IOhandler::~IOhandler(){};

 std::string IOhandler::readReq(int fd, int *n){
        int res = 0;
        int i = *n;
        *n = 0;
        char buff[1024] = {0};
        bzero(buff,1024);
        std::string ret;
        res = recv(fd, buff, sizeof(buff) - 1, 0);
        if (res == 0){
            this->deleteS(i);
            *n = 0;
        }
        else if (res < 0){
            this->deleteS(i);
            *n = -1;
            std::cout << BLU << "\e[1m" << "read Error";
        }
        else{
            ret.append(buff, res);
            *n += res;
        }
        return ret;
    }