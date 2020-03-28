#pragma once

class CFileInfo
{
private:
	explicit CFileInfo();
	virtual ~CFileInfo();

public:
	enum Extract_Mesh
	{
		Extract_Static, Extract_Dynamic, Extract_Essential, Extract_Weapon, Extract_End
	};

	static CString ConvertRelativePath(const TCHAR* pFullPath);

	static void Create_Mesh_PathInfo(Extract_Mesh _eExtract_Mesh = Extract_Essential);
	static void Save_Mesh_PathInfo(list<MESH_INFO*>& rPathInfoLst , Extract_Mesh _eExtract_Mesh = Extract_Essential);
	static void Extract_Mesh_PathInfo(const TCHAR* pPath, list<MESH_INFO*>& rPathInfoLst, _bool _bIsDynamic = false);

	static void Create_Texture_PathInfo();
	static void Save_Texture_PathInfo(list<TEX_INFO*>& rPathInfoLst);
	static void Extract_Texture_PathInfo(const TCHAR* pPath, list<TEX_INFO*>& rPathInfoLst, _bool _bIsDynamic = false);

	static int CountImageFile(const TCHAR* pPath);
};

