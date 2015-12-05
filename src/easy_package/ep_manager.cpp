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

