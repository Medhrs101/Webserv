#include "./request.hpp"

request::request(/* args */)
{

}

request::request(std::string req): _reqstr(req)
{
    //NOTE:: just for the moment;
    this->getRequestLine();
    this->hundleReqLine();
}

void	request::getRequestLine()
{
    getline(_reqstr, _reqLine);
}



const char* request::ErrorException::what() const throw()
{
	return (this->_message);
}

request::ErrorException::ErrorException(const char *message): _message(message) {}

void    request::hundleReqLine()
{
    size_t  lengthRqLine = _reqLine.length();
    int     sp = 0;
    for(int i = 0; i < lengthRqLine; i++)
    {
        if (sp > 2)
            break;
        else if (_reqLine[i] != ' ' && !sp)
            this->_reqMethod += _reqLine[i];
        else if (_reqLine[i] != ' ' && sp == 1)
            this->_reqUri += _reqLine[i];
        else if (_reqLine[i] != ' ' && sp == 2)
            this->_httpVersion += _reqLine[i];
        else
            sp++;
    }
    if (sp > 2)
        throw request::ErrorException("Error request line");
}

void    request::printReqData( void )
{
    std::cout << "Method: |" << this->_reqMethod << "|" << std::endl;
    std::cout << "URI: |" << this->_reqUri << "|" << std::endl;
    std::cout << "HTTP-version: |" << this->_httpVersion << "|" << std::endl;
}


request::~request()
{

}
