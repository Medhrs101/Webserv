/**
 * @Author: Your name
 * @Date:   2022-01-21 18:25:18
 * @Last Modified by:   Your name
 * @Last Modified time: 2022-01-31 18:14:27
 */
#include "../includes/Webserv.hpp"
#include <string>
void	errorHandler(std::string	msgError, request & req);
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
	// std::cout << "request str: |" <<  _reqstr << "|" << std::endl;
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
	this->findServer();
	std::cout << "number of server: " << _nbServer <<std::endl;
	this->findLocations();
	std::cout << "number of location: " << _nbLocation << std::endl;
	this->handleRequests();
	// this->printReqData();
}

void	pathCorrection(std::string & path)
{
	for (size_t i = 0; i + 1 < path.size(); i++)
		if (path[i] == '/' && path[i + 1] == '/')
			path.erase(i, 1);
	
}

// bool	isDirectory(std::string  path)
// {
//     struct stat info;

//     if(stat(path.c_str(), &info ) != 0)
//         return 0;
//     else if(info.st_mode & S_IFDIR)
//         return 1;
//     else
//         return 0;
// }

void	makeResponse(response & response, request & req)
{
	std::string respStr;
	char		buff[1000];
	respStr = response._statusLine;
	// std::cout << respStr << std::endl;
	respStr += CRLF;
	// if (response._headers.find("Content-Type") == response._headers.end())
		// respStr += "\r\nContent-Length: " + std::to_string(response._body.length());
	for (std::map<std::string, std::string>::iterator it = response._headers.begin(); it != response._headers.end(); it++)
	{
		respStr += it->first;
		respStr += ": ";
		respStr += it->second;
		respStr += CRLF;
	}
	respStr += "Server: WebServer/med&marji";
	respStr += CRLF;
	time_t now   = time(0);
	struct tm tm = *gmtime(&now);
	strftime(buff, sizeof buff, "%a, %d %b %Y %H:%M:%S %Z", &tm);
	respStr += "Date: " + std::string(buff);
	respStr += "\r\n";
	respStr += "\r\n";
	respStr += response._body;
	respStr += "\r\n\r\n";
	// std::cout << "begin resp |" << respStr << "|end resp" << std::endl;
	req.setResponse(respStr);	
}

std::string contentType (std::string path)
{
	std::string	extension = path.substr(path.find(".") + 1, path.length() - path.find("."));
	if (extension == "html")
		return "text/html";
	else if (extension == "png" || extension == "ico")
		return "image/png";
	else if (extension == "mp4")
		return "video/mp4";
	else
		return "text/plain";
	// std::cout << "this is the path extension : " << extension << std::endl;
	return extension;
}

// void	request::isRedirection(response & response)
// {
// 	int statusCode;
// 	if (_locations[_nbLocation].isRedirection() == true)
// 	{
// 		statusCode = _locations[_nbLocation].getReturnCode();

// 	}
// }

void	request::GETRequest()
{
	response		response;
	struct stat		info;
	std::ifstream 	file;
	std::string		returnCode = " 200 OK";

	if (_locations[_nbLocation]. getAllowedMethods().find("GET")->second == false)
		errorHandler("405 Not Allowed", *this);
	if (_locations[_nbLocation].isRedirection() == true)
	{
		returnCode = std::to_string(_locations[_nbLocation].getReturnCode());
		returnCode += " Moved Permanently";
		response._statusLine = HTTPV1" " + returnCode;
		std::cout << "arani hna ya l9999" << response._statusLine <<std::endl;
		response._headers["Location"] = _locations[_nbLocation].getReturnUrl();
		goto bitbit;
	}
	if (_locations[_nbLocation].getRootDir().empty())
	{
		_path = _data[_nbServer].getRootDir() + _path;
	}
	else
		_path = _locations[_nbLocation].getRootDir() + _path;
	pathCorrection(_path);
	if (stat(_path.c_str(), &info) == 0)
	{
		if (info.st_mode & S_IFDIR)
		{
			_path += "/" + _locations[_nbLocation].getDefaultFile();
			pathCorrection(_path);
		}
	}
	else
	{
		errorHandler("404 Not Found", *this);
	}
	if (response._body.empty() == true)
	{
		std::ostringstream streambuff;
		file.open(_path, std::ios::binary);
		if (file.is_open()) {
			streambuff << file.rdbuf();
			response._body = streambuff.str();
			file.close();
		}
		else
			errorHandler("500 internal Server Error", *this);
	}
	response._statusLine = HTTPV1;
	response._statusLine += " " + returnCode;
	// response._headers["Content-Type"] = contentType(_path);
	response._headers["Content-Length"] = std::to_string(response._body.length());

	bitbit: makeResponse(response, *this);
	// _locations[_nbLocation].getDefaultFile()
	
}

void	request::handleRequests()
{
	//TODO: allowed method
	if (_reqMethod == "GET")
		GETRequest();
	else
		errorHandler("404 Bad request", *this);
}	

void	request::findLocations()
{
	this->_locations = _data[_nbServer].getLocations();
	int		LocationNum  = 0;
	for (size_t i = 0; i < _locations.size(); i++)
	{
		// std::cout << "location path: " << _locations[i].getPath() << std::endl;
		// std::cout << "_path: " << _path << std::endl;
		if (_path.find(_locations[i].getPath()) != std::string::npos)
			LocationNum = i;
	}
	_nbLocation = LocationNum;
}

