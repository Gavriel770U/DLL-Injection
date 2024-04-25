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
		system("\"C:\\Users\\Giga Gavriel\\source\\repos\\WindowForDLL\\x64\\Release\\WindowForDLL.exe\"");
	}
}