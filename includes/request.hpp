#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <iostream>
#include <map>
#include <sstream>
class request
{
private:
	std::string	_reqstr;
	std::string	_reqLine;
	std::string _reqMethod;
	std::string _reqUri;
	std::string	_httpVersion;
	std::string	*_reqHeader;
	std::map<std::string, std::string> 	_header;
	std::string	_bodyMessage;
	int			_statusCode;
	std::vector<ServerData> _data;

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
