#include "../includes/Webserv.hpp"

queue::queue():_contentSent(0),_contentLent(0){}

queue    queue::initQueueElm(int  fd, request req){
    this->_contentSent = 0;
    this->_contentLent = 0;
    this->_fd = fd;
    this->_req = req;
    return (*this);
}

void        queue::updateReqSent(int size){
    _contentSent += size;
}

void    queue::setReq(request &req){
    this->_req = req;
}

request queue::getReq() const{
    return this->_req;
}

int         queue::getReqLent() const{
    return this->_contentLent;
}

int         &queue::getReqSent(){
    return this->_contentSent;
}
int     queue::getFD() const{
    return this->_fd;
}

queue::~queue(){}