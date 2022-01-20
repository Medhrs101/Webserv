#include "../includes/Webserv.hpp"


ServerData::ServerData() : _client_body_size(2)
{
}

ServerData::ServerData(ServerData const &rhs)
{
	*this = rhs;
}

ServerData &ServerData::operator=(ServerData const &rhs)
{
	if (this != &rhs)
	{
		_port = rhs._port;
		_ports = rhs._ports;
		_host = rhs._host;
		_names = rhs._names;
		_client_body_size = rhs._client_body_size;
		_error_pages = rhs._error_pages;
		_root_dir = rhs._root_dir;
		_locations = rhs._locations;
		for (size_t i = 0; i < NUMBER_OF_NECESSARY_ELEMENTS; i++)
		{
			_necessary_elements[i] = rhs._necessary_elements[i];
		}
	}
	return *this;
}

ServerData::~ServerData()
{
	_ports.clear();
	_error_pages.clear();
	_locations.clear();
}

void ServerData::activateParsingMode()
{
	for (size_t i = 0; i < NUMBER_OF_NECESSARY_ELEMENTS; i++)
	{
		_necessary_elements[i] = false;
	}
	_necessary_elements_identifiers.push_back("listen {port}");
	_necessary_elements_identifiers.push_back("host");
	_necessary_elements_identifiers.push_back("root");
	_error_pages.clear();
}

void ServerData::setPort(int const &port)
{
	_port = port;
}

int const &ServerData::getPort() const
{
	return _port;
}

void ServerData::addPort(int const &p)
{
	for (size_t i = 0; i < _ports.size(); i++)
	{
		if (_ports[i] == p)
			throw std::invalid_argument("Error: duplicate port number [" + std::to_string(p) + "]");
	}
	_ports.push_back(p);
	_necessary_elements[PORT_NECESSITY_NUMBER] = true;
}

std::vector<int> const &ServerData::getPorts() const
{
	return _ports;
}

void ServerData::setHost(std::string const &h)
{
	_host = h;
	_necessary_elements[HOST_NECESSITY_NUMBER] = true;
}

std::string const &ServerData::getHost() const
{
	return _host;
}

void ServerData::setNames(std::vector<std::string> const &names)
{
	std::set<std::string> namesSet;
	for (size_t i = 0; i < names.size();)
	{
		namesSet.insert(names[i++]);
		if (namesSet.size() != i)
			throw std::runtime_error("Error: duplicate server name [ " + names[i - 1] + " ]");
	}
	_names = names;
}

std::vector<std::string> const &ServerData::getNames() const
{
	return _names;
}

void ServerData::setClientBodySize(int const &cbz)
{
	_client_body_size = cbz;
}

int const &ServerData::getClientBodySize() const
{
	return _client_body_size;
}

void ServerData::addErrorPage(int const &code, std::string const &path)
{
	if (path.empty())
		throw std::invalid_argument("empty path for error page");
	for (std::map<int, std::string>::iterator it = _error_pages.begin(); it != _error_pages.end(); it++)
		if (it->first == code)
			throw std::invalid_argument("Error code is not unique for the error page which has the path: " + path);
	_error_pages.insert(std::pair<int, std::string>(code, path));
}

std::map<int, std::string> const &ServerData::getErrorPageMap() const
{
	return _error_pages;
}

void ServerData::setRootDir(std::string const &rd)
{
	_root_dir = rd;
	_necessary_elements[ROOT_NECESSITY_NUMBER] = true;
}

std::string const &ServerData::getRootDir() const
{
	return _root_dir;
}

void ServerData::addLocation(Location const &loc)
{
	for (size_t i = 0; i < _locations.size(); i++)
	{
		if (_locations[i].getPath() == loc.getPath())
			throw std::invalid_argument("Error: duplicated location path: " + loc.getPath());
	}
	_locations.push_back(loc);
}

std::vector<Location> const &ServerData::getLocations() const
{
	return _locations;
}

std::vector<std::string> ServerData::hasNecessaryElements() const
{
	std::vector<std::string> missing_elements;
	for (size_t i = 0; i < NUMBER_OF_NECESSARY_ELEMENTS; i++)
	{
		if (!_necessary_elements[i])
		{
			missing_elements.push_back(_necessary_elements_identifiers[i]);
		}
	}
	return missing_elements;
}

std::ostream &operator<<(std::ostream &out, const ServerData &sv)
{
	out << "\n==================== ServerData ===================" << std::endl;
	out << "server port: [" << sv.getPort() << "]" << std::endl;
	std::vector<int> ports = sv.getPorts();
	out << "all ports: ";
	for (size_t i = 0; i < ports.size(); i++)
	{
		out << "[" << ports[i] << "]  ";
	}
	out << std::endl;
	out << "host :[" << sv.getHost() << "]" << std::endl;
	std::vector<std::string> names = sv.getNames();
	out << "names: ";
	for (size_t i = 0; i < names.size(); i++)
	{
		out << "[ " << names[i] << " ]  ";
	}
	out << std::endl;
	out << "client body size: [" << sv.getClientBodySize() << "]" << std::endl;
	std::map<int, std::string> map(sv.getErrorPageMap());
	for (std::map<int, std::string>::iterator it = map.begin(); it != map.end(); it++)
	{
		out << "error page: code [" << it->first << "], path [" << it->second << "]" << std::endl;
	}
	out << "root dir: [" << sv.getRootDir() << "]" << std::endl;
	std::vector<Location> const &locs = sv.getLocations();
	for (size_t i = 0; i < locs.size(); i++)
	{
		out << locs[i];
	}
	out << "==================== ServerData End =================" << std::endl;
	return out;
}
