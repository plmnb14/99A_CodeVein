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

	//// From���� To�� �Ѿ�� ����θ� ���� Output�� �����ϴ� �Լ�.
	//::PathRelativePathTo(Output, From, FILE_ATTRIBUTE_DIRECTORY,
	//	To, FILE_ATTRIBUTE_DIRECTORY);
	// ��, From�� To�� ���� ����̺� �ȿ� �־�� �Ѵ�.
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

	lstrcpy(szStaticPath, L"..\\..\\Client\\Bin\\Resource\\Mesh\\");	// ���ڿ� ����
	lstrcat(szStaticPath, L"StaticMesh");								// ���ڿ� ����
	Extract_Mesh_PathInfo(szStaticPath, m_listPathInfo);

	//cout << "Extracting DynamicMesh Path . . ." << endl;

	lstrcpy(szDynamicPath, L"..\\..\\Client\\Bin\\Resource\\Mesh\\");	// ���ڿ� ����
	lstrcat(szDynamicPath, L"DynamicMesh");								// ���ڿ� ����
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
	const TCHAR* pPath, /* D:\\������\\4������\\98��\\Framework98\\Texture\\Stage\\Effect\\BossMultiAttack */
	list<MESH_INFO*>& rPathInfoLst , _bool _bIsDynamic)
{
	//cout << "Extracting Path . . ." << endl;

	wstring wstrFindPath = wstring(pPath) +L"\\*";

	CFileFind find;

	BOOL bIsFind = find.FindFile(wstrFindPath.c_str());

	while (bIsFind)
	{
		// ���̻� ã���� ���ų� ������ �����̳� ������ ã���� FALSE ��ȯ.
		bIsFind = find.FindNextFile();

		if (find.IsDots()) // ã�� ���� ��Ŀ��� �ǳʶپ��.
			continue;

		else if (find.IsDirectory())
		{
			Extract_Mesh_PathInfo(find.GetFilePath(), rPathInfoLst , _bIsDynamic); // ���� ã�� ������ ���.
		}

		else
		{
			if (find.IsSystem())	// �ý����̸� �ǳ� �ٰ�
				continue;

			CString strFullPath = find.GetFilePath();

			if (strFullPath.Find(L".X") == -1)
				continue;

			CString strRelative = ConvertRelativePath(strFullPath);

			Engine::MESH_INFO* pPathInfo = new Engine::MESH_INFO;

			///////////////////////////////////////////////////////////////////////////

			TCHAR szBuf[MAX_STR] = L"";
			lstrcpy(szBuf, strRelative);


			// ���������� ���
			::PathRemoveFileSpec(szBuf);
			wstring tmpPathString = szBuf;
			pPathInfo->wstrImgPath = tmpPathString + L"\\";
			//

			// ���� �̸�
			lstrcpy(szBuf, strRelative);
			::PathRemoveBackslash(szBuf);
			wstring tmpString = ::PathFindFileName(szBuf);

			// ���ϸ�
			pPathInfo->wstrFileName = tmpString;

			//::PathRemoveExtension(szBuf);
			PathRemoveExtension(szBuf);
			tmpString = PathFindFileName(szBuf);

			// Ű ��
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

	lstrcpy(szStaticPath, L"..\\..\\Client\\Bin\\Resource\\UI\\UI_Extended\\");	// ���ڿ� ����
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
		// ���̻� ã���� ���ų� ������ �����̳� ������ ã���� FALSE ��ȯ.
		bIsFind = find.FindNextFile();

		if (find.IsDots()) // ã�� ���� ��Ŀ��� �ǳʶپ��.
			continue;

		else if (find.IsDirectory())
		{
			Extract_Texture_PathInfo(find.GetFilePath(), rPathInfoLst, _bIsDynamic); // ���� ã�� ������ ���.
		}

		else
		{
			if (find.IsSystem())	// �ý����̸� �ǳ� �ٰ�
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


			// ���������� ���
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

			// ���� �̸�
			lstrcpy(szBuf, strRelative);
			::PathRemoveBackslash(szBuf);
			wstring tmpString = ::PathFindFileName(szBuf);

			// ���ϸ�
			pPathInfo->wstrFileName = tmpString;

			//::PathRemoveExtension(szBuf);
			PathRemoveExtension(szBuf);
			tmpString = PathFindFileName(szBuf);

			// Ű ��
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