#include "../includes/Webserv.hpp"
queue::queue(std::vector<ServerData> data):_req(data),_contentLent(0),_contentSent(0),_contentRead(0),_isChunked(false),_isDone(false){ getReq().initialize();}

queue    queue::initQueueElm(int  fd, request req){
    this->_contentSent = 0;
    this->_contentLent = 0;
    this->_fd = fd;
    this->_req = req;
    return (*this);
}
void    queue::reset(){

    this->_isDone = false;
    this->getReq().initialize();
    this->setcontentSent(0);
    this->_resString.clear();
    this->setcontentRead(0);
    this->_reqString.clear();
    this->_isChunked = false;
}

int     HexToDec(std::string hexVal)
{
    int len = hexVal.size();
    int base = 16;
    int dec_val = 0;

    
    for (int i = 0; i < len; i++) {
        if (hexVal[i] >= '0' && hexVal[i] <= '9') {
            dec_val += (int(hexVal[i]) - 48) * (std::pow(base, len - i - 1));
        } 
        else if ((hexVal[i] >= 'A' && hexVal[i] <= 'F')) {
            dec_val += (int(hexVal[i]) - 55) * (std::pow(base, len - i - 1));
        }
        else if ((hexVal[i] >= 'a' && hexVal[i] <= 'f')) {
            dec_val += (int(hexVal[i]) - 87) * (std::pow(base, len - i - 1));
        }
        else
            break;
    }
    return dec_val;
}

bool    isHex(std::string tmp){
    if (tmp[0] == '\r')
        return false;
    size_t pos = tmp.find_first_not_of("0123456789ABCDEFabcdef");
    if (tmp[pos] == '\r')
        return true;
    else
        return false;
}

void    queue::chunkParser(){
    std::string body;
    std::string tmp;
    std::string newBody;
    size_t i = 0;
    size_t cnt = 0;
    bool    alter = false;
    size_t cLent = 0;
    std::string Head;// = _reqString.substr(0, i + 4);


    if (_isChunked && _isDone){
        i = _reqString.find("\r\n\r\n");

        if (i != std::string::npos){
            Head = _reqString.substr(0, i + 4);
            body = _reqString.substr(i + 4);
            while (!body.empty())
            {
                cnt = body.find("\r\n");
                std::string tmp;
                tmp = body.substr(0, cnt + 1);
                if ((alter = isHex(tmp))){
                    tmp.pop_back();
                    cLent = HexToDec(tmp);
                    body.erase(0, cnt + 2);
                    newBody.append(body,0, cLent);
                    body.erase(0, cLent);
                }
                else if (tmp[0] == '\r'){
                    body.erase(0, 2);
                }
                cLent = 0;
            }
            _reqString = Head + newBody;
            _isChunked = false;
        }
    }
}

bool        queue::isBodyDone(){
    size_t  i = this->_reqString.find("\r\n\r\n");
    size_t body_size = _reqString.length() - (i + 4);
    std::cout << " bodd_size /////////////////" << body_size << std::endl;
    //TODO: size_T countentleny
    if (_isDone == false && body_size < static_cast<size_t>(this->_contentLent)){
        return false;
    }
    return true;
}

void     queue::reqCheack(){
    std::string req = this->_reqString;
    size_t cp = 0;
    size_t headEnd = 0;
    size_t i = 0;
    size_t c_lent = 0;

    headEnd = req.find("\r\n\r\n");
    if (headEnd != std::string::npos){
        cp = req.find("Transfer-Encoding: chunked");
        if( cp != std::string::npos){
            _isChunked = true;
            cp = req.find("0\r\n\r\n", headEnd);
            if ( cp != std::string::npos ){
                _isDone = true;
            }
            else
                _isDone = false;
            return;
        }
        else if (!_isChunked && (i = req.find("Content-Length:")) != std::string::npos) {
            c_lent = ::atoi(req.substr(req.find(":", i) + 1).c_str());
            std::cout << "c_lent /////////////////|" << c_lent<< "|" << std::endl;
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

void    queue::parseReq(){
    this->_req.requestParser(this->_reqString);
    this->_resString = this->_req.getResponse();
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


void    queue::setReq(request &req){
    this->_req = req;
}

void         queue::setcontentLent(int nb){
    this->_contentLent = nb;
}

void         queue::setcontentRead(size_t nb){
    this->_contentRead += nb;
}

const std::string &queue::getResponse() const{
    return this->_resString;
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

int         &queue::getReqSent(){
    return this->_contentSent;
}

int     queue::getFD() const{
    return this->_fd;
}

bool    queue::isChunked(void){
    return this->_isChunked;
}

bool    queue::isDone(void){
    return this->_isDone;
}

queue::~queue(){}