#include "../includes/Webserv.hpp"

queue::queue():_contentSent(0),_contentLent(0),_isDone(true){}

queue    queue::initQueueElm(int  fd, request req){
    this->_contentSent = 0;
    this->_contentLent = 0;
    this->_fd = fd;
    this->_req = req;
    return (*this);
}

int        queue::updateReqSent(int size){
    _contentSent += size;
    return _contentSent;
}

void    queue::parseReq(){
    this->_req.requestParser(this->_reqString);
}

void    queue::setReq(request &req){
    this->_req = req;
}

request &queue::getReq(){
    return this->_req;
}

int         queue::getcontentLent() const{
    return this->_contentLent;
}

void         queue::setcontentLent(int nb){
    this->_contentLent = nb;
}

int         &queue::getReqSent(){
    return this->_contentSent;
}
int     queue::getFD() const{
    return this->_fd;
}

bool        queue::isBodyDone(){
    size_t  i = this->_reqString.find_first_of("\r\n\r\n");
    size_t body_size = _reqString.length() - i + 4;
    if (_isDone == false && body_size < this->_contentLent){
        return false;
    }
    return true;
}

void     queue::reqCheack(){
    std::string req = this->_reqString;
    size_t dmt = 0;
    size_t i = 0;
    size_t c_lent = 0;

    dmt = req.find("\r\n\r\n");
    if (dmt != std::string::npos){
        if ((i = req.find("Content-Length:")) != std::string::npos) {
            c_lent = ::atoi(req.substr(req.find("Content-Length:") + 15, req.find('\r', i)).c_str());
            std::cout << "/////////////////" << c_lent << std::endl;
            this->setcontentLent(c_lent);
            _isDone = isBodyDone();
            return ;
        }
        else{
            _isDone = 1;
            return ;
        }
    }
    else{
        _isDone = 0;
    }
}

queue::~queue(){}