#include "stdafx.h"
#include "FileInfo.h"


CFileInfo::CFileInfo()
{
}


CFileInfo::~CFileInfo()
{
}

CString CFileInfo::ConvertRelativePath(const TCHAR* pFullPath)
{
	TCHAR szCurrentDir[256] = L"";
	::GetCurrentDirectory(256, szCurrentDir);

	TCHAR szRelativePath[256] = L"";

	//// From에서 To로 넘어가는 상대경로를 만들어서 Output에 저장하는 함수.
	//::PathRelativePathTo(Output, From, FILE_ATTRIBUTE_DIRECTORY,
	//	To, FILE_ATTRIBUTE_DIRECTORY);
	// 단, From과 To는 같은 드라이브 안에 있어야 한다.
	::PathRelativePathTo(szRelativePath, szCurrentDir, FILE_ATTRIBUTE_DIRECTORY,
		pFullPath, FILE_ATTRIBUTE_DIRECTORY);

	return CString(szRelativePath);
}

void CFileInfo::Create_Mesh_PathInfo()
{
	list<Engine::MESH_INFO*> m_listPathInfo;

	_tchar szStaticPath[MAX_STR] = L"";
	_tchar szDynamicPath[MAX_STR] = L"";

	//cout << "Extracting StaticMesh Path . . ." << endl;

	lstrcpy(szStaticPath, L"..\\..\\Client\\Bin\\Resource\\Mesh\\");	// 문자열 복사
	lstrcat(szStaticPath, L"StaticMesh");								// 문자열 결합
	Extract_Mesh_PathInfo(szStaticPath, m_listPathInfo);

	//cout << "Extracting DynamicMesh Path . . ." << endl;

	lstrcpy(szDynamicPath, L"..\\..\\Client\\Bin\\Resource\\Mesh\\");	// 문자열 복사
	lstrcat(szDynamicPath, L"DynamicMesh");								// 문자열 결합
	Extract_Mesh_PathInfo(szDynamicPath, m_listPathInfo, true);

	//cout << "Extracting Complete . . ." << endl;

	Save_Mesh_PathInfo(m_listPathInfo);
}

void CFileInfo::Save_Mesh_PathInfo(list<MESH_INFO*>& rPathInfoLst)
{
	//cout << "Saving MeshPath . . ." << endl;

	wofstream fout;

	fout.open(L"../../Data/MeshPath.dat");

	if (fout.fail())
		return;

	DWORD dwByte = 0;
	wstring wstrCombined = L"";

	for (auto& pPathInfo : rPathInfoLst)
	{
		wstrCombined =
			pPathInfo->wstrStateKey + L"|" +
			pPathInfo->wstrFileName + L"|" +
			pPathInfo->wstrImgPath + L"|" +
			pPathInfo->szIsDynamic + L"|";

		fout << wstrCombined << endl;
	}

	fout.close();

	for (auto& pPathInfo : rPathInfoLst)
	{
		Engine::Safe_Delete(pPathInfo);
	}
	rPathInfoLst.clear();

	//cout << "Done . . . !" << endl;
}

void CFileInfo::Extract_Mesh_PathInfo(
	const TCHAR* pPath, /* D:\\김태윤\\4개월차\\98기\\Framework98\\Texture\\Stage\\Effect\\BossMultiAttack */
	list<MESH_INFO*>& rPathInfoLst , _bool _bIsDynamic)
{
	//cout << "Extracting Path . . ." << endl;

	wstring wstrFindPath = wstring(pPath) +L"\\*";

	CFileFind find;

	BOOL bIsFind = find.FindFile(wstrFindPath.c_str());

	while (bIsFind)
	{
		// 더이상 찾을게 없거나 마지막 파일이나 폴더를 찾으면 FALSE 반환.
		bIsFind = find.FindNextFile();

		if (find.IsDots()) // 찾은 것이 마커라면 건너뛰어라.
			continue;

		else if (find.IsDirectory())
		{
			Extract_Mesh_PathInfo(find.GetFilePath(), rPathInfoLst , _bIsDynamic); // 파일 찾을 때까지 재귀.
		}

		else
		{
			if (find.IsSystem())	// 시스템이면 건너 뛰고
				continue;

			CString strFullPath = find.GetFilePath();

			if (strFullPath.Find(L".X") == -1)
				continue;

			CString strRelative = ConvertRelativePath(strFullPath);

			Engine::MESH_INFO* pPathInfo = new Engine::MESH_INFO;

			///////////////////////////////////////////////////////////////////////////

			TCHAR szBuf[MAX_STR] = L"";
			lstrcpy(szBuf, strRelative);


			// 폴더까지의 경로
			::PathRemoveFileSpec(szBuf);
			wstring tmpPathString = szBuf;
			pPathInfo->wstrImgPath = tmpPathString + L"\\";
			//

			// 파일 이름
			lstrcpy(szBuf, strRelative);
			::PathRemoveBackslash(szBuf);
			wstring tmpString = ::PathFindFileName(szBuf);

			// 파일명
			pPathInfo->wstrFileName = tmpString;

			//::PathRemoveExtension(szBuf);
			PathRemoveExtension(szBuf);
			tmpString = PathFindFileName(szBuf);

			// 키 값
			pPathInfo->wstrStateKey = L"Mesh_" + tmpString;

			TCHAR szBuf_2[MAX_STR] = L"";
			(_bIsDynamic ? lstrcpy(szBuf_2, L"1") : lstrcpy(szBuf_2, L"0"));

			pPathInfo->szIsDynamic = szBuf_2;

			rPathInfoLst.push_back(pPathInfo);

			//Engine::Safe_Delete(szBuf_2);
			//break;
		}

	}
}

