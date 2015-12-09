#include "ep_reader.h"
#include "ep_package.h"
#include "zlib.h"
#include <vector>
#include <string>

#include "ep_utils.h"

ep_reader* ep_reader::create_ep_reader(const char* package_dir)
{
	ep_reader* reader = new ep_reader();
	if (!reader->init(package_dir))
	{
		EP_SAFE_DELETE(reader);
	}

	reader->show_all_file_path();

	return reader;
}

ep_reader::ep_reader()
{
}

ep_reader::~ep_reader()
{
}

bool ep_reader::init(const char* pkg_dir)
{
	_p_ep_package = ep_package::create_package(pkg_dir);
	return _p_ep_package != nullptr;
}

void ep_reader::show_all_file_path() const
{
	MAP_EP_FILE_ENTITY_EX map_file_info = _p_ep_package->get_ep_file_info();
	
	MAP_EP_FILE_ENTITY_EX_ITERATOR it = map_file_info.begin();
	MAP_EP_FILE_ENTITY_EX_CONST_ITERATOR it_end = map_file_info.end();
	while (it != it_end)
	{
		printf("%s\n", it->second.relative_path);
		++it;
	}
}

int ep_reader::export_package(const char* export_dir)
{
	// to-do: verify export_dir valid ?
	const std::string pkg_dir = _p_ep_package->get_package_dir();

	MAP_EP_FILE_ENTITY_EX map_file_info = _p_ep_package->get_ep_file_info();
	MAP_EP_FILE_ENTITY_EX_ITERATOR it = map_file_info.begin();
	MAP_EP_FILE_ENTITY_EX_CONST_ITERATOR it_end = map_file_info.end();

	std::string str_export_dir = export_dir;
	int index = 0;
	while (it != it_end)
	{
		printf("export file[%d] %s ...\n", index++, it->second.relative_path);

		// read
		char* buf = nullptr;
		uint32_t data_size = get_file_data_from_package(it->second.relative_path, &buf);
		// write
		std::string export_absolute_path = str_export_dir + "\\" + it->second.relative_path;
		std::string export_absolute_dir = export_absolute_path.substr(0, export_absolute_path.rfind('\\') + 1);
		ep_mk_dir(export_absolute_dir.c_str());
		EP_WRITE(export_absolute_path.c_str(), EP_PACK_MODE_WRITE, 0, data_size, buf);
		EP_SAFE_DELETE_ARR(buf);

		++it;
	}

	return 0;
}

bool ep_reader::file_exist(const char* file_path)
{
	// to-do: optimize
	uint64_t str_hash = ep_bkdr_hash(file_path, EP_HASH_SEED);
	MAP_EP_FILE_ENTITY_EX map_ep_files = _p_ep_package->get_ep_file_info();
	MAP_EP_FILE_ENTITY_EX_ITERATOR it = map_ep_files.find(str_hash);
	if (it != map_ep_files.end())
	{
		return true;
	}

	return false;
}

uint32_t ep_reader::get_file_data_from_package(const char* file_path, char** buf)
{
	const std::string pkg_dir = _p_ep_package->get_package_dir();

	uint64_t str_hash = ep_bkdr_hash(file_path, EP_HASH_SEED);
	const MAP_EP_FILE_ENTITY_EX& map_ep_files = _p_ep_package->get_ep_file_info();
	MAP_EP_FILE_ENTITY_EX_CONST_ITERATOR it = map_ep_files.find(str_hash);

	if (it != map_ep_files.end())
	{
		*buf = new char[it->second.source_data_size + 1];
		memset(*buf, 0, it->second.source_data_size + 1);

		char* compressed_buf = new char[it->second.compressed_data_size];
		memset(compressed_buf, 0, it->second.compressed_data_size);

		uint32_t data_offset = it->second.offset + sizeof(EPFileEntity)+it->second.compress_relative_path_size;
		ep_read(pkg_dir.c_str(), data_offset, it->second.compressed_data_size, compressed_buf);

		uLongf dest_len = it->second.source_data_size + 1;
		uncompress((Bytef*)*buf, &dest_len, (Bytef*)compressed_buf, it->second.compressed_data_size);

		if (dest_len != it->second.source_data_size)
		{
			printf("error");
		}

		EP_SAFE_DELETE_ARR(compressed_buf);

		return it->second.source_data_size;
	}

	return 0;
}