void	request::findServer()
{
	size_t i = this->_data.size();
	std::cout << i << std::endl;
	// exit(0);
	for(size_t j = 0; j < i; j++)
	{
		// std::cout << std::to_string(_data[j].getPort()) << "  |  " << _port << std::endl;
		if (std::find(_data[j].getNames().begin(), _data[j].getNames().end(), _header.find("Host")->second) != _data[j].getNames().end() && std::to_string(_data[j].getPort()) == _port)
		{
			_nbServer = j;
			return ;
		}
		// std::vector<std::string> const & v = _data[j].getNames();
		// // _data[j].ge
		// std::cout << "here" << std::endl;
		// size_t	s = v.size();
		// std::cout << s << std::endl;
		// for (size_t i = 0; i < s; i++)
		// {
		// 	std::cout << v[i] << std::endl;
		// }
	}
	this->_nbServer = 0;
}

void	request::parseBody(std::string	reqBody)
{
	this->_bodyMessage = reqBody;
}

void	request::HeaderLine(std::string &reqHeader, std::string & Line,  size_t &i)
{
	size_t	j = reqHeader.find("\r\n", i);
	if (j == std::string::npos)
		errorHandler("400 Bad Request", *this);
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
		errorHandler("400 Bad Request", *this);
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
		// this->_header[key] = value;
		_header.insert(std::pair<std::string, std::string>(key, value));
		// std::cout << "key: |" << key << "|" <<  value << "|" << std::endl;
		if (reqHeader[i] == '\r' && reqHeader[i + 1] == '\n')
			break;
	}
	if (this->_header.count("Host") != 1)
		errorHandler("400 Bad Request", *this);
	else
	{
		std::string	hosTemp;
		hosTemp = _header.find("Host")->second;
		size_t	j = (hosTemp).find(':');
		std::cout << hosTemp << std::endl;
		int port;
		if (j != std::string::npos)
		{
			(_header.find("Host")->second).erase(j, hosTemp.length() - j);
			port = atoi((hosTemp).substr(j + 1, (hosTemp).length() - j + 1).c_str());
			if (port == 0)
				_port = "80";
			else
				_port = std::to_string(port);
		}
		else
			_port = "80";

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
		errorHandler("400 Bad Request", *this);
	this->_reqMethod = reqLine.substr(0, i);
}

void    request::hundleUri(std::string &reqLine, size_t & j)
{
	size_t i = reqLine.find_first_of(' ', j);
	// std::cout << i << std::endl;
	if (i == std::string::npos || reqLine[i + 1] == ' ')
		errorHandler("400 Bad Request", *this);
	this->_reqUri = reqLine.substr(j, i - j);
	j = i;
	i = _reqUri.find('?');
	this->_query = _reqUri.substr(i + 1, _reqUri.length() - i + 1);
	this->_path = _reqUri.substr(0, i);
}

void    request::hundleHttpv(std::string & reqLine, size_t & j)
{
	size_t  i = reqLine.find("\r\n", j);
	if (i == std::string::npos || reqLine[i + 1] == ' ')
		errorHandler("400 Bad Request", *this);
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
	std::cout << "path: |" << this->_path << "|" << std::endl;
	std::cout << "query: |" << this->_query << "|" << std::endl;
	std::cout << "port: |" << this->_port << "|" << std::endl;
	std::cout << "number of server: " << _nbServer << std::endl;
	std::cout << "nubmer of location: " << _nbLocation << std::endl;
	std::cout << "HTTP-version: |" << this->_httpVersion << "|" << std::endl;
	std::map<std::string, std::string>::iterator	it = _header.begin();
	// std::cout << _header.size() << std::endl;
	while (it != _header.end())
	{
		std::cout << "key: |" << it->first << "|\t" << "value: |" << it->second << "|" << std::endl;
		++it;
	}
	// std::cout << "reqbody: |" << this->_bodyMessage << "|" << std::endl;
}

void	errorHandler(std::string	msgError, request & req)
{
	response	errorRsp;

	errorRsp._statusLine = HTTPV1;
	errorRsp._statusLine += SPACE;
	errorRsp._statusLine += msgError;
	errorRsp._headers["Connection"] = "close";
	errorRsp._headers["Content-Type"] = "text/html; charset=UTF-8";
	
	errorRsp._body = "<!DOCTYPE html>";
	errorRsp._body += CRLF;
	errorRsp._body += "<html>";
	errorRsp._body += CRLF;
	errorRsp._body += "<head>";
	errorRsp._body += CRLF;
	errorRsp._body += "<title>";
	errorRsp._body += msgError;
	errorRsp._body += "</title>";
	errorRsp._body += CRLF;
	errorRsp._body += "</head>";
	errorRsp._body += CRLF;
	errorRsp._body += "<body>";
	errorRsp._body += CRLF;
	errorRsp._body += "<h1>";
	errorRsp._body += msgError;
	errorRsp._body += "</h1>";
	errorRsp._body += CRLF;
	errorRsp._body += "</body>";
	errorRsp._body += CRLF;
	errorRsp._body += "</html>";
	errorRsp._body +=  CRLF;
	errorRsp._body += CRLF;
	errorRsp._headers["Content-Length"] = std::to_string(errorRsp._body.length());
	makeResponse(errorRsp, req);

}

request::~request()
{

}
