#include "ep_manager.h"

#include "ep_utils.h"
#include "ep_reader.h"
#include "ep_writer.h"

#include <io.h>

ep_manager::ep_manager()
{
	_reader = nullptr;
	_ep_writer = new ep_writer();
}

ep_manager::~ep_manager()
{
	EP_SAFE_DELETE(_reader);
	EP_SAFE_DELETE(_ep_writer);
}

// int ep_manager::package(const char* dir)
// {
// 	_ep_header.compressed = EP_UNCOMPRESSED_FALG;
// 
// 	// F_OK 0	R_OK 2	W_OK 4	X_OK 6
// 	if (int ret = _access(dir, 0 | 2) == -1)
// 	{
// 		printf("the path \"%s\" invalid.\n", dir);
// 		return -1;
// 	}
// 	printf("the path \"%s\" valid.\n", dir);
// 
// 	if (int ret_is_dir = is_dir(dir) == 0)	// is dir
// 	{
// 		_root_dir = dir;
// 
// 		travel_dir(dir);
// 	}
// 
// 	return 0;
// }

// void ep_manager::travel_dir(const std::string& dir)
// {
// 	_finddata_t findData;
// 	std::string findPath = dir + "\\*";
// 	long		handle = _findfirst(findPath.c_str(), &findData);
// 
// 	do 
// 	{	
// 		// sub dir
// 		if ((findData.attrib & _A_SUBDIR))
// 		{
// 			if (strcmp(findData.name, ".") != 0 && strcmp(findData.name, "..") != 0)
// 			{
// 				travel_dir(dir + "\\" + findData.name);
// 			}
// 		}
// 		else
// 		{
// 			std::string str_file_path = dir + "\\" + findData.name;
// 			std::string str_re_path = str_file_path.substr(_root_dir.length() + 1, str_file_path.length() - _root_dir.length());
// 			EPFileEntityEx file_entity;
// 			strcpy(file_entity.relative_path, str_re_path.c_str());
// 			file_entity.relative_path_size = str_re_path.length();
// 			file_entity.data_size = findData.size;
// 			_v_ep_files.push_back(file_entity);
// 		}
// 	} while (_findnext(handle, &findData) == 0);
// 
// 	_findclose(handle);
// }

int ep_manager::package(const char* package_dir, const char* dir, const char* mode)
{
	return _ep_writer->package_dir(package_dir, dir, mode[0]);

// 	bool b_package_exist = dir_valid(package_dir, 0) == 0;
// 
// 	bool b_package_mode_write = strcmp(EP_PACK_MODE_WRITE, mode) == 0;
// 
// 	package(package_dir, dir);
// 
// 	return 0;
}

// int ep_manager::package(const char* package_dir, const char* dir)
// {
// 	package(dir);
// 
// 	write_package(package_dir);
// 
// 	return 0;
// }

// void ep_manager::write_package(const char* package_dir)
// {
// 	long offset = 0;
// 
// 	// version info
// 	ep_write(package_dir, EP_PACK_MODE_WRITE, offset, EP_VERSION_LENGTH, EP_VERSION);
// 	offset += EP_VERSION_LENGTH;
// 
// 	// package header
// 	ep_write(package_dir, EP_PACK_MODE_APPEND, offset, sizeof(_ep_header), (char *)&_ep_header);
// 	offset += sizeof(_ep_header);
// 
// 	// file data
// 	const unsigned int ep_file_entity_size = sizeof(EPFileEntity);
// 	for (EPFileEntityEx file_entity : _v_ep_files)
// 	{
// 		file_entity.offset = offset;
// 
// 		// EPFileEntityEx	-- EPFileEntity Information
// 		ep_write(package_dir, EP_PACK_MODE_APPEND, offset, ep_file_entity_size, (char*)&file_entity);
// 		offset += ep_file_entity_size;
// 
// 		// EPFileEntityEx	-- relative_path
// 		ep_write(package_dir, EP_PACK_MODE_APPEND, offset, file_entity.relative_path_size, file_entity.relative_path);
// 		offset += file_entity.relative_path_size;
// 
// 		// EPFileEntityEx	-- file_data
// 		char* buf_data = new char[file_entity.data_size];
// 		std::string file_absolute_path = _root_dir + "\\" + file_entity.relative_path;
// 		ep_read(file_absolute_path.c_str(), 0, file_entity.data_size, buf_data);
// 		ep_write(package_dir, EP_PACK_MODE_APPEND, offset, file_entity.data_size, buf_data);
// 		offset += file_entity.data_size;
// 
// 		EP_SAFE_DELETE_ARR(buf_data);
// 	}
// }

int ep_manager::parse_package(const char* package_path)
{
	_reader = ep_reader::create_ep_reader(package_path);

	return 0;
}


