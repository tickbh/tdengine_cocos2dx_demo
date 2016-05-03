#ifndef _PLATFORM_UTILE_ONE_H_
#define _PLATFORM_UTILE_ONE_H_

#ifdef _WIN32
#pragma once
#endif

#ifdef WIN32
#include <direct.h>  
#include <io.h>
#else
#include <stdarg.h>  
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#endif  

#include <vector>
#include <map>
#include <string>
#include <fstream>
using namespace std;

class TDUtils
{
public:
	static int CreatDir(char *pszDir);
	static int EnsurePathExist(string filename);
	static void DfsFolder(vector<string>& result, string folderPath, bool is_deep = false);
	static std::string fullPathForFilename(const std::string& filename);
};



#endif