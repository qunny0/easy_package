#include "ep_reader.h"
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
	reader->get_file_data_by_path("ba.txt");

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
	const std::vector<EPFileEntityEx> v_file_info = _p_ep_package->get_ep_file_info();
	
	for (EPFileEntityEx file_entity : v_file_info)
	{
		printf("file relative path : %s \n", file_entity.relative_path);
	}
}

void ep_reader::get_file_data_by_path(const char* file_dir)
{
	const std::string pkg_dir = _p_ep_package->get_package_dir();
	const std::vector<EPFileEntityEx> v_file_info = _p_ep_package->get_ep_file_info();

	for (EPFileEntityEx file_entity : v_file_info)
	{
		if (strcmp(file_entity.relative_path, file_dir) == 0)
		{
			char* buf = new char[file_entity.data_size + 1];
			memset(buf, 0, file_entity.data_size + 1);
			uint32_t data_offset = file_entity.offset + file_entity.relative_path_size + sizeof(EPFileEntity);
			ep_read(pkg_dir.c_str(), data_offset, file_entity.data_size, buf);
			printf("%s data : %s \n", file_dir, buf);
			EP_SAFE_DELETE_ARR(buf);
			break;
		}
	}
}

