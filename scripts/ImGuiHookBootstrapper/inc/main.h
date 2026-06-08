#pragma once
#include <windows.h>

#define IMPORT __declspec(dllimport)

IMPORT void scriptRegister(HMODULE module, void(*LP_SCRIPT_MAIN)());
IMPORT void scriptUnregister(HMODULE module);
