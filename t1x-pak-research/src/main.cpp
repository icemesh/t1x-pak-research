/******************************************************************************/
/*
  Author  - icemesh
*/
/******************************************************************************/
#include <stdio.h>
#include "sidbase/sidbase.h"
#include "package/package.h"
#include "utils/utils.h"

int main(int argc, const char* aArgv[])
{
	puts("-------------[ t1x pak loader by icemesh ]--------------\n");
	if (argc > 1)
	{
		Package* pPackage = new Package(aArgv[1]);
		if (pPackage)
		{
			InitSidbase();
			if (pPackage->PackageLogin())
			{
				puts(">>> Further data processing goes here");
			}
			delete pPackage;
			ShutdownSidbase();
		}
		else
		{
			MsgErr("Failed to allocate package class\n");
		}
	}
	else
	{
		puts("Usage: main.exe <name>.pak (PS4 only)");
	}
}