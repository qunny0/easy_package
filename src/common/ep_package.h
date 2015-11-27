#pragma once
#include "ep_define.h"
#include <string>
#include <vector>

class ep_writer;

class ep_package
{
	friend ep_writer;

public:
	static ep_package* create_package(const char* package_dir);

	int parse(const char* package_dir);

	const std::vector<EPFileEntityEx>& get_ep_file_info() const;

	const EPHeader& get_ep_header() const;

	const std::string& get_package_dir() const;

protected:
	ep_package();
	~ep_package();
	// parse easy_package
	int parse_package();

private:
	std::string					_package_dir;
	EPHeader					_ep_header;
	std::vector<EPFileEntityEx>	_v_ep_files;
};

