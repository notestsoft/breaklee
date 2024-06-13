#pragma once

#include <CoreLib/CoreLib.h>

#include <iostream>
#include <sstream>
#include <string>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <set>
#include <functional>
#include <windows.h>
#include <xinput.h>

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

#include "Primitives.h"

typedef std::string String;
typedef std::istringstream StringStream;
typedef std::mutex CMutex;

template<
	typename T
>
using CArc = std::shared_ptr<T>;

template<
	typename T
> 
using CArray = std::vector<T>;

template<
	typename T
>
using CSet = std::set<T>;

template<
	typename Key,
	typename Value
>
using CPair = std::pair<Key, Value>;

template<
	typename Key, 
	typename Value
> 
using CMap = std::unordered_map<Key, Value>;

struct CTexture;

class CArchive;
class CArchiveIterator;
struct CFile;
class CFileManager;
class CLocalization;
class CRenderer;
class CApplication;
class CUIManager;
