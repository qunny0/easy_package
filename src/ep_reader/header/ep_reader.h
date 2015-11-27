#pragma once
#include "ep_define.h"
#include "ep_package.h"

class ep_reader : public IEP_READER
{
public:
	static ep_reader* create_ep_reader(const char* package_dir);

	void show_all_file_path() const;

	void get_file_data_by_path(const char* file_dir);

protected:
	ep_reader();
	~ep_reader();

	bool init(const char* pkg_dir);

private:
	ep_package*		_p_ep_package;
};

