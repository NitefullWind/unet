#include "log.h"
#include "../../../include/unp.h"
#include "control.h"

int main(int argc, char **argv)
{
	try {
		//获取程序绝对路径
		char path[1024];
		int cnt = readlink("/proc/self/exe", path, 1024);
		for(int i=cnt; i>=0; --i) {
			if(path[i]=='/') {
				path[i+1]='\0';
				break;
			}
		}
		strcat(path, "log.props");
		log4cxx::PropertyConfigurator::configure(path);
	} catch (log4cxx::helpers::Exception&) {
		printf("Init log failed.\n");
	}
	
	LOG_INFO("========== Program Started =========");

	Control control(argc, argv);
	return control.exec();
}
