#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <iostream>
#include <map>
#include <sstream>
#define HTTPV1 "HTTP/1.1"
#define SPACE " "
#define CRLF "\r\n"

class response
{
public:
	std::string	_statusLine;
	std::map<std::string, std::string> _headers;
	std::string	_body;
public:
	response() {}
	~response() {}
};

class request
{
private:
	std::string	_reqstr;
	std::string	_reqLine;
	std::string _reqMethod;
	std::string _reqUri;
	std::string	_httpVersion;
	std::string	*_reqHeader;
	std::multimap<std::string, std::string> 	_header;
	std::string	_bodyMessage;
	int			_statusCode;
	std::string	_path;
	std::string	_query;
	std::string _port;
	std::vector<ServerData> _data;
	std::vector<Location>	_locations;
	std::string				_responseStr;

	int			_nbServer;
	int			_nbLocation;

public:
	request();
	request(std::vector<ServerData>);
	~request();

	std::string	getBody() const {return _bodyMessage;};
	// by Youssef ( find and return if a hedear  exist by key and return its value)
	std::pair<bool, std::map<std::string, std::string>::const_iterator> getHeaderOf(std::string req) const;
	// bu Youssef initialize variable after im done;
	void	initialize(void);
	void	requestParser(std::string req);
	void	parseRequestLine(std::string);
	void	parseReqHeader(std::string);
	void	parseBody(std::string);
	void	findServer();
	void	findLocations();
	void	handleRequests();
	void	GETRequest();
	void	POSTRequest();
	void	isRedirection();

	std::string const & getResponse() const
	{
		return _responseStr;
	}

	void	setResponse(std::string & str)
	{
		this->_responseStr = str;
	}

	void	hundleMethod(std::string& , size_t &);
	void	hundleUri(std::string& , size_t &);
	void	hundleHttpv(std::string& , size_t &);

	void	HeaderLine(std::string&, std::string & , size_t&);
	void	headerInMap(std::string &, std::string &, std::string &);

	// void	getRequestLine();
	class ErrorException : public std::exception
	{
		private:
			const char	*_message;
			virtual const char* what() const throw();
		public:
			ErrorException(const char *message);
	};

	//Methods for debuging It will be deleted later
	void	printReqData();
};


#endif
