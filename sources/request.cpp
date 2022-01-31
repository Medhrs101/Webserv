#include "../includes/Webserv.hpp"
#include <string>

request::request(/* args */)
{

}

std::pair<bool, std::map<std::string, std::string>::const_iterator> request::getHeaderOf(std::string req) const{
	std::map<std::string, std::string>::const_iterator it;
	if ((it = this->_header.find(req)) != this->_header.end())
		return	std::make_pair(true, it);
	return std::make_pair(false, it);
};

void	request::initialize(void) {
	this->_bodyMessage = std::string();
	this->_header.clear();
	this->_httpVersion = std::string();
	this->_reqLine = std::string();
	this->_reqMethod = std::string();
	this->_reqstr = std::string();
	this->_reqUri = std::string();
	this->_statusCode = int();
	this->_reqHeader = nullptr;
};
request::request(std::vector<ServerData> data):_statusCode(200), _data(data)
{
	//NOTE:: just for the moment;
	/*NOTE:: first of all i will parse the request begin with:
	1-/ request Line and if i found an error i will set 400 in a ret variable (method - URI - HTTP version)
	2-/ header fieldes
	3-/message bodyc
	*/
	/* NOTE: requestParser should parse the request here; */
	// this->requestParser();
	// this->getRequestLine();
	// this->hundleReqLine();
}


void    request::requestParser(std::string req)
{
	_reqstr = req;
	size_t i(0);
	i = _reqstr.find("\r\n");
	if (i == std::string::npos)
		throw ErrorException("Error in the request Line");
	i = i + 2;
	this->parseRequestLine(_reqstr.substr(0, i));
	size_t	j = _reqstr.find("\r\n\r\n");
	// std::cout << j << std::endl;
	if (j == std::string::npos)
		throw ErrorException("Error in the Header Fieldes");
	j = j + 4;
	this->parseReqHeader(_reqstr.substr(i, j - i));
	this->parseBody(_reqstr.substr(j, _reqLine.size() - j));
	this->printReqData();
}

void	request::parseBody(std::string	reqBody)
{
	this->_bodyMessage = reqBody;
}

void	request::HeaderLine(std::string &reqHeader, std::string & Line,  size_t &i)
{
	size_t	j = reqHeader.find("\r\n", i);
	if (j == std::string::npos)
		throw ErrorException("Error in the Header Fieldes");
	Line = reqHeader.substr(i, j - i);
	i = j + 2;
}

void	trim(std::string &str)
{
	while (!str.empty() && isspace(str.front()))
		str.erase(0, 1);
	while (!str.empty() && isspace(str.back()))
		str.erase(str.size() - 1, 1);
}
void	request::headerInMap(std::string & Line, std::string & key, std::string & value)
{
	size_t	i = Line.find(":");
	if (i == std::string::npos)
		throw ErrorException ("Error in header fieldes");
	key = Line.substr (0, i);
	value = Line.substr(i + 1, Line.size() - i + 1);
	trim(value);
}


void	request::parseReqHeader(std::string reqHeader)
{
	// std::cout << "Header fields: |" << reqHeader << "|" << std::endl;
	std::string	Line;
	std::string key;
	std::string value;
	size_t i(0);
	while (1)
	{
		HeaderLine(reqHeader, Line, i);
		headerInMap(Line, key, value);
		this->_header[key] = value;
		// std::cout << "key: |" << key << "|" <<  value << "|" << std::endl;
		if (reqHeader[i] == '\r' && reqHeader[i + 1] == '\n')
			break;
	}	
}

void    request::parseRequestLine(std::string reqLine)
{
	std::cout << "reqLine: <" << reqLine << ">" << std::endl;
	size_t i(0);
	this->hundleMethod(reqLine, i);
	this->hundleUri(reqLine, ++i);
	this->hundleHttpv(reqLine, ++i);
	/*NOTE: After getting all values of the request Line you should verificate it carefully*/
}

void    request::hundleMethod(std::string & reqLine, size_t & i)
{
	i = reqLine.find_first_of(' ');
	if (i == std::string::npos || reqLine[i + 1] == ' ')
		throw ErrorException("Error in the request Line 400");
	this->_reqMethod = reqLine.substr(0, i);
}

void    request::hundleUri(std::string &reqLine, size_t & j)
{
	size_t i = reqLine.find_first_of(' ', j);
	// std::cout << i << std::endl;
	if (i == std::string::npos || reqLine[i + 1] == ' ')
		throw ErrorException("Error in the request Line 400");
	this->_reqUri = reqLine.substr(j, i - j);
	j = i;
}

void    request::hundleHttpv(std::string & reqLine, size_t & j)
{
	size_t  i = reqLine.find("\r\n", j);
	if (i == std::string::npos || reqLine[i + 1] == ' ')
		throw ErrorException ("Error the request Line http 400");
	this->_httpVersion = reqLine.substr(j, i - j);
}

// void	request::getRequestLine()
// {
//     getline(_reqstr, _reqLine);
// }



const char* request::ErrorException::what() const throw()
{
	return (this->_message);
}

request::ErrorException::ErrorException(const char *message): _message(message) {}

// void    request::hundleReqLine()
// {
//     size_t  lengthRqLine = _reqLine.length();
//     int     sp = 0;
//     for(int i = 0; i < lengthRqLine; i++)
//     {
//         if (sp > 2)
//             break;
//         else if (_reqLine[i] != ' ' && !sp)
//             this->_reqMethod += _reqLine[i];
//         else if (_reqLine[i] != ' ' && sp == 1)
//             this->_reqUri += _reqLine[i];
//         else if (_reqLine[i] != ' ' && sp == 2)
//             this->_httpVersion += _reqLine[i];
//         else
//             sp++;
//     }
//     if (sp > 2)
//         throw request::ErrorException("Error request Line");
// }

void    request::printReqData( void )
{
	std::cout << "Method: |" << this->_reqMethod << "|" << std::endl;
	std::cout << "URI: |" << this->_reqUri << "|" << std::endl;
	std::cout << "HTTP-version: |" << this->_httpVersion << "|" << std::endl;
	std::map<std::string, std::string>::iterator	it = _header.begin();
	// std::cout << _header.size() << std::endl;
	while (it != _header.end())
	{
		std::cout << "key: |" << it->first << "|\t" << "value: |" << it->second << "|" << std::endl;
		++it;
	}
	std::cout << "reqbody: |" << this->_bodyMessage << "|" << std::endl;
}

request::~request()
{

}
