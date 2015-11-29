#include "ep_manager.h"

#include "ep_utils.h"
#include "ep_reader.h"
#include "ep_writer.h"
#include "zlib.h"

#include <io.h>

// #define ZLIB_WINAPI

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


int ep_manager::package(const char* package_dir, const char* dir, const char* mode)
{
	return _ep_writer->package_dir(package_dir, dir, mode[0]);
}

int ep_manager::parse_package(const char* package_path)
{
	_reader = ep_reader::create_ep_reader(package_path);

	return 0;
}

int ep_manager::package_export(const char* package_dir, const char* export_dir)
{
	parse_package(package_dir);

	_reader->export_package(export_dir);

	return 0;
}

void ep_manager::test()
{
	char test_path[] = "C:\\Users\\Administrator\\Desktop\\export\\test.txt";

	_finddata_t findData;
	if (_findfirst(test_path, &findData))
	{
		uint32_t file_size = findData.size + 1;
		char* buf = new char[file_size];
		memset(buf, 0, file_size);
		ep_read(test_path, 0, file_size, buf);
		
		Byte* dest_buf = nullptr;
		uLongf dest_size = compressBound(file_size);
		dest_buf = new Byte[dest_size];
		int ret = compress(dest_buf, &dest_size, (Bytef*)buf, file_size);

		Bytef* un_dest_buf = new Bytef[file_size];
		unsigned long un_dest_size = file_size;
		ret = uncompress(un_dest_buf, &un_dest_size, (Bytef*)dest_buf, dest_size);

		if (ret == 0)
		{
			printf("sha");
			return;
		}
	}
}

