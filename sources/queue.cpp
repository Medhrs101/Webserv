#include "../includes/Webserv.hpp"

queue::queue(std::vector<ServerData> data):_contentSent(0),_contentLent(0),_isDone(true),_contentRead(0), _req(data){}

queue    queue::initQueueElm(int  fd, request req){
    this->_contentSent = 0;
    this->_contentLent = 0;
    this->_fd = fd;
    this->_req = req;
    return (*this);
}

void     queue::setFD(int fd){
    this->_fd = fd;
}

int        queue::updateReqSent(int size){
    _contentSent += size;
    return _contentSent;
}

void    queue::setcontentSent(int size){
    this->_contentSent = size;
}

void    queue::parseReq(){
    this->_req.requestParser(this->_reqString);
    this->_resString = this->_req.getResponse();
}

const std::string &queue::getResponse() const{
    return this->_resString;
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

size_t         queue::getcontentRead() const{
    return this->_contentRead;
}

void         queue::setcontentLent(int nb){
    this->_contentLent = nb;
}
void         queue::setcontentRead(size_t nb){
    this->_contentRead += nb;
}
int         &queue::getReqSent(){
    return this->_contentSent;
}
int     queue::getFD() const{
    return this->_fd;
}

bool        queue::isBodyDone(){
    size_t  i = this->_reqString.find("\r\n\r\n");
    // size_t body_size = std::max(_reqString.length() - (i + 4), this->_contentRead);
    size_t body_size = _reqString.length() - (i + 4);
    // size_t body_size = this->_contentRead;
    std::cout << "/////////////////" << body_size << std::endl;
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
    std::cout << "/////////////////" << dmt << std::endl;
    if (dmt != std::string::npos){
        if ((i = req.find("Content-Length:")) != std::string::npos) {
            c_lent = ::atoi(req.substr(req.find("Content-Length:") + 15, req.find('\r', i)).c_str());
            this->setcontentLent(c_lent);
            std::cout << "/////////////////" << c_lent << std::endl;
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