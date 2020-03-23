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

#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Enum.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"
#include "Comfort_Macro.h"

#include "Calculater.h"

using namespace std;
using namespace Engine;