#include "ep_writer.h"
#ifdef _WIN32
	#include <io.h>
#else
	#include <unistd.h>
	#include <stdio.h>
    #include <dirent.h>
    #include <sys/stat.h>
#endif
#include <time.h>
#include "ep_utils.h"
#include "ep_package.h"
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
	
	if (analyze_dir(file_dir, 0) != 0)
		return -1;

	time_t pre_time, aft_time;
	time(&pre_time);
	if (write_package() != 0)
		return -1;
	time(&aft_time);
	uint32_t dif_time = uint32_t(aft_time - pre_time);

	printf("package cost %d s.\n", dif_time);

	return 0;
}

int ep_writer::analyze_dir(const std::string path, int level)
{
#ifdef _WIN32
	_finddata_t findData;
	std::string findPath = path + "\\*";
	long handle = _findfirst(findPath.c_str(), &findData);

	do
	{
		// sub path
		if ((findData.attrib & _A_SUBDIR))
		{
			if (strcmp(findData.name, ".") != 0 && strcmp(findData.name, "..") != 0)
			{
				analyze_dir(path + "\\" + findData.name, level+1);
			}
		}
		else
		{
			std::string file_absolute_path = path + "\\" + findData.name;
			std::string file_relative_path = file_absolute_path.substr(_file_root_dir.length() + 1, file_absolute_path.length() - _file_root_dir.length());
			new_a_file_entity(file_relative_path.c_str(), findData.size);
		}
	} while (_findnext(handle, &findData) == 0);

	_findclose(handle);
    
#else
   	DIR* dir;
   	struct dirent* entry;

   	// if (!(dir = opendir(path.c_str())) || !(entry = readdir(dir)))
   	// 	return 0;
   	if (!(dir = opendir(path.c_str())))
   		return 0;
   	if (!(entry = readdir(dir)))
   		return 0;

   	do {
   		if (entry->d_type == DT_DIR)
   		{
   			if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
   				continue;

   			char next_path[1024] = {};
   			snprintf(next_path, sizeof(next_path)-1, "%s/%s", path.c_str(), entry->d_name);
   			// printf("%s %*s[%s]\n", next_path, level*2, "", entry->d_name);
   			analyze_dir(next_path, level+1);
   		}
   		else
   		{
   			std::string file_absolute_path = path + "/" + entry->d_name;
   			std::string file_relative_path = file_absolute_path.substr(_file_root_dir.length() + 1, file_absolute_path.length() - _file_root_dir.length());
   			uint32_t file_size = ep_get_file_size(file_absolute_path.c_str());
			new_a_file_entity(file_relative_path.c_str(), file_size);
   		}
   	}while(entry = readdir(dir));
   	closedir(dir);

#endif

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
		_map_ep_files_to_pack.insert(MAP_EP_FILE_ENTITY_EX_PAIR(file_entity.relative_path_hash, file_entity));
	}
	else
	{
		it->second.n_hash_collision_flag += 1;
		// hash collision
		printf("%s hash collision!!\n", file_relative_path);

		char* src_buf = new char[file_size];
		std::string file_absolute_path = _file_root_dir + "\\" + it->second.relative_path;
		ep_read(file_absolute_path.c_str(), 0, file_size, src_buf);

		uint64_t crc32_src_data = 0;
		crc32_src_data = crc32(0L, (Bytef*)src_buf, file_size);
		EP_SAFE_DELETE_ARR(src_buf);

		if (crc32_src_data != it->second.crc32_source_data)
		{
			it->second.b_delete_flag = true;
			_map_ep_files_to_pack.insert(MAP_EP_FILE_ENTITY_EX_PAIR(file_entity.relative_path_hash, file_entity));
		}
	}
}

int ep_writer::write_package()
{
	long offset = 0;

	offset = write_package_header(offset);

	offset = update_package(offset);

  	int ret = write_dir_to_package(offset);

	return ret;
}

long ep_writer::write_package_header(long offset)
{
	const char* package_dir = _p_ep_package->_package_dir.c_str();

	bool pkg_exist = dir_valid(package_dir, 0) == 0;

	if (!pkg_exist)
	{

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
	}
	else
	{
		offset += EP_SIGN_LENGTH;
		offset += EP_VERSION_LENGTH;
		offset += sizeof(EPHeader);
	}

	return offset;
}

long ep_writer::update_package(long offset)
{
	const char* package_dir = _p_ep_package->_package_dir.c_str();

	// file data
	const unsigned int ep_file_entity_size = sizeof(EPFileEntity);
	int index = 0;
	int index_delete = 0;
	MAP_EP_FILE_ENTITY_EX_ITERATOR it = _p_ep_package->_map_ep_files.begin();
	MAP_EP_FILE_ENTITY_EX_CONST_ITERATOR it_end = _p_ep_package->_map_ep_files.end();
	while (it != it_end)
	{
		if (it->second.b_delete_flag)
		{
			printf("package delete file[%d] %s ...\n", index_delete++, it->second.relative_path);
		}
		else
		{
			if (offset == it->second.offset)
			{
				offset += ep_file_entity_size;
				offset += it->second.compress_relative_path_size;
				offset += it->second.compressed_data_size;
			}
			else
			{
				printf("package move file[%d] %s ...\n", index++, it->second.relative_path);

				uint32_t origin_offset = it->second.offset;
				it->second.offset = offset;

				// write
				// EPFileEntityEx	-- EPFileEntity Information
				EP_WRITE(package_dir, EP_PACK_MODE_REWRITE, offset, ep_file_entity_size, (char*)&it->second);
				offset += ep_file_entity_size;

				// relative_path + file_data
				uint32_t relative_offset = origin_offset + ep_file_entity_size;
				uint32_t origin_size = it->second.compress_relative_path_size + it->second.compressed_data_size;
				char* buf = new char[origin_size];
				ep_read(package_dir, relative_offset, origin_size, buf);
				EP_WRITE(package_dir, EP_PACK_MODE_REWRITE, offset, origin_size, buf);
				EP_SAFE_DELETE_ARR(buf);

				offset += origin_size;
			}
		}

		++it;
	}

	if (index_delete > 0)
	{
		ep_set_file_length(package_dir, offset);
	}

	return offset;
}

int ep_writer::write_dir_to_package(long offset)
{
	const char* package_dir = _p_ep_package->_package_dir.c_str();

	uint32_t t_size = _map_ep_files_to_pack.size();

	// file data
	const unsigned int ep_file_entity_size = sizeof(EPFileEntity);
	int index = 0;
	MAP_EP_FILE_ENTITY_EX_ITERATOR it = _map_ep_files_to_pack.begin();
	MAP_EP_FILE_ENTITY_EX_CONST_ITERATOR it_end = _map_ep_files_to_pack.end();
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
