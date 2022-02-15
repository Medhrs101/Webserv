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
    std::cout << "add new elemnt" << std::endl;
    _reqQueue.push_back(elm);
};

// void    IOhandler::newElem(){
//     queue elm;

//     elm.initQueueElm()
//     this->_reqQueue.resize(_reqQueue.size() + 1, queue());
    
// }
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
        std::cout << "[ ---------- Accept connection from " << inet_ntoa(_address.sin_addr) << ":" << ntohs(_address.sin_port) << " ---------------- ]"  << std::endl;
        evPoll.fd = new_socket;
        evPoll.events = POLLIN;
        evPoll.revents = 0;
        _pollfd_list.push_back(evPoll);
        _fdNum++;
    }
    else{
        req_string = readReq(fd, &ret);
        // std::cout << "||" << req_string<< "||" << std::endl;
        if (ret > 0){
            if (req_string[0] != '\r' || req_string.length()){
                if ((*this)[fd] == this->_reqQueue.end()){
                    queue elm;
                    _req.initialize();
                    elm.initQueueElm(fd, _req);
                    this->addToQueue(elm);
                    (*this)[fd]->_reqString.append(req_string);
                }else{
                    (*this)[fd]->_reqString.append(req_string);
                }
                // if (req_string.find("\r\n\r\n") != std::string::npos){
                //     std::cout << "frommmmmmmmmmmmmmmmmmmmmmmm" << std::endl;
                //     _pollfd_list[index].events = POLLIN | POLLOUT;
                //     return ;
                // }
            }
            // if (req_string.find("\r\n\r\n") == std::string::npos && req_string[0] != '\r'){
            //     _pollfd_list[index].events = POLLIN;
            // }
            // else 
            _pollfd_list[index].events = POLLIN | POLLOUT;
        }
    }
}

// void    IOhandler::inputEvent(int fd, int index){
//     int     new_socket;
//     int     ret = index;
//     std::string req_string;
//     struct pollfd   evPoll;

//     if (std::find(_master_fds.begin(), _master_fds.end(), fd) != _master_fds.end()){
//         new_socket = accept(fd, (struct sockaddr *) &_address, (socklen_t *) &_addrlen);
//         if (new_socket < 0){
//             perror("vv  ");
//             throw Socketexeption("accept");
//         }
//         fcntl(new_socket, F_SETFL, O_NONBLOCK);
//         std::cout << "[ ---------- Accept connection from " << inet_ntoa(_address.sin_addr) << ":" << ntohs(_address.sin_port) << " ---------------- ]"  << std::endl;
//         evPoll.fd = new_socket;
//         evPoll.events = POLLIN;
//         evPoll.revents = 0;
//         _pollfd_list.push_back(evPoll);
//         _fdNum++;
//     }
//     else{
//         req_string = readReq(fd, &ret);
//         // isReqDone(req_string);
//         std::cout << "||" << req_string << "||" << std::endl;
//         // exit(0);
//         if (ret == 0)
//             return ;
//         if (ret > 0){
//             // std::string firstline = req_string.substr(0, req_string.find("\r\n"));

//             if ((*this)[fd] == this->_reqQueue.end()){
//                 queue elm;
//                 _req.initialize();
//                 elm.initQueueElm(fd, _req);
//                 this->addToQueue(elm);
//                 (*this)[fd]->_reqString.append(req_string);
//             }else{
//                 (*this)[fd]->_reqString.append(req_string);
//             }
//             (*this)[fd]->getReq().requestParser((*this)[fd]->_reqString);
//             _pollfd_list[index].events = POLLIN | POLLOUT;
//             // _req.initialize();
//             // _req.requestParser(req_string);
//         }
//     }
// };

std::vector<queue>::iterator   IOhandler::operator[](int n){
    for (int i = 0; i < _reqQueue.size(); i++)
    {
        if (_reqQueue[i].getFD() == n)
            return _reqQueue.begin() + i;
    }
    return _reqQueue.end();
}

void    IOhandler::outputEvent(int fd, int index){
    queue &current = *((*this)[fd]);
    current.reqCheack();
    std::cout << "cheaking" << std::endl;
    if (current._isDone){
        // current.getReq().requestParser(current._reqString);CHANNE
        current.parseReq();
        std::cout << (*this)[fd]->getReq().getResponse() << std::endl;
        const std::string &hello = current.getReq().getResponse();
        int  reqSize = hello.size();
        int  reqSent = current.getReqSent();
        if (reqSent < reqSize){
            int sen = send(fd, hello.c_str() + reqSent, reqSize - reqSent, 0);
            if (sen == -1){
                throw Socketexeption(strerror(errno));
            }
            reqSent = current.updateReqSent(sen);
            if (reqSent < reqSize){
                _pollfd_list[index].events = POLLOUT;
                _pollfd_list[index].revents = 0;
                return ;
            }
            current.getReq().initialize();
        }
        std::cout  <<  reqSent << " response Sent | " << hello << std::endl;
        if (current.getReq().getHeaderOf("Connection").first == false || (current.getReq().getHeaderOf("Connection").first == true && current.getReq().getHeaderOf("Connection").second->second == "close")){
            this->deleteS(index);
            std::cout << "---------------- close debug ---------------- " <<_pollfd_list[index].fd <<"\n";
            return;
        }
    }
    // exit(0);
    _pollfd_list[index].events = POLLIN;
    _pollfd_list[index].revents = 0;
    // _outSize = 0;
    // current.updateReqSent(0);
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
                bzero(buff,1024);
            }
        }
        return ret;
    }