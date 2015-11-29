#pragma once
#include "ep_define.h"
#include <string>
#include <vector>

class ep_manager
{
public:
	ep_manager();
	~ep_manager();

	int package(const char* package_dir, const char* dir, const char* mode);

	int parse_package(const char* package_dir);

	int package_export(const char* package_dir, const char* export_dir);

	void show_tips();

	//
	void test();

// 	int analyze_dir(const std::string dir);

private:
	IEP_READER*					_reader;
	IEP_WRITER*					_ep_writer;
};

