#include "ep_manager.h"

#include "ep_utils.h"
#include "ep_reader.h"
#include "ep_writer.h"
#include "zlib.h"

#include <io.h>

// #include <map>
// const uint32_t HASH_SEED = 131;
// std::map<uint64_t, uint32_t> _map_test_hash;
// uint64_t stringHash(const char* str, uint32_t seed)
// {
// 	uint64_t out = 0;
// 	while (*str)
// 	{
// 		char ch = *(str++);
// 		if (ch == ('\\'))
// 		{
// 			ch = ('/');
// 		}
// #if (ZP_CASE_SENSITIVE)
// 		out = out * seed + ch;
// #else
// #if defined ZP_USE_WCHAR
// 		out = out * seed + towlower(ch);
// #else
// 		out = out * seed + tolower(ch);
// #endif
// #endif
// 	}
// 	return out;
// }

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
	if (dir_valid(package_dir, 0) == 0)
	{
		printf("%s exist!\n", package_dir);
		return -1;
	}

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

// printf("hash size: %d\n", _map_test_hash.size());
// std::map<uint64_t, uint32_t>::iterator it = _map_test_hash.begin();
// while (it != _map_test_hash.end())
// {
// 	if (it->second > 1)
// 	{
// 		printf("%d\n", it->second);
// 	}
// }

// int ep_manager::analyze_dir(const std::string dir)
// {
// 	_finddata_t findData;
// 	std::string findPath = dir + "\\*";
// 	long handle = _findfirst(findPath.c_str(), &findData);
// 
// 	do
// 	{
// 		// sub dir
// 		if ((findData.attrib & _A_SUBDIR))
// 		{
// 			if (strcmp(findData.name, ".") != 0 && strcmp(findData.name, "..") != 0)
// 			{
// 				analyze_dir(dir + "\\" + findData.name);
// 			}
// 		}
// 		else
// 		{
// 			std::string file_absolute_path = dir + "\\" + findData.name;
// // 			std::string file_relative_path = file_absolute_path.substr(_file_root_dir.length() + 1, file_absolute_path.length() - _file_root_dir.length());
// 
// 			uint64_t hash_value = stringHash(file_absolute_path.c_str(), HASH_SEED);
// 			_map_test_hash[hash_value]++;
// 
// 			printf("path:%s, hash:%d\n", file_absolute_path.c_str(), hash_value);
// 
// 			if (_map_test_hash[hash_value] > 1)
// 			{
// 				printf("collision path : %s\n", file_absolute_path);
// 			}
// 
// // 			new_a_file_entity(file_relative_path.c_str(), findData.size);
// 		}
// 	} while (_findnext(handle, &findData) == 0);
// 
// 	_findclose(handle);
// 
// 	return 0;
// }
