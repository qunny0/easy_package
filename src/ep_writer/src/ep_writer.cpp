#include "ep_writer.h"
#include "ep_utils.h"
#include <io.h>
#include <time.h>

#include "zlib.h"

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

	time_t pre_time, aft_time;
	time(&pre_time);
	if (write_dir_to_package() != 0)
		return -1;
	time(&aft_time);
	uint32_t dif_time = uint32_t(aft_time - pre_time);

	printf("package cost %d s.\n", dif_time);

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
	file_entity.source_data_size = file_size;
	_p_ep_package->_v_ep_files.push_back(file_entity);
}

int ep_writer::write_dir_to_package()
{
	const char* package_dir = _p_ep_package->_package_dir.c_str();
	const std::vector<EPFileEntityEx>& v_ep_files = _p_ep_package->get_ep_file_info();

	long offset = 0;

	// package sign
	printf("package sign...\n");
	EP_WRITE(package_dir, EP_PACK_MODE_WRITE, offset, EP_SIGN_LENGTH, EP_PACKAGE_SIGN);
	offset += EP_SIGN_LENGTH;

	// version info
	printf("package version information ...\n");
	EP_WRITE(package_dir, EP_PACK_MODE_APPEND, offset, EP_VERSION_LENGTH, EP_VERSION);
	offset += EP_VERSION_LENGTH;

	// package header
	printf("package header ...\n");
	EP_WRITE(package_dir, EP_PACK_MODE_APPEND, offset, sizeof(EPHeader), (char *)&_p_ep_package->_ep_header);
	offset += sizeof(EPHeader);

	// file data
	const unsigned int ep_file_entity_size = sizeof(EPFileEntity);
	int index = 0;
	for (EPFileEntityEx file_entity : v_ep_files)
	{
		printf("package file[%d] %s ...\n", index++, file_entity.relative_path);

		char* src_data = new char[file_entity.source_data_size];
		std::string file_absolute_path = _file_root_dir + "\\" + file_entity.relative_path;
		ep_read(file_absolute_path.c_str(), 0, file_entity.source_data_size, src_data);

		// calculate compress data
		uLongf dest_len = compressBound(file_entity.source_data_size);
		Bytef* dest_buf = new Bytef[dest_len];
		compress(dest_buf, &dest_len, (Bytef*)src_data, file_entity.source_data_size);
		EP_SAFE_DELETE_ARR(src_data);

		file_entity.offset = offset;
		file_entity.compressed_data_size = dest_len;

		// EPFileEntityEx	-- EPFileEntity Information
		EP_WRITE(package_dir, EP_PACK_MODE_APPEND, offset, ep_file_entity_size, (char*)&file_entity);
		offset += ep_file_entity_size;

		// EPFileEntityEx	-- relative_path
		EP_WRITE(package_dir, EP_PACK_MODE_APPEND, offset, file_entity.relative_path_size, file_entity.relative_path);
		offset += file_entity.relative_path_size;

		EP_WRITE(package_dir, EP_PACK_MODE_APPEND, offset, dest_len, (char*)dest_buf);

		EP_SAFE_DELETE_ARR(dest_buf);

		offset += file_entity.compressed_data_size;
	}

	return 0;
}

void ep_compress()
{
// 	compress()
}
