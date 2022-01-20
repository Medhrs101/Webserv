#include "../includes/Webserv.hpp"


int main(int ac, char **av)
{
	try
	{
		ConfigParser parser(ac, av);
		request req("GET ///// HTTP/1.1\r\n\r\n", parser.getServers());
	}
	catch (std::exception &e)
	{
		log "Error: " + std::string(e.what()) line;
		outputLogs("Error: " + std::string(e.what()));
	}
	return 0;
}
