#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <iostream>
#include <map>
#include <sstream>
#define HTTPV1 "HTTP/1.1"
// #define HTTPV2 "HTTP/2"
#define SPACE " "
#define CRLF "\r\n"
#define B_N "\n"

#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */

class response
{
public:
	std::string	_statusLine;
	std::map<std::string, std::string> _headers;
	std::multimap<std::string, std::string> 	_cookie;
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
	t_blockPost(){};
}				s_blockPost;


class request
{
public:
	std::string									_responseStr;
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
	std::vector<s_blockPost>					blockPost;
	std::string									_queryStr;
	int											_nbServer;
	int											_nbLocation;

public:
	request();
	request(std::vector<ServerData>);
	request(const request &req);
	~request();
	request &operator=(const request & req);

	std::string	getBody() const {return _bodyMessage;};
	// by Youssef ( find and return if a hedear  exist by key and return its value)
	std::pair<bool, std::map<std::string, std::string>::const_iterator> getHeaderOf(std::string req) const;
	std::string	getPostData();
	bool	after_sgi_string(response & response);
	// bu Youssef initialize variable after im done;

	void		initialize(void);
	void		requestParser(std::string req);
	std::string	parseRequestLine(std::string);
	std::string	parseBody(std::string);
	void		findServer();
	void		findLocations();
	bool		handleRequests();

	bool		GETRequest();
	bool		POSTRequest();
	bool		DELETERequest();
	std::string	autoIndexGenerator(response &, std::string);
	void		isRedirection();
	std::string	boundaryParser(std::string, std::string);
	bool		errorHandler(std::string	msgError);

	std::string const & getResponse() const;
	std::string const & getQueryString() const;
	void		setResponse(std::string & str);

	std::string	hundleMethod(std::string& , size_t &);
	std::string	hundleUri(std::string& , size_t &);
	std::string	hundleHttpv(std::string& , size_t &);	
	std::string	HeaderLine(std::string&, std::string & , size_t&);
	std::string	headerInMap(std::string &, std::string &, std::string &);
	std::string	parseReqHeader(std::string);
};

bool    findInDir(std::string & path, std::string & root);
void	makeResponse(response & response, request & req);
#endif
