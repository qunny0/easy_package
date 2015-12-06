#include "ep_manager.h"

#include "ep_utils.h"
#include "ep_reader.h"
#include "ep_writer.h"
#include "zlib.h"

#include <time.h>
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

void ep_manager::show_tips()
{
	const char tips[] = "pack \t[package dir]\t[dir][a, w]\n \
		parse \t[package dir]\t \
		export \t[package dir] \t[export dir]" ;

	printf("%s", tips);
}

int ep_manager::package(const char* package_dir, const char* dir, const char* mode)
{
// 	if (dir_valid(package_dir, 0) == 0)
// 	{
// 		printf("%s exist!\n", package_dir);
// 		return -1;
// 	}

	uint32_t offset = 0;

	EPFileEntity fileEntity;

	fileEntity.offset = offset;
	fileEntity.relative_path_hash = 2450701060051609895;
	fileEntity.relative_path_size = 18;
	fileEntity.compress_relative_path_size = 26;
	fileEntity.source_data_size = 2566;
	fileEntity.compressed_data_size = 1864;
	fileEntity.crc32_source_data = 240946668;

	uint32_t size = sizeof(EPFileEntity);

	const char* path = "C:\\Users\\Administrator\\Desktop\\1.t";
// 	EP_WRITE(path, EP_PACK_MODE_REWRITE, offset, size, (char*)&fileEntity);
	EP_WRITE(path, EP_PACK_MODE_REWRITE, offset, size, (char*)&fileEntity);

	char* buf = new char[size];
	ep_read(path, offset, size, buf);
	EPFileEntity* outFile = (EPFileEntity*)buf;

	// 写模式下，写入上述数据是没有问题的
	// 但是在r+模式下，r+模式是为了覆盖文件本身的数据。但是在r+模式下，上述数据写入文件时，会多出一位。不是在末尾，而是在中间的一个值。不知道什么原因。

	return 0;

	return _ep_writer->package_dir(package_dir, dir, mode[0]);
}

int ep_manager::parse_package(const char* package_path)
{
	_reader = ep_reader::create_ep_reader(package_path);

	return 0;
}

int ep_manager::package_export(const char* package_dir, const char* export_dir)
{
	time_t pre_time, aft_time;
	
	time(&pre_time);
	parse_package(package_dir);
	time(&aft_time);
	uint32_t dif_time = uint32_t(aft_time - pre_time);
	printf("parse cost %d s.\n", dif_time);

	time(&pre_time);
	_reader->export_package(export_dir);
	time(&aft_time);
	dif_time = uint32_t(aft_time - pre_time);
	printf("export cost %d s.\n", dif_time);

	return 0;
}

