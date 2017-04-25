#include <stdio.h>
#include "test.h"
#include "pbrtest.h"
#include <core/log.h>
#include <string>
#include <util/vfs/vfs.h>

#define PRINT(str) FD_INFO("%s", *str)



int main() {
	
	PBRTest test;
	test.Start();

	return 0;
}