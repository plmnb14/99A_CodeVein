#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
#include <string>
#include <algorithm>
#include <functional>
#include <ctime>
#include <process.h>
#include <iostream>
#include <fstream>
#include <random>

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//#ifndef DBG_NEW 
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
//#define new DBG_NEW 
//#endif

#endif

#ifdef _DEBUG
#pragma comment(lib, "PhysX3DEBUG_x64.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x64.lib")
#pragma comment(lib, "PxFoundationDEBUG_x64.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysX3CookingDEBUG_x64.lib")
#else
#pragma comment(lib, "PhysX3_x64.lib")
#pragma comment(lib, "PhysX3Common_x64.lib")
#pragma comment(lib, "PxFoundation_x64.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PhysX3Cooking_x64.lib")
#endif

#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Enum.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"
#include "Comfort_Macro.h"

#include "Calculater.h"

using namespace std;
using namespace Engine;