void CFileInfo::Create_Texture_PathInfo()
{
	list<Engine::TEX_INFO*> m_listPathInfo;

	_tchar szStaticPath[MAX_STR] = L"";
	_tchar szDynamicPath[MAX_STR] = L"";

	//cout << "Extracting Texture Path . . ." << endl;

	lstrcpy(szStaticPath, L"..\\..\\Client\\Bin\\Resource\\UI\\UI_Extended\\");	// 문자열 복사
	Extract_Texture_PathInfo(szStaticPath, m_listPathInfo);

	Save_Texture_PathInfo(m_listPathInfo);
}

void CFileInfo::Save_Texture_PathInfo(list<Engine::TEX_INFO*>& rPathInfoLst)
{
	//cout << "Saving TexPath . . ." << endl;

	wofstream fout;

	fout.open(L"../../Data/TexPath.dat");

	if (fout.fail())
		return;

	DWORD dwByte = 0;
	wstring wstrCombined = L"";

	for (auto& pPathInfo : rPathInfoLst)
	{
		wstrCombined = pPathInfo->wstrStateKey
			+ L"|" + pPathInfo->wstrFileName + L"|" + pPathInfo->wstrImgPath + L"|" + pPathInfo->szIsDynamic + L"|" + pPathInfo->szImgCnt;

		fout << wstrCombined << endl;
	}

	fout.close();

	for (auto& pPathInfo : rPathInfoLst)
	{
		Engine::Safe_Delete(pPathInfo);
	}
	rPathInfoLst.clear();

	//cout << "Done . . . !" << endl;
}

void CFileInfo::Extract_Texture_PathInfo(const TCHAR * pPath, list<Engine::TEX_INFO*>& rPathInfoLst, _bool _bIsDynamic)
{
	//cout << "Extracting Path . . ." << endl;

	wstring wstrFindPath = wstring(pPath) + L"\\*.*";

	CFileFind find;

	BOOL bIsFind = find.FindFile(wstrFindPath.c_str());

	TCHAR szBeforeName[MAX_STR] = L"";

	while (bIsFind)
	{
		// 더이상 찾을게 없거나 마지막 파일이나 폴더를 찾으면 FALSE 반환.
		bIsFind = find.FindNextFile();

		if (find.IsDots()) // 찾은 것이 마커라면 건너뛰어라.
			continue;

		else if (find.IsDirectory())
		{
			Extract_Texture_PathInfo(find.GetFilePath(), rPathInfoLst, _bIsDynamic); // 파일 찾을 때까지 재귀.
		}

		else
		{
			if (find.IsSystem())	// 시스템이면 건너 뛰고
				continue;

			CString strFullPath = find.GetFilePath();

			if (strFullPath.Find(L".png") != -1)
			{
				strFullPath.Replace(L"0.", L"%d.");
			}

			CString strRelative = ConvertRelativePath(strFullPath);

			Engine::TEX_INFO* pPathInfo = new Engine::TEX_INFO;

			///////////////////////////////////////////////////////////////////////////

			TCHAR szBuf[MAX_STR] = L"";
			lstrcpy(szBuf, strRelative);


			// 폴더까지의 경로
			::PathRemoveFileSpec(szBuf);
			wstring tmpPathString = szBuf;
			pPathInfo->wstrImgPath = tmpPathString + L"\\";
			//


			if (lstrcmp(szBeforeName, szBuf))
			{
				lstrcpy(szBeforeName, szBuf);
			}

			else
			{
				Engine::Safe_Delete(pPathInfo);
				continue;
			}

			TCHAR szCnt[MAX_STR] = L"";

			_itow_s(CountImageFile(szBuf), szCnt, 10);
			pPathInfo->szImgCnt = szCnt;

			// 파일 이름
			lstrcpy(szBuf, strRelative);
			::PathRemoveBackslash(szBuf);
			wstring tmpString = ::PathFindFileName(szBuf);

			// 파일명
			pPathInfo->wstrFileName = tmpString;

			//::PathRemoveExtension(szBuf);
			PathRemoveExtension(szBuf);
			tmpString = PathFindFileName(szBuf);

			// 키 값
			pPathInfo->wstrStateKey = L"Tex_" + tmpString;

			TCHAR szBuf_2[MAX_STR] = L"";
			(_bIsDynamic ? lstrcpy(szBuf_2, L"1") : lstrcpy(szBuf_2, L"0"));

			pPathInfo->szIsDynamic = szBuf_2;

			rPathInfoLst.push_back(pPathInfo);

			//Engine::Safe_Delete(szBuf_2);
			//break;
		}

	}
}

int CFileInfo::CountImageFile(const TCHAR* pPath)
{
	wstring wstrFindPath = wstring(pPath) + L"\\*.*";
	int iCount = 0;

	CFileFind find;

	// ../Texture/Stage/Effect/BossMultiAttack/*.*
	BOOL bIsFind = find.FindFile(wstrFindPath.c_str());

	while (bIsFind)
	{
		bIsFind = find.FindNextFile();

		if (find.IsDots() || find.IsSystem())
			continue;

		++iCount;
	}

	return iCount;
}