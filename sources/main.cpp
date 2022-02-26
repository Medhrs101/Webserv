#include "../includes/Webserv.hpp"

void	rootVer(std::vector<ServerData> const & data){
	for (size_t i = 0; i < data.size(); i++){
		struct stat info;
		if(stat(data[i].getRootDir().c_str(), &info) != 0){
			throw std::runtime_error("Incorrect main root;");
		}
		for(size_t j = 0; j < data[i].getLocations().size(); j++){
			if (!data[i].getLocations()[j].getRootDir().empty()){
				if(stat(data[i].getLocations()[j].getRootDir().c_str(), &info) != 0){
					throw std::runtime_error("Incorrect root;");
				}
			}
		}
	}
}

int main(int ac, char **av)
{
	try
	{
		ConfigParser parser(ac, av);

		rootVer(parser.getServers());
		server Server(parser.getServers());
        Server.init();

	}
	catch (std::exception &e)
	{
		log "Error: " + std::string(e.what()) line;
		outputLogs("Error: " + std::string(e.what()));
	}
	return 0;
}
