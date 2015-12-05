#include "ep_writer.h"
#include <io.h>
#include <time.h>
#include "ep_utils.h"
#include "ep_package.h"
#include "zlib.h"
#include "crc32.h"

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
	file_entity.relative_path_hash = ep_bkdr_hash(file_relative_path, EP_HASH_SEED);
	file_entity.relative_path_size = strlen(file_relative_path);
	strncpy(file_entity.relative_path, file_relative_path, file_entity.relative_path_size);
	file_entity.source_data_size = file_size;
	file_entity.n_hash_collision_flag = 1;

	MAP_EP_FILE_ENTITY_EX_ITERATOR it = _p_ep_package->_map_ep_files.find(file_entity.relative_path_hash);
	if (it == _p_ep_package->_map_ep_files.end())
	{
		_p_ep_package->_map_ep_files.insert(MAP_EP_FILE_ENTITY_EX_PAIR(file_entity.relative_path_hash, file_entity));
	}
	else
	{
		it->second.n_hash_collision_flag += 1;
		// hash collision
		printf("%s\n", it->second.relative_path);
		printf("%s\n hash collision", file_relative_path);
	}
}

int ep_writer::write_dir_to_package()
{
	const char* package_dir = _p_ep_package->_package_dir.c_str();
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
	MAP_EP_FILE_ENTITY_EX_ITERATOR it = _p_ep_package->_map_ep_files.begin();
	MAP_EP_FILE_ENTITY_EX_CONST_ITERATOR it_end = _p_ep_package->_map_ep_files.end();
	while (it != it_end)
	{
		printf("package file[%d] %s ...\n", index++, it->second.relative_path);

		char* src_data = new char[it->second.source_data_size];
		std::string file_absolute_path = _file_root_dir + "\\" + it->second.relative_path;
		ep_read(file_absolute_path.c_str(), 0, it->second.source_data_size, src_data);

		uint64_t crc32_src_data = 0;
		crc32_src_data = crc32(0L, (Bytef*)src_data, it->second.source_data_size);

		// calculate compress data
		uLongf compress_len = compressBound(it->second.source_data_size);
		Bytef* compress_buf = new Bytef[compress_len];
		compress(compress_buf, &compress_len, (Bytef*)src_data, it->second.source_data_size);
		EP_SAFE_DELETE_ARR(src_data);

		uLongf compress_path_len = compressBound(it->second.relative_path_size);
		Bytef* compress_path_buf = new Bytef[compress_path_len];
		compress(compress_path_buf, &compress_path_len, (Bytef*)it->second.relative_path, it->second.relative_path_size);

		it->second.offset = offset;
		it->second.compressed_data_size = compress_len;
		it->second.compress_relative_path_size = compress_path_len;
		it->second.crc32_source_data = crc32_src_data;

		// write
		// EPFileEntityEx	-- EPFileEntity Information
		EP_WRITE(package_dir, EP_PACK_MODE_APPEND, offset, ep_file_entity_size, (char*)&it->second);
		offset += ep_file_entity_size;

		// EPFileEntityEx	-- relative_path -- compress
		EP_WRITE(package_dir, EP_PACK_MODE_APPEND, offset, it->second.compress_relative_path_size, (char*)compress_path_buf);
		offset += it->second.compress_relative_path_size;
		EP_SAFE_DELETE_ARR(compress_path_buf);

		EP_WRITE(package_dir, EP_PACK_MODE_APPEND, offset, it->second.compressed_data_size, (char*)compress_buf);
		offset += it->second.compressed_data_size;
		EP_SAFE_DELETE_ARR(compress_buf);

		++it;
	}

	return 0;
}
