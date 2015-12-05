#pragma once
#include "ep_define.h"
#include <string>

class ep_package;

class ep_writer : public IEP_WRITER
{
public:
	ep_writer();
	~ep_writer();

	virtual int package_dir(const char* pkg_dir, const char* file_dir, const char mode);

protected:
	int analyze_dir(const std::string dir);

	void new_a_file_entity(const char* file_relative_path, const uint32_t file_size);

	int write_package();

	long write_package_header(long offset);

	long update_package(long offset);

	int write_dir_to_package(long offset);

private:
	std::string					_file_root_dir;
	ep_package*					_p_ep_package;

	MAP_EP_FILE_ENTITY_EX		_map_ep_files_to_pack;
};

