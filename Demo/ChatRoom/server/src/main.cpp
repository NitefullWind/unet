#include "../../../include/unp.h"
#include "control.h"

int main(int argc, char **argv)
{
	Control control(argc, argv);
	return control.exec();
}
