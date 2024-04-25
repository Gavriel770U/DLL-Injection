// GavriDLL.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#define DLL_EXPORT
#include "GavriDLL.h"

extern "C"
{
	DECLDIR void callMessageBox(void)
	{
		system(<EXE_TO_OPEN_PATH>);
	}
}