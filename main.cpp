#include "Einstein.h"

#include <list>
#include <string>
#include <iostream>
#include <list>
#include <typeinfo>
#include <sstream>

int play()
{
	return 1;
}

int main()
{
	Einstein e;
	int running = 1;
	while (running)
	{
		running = e.handle();
	}
	
	e.logging("done!");
	e.writelog();

	//system("pause");
	return 0;
}
