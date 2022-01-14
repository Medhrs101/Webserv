#include "fstream"
#include "request.hpp"

int main()
{
	try
	{
		std::ifstream inFile("reqExample.txt");
		std::string	line;
		std::string buffer;
		while(getline(inFile, line))
			buffer = buffer + line + "\n";

		// std::cout << "buffer: |" << buffer << "|" << std::endl;
		request *req = new request(buffer);
		req->printReqData();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	return (0);
}