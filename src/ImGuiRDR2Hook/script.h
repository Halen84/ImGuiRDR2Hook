/*
	THIS FILE IS A PART OF RDR 2 SCRIPT HOOK SDK
				http://dev-c.com
			(C) Alexander Blade 2019
*/

#pragma once

#include "..\..\inc\natives.h"
#include "..\..\inc\types.h"
#include "..\..\inc\enums.h"

#include "..\..\inc\main.h"

void ScriptMain();

// NOTE: Use this function instead of Scripthook's WAIT (scriptWait) function to prevent a crash. 
void WaitAndRender(unsigned ms);
