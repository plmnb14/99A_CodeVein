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
	TCHAR szCurrentDir[STR_128] = L"";
	::GetCurrentDirectory(STR_128, szCurrentDir);

	TCHAR szRelativePath[STR_128] = L"";

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

	_tchar szStaticPath[STR_128] = L"";
	_tchar szDynamicPath[STR_128] = L"";

	cout << "Extracting StaticMesh Path . . ." << endl;

	lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\");	// ���ڿ� ����
	lstrcat(szStaticPath, L"EffectMesh");							// ���ڿ� ����
	Extract_Mesh_PathInfo(szStaticPath, m_listPathInfo);

	cout << "Extracting Complete . . ." << endl;

	Save_Mesh_PathInfo(m_listPathInfo);
}

void CFileInfo::Save_Mesh_PathInfo(list<MESH_INFO*>& rPathInfoLst)
{
	cout << "Saving MeshPath . . ." << endl;

	wofstream fout;

	fout.open(L"../../Data/EffectMesh_Path.dat");

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
			pPathInfo->szIsDynamic;

		fout << wstrCombined << endl;
	}

	fout.close();

	for (auto& pPathInfo : rPathInfoLst)
	{
		Engine::Safe_Delete(pPathInfo);
	}
	rPathInfoLst.clear();

	cout << "Done . . . !" << endl;
}

void CFileInfo::Extract_Mesh_PathInfo(
	const TCHAR* pPath,
	list<MESH_INFO*>& rPathInfoLst , _bool _bIsDynamic)
{
	cout << "Extracting Path . . ." << endl;

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

			TCHAR szBuf[STR_128] = L"";
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

			TCHAR szBuf_2[STR_128] = L"";
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

	_tchar szStaticPath[STR_128] = L"";
	_tchar szDynamicPath[STR_128] = L"";

	cout << "Extracting Texture Path . . ." << endl;

	lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Texture\\Effect");	// ���ڿ� ����
	Extract_Texture_PathInfo(szStaticPath, m_listPathInfo);

	Save_Texture_PathInfo(m_listPathInfo);
}

void CFileInfo::Save_Texture_PathInfo(list<Engine::TEX_INFO*>& rPathInfoLst)
{
	//cout << "Saving TexPath . . ." << endl;

	wofstream fout;

	fout.open(L"../../Data/EffectTex_Path.dat");

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

	TCHAR szBeforeName[STR_128] = L"";

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

			// GetTitleName : ���� �����̸�
			// GetFileName : ���� �����̸� + Ȯ����

			CString strFullPath = find.GetFilePath();
			CString strRelative = ConvertRelativePath(strFullPath);

			if (strRelative.Find(L".png") != -1)
			{
				strRelative.Replace(L"0.", L"%d.");
			}

			else if (strRelative.Find(L".dds") != -1)
			{
				strRelative.Replace(L"0.", L"%d.");
			}

			else if (strRelative.Find(L".tga") != -1)
			{
				strRelative.Replace(L"0.", L"%d.");
			}

			Engine::TEX_INFO* pPathInfo = new Engine::TEX_INFO;

			///////////////////////////////////////////////////////////////////////////

			TCHAR szBuf[STR_128] = L"";
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

			TCHAR szCnt[STR_128] = L"";

			pPathInfo->sImgCnt = CountImageFile(szBuf);

			// ���� �̸�
			lstrcpy(szBuf, strRelative);
			::PathRemoveBackslash(szBuf);
			wstring tmpString = ::PathFindFileName(szBuf);


			wstring tmpString2 = ::PathFindFileName(szBeforeName);

			// ���ϸ�
			pPathInfo->wstrFileName = tmpString;

			//::PathRemoveExtension(szBuf);
			PathRemoveExtension(szBuf);
			tmpString = PathFindFileName(szBuf);

			// Ű ��
			pPathInfo->wstrStateKey = L"Tex_" + tmpString2;

			rPathInfoLst.push_back(pPathInfo);
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