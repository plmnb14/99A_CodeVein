#pragma once

class CFileInfo
{
private:
	explicit CFileInfo();
	virtual ~CFileInfo();

public:
	static CString ConvertRelativePath(const TCHAR* pFullPath);

	static void Create_Mesh_PathInfo();
	static void Save_Mesh_PathInfo(list<MESH_INFO*>& rPathInfoLst);
	static void Extract_Mesh_PathInfo(const TCHAR* pPath, list<MESH_INFO*>& rPathInfoLst, _bool _bIsDynamic = false);

	static void Create_Texture_PathInfo();
	static void Save_Texture_PathInfo(list<TEX_INFO*>& rPathInfoLst);
	static void Extract_Texture_PathInfo(const TCHAR* pPath, list<TEX_INFO*>& rPathInfoLst, _bool _bIsDynamic = false);

	static int CountImageFile(const TCHAR* pPath);
};

