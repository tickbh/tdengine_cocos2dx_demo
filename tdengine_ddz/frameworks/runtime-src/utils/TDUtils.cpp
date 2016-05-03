#include "TDUtils.h"
#include "cocos2d.h"

#ifdef WIN32
#include <wtypes.h>
#define ACCESS _access  
#define MKDIR(a) _mkdir((a))
#else
#define ACCESS access  
#define MKDIR(a) mkdir((a),0755)
#endif  


int TDUtils::CreatDir( char *pszDir )
{
	int i = 0;  
	int iRet;  
	int iLen = strlen(pszDir);  

	// 创建目录  
	for (i = 1;i <= iLen;i++)  
	{  
		if (pszDir[i] == '\\' || pszDir[i] == '/')  
		{   
			pszDir[i] = '\0';
			//如果不存在,创建  
			iRet = ACCESS(pszDir,0);  
			if (iRet != 0)  
			{  
				iRet = MKDIR(pszDir);
				if (iRet != 0)  
				{
					return -1;  
				}   
			}
			//支持linux,将所有\换成/  
			pszDir[i] = '/';  
		}   
	}  
	return 0; 
}

int TDUtils::EnsurePathExist( string filename )
{
	return CreatDir((char *)filename.c_str());
}

void TDUtils::DfsFolder( vector<string>& result, string folderPath, bool is_deep )
{
#ifdef WIN32
	_finddata_t FileInfo;
	string strfind = folderPath + "\\*";
	long Handle = _findfirst(strfind.c_str(), &FileInfo);

	if (Handle == -1L)
	{
		//std::cout << "can not match the folder path" << endl;
		return;
	}
	do{
		//判断是否有子目录
		if (FileInfo.attrib & _A_SUBDIR)    
		{
			if( (strcmp(FileInfo.name,".") != 0 ) &&(strcmp(FileInfo.name,"..") != 0))   
			{
				if (is_deep) {
					string newPath = folderPath + "\\" + FileInfo.name;
					DfsFolder(result, newPath, is_deep);
				}
			}
		}
		else  
		{
			string filename = (folderPath + "\\" + FileInfo.name);
			result.push_back(filename);
		}
	}while (_findnext(Handle, &FileInfo) == 0);
	_findclose(Handle);
#else
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	if((dp = opendir(folderPath.c_str())) == NULL) {
		fprintf(stderr,"cannot open directory: %s\n", folderPath.c_str());
		return;
	}
	while((entry = readdir(dp)) != NULL) {
		string filename = folderPath + "/" + entry->d_name;
		lstat(filename.c_str(),&statbuf);
		if(S_ISDIR(statbuf.st_mode)) {

			if(strcmp(".",entry->d_name) == 0 ||
				strcmp("..",entry->d_name) == 0)
				continue;
			if(is_deep) {
				DfsFolder(result, filename,is_deep);
			}
		} else {
			result.push_back(filename);
		}
	}
	closedir(dp);
#endif
}

std::string TDUtils::fullPathForFilename(const std::string& filename)
{
	auto find_path = cocos2d::FileUtils::getInstance()->fullPathForFilename(filename);
	if (find_path.size() > 0) {
		return find_path;
	}
	for (auto path : cocos2d::FileUtils::getInstance()->getSearchPaths()) {
		auto new_path = path + "/" + filename;
		if (cocos2d::FileUtils::getInstance()->isDirectoryExist(new_path)) {
			return new_path;
		}
	}
	return "";
}

