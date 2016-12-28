// MqttServicePch.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "Thread.h"

// TODO: reference additional headers your program requires here
#include <string>
#include <iostream>
#include <functional>

extern "C"
{
#include "MQTTAsync.h"
}

#ifdef XFRAME_PLUGIN_EXPORTS 
#       define XFRAME_PLUGIN_EXP __declspec(dllexport) 
#else 
#       define XFRAME_PLUGIN_EXP __declspec(dllimport) 
#endif 