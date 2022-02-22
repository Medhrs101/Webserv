/**
 * @Author: Your name
 * @Date:   2022-01-21 18:25:18
 * @Last Modified by:   Your name
 * @Last Modified time: 2022-02-19 15:09:31
 */
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
	this->_reqHeader = nullptr;
	this->_header.clear();
};
request::request(std::vector<ServerData> data): _data(data)
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
	std::string	ret;
	_reqstr = req;
	// std::cout << "request str: |" <<  _reqstr << "|" << std::endl;
	size_t i(0);
	i = _reqstr.find("\r\n");
	if (i == std::string::npos)
	{
		errorHandler("400 Bad Request");
		return ;
	}
	i = i + 2;
	if ((ret = parseRequestLine(_reqstr.substr(0, i))) != "OK")
	{
		errorHandler (ret);
		return;
	}
	size_t	j = _reqstr.find("\r\n\r\n");
	// std::cout << j << std::endl;
	if (j == std::string::npos)
	{
		errorHandler("400 Bad Request");
		return ;
	}
	j = j + 4;
	if ((ret = parseReqHeader(_reqstr.substr(i, j - i))) != "OK")
	{
		errorHandler (ret);
		return;
	}
	// std::cout << RED << "Client body size should respect: |" << _bodyMessage.length() << "|" << RESET << std::endl;

	this->findServer();
	// std::cout << "number of server: " << _nbServer <<std::endl;
	this->findLocations();
	// std::cout << "number of location: " << _nbLocation << std::endl;
	if ((ret = parseBody(_reqstr.substr(j, _reqLine.size() - j))) != "OK")
	{
		errorHandler (ret);
		return;
	}
		// std::cout << RED << "ATTENTION*********there is a seg-fault********" << RESET << std::endl;
	if (handleRequests())
	{
		// this->printReqData();
		return ;
	}
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
	respStr += CRLF;
	respStr += CRLF;
	respStr += response._body;
	respStr += "\r\n\r\n";
	if (respStr.length() < 1000)
	// std::cout << "begin resp |" << respStr  << "|end resp" << std::endl;
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

// void	isRedirection()
// {
	
// }

bool	request::GETRequest()
{
	response		response;
	std::ifstream 	file;
	std::string		returnCode = " 200 OK";
	std::string		errorMessage;
	struct stat		info;

	// std::cout << "path: | " << _path << std::endl;
	if (_locations[_nbLocation].isRedirection())
	{
		returnCode = std::to_string(_locations[_nbLocation].getReturnCode());
		returnCode += " Moved Permanently";
		response._statusLine = HTTPV1" " + returnCode;
		response._headers["Location"] = _locations[_nbLocation].getReturnUrl();
			goto bitbit;
	}
	if (_locations[_nbLocation]. getAllowedMethods().find("GET")->second == false)
		return errorHandler("405 Not Allowed");
	if (_locations[_nbLocation].getRootDir().empty())
		_path = _data[_nbServer].getRootDir() + _path;
	else
		_path = _locations[_nbLocation].getRootDir() + _path;
	pathCorrection(_path);
	// std::cout << "PATH: |" << _path << std::endl;
	if (stat(_path.c_str(), &info) == 0)
	{
		if (access(_path.c_str(), R_OK))
			return errorHandler("403 Forbidden");
		if (info.st_mode & S_IFDIR)
		{
			_path += "/" + _locations[_nbLocation].getDefaultFile();
			pathCorrection(_path);
		}
	}
	else
		return errorHandler("404 Not Found");
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
			return errorHandler("500 internal Server Error");
	}
	response._statusLine = HTTPV1;
	response._statusLine += " " + returnCode;
	// response._headers["Content-Type"] = contentType(_path);
	response._headers["Content-Length"] = std::to_string(response._body.length());
	if (_header.count("Connection") == 0 || _header.find("Connection")->second == "closed")
		response._headers["Connection"] = "closed";
	else
		response._headers["Connection"] = "keep-alive"; 
	
	bitbit:
	if (this->_path.substr(_path.find_last_of('.')) == ".php" || this->_path.substr(_path.find_last_of('.')) == ".py"){
		after_sgi_string(response);
	}
		makeResponse(response, *this);
	// _locations[_nbLocation].getDefaultFile()
	return true;
}

std::string getBoundary(std::string & str)
{
	size_t	i = str.find("boundary=");
	return str.substr(i + 9, str.length() - i - 9);

}

