#include "ep_manager.h"

#define COMMAND_PACK	"pack"
#define COMMAND_PARSE	"parse"
#define COMMAND_EXPORT	"export"

//	pack C:\Users\qunny\Desktop\ep_temp.ep C:\Users\qunny\Desktop\rick04-1116\baoji_buff w
// parse C:\Users\qunny\Desktop\ep_temp.ep

// pack C:\Users\Administrator\Desktop\ep_test.ep E:\Work\FuBeiProject\Parkour\Run\client\iPhone\armatures w
// parse C:\Users\Administrator\Desktop\ep_test.ep
// export C:\Users\Administrator\Desktop\ep_test.ep C:\Users\Administrator\Desktop\export

void help()
{
	printf("input valid!\n");
		char tips[] = "pack \t[package dir]\t [dir] [a,w]\n \
parse \t[package dir]\t \
export \t[package dir] \t[export dir]" ;

	printf("tips:\n %s \n", tips);
}

int main(int argc, char* argv[])
{
	ep_manager manager;

	if (argc == 1)
	{
		goto ERROR;
	}

	const char* command = argv[1];
	if (strcmp(command, COMMAND_PACK) == 0)
	{
		manager.package(argv[2], argv[3], argv[4]);
	}
	else if (strcmp(command, COMMAND_PARSE) == 0)
	{
		manager.parse_package(argv[2]);
	}
	else if (strcmp(command, COMMAND_EXPORT) == 0)
	{
		manager.package_export(argv[2], argv[3]);
	}
	else
	{
		goto ERROR;
	}

	system("pause");
	return 0;

ERROR:
	help();
	system("pause");
	return 0;
}