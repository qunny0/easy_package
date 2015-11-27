#pragma once
#include "ep_define.h"
#include <string>
#include <vector>

class ep_manager
{
public:
	ep_manager();
	~ep_manager();
	
	// 
	void new_package(const char* dir, const char* package_name);

	void add_dir(const char* package_path, const char* dir);

	//
	int package(const char* dir);

	int package(const char* package_dir, const char* dir);

	int package(const char* package_dir, const char* dir, const char* mode);

	void write_package(const char* package_dir);

	int parse_package(const char* package_dir);

	void show_data(const char* package_dir, const char* relative_path);

	// input is direction
	void travel_dir(const std::string& dir);

private:
	std::string					_root_dir;
	EPHeader					_ep_header;
	std::vector<EPFileEntityEx>	_v_ep_files;
	std::string					_package_name;

	IEP_READER*					_reader;

	IEP_WRITER*					_ep_writer;
};