void	request::boundaryParser(std::string boundary, std::string str)
{
	// std::cout << 
	// std::cout << "boundary: |" << str << "|" << std::endl;
	size_t i = str.find("--" + boundary + "--");
	if (i == std::string::npos)
	{
		std::cout << RED"Error in the boundary assi Marji" << RESET << std::endl;
		exit(0);
		// errorHandler("405 Error in the boundary assi Marji");
		// return ;
	}
	str = str.substr(0, i + boundary.length() + 4);
	i = boundary.length() + 4;
	for (size_t z = 0; i < str.length(); z++)
	{
		t_blockPost		block = t_blockPost();
		block.isFile = false;
		size_t j = str.find("--" + boundary, i);
        // std::cout << RED"*************************begin********************************"RESET << std::endl;
        std::string content = str.substr(i , j - i);
		// std::cout << "body: |" << content << "|" << std::endl;
        size_t x;
        x = content.find("name=\"") + 6;
        while (content[x] != '\"')
            block.key += content[x++];
		// std::cout << "filename: " <<block.filename << std::endl;
        size_t y = x;
		if ((y = content.find("filename=\"")) != std::string::npos)
        {
            block.isFile = true;
            y += 10;
            while (content[y] != '\"')
                block.filename += content[y++];
            x = y;
        }
        x = content.find("\r\n\r\n", x) + 4;
        size_t len = content.length() - 2;
        while(x < len)
                block.value += content[x++];
        // std::cout << RED"***************************end******************************"RESET << std::endl;
        // std::cout << RED"name: |" << block.key << "| isFile: |" << block.isFile << "| filename: |" << block.filename << "| data: |" << block.value << "|"RESET << std::endl;
        i = j + boundary.length() + 4;
		blockPost.push_back(block);
	}
}

bool	request::POSTRequest()
{
	response		resp;
	std::ofstream	output;
	std::string		pathTemp;
	struct stat info;
	blockPost.clear();
	_queryStr.clear();

	if (_locations[_nbLocation].getRootDir().empty())
		_path = _data[_nbServer].getRootDir() + _path;
	else
		_path = _locations[_nbLocation].getRootDir() + _path;
	if (stat(_path.c_str(), &info) == 0)
	{
		if (access(_path.c_str(), R_OK))
			return errorHandler("403 Forbidden");
	}
	else
		return errorHandler ("404 Not Found");
	if (_locations[_nbLocation].isCGI())
	{
		pathTemp = _path;
		_path = pathTemp.substr(0, _path.find_last_of("/") + 1);
		findLocations();
	}
	if (_locations[_nbLocation].getAllowedMethods().find("POST")->second == false \
		|| _locations[_nbLocation].getUploadEnabled() == false)
		return errorHandler("405 Not Alowed");
	std::string	uploadpath = _locations[_nbLocation].getUploadLocation();
	uploadpath = _data[_nbServer].getRootDir() + uploadpath + "/";
	std::string	ContentType = _header.find("Content-Type")->second;
	// std::cout << "uploadPath: ||" << uploadpath << std::endl;
	if (ContentType.find("multipart/form-data") != std::string::npos)
	{
		// std::cout << "***********************begin****************************<\n"GREEN << this->_bodyMessage << RESET"\n>*****************end********************" << std::endl;
		std::string boundary = getBoundary(ContentType);
		boundaryParser(boundary, _bodyMessage);
		for (size_t i = 0; i < blockPost.size(); i++)
		{
			// std::cout << MAG <<"fileeeeeeee: |"<< blockPost[i].isFile << "|" << RESET << std::endl;
			if (blockPost[i].isFile)
			{
				blockPost[i].filename = uploadpath +  blockPost[i].filename;
				output.open(blockPost[i].filename);
				pathCorrection(blockPost[i].filename);
				if (output.is_open())
				{
					output << blockPost[i].value;
					output.close();
				}
				else
					return errorHandler("500 internal Server Error pew");
			}
		}
	}
	else if (ContentType.find("application/x-www-form-urlencoded") != std::string::npos)
	{
		// std::cout << "***************************************************<\n"GREEN << this->_bodyMessage << RESET"\n>*************************************" << std::endl;
		// std::string boundary = getBoundary(ContentType);
		// boundaryParser(boundary, _bodyMessage);
		// for (size_t i = 0; i < blockPost.size(); i++)
		// {
		// 	std::cout << "key: |" << blockPost[i].key << "| value: |" << "|" << blockPost[i].value << std::endl;
		// }
		_queryStr = _bodyMessage;
		// errorHandler("500 Tkays a chabab mzl ma sowebna hadi");
		// return ;
	}
	// else
	// 	return errorHandler("415 Unsupported Media Type");
	
	_path = pathTemp;
	resp._statusLine = HTTPV1;
	resp._statusLine += " 200 OK";
	resp._body = "<html><body><h1>The Media has been uploaded successfully</h1></body></html>";
	resp._headers["Content-Length"] = std::to_string(resp._body.length());
	resp._headers["Content-Type"] = "text/html";
	if (_header.count("Connection") == 0 || _header.find("Connection")->second == "closed")
		resp._headers["Connection"] = "closed";
	else
		resp._headers["Connection"] = "keep-alive";
	if (_path.find_last_of('.') != std::string::npos){
		if (this->_path.substr(_path.find_last_of('.')) == ".php" || this->_path.substr(_path.find_last_of('.')) == ".py"){
			after_sgi_string(resp);
		};
	}
	std::cout << RED << "ATTENTION*********there is a seg-fault********" << RESET << std::endl;
	makeResponse(resp, *this);
	return true;
}

