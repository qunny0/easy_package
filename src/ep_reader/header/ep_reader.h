#pragma once
#include "ep_define.h"
#include "ep_package.h"

class ep_reader : public IEP_READER
{
public:
	static ep_reader* create_ep_reader(const char* package_dir);

	virtual int export_package(const char* export_dir);

	virtual bool file_exist(const char* file_path);

	virtual uint32_t get_file_data(const char* file_path, char** buf);

protected:
	ep_reader();
	~ep_reader();

	bool init(const char* pkg_dir);

	void show_all_file_path() const;

	void print_file_data_by_path(const char* file_dir);

private:
	ep_package*		_p_ep_package;
};

