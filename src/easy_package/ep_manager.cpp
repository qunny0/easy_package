#include "ep_manager.h"

#include "ep_utils.h"
#include "ep_reader.h"
#include "ep_writer.h"
#include "zlib.h"

#include <time.h>

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
	const char tips[] = "pack \t[achieve_name...] [dir]\n\
parse \t[achieve_name...]\t\n\
export \t[achieve_name...] [dir]\n" ;

	printf("%s", tips);
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
	ep_mk_dir("C:\\Users\\Administrator\\Desktop\\test\\t1\\t2 ");

	return 0;

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