// void	pathFix(std::string path)
// {
// 	for (size_t i = path.length(); i > 0; i--)
// 	{
		
// 	}
	
// }

bool	request::DELETERequest()
{
	response	resp;
	struct stat info;
	
	if (_locations[_nbLocation].getAllowedMethods().find("POST")->second == false)
		return errorHandler ("405 Not Allowed");
	if (_locations[_nbLocation].getRootDir().empty())
		_path = _data[_nbServer].getRootDir() + _path;
	else
		_path = _locations[_nbLocation].getRootDir() + _path;
	pathCorrection(_path);
	if (stat(_path.c_str(), &info) == 0)
	{
		std::cout << "path: |" << _path << std::endl;
		std::cout << "path: |" << _data[_nbServer].getRootDir() << std::endl;
		// pathFix(_path);
		if (_path.find(".."))
			return errorHandler("404 Not Found");
		if (_path.find(_data[_nbServer].getRootDir()) != std::string::npos)
		{
			if (remove(_path.c_str()) != 0)
				return errorHandler("500 Internal Server Error");
		}
		else
			return errorHandler("400 you can only remove file from root");
			
	}
	else
		return errorHandler("404 Not Found");
	resp._statusLine = HTTPV1;
	resp._statusLine += " 200 OK";
	resp._body = "<html><body><h1>The file was removed successfully</h1></body></html>";
	resp._headers["Content-Length"] = std::to_string(resp._body.length());
	resp._headers["Content-Type"] = "text/html";
	makeResponse(resp, *this);
	return true;
}

