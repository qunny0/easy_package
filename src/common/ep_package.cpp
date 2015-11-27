#include "ep_package.h"
#include "ep_utils.h"
#include <io.h>

ep_package* ep_package::create_package(const char* package_dir)
{
	ep_package* package = new ep_package();
	
	if (package->parse(package_dir) != 0) {
		EP_SAFE_DELETE(package);
	}

	return package;
}

ep_package::ep_package()
{
	_v_ep_files.clear();
}

ep_package::~ep_package()
{
	_v_ep_files.clear();
}

int ep_package::parse(const char* package_dir)
{
	_package_dir = package_dir;

	bool pkg_exist = dir_valid(package_dir, 0) == 0;
	
	if (pkg_exist)
	{
		return parse_package();
	}

	return 0;
}

int ep_package::parse_package()
{
	_finddata_t findData;
	_findfirst(_package_dir.c_str(), &findData);
	long package_size = findData.size;

	long offset = 0;

	// read ep_package info
	uint32_t size = EP_VERSION_LENGTH;
	char* buf = new char[size];
	ep_read(_package_dir.c_str(), offset, size, buf);
	offset += size;
	if (strcmp(buf, EP_VERSION) != 0)
		goto ERROR;
	EP_SAFE_DELETE_ARR(buf);

	// ep_header
	size = sizeof(EPHeader);
	buf = new char[size];
	ep_read(_package_dir.c_str(), offset, size, buf);
	EPHeader* p_header = (EPHeader*)buf;
	_ep_header.compressed = p_header->compressed;
	offset += size;
	EP_SAFE_DELETE_ARR(buf);

	// ep_file_entity
	unsigned int file_entity_size = sizeof(EPFileEntity);
	while (offset < package_size)
	{
		buf = new char[file_entity_size];

		// EPFileEntityEx		-- EPFileEntity
		if (ep_read(_package_dir.c_str(), offset, file_entity_size, buf) != 0){
			break; goto ERROR;
		}
		EPFileEntityEx file_entity_ex;
		EPFileEntity* p_file_entity = (EPFileEntity*)buf;
		file_entity_ex.offset = p_file_entity->offset;
		file_entity_ex.relative_path_size = p_file_entity->relative_path_size;
		file_entity_ex.data_size = p_file_entity->data_size;
		//
		offset += file_entity_size;
		EP_SAFE_DELETE_ARR(buf);

		// EPFileEntityEx		-- EPFileEntity
		buf = new char[file_entity_ex.relative_path_size];
		if (ep_read(_package_dir.c_str(), offset, file_entity_ex.relative_path_size, buf) != 0) {
			break; goto ERROR;
		}
		strncpy(file_entity_ex.relative_path, buf, file_entity_ex.relative_path_size);
		_v_ep_files.push_back(file_entity_ex);
		// 
		offset += file_entity_ex.relative_path_size;
		offset += file_entity_ex.data_size;
	}

// 	for (EPFileEntityEx file_entity : _v_ep_files)
// 	{
// 		printf("file relative path : %s \n", file_entity.relative_path);
// 	}
	return 0;
ERROR:
	EP_SAFE_DELETE_ARR(buf);
	return -1;
}

const std::vector<EPFileEntityEx>& ep_package::get_ep_file_info() const
{
	return _v_ep_files;
}

const EPHeader& ep_package::get_ep_header() const
{
	return _ep_header;
}

const std::string& ep_package::get_package_dir() const
{
	return _package_dir;
}
