#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <fstream>
#include <limits.h>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <cctype>
#include <map>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <arpa/inet.h>
#include <cstdio>
#include <dirent.h>
#include <cstdlib>

//Med hrs includes
#include <sys/stat.h>


#include "Location.hpp"
#include "ServerData.hpp"
#include "ConfigParser.hpp"
#include "request.hpp"

#include "socket.hpp"
#include "IOhandler.hpp"
#include "Webserver.hpp"


#define LOGS_FILE "webserv_logs"

#ifndef DEBUG
# define DEBUG 1
#endif

void outputLogs(std::string);

#define log std::cout <<
#define line << std::endl

#endif
