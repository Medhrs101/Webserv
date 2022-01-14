#ifndef REQUEST_HPP
#define REQUEST_HPP
#include <iostream>
#include <map>
#include <sstream>
class request
{
private:
	std::stringstream	_reqstr;
	std::string	_reqLine;
	std::string _reqMethod;
	std::string _reqUri;
	std::string	_httpVersion;
	std::string	*_reqHeader;
	std::map<std::string, std::string> 	_header;

public:
	request();
	request(std::string );
	~request();

	void	getRequestLine();
	void	hundleReqLine();
	class ErrorException : public std::exception
	{
		private:
			const char	*_message;
			virtual const char* what() const throw();
		public:
			ErrorException(const char *message);
	};

	//Methods for debuging
	void	printReqData();
};


#endif /* REQUEST_HPP */
