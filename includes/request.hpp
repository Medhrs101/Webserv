#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <iostream>
#include <map>
#include <sstream>
#define HTTPV1 "HTTP/1.1"
#define SPACE " "
#define CRLF "\r\n"

#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

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

typedef struct	t_blockPost 
{
	bool 		isFile;
	std::string key;
	std::string filename;
	std::string value;
	t_blockPost()
	{
		
	}
}				s_blockPost;


class request
{
private:
	std::string									_reqstr;
	std::string									_reqLine;
	std::string 								_reqMethod;
	std::string 								_reqUri;
	std::string									_httpVersion;
	std::string									*_reqHeader;
	std::multimap<std::string, std::string> 	_header;
	std::string									_bodyMessage;
	std::string									_path;
	std::string									_query;
	std::string 								_port;
	std::vector<ServerData> 					_data;
	std::vector<Location>						_locations;
	std::string									_responseStr;
	std::vector<s_blockPost>					blockPost;
	std::string									_queryStr;
	int											_nbServer;
	int											_nbLocation;

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
	std::string	parseRequestLine(std::string);
	std::string	parseBody(std::string);
	void	findServer();
	void	findLocations();
	bool	handleRequests();

	bool	GETRequest();
	bool	POSTRequest();
	bool	DELETERequest();
	void	isRedirection();
	void	boundaryParser(std::string, std::string);

	std::string const & getResponse() const
	{
		return _responseStr;
	}
	std::string const & getQueryString() const
	{
		return _queryStr;
	}

	void	setResponse(std::string & str)
	{
		this->_responseStr = str;
	}

	std::string	hundleMethod(std::string& , size_t &);
	std::string	hundleUri(std::string& , size_t &);
	std::string	hundleHttpv(std::string& , size_t &);

	std::string	HeaderLine(std::string&, std::string & , size_t&);
	std::string	headerInMap(std::string &, std::string &, std::string &);
	std::string		parseReqHeader(std::string);

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
