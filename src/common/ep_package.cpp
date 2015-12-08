#include "ep_package.h"
#include "ep_utils.h"
#include "zlib.h"

ep_package* ep_package::create_package(const char* package_dir)
{
	ep_package* package = new ep_package();
	
	if (package->parse(package_dir) != 0) {
		EP_SAFE_DELETE(package);
	}

	return package;
}

ep_package::ep_package()
{
	_map_ep_files.clear();
}

ep_package::~ep_package()
{
	_map_ep_files.clear();
}

int ep_package::parse(const char* package_dir)
{
	_package_dir = package_dir;

	bool pkg_exist = dir_valid(package_dir, 0) == 0;
	
	if (pkg_exist)
	{
		return parse_package();
	}

	printf("ep_package parse %s\n", package_dir);

	return 0;
}

int ep_package::parse_package()
{
	unsigned long package_size = ep_get_file_size(_package_dir.c_str());
	unsigned long offset = 0;

	// read ep_package sign
	uint32_t size = EP_SIGN_LENGTH;
	char* buf = new char[size];
	ep_read(_package_dir.c_str(), offset, size, buf);
	offset += size;
	if (strcmp(buf, EP_PACKAGE_SIGN) != 0) {
		EP_SAFE_DELETE_ARR(buf);
		return -1;
	}

	// read ep_package version
	size = EP_VERSION_LENGTH;
	buf = new char[size];
	ep_read(_package_dir.c_str(), offset, size, buf);
	offset += size;
	if (strcmp(buf, EP_VERSION) != 0) {
		EP_SAFE_DELETE_ARR(buf);
		return -1;
	}

	// ep_header
	size = sizeof(EPHeader);
	buf = new char[size];
	ep_read(_package_dir.c_str(), offset, size, buf);
	EPHeader* p_header = (EPHeader*)buf;
	_ep_header.compressed = p_header->compressed;
	offset += size;
	EP_SAFE_DELETE_ARR(buf);

	// ep_file_entity
	unsigned int file_entity_size = sizeof(EPFileEntity);
	while (offset < package_size)
	{
		buf = new char[file_entity_size];

		// EPFileEntityEx		-- EPFileEntity
		if (ep_read(_package_dir.c_str(), offset, file_entity_size, buf) != 0){
			EP_SAFE_DELETE_ARR(buf);
			return -1;
		}
		EPFileEntityEx file_entity_ex;
		EPFileEntity* p_file_entity = (EPFileEntity*)buf;
		file_entity_ex.offset = p_file_entity->offset;
		file_entity_ex.relative_path_hash = p_file_entity->relative_path_hash;
		file_entity_ex.relative_path_size = p_file_entity->relative_path_size;
		file_entity_ex.compress_relative_path_size = p_file_entity->compress_relative_path_size;
		file_entity_ex.source_data_size = p_file_entity->source_data_size;
		file_entity_ex.compressed_data_size = p_file_entity->compressed_data_size;
		file_entity_ex.crc32_source_data = p_file_entity->crc32_source_data;
		//
		offset += file_entity_size;
		EP_SAFE_DELETE_ARR(buf);

		// EPFileEntityEx		-- EPFileEntity relative_path
		uint32_t compress_path_size = file_entity_ex.compress_relative_path_size;
		uLongf relative_size = file_entity_ex.relative_path_size;
		buf = new char[compress_path_size];
		if (ep_read(_package_dir.c_str(), offset, file_entity_ex.compress_relative_path_size, buf) != 0) {
			EP_SAFE_DELETE_ARR(buf);
			return -1;
		}
		uncompress((Bytef*)file_entity_ex.relative_path, &relative_size, (Bytef*)buf, compress_path_size);
		EP_SAFE_DELETE_ARR(buf);

		_map_ep_files.insert(MAP_EP_FILE_ENTITY_EX_PAIR(file_entity_ex.relative_path_hash, file_entity_ex));

		// 
		offset += file_entity_ex.compress_relative_path_size;
		offset += file_entity_ex.compressed_data_size;
	}

	return 0;
}

const MAP_EP_FILE_ENTITY_EX& ep_package::get_ep_file_info() const
{
	return _map_ep_files;
}

const EPHeader& ep_package::get_ep_header() const
{
	return _ep_header;
}

const std::string& ep_package::get_package_dir() const
{
	return _package_dir;
}
