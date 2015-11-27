#include "ep_writer.h"
#include "ep_utils.h"
#include <io.h>

ep_writer::ep_writer()
{

}

ep_writer::~ep_writer()
{

}

int ep_writer::package_dir(const char* pkg_dir, const char* file_dir, const char mode)
{
	_file_root_dir = file_dir;
	_p_ep_package = ep_package::create_package(pkg_dir);
	_p_ep_package->_ep_header.compressed = mode;

	if (!_p_ep_package)
		return -1;
	
	if (analyze_dir(file_dir) != 0)
		return -1;

	if (write_dir_to_package() != 0)
		return -1;

	return 0;
}

int ep_writer::analyze_dir(const std::string dir)
{
	_finddata_t findData;
	std::string findPath = dir + "\\*";
	long handle = _findfirst(findPath.c_str(), &findData);

	do
	{
		// sub dir
		if ((findData.attrib & _A_SUBDIR))
		{
			if (strcmp(findData.name, ".") != 0 && strcmp(findData.name, "..") != 0)
			{
				analyze_dir(dir + "\\" + findData.name);
			}
		}
		else
		{
			std::string file_absolute_path = dir + "\\" + findData.name;
			std::string file_relative_path = file_absolute_path.substr(_file_root_dir.length() + 1, file_absolute_path.length() - _file_root_dir.length());
			new_a_file_entity(file_relative_path.c_str(), findData.size);
		}
	} while (_findnext(handle, &findData) == 0);

	_findclose(handle);

	return 0;
}

void ep_writer::new_a_file_entity(const char* file_relative_path, const uint32_t file_size)
{
	EPFileEntityEx file_entity;
	file_entity.relative_path_size = strlen(file_relative_path);
	strncpy(file_entity.relative_path, file_relative_path, file_entity.relative_path_size);
	file_entity.data_size = file_size;
	_p_ep_package->_v_ep_files.push_back(file_entity);
}

int ep_writer::write_dir_to_package()
{
	const char* package_dir = _p_ep_package->_package_dir.c_str();
	const std::vector<EPFileEntityEx>& v_ep_files = _p_ep_package->get_ep_file_info();

	long offset = 0;

	// version info
	EP_WRITE(package_dir, EP_PACK_MODE_WRITE, offset, EP_VERSION_LENGTH, EP_VERSION);
	offset += EP_VERSION_LENGTH;

	// package header
	EP_WRITE(package_dir, EP_PACK_MODE_APPEND, offset, sizeof(EPHeader), (char *)&_p_ep_package->_ep_header);
	offset += sizeof(EPHeader);

	// file data
	const unsigned int ep_file_entity_size = sizeof(EPFileEntity);
	for (EPFileEntityEx file_entity : v_ep_files)
	{
		file_entity.offset = offset;

		// EPFileEntityEx	-- EPFileEntity Information
		EP_WRITE(package_dir, EP_PACK_MODE_APPEND, offset, ep_file_entity_size, (char*)&file_entity);
		offset += ep_file_entity_size;

		// EPFileEntityEx	-- relative_path
		EP_WRITE(package_dir, EP_PACK_MODE_APPEND, offset, file_entity.relative_path_size, file_entity.relative_path);
		offset += file_entity.relative_path_size;

		// EPFileEntityEx	-- file_data
		char* buf_data = new char[file_entity.data_size];
		std::string file_absolute_path = _file_root_dir + "\\" + file_entity.relative_path;
		ep_read(file_absolute_path.c_str(), 0, file_entity.data_size, buf_data);
		EP_WRITE(package_dir, EP_PACK_MODE_APPEND, offset, file_entity.data_size, buf_data);
		offset += file_entity.data_size;

		EP_SAFE_DELETE_ARR(buf_data);
	}

	return 0;
}