bool	request::handleRequests()
{
	//TODO: allowed method
	if (_reqMethod == "GET")
		return GETRequest();
	else if (_reqMethod == "POST")
		return POSTRequest();
	else if (_reqMethod == "DELETE")
		return DELETERequest();
	else
		return errorHandler("404 Bad request");
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
	// std::cout << i << std::endl;
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

std::string	request::parseBody(std::string	reqBody)
{
	this->_bodyMessage = reqBody;
	// std::cout << "hello" << std::endl;
	if (_bodyMessage.length() > _data[_nbServer].getClientBodySize() * 1000000)
	{
		return ("413 Payload Too Large");
	}
	return ("OK");
}


void	trim(std::string &str)
{
	while (!str.empty() && isspace(str.front()))
		str.erase(0, 1);
	while (!str.empty() && isspace(str.back()))
		str.erase(str.size() - 1, 1);
}

std::string	request::HeaderLine(std::string &reqHeader, std::string & Line,  size_t &i)
{
	size_t	j = reqHeader.find("\r\n", i);
	if (j == std::string::npos)
		return ("400 Bad Request");
	Line = reqHeader.substr(i, j - i);
	i = j + 2;
	return "OK";
}

std::string	request::headerInMap(std::string & Line, std::string & key, std::string & value)
{
	size_t	i = Line.find(":");
	if (i == std::string::npos)
		return ("400 Bad Request");
	key = Line.substr (0, i);
	value = Line.substr(i + 1, Line.size() - i + 1);
	trim(value);
	return "OK";
}


std::string	request::parseReqHeader(std::string reqHeader)
{
	// std::cout << "Header fields: |" << reqHeader << "|" << std::endl;
	std::string	Line;
	std::string key;
	std::string value;
	std::string ret;

	size_t i(0);
	while (1)
	{
		if ((ret = HeaderLine(reqHeader, Line, i)) != "OK")
			return ret;
		if ((ret = headerInMap(Line, key, value)) != "OK")
			return ret;
		// this->_header[key] = value;
		_header.insert(std::pair<std::string, std::string>(key, value));
		// std::cout << "key: |" << key << "|" <<  value << "|" << std::endl;
		if (reqHeader[i] == '\r' && reqHeader[i + 1] == '\n')
			break;
	}
	if (this->_header.count("Host") != 1)
		return ("400 Bad Request");
	else
	{
		std::string	hosTemp;
		hosTemp = _header.find("Host")->second;
		size_t	j = (hosTemp).find(':');
		// std::cout << hosTemp << std::endl;
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
	return "OK";
}

std::string    request::parseRequestLine(std::string reqLine)
{
	// std::cout << "reqLine: <" << reqLine << ">" << std::endl;
	size_t i(0);
	std::string ret;
	if ((ret = hundleMethod(reqLine, i)) != "OK")
		return ret;
	if ((ret = hundleUri(reqLine, ++i)) != "OK")
		return ret;
	if ((hundleHttpv(reqLine, ++i)) != "OK")
		return ret;
	return "OK";
	/*NOTE: After getting all values of the request Line you should verificate it carefully*/
}

std::string    request::hundleMethod(std::string & reqLine, size_t & i)
{
	i = reqLine.find_first_of(' ');
	if (i == std::string::npos || reqLine[i + 1] == ' ')
		return ("400 Bad Request");
	this->_reqMethod = reqLine.substr(0, i);
	return "OK";
}


std::string    request::hundleUri(std::string &reqLine, size_t & j)
{
	size_t i = reqLine.find_first_of(' ', j);
	// std::cout << i << std::endl;
	if (i == std::string::npos || reqLine[i + 1] == ' ')
		return ("400 Bad Request");
	this->_reqUri = reqLine.substr(j, i - j);
	j = i;
	i = _reqUri.find('?');
	if (i != std::string::npos)
		this->_query = _reqUri.substr(i + 1, _reqUri.length() - i + 1);
	else
		this->_query = "";
	this->_path = _reqUri.substr(0, i);
	return "OK";
}

std::string    request::hundleHttpv(std::string & reqLine, size_t & j)
{
	size_t  i = reqLine.find("\r\n", j);
	if (i == std::string::npos || reqLine[i + 1] == ' ')
		return ("400 Bad Request");
	_httpVersion = reqLine.substr(j, i - j);
	if (_httpVersion != HTTPV1)
		return ("400 Bad Request");
	return "OK";
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

void	fillError(response & errorRsp, std::string & msgError)
{
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
}

bool	request::errorHandler(std::string	msgError)
{
	response	errorRsp;
	int			statusCode = std::stoi(msgError);
	errorRsp._statusLine = HTTPV1;
	errorRsp._statusLine += SPACE;
	errorRsp._statusLine += msgError;
	errorRsp._headers["Connection"] = "close";
	errorRsp._headers["Content-Type"] = "text/html;";
	std::cout  << "statusCode: ||"<< statusCode << std::endl;
	std::cout << "hello from here: || " << _nbServer << "|"<< std::endl;

	if (_data[_nbServer].getErrorPageMap().count(statusCode) == 1)
	{
		std::string	errorPath  = _data[_nbServer].getErrorPageMap().find(statusCode)->second;
		errorPath = _data[_nbServer].getRootDir() + errorPath;
		pathCorrection(errorPath);
		std::ifstream	file;
		std::ostringstream streambuff;
		std::cout << "errorPtah: ||" << errorPath << std::endl;
		file.open(errorPath, std::ios::binary);
		if (file.is_open())
		{
			streambuff << file.rdbuf();
			errorRsp._body = streambuff.str();
			// std::cout << "hello from here: || " << errorRsp._body << std::endl;
			file.close();
		}
		else
		{
			msgError = "500 internal Server Error";
			fillError(errorRsp, msgError);
		}
	}
	else
		fillError(errorRsp, msgError);
	errorRsp._headers["Content-Length"] = std::to_string(errorRsp._body.length());
	makeResponse(errorRsp, *this);
	return false;
}

request::~request()
{

}

std::string	request::getPostData(){
	std::string query;

	if (!this->_queryStr.empty()){
		query = this->_queryStr;
		return query;
	}
	log BLU << "*******************************************" << RESET line;
	for(int i = 0; i < blockPost.size(); i++){
		if (blockPost[i].isFile){
			query += blockPost[i].key + "=" + blockPost[i].filename;
		}
		else{
			query = blockPost[i].key + "=" + blockPost[i].value;
		}
		if (i + 1 != blockPost.size())
			query.append("&");
	}
	return query;
}

std::string upCase(std::string str){
	std::string ret;
	for (size_t i = 0; i < str.length(); i++)
	{
		ret.append(1, toupper(str[i]));
	}
	return ret;
}

void	request::after_sgi_string(response & response){
    std::string body;
	std::string	data;
	char	buff[1024] = {0};
	pid_t pid;
    int res;
    int Opipe[2];
	int Ipipe[2];
	std::string ext = this->_path.substr(_path.find_last_of('.'));

	std::vector<const char *> args;
	for (int i = 0; i < this->_locations.size(); i++){
		if (this->_locations[i].isCGI()){
			if (this->_locations[i].getPath() == ext){
				args.push_back(this->_locations[i].getFastCgiPass().c_str());
				if (ext == ".py" || ext == ".php")
					args.push_back(this->_path.c_str());
				args.push_back(NULL);
				break ;
			}
		}
	}
	int i = 0;
    if (pipe(Opipe) < 0)
        throw	std::runtime_error("FATAL: internal error, try again!");
	if (pipe(Ipipe) < 0)
        throw	std::runtime_error("FATAL: internal error, try again!");
	
	std::cout << RED << "*******************************************" << std::endl;
	
	while (args[i])
	{
		std::cout << args[i] << std::endl;
		i++;
	}

	log "REQUEST_METHOD " << this->_reqMethod.c_str() line;
	log "SCRIPT_FILENAME " << this->_path.c_str()line;
	log "REQUEST_METHOD " << this->_data[_nbServer].getRootDir().c_str() line;

	std::cout << "*******************************************" << RESET << std::endl;
	setenv("SERVER_SOFTWARE", "WebServer/med&marji", 1);
	setenv("GATEWAY_INTERFACE", "CGI/1.1",1);
	setenv("SERVER_PROTOCOL", "HTTP/1.1",1);
	setenv("REQUEST_METHOD", this->_reqMethod.c_str(), 1);
	setenv("SCRIPT_FILENAME", this->_path.c_str(), 1);
	setenv("PATH_INFO", this->_data[_nbServer].getRootDir().c_str(), 1);
	setenv("SERVER_PORT", std::to_string(this->_data[_nbServer].getPort()).c_str(), 1);
	setenv("REDIRECT_STATUS", "0", 1);
	setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", 1);
	for(std::multimap<std::string, std::string>::iterator i = this->_header.begin(); i != this->_header.end(); i++){
		setenv( ("HTTP_" + upCase(i->first)).c_str(), (i->second).c_str(), 1);
	}
	if (this->_reqMethod == "GET"){
		setenv("QUERY_STRING", this->_query.c_str(), 1);
	}
	else if (this->_reqMethod == "POST"){
		data = getPostData();
		setenv("CONTENT_LENGTH", std::to_string(data.length()).c_str() , 1);
	}

    extern char **environ;
	int IN_FD = dup(0);
	int OUT_FD = dup(1);
    pid = fork();
    if (pid < 0) 
        throw std::runtime_error("FATAL: internal error, try again!");
    else if (pid == 0){
        close(Opipe[0]);
		close(Ipipe[1]);
        if (this->_reqMethod == "POST"){
			if (dup2(Ipipe[0], 0) < 0){
				std::cerr << "Error : CGI crash, try again -dup2 I -" << std::endl;
			}
		}
		close(Ipipe[0]);
		if (dup2(Opipe[1], 1) < 0){
			std::cerr << "Error : CGI crash, try again -dup2 O -" << std::endl;
		}
		dup2(Opipe[1], 1);
		close(Opipe[1]);
        if (execve(args[0], const_cast<char *const *>(&args[0]), environ) < 0){
			std::cerr << "Error : CGI crash, try again -execve-" << std::endl;
			throw std::runtime_error(strerror(errno));
		}
		exit(EXIT_FAILURE);
    }
    else{
		close(Opipe[1]);
		close(Ipipe[0]);
		int res =  ::write(Ipipe[1], data.c_str(),data.length());
		close(Ipipe[1]);
		while ((res = read(Opipe[0], buff, sizeof(buff)))){
            body.append(buff, res);
		}
		wait(NULL);
		char *header = ::strdup(body.substr(0, body.find("\n\r")).c_str());
		char *token = ::strtok(header, "\n");
		while (token != NULL)
		{
			{
				std::string stoken = token;
				std::string key = stoken.substr(0, stoken.find(":"));
				std::string value = stoken.substr(stoken.find(":") + 1);
				response._headers[key] = value.substr(0, value.find_first_of('\r'));
			}
			token = ::strtok(NULL, "\n");
		}
		response._body = body.substr(body.find("\r\n\r\n") + 4);
		response._headers["Content-Length"] = std::to_string(response._body.size());
		close(Opipe[0]);
    }
}