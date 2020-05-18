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

	//// From에서 To로 넘어가는 상대경로를 만들어서 Output에 저장하는 함수.
	//::PathRelativePathTo(Output, From, FILE_ATTRIBUTE_DIRECTORY,
	//	To, FILE_ATTRIBUTE_DIRECTORY);
	// 단, From과 To는 같은 드라이브 안에 있어야 한다.
	::PathRelativePathTo(szRelativePath, szCurrentDir, FILE_ATTRIBUTE_DIRECTORY,
		pFullPath, FILE_ATTRIBUTE_DIRECTORY);

	return CString(szRelativePath);
}

void CFileInfo::Create_Mesh_PathInfo(Extract_Mesh _eExtract_Mesh)
{
	list<Engine::MESH_INFO*> m_listPathInfo;

	_tchar szStaticPath[STR_128] = L"";
	_tchar szDynamicPath[STR_128] = L"";

	_bool	bDynamic = false;

	cout << "=============================================================" << endl;

	switch (_eExtract_Mesh)
	{
	case Extract_Essential:
	{
		cout << "Extracting EssentialMesh Path . . ." << endl;
		cout << "=============================================================" << endl;
		
		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\");
		lstrcat(szStaticPath, L"Essential");

		break;
	}

	case Extract_Dynamic_Essential:
	{
		cout << "Extracting Hero Path . . ." << endl;
		cout << "=============================================================" << endl;

		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\");
		lstrcat(szStaticPath, L"Hero");

		bDynamic = true;

		break;
	}

	case Extract_Costume_Dynamic:
	{
		cout << "Extracting Costume Dynamic Path . . ." << endl;
		cout << "=============================================================" << endl;

		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\");
		lstrcat(szStaticPath, L"Costume_Dynamic");

		bDynamic = true;

		break;
	}

	case Extract_Costume_Static:
	{
		cout << "Extracting Costume Static Path . . ." << endl;
		cout << "=============================================================" << endl;

		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\");
		lstrcat(szStaticPath, L"Costume_Static");

		break;
	}

	case Extract_NPC:
	{
		cout << "Extracting NPC Mesh Path . . ." << endl;
		cout << "=============================================================" << endl;

		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\");
		lstrcat(szStaticPath, L"NPC");

		bDynamic = true;

		break;
	}

	case Extract_Static_Common:
	{
		cout << "Extracting StaticMesh_Common Path . . ." << endl;
		cout << "=============================================================" << endl;

		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\StaticMesh\\");
		lstrcat(szStaticPath, L"Common");

		break;
	}

	case Extract_Field_Gimmick:
	{
		cout << "Extracting Field_Gimmick Path . . ." << endl;
		cout << "=============================================================" << endl;

		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\StaticMesh\\");
		lstrcat(szStaticPath, L"Field_Gimmick");

		break;
	}

	case Extract_Static_Stage00:
	{
		cout << "Extracting StaticMesh_Stage00 Path . . ." << endl;
		cout << "=============================================================" << endl;

		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\StaticMesh\\");
		lstrcat(szStaticPath, L"Stage_00");

		break;
	}

	case Extract_Static_Stage01:
	{
		cout << "Extracting StaticMesh_Stage01 Path . . ." << endl;
		cout << "=============================================================" << endl;

		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\StaticMesh\\");
		lstrcat(szStaticPath, L"Stage_01");

		break;
	}

	case Extract_Static_Stage02:
	{
		cout << "Extracting StaticMesh_Stage02 Path . . ." << endl;
		cout << "=============================================================" << endl;

		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\StaticMesh\\");
		lstrcat(szStaticPath, L"Stage_02");

		break;
	}

	case Extract_Static_Stage03:
	{
		cout << "Extracting StaticMesh_Stage03 Path . . ." << endl;
		cout << "=============================================================" << endl;

		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\StaticMesh\\");
		lstrcat(szStaticPath, L"Stage_03");

		break;
	}

	case Extract_Static_Stage04:
	{
		cout << "Extracting StaticMesh_Stage04 Path . . ." << endl;
		cout << "=============================================================" << endl;

		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\StaticMesh\\");
		lstrcat(szStaticPath, L"Stage_04");

		break;
	}

	case Extract_Dynamic:
	{
		cout << "Extracting DynamicMesh Path . . ." << endl;
		cout << "=============================================================" << endl;
		
		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\");
		lstrcat(szStaticPath, L"DynamicMesh");						

		bDynamic = true;

		break;
	}

	case Extract_Weapon:
	{
		cout << "Extracting WeaponMesh Path . . ." << endl;
		cout << "=============================================================" << endl;

		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\");
		lstrcat(szStaticPath, L"Weapons");

		break;
	}

	case Extract_DrainWeapon:
	{
		cout << "Extracting DrainWeaponMesh Path . . ." << endl;
		cout << "=============================================================" << endl;

		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\");
		lstrcat(szStaticPath, L"DrainWeapons");

		bDynamic = true;

		break;
	}

	case Extract_Effect:
	{
		cout << "Extracting EffectMesh Path . . ." << endl;
		cout << "=============================================================" << endl;

		lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Mesh\\");
		lstrcat(szStaticPath, L"EffectMesh");
		break;
	}

	}

	Extract_Mesh_PathInfo(szStaticPath, m_listPathInfo, bDynamic);
	Save_Mesh_PathInfo(m_listPathInfo, _eExtract_Mesh);
}

void CFileInfo::Save_Mesh_PathInfo(list<MESH_INFO*>& rPathInfoLst, Extract_Mesh _eExtract_Mesh)
{
	cout << "Saving MeshPath . . ." << endl;

	_tchar szDataPath[STR_128] = L"";

	switch (_eExtract_Mesh)
	{
	case Extract_Essential:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_Essential_Path.dat");
		break;
	}

	case Extract_Dynamic_Essential:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_Essential_Dynamic_Path.dat");
		break;
	}

	case Extract_Costume_Static:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_Costume_Static_Path.dat");
		break;
	}

	case Extract_Costume_Dynamic:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_Costume_Dynamic_Path.dat");
		break;
	}

	case Extract_NPC:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_NPC_Path.dat");
		break;
	}

	case Extract_Dynamic:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_Dynamic_Path.dat");

		break;
	}

	case Extract_Static_Common:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_Static_Common_Path.dat");

		break;
	}

	case Extract_Field_Gimmick:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_Field_Gimmick_Path.dat");

		break;
	}

	case Extract_Static_Stage00:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_Static_Stage00_Path.dat");

		break;
	}

	case Extract_Static_Stage01:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_Static_Stage01_Path.dat");

		break;
	}

	case Extract_Static_Stage02:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_Static_Stage02_Path.dat");

		break;
	}

	case Extract_Static_Stage03:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_Static_Stage03_Path.dat");

		break;
	}

	case Extract_Static_Stage04:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_Static_Stage04_Path.dat");

		break;
	}

	case Extract_Weapon:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_Weapon_Path.dat");
		break;
	}
	case Extract_DrainWeapon:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_DrainWeapon_Path.dat");
		break;
	}
	case Extract_Effect:
	{
		lstrcpy(szDataPath, L"../../Data/Load_MeshData/Mesh_Effect_Path.dat");
		break;
	}
	}

	//wofstream fout(szDataPath, ios::out | ios::binary);
	//
	//if (fout.fail())
	//	return;
	//
	//for (auto& pPathInfo : rPathInfoLst)
	//{
	//	fout << pPathInfo->wstrStateKey;
	//	fout << L"|";
	//	fout << pPathInfo->wstrFileName;
	//	fout << L"|";
	//	fout << pPathInfo->wstrImgPath;
	//	fout << L"|";
	//	fout << pPathInfo->szIsDynamic << endl;
	//
	//	//fout.write(pPathInfo->wstrStateKey.c_str(), pPathInfo->wstrStateKey.size());
	//	//fout.write(pPathInfo->wstrFileName.c_str(), pPathInfo->wstrFileName.size());
	//	//fout.write(pPathInfo->wstrImgPath.c_str(), pPathInfo->wstrImgPath.size());
	//	//fout.write(pPathInfo->szIsDynamic.c_str(), pPathInfo->szIsDynamic.size()) << endl;
	//}
	//
	//fout.flush();
	//fout.close();
	//
	//for (auto& pPathInfo : rPathInfoLst)
	//{
	//	Engine::Safe_Delete(pPathInfo);
	//}
	//
	//rPathInfoLst.clear();

	HANDLE hFile = ::CreateFile(szDataPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	
	DWORD dwByte = 0;

	for (auto& pPathInfo : rPathInfoLst)
	{
		if (INVALID_HANDLE_VALUE == hFile)
			::MSG_BOX("Save Failed. [INVALID_HANDLE_VALUE]");
	
		const _tchar* szKey = pPathInfo->wstrStateKey.c_str();
		const _tchar* szFile = pPathInfo->wstrFileName.c_str();
		const _tchar* szPath = pPathInfo->wstrImgPath.c_str();
		const _bool bDynamic = pPathInfo->bIsDynamic;

		::WriteFile(hFile, szKey, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::WriteFile(hFile, szFile, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::WriteFile(hFile, szPath, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::WriteFile(hFile, &bDynamic, sizeof(_bool), &dwByte, nullptr);

		if (rPathInfoLst.size() == 1)
			break;
	}
	
	CloseHandle(hFile);

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

			TCHAR szBuf[STR_128] = L"";
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

			pPathInfo->bIsDynamic = _bIsDynamic;

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

	lstrcpy(szStaticPath, L"..\\..\\Client\\Resources\\Texture");	// 문자열 복사
	Extract_Texture_PathInfo(szStaticPath, m_listPathInfo);

	Save_Texture_PathInfo(m_listPathInfo);
}

void CFileInfo::Save_Texture_PathInfo(list<Engine::TEX_INFO*>& rPathInfoLst)
{
	cout << "Saving TexPath . . ." << endl;

	HANDLE hFile = ::CreateFile(L"../../Data/Load_TexData/Tex_Path.dat", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	for (auto& pPathInfo : rPathInfoLst)
	{
		if (INVALID_HANDLE_VALUE == hFile)
			::MSG_BOX("Save Failed. [INVALID_HANDLE_VALUE]");

		DWORD dwByte = 0;

		const _tchar* szKey = pPathInfo->wstrStateKey.c_str();
		const _tchar* szFile = pPathInfo->wstrFileName.c_str();
		const _tchar* szPath = pPathInfo->wstrImgPath.c_str();
		const _ushort sCnt = pPathInfo->sImgCnt;

		::WriteFile(hFile, szKey, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::WriteFile(hFile, szFile, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::WriteFile(hFile, szPath, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::WriteFile(hFile, &sCnt, sizeof(_ushort), &dwByte, nullptr);

		if (rPathInfoLst.size() == 1)
			break;
	}

	CloseHandle(hFile);

	//if (fout.fail())
	//	return;
	//
	//DWORD dwByte = 0;
	//wstring wstrCombined = L"";
	//
	//for (auto& pPathInfo : rPathInfoLst)
	//{
	//	wstrCombined = pPathInfo->wstrStateKey
	//		+ L"|" + pPathInfo->wstrFileName + L"|" + pPathInfo->wstrImgPath + L"|" + pPathInfo->szIsDynamic + L"|" + pPathInfo->szImgCnt;
	//
	//	fout << wstrCombined << endl;
	//}
	//
	//fout.close();
	//
	//for (auto& pPathInfo : rPathInfoLst)
	//{
	//	Engine::Safe_Delete(pPathInfo);
	//}
	//rPathInfoLst.clear();

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

			// GetTitleName : 순수 파일이름
			// GetFileName : 순수 파일이름 + 확장자

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

			TCHAR szCnt[STR_128] = L"";

			pPathInfo->sImgCnt = CountImageFile(szBuf);

			// 파일 이름
			lstrcpy(szBuf, strRelative);
			::PathRemoveBackslash(szBuf);
			wstring tmpString = ::PathFindFileName(szBuf);


			wstring tmpString2 = ::PathFindFileName(szBeforeName);

			// 파일명
			pPathInfo->wstrFileName = tmpString;

			//::PathRemoveExtension(szBuf);
			PathRemoveExtension(szBuf);
			tmpString = PathFindFileName(szBuf);

			// 키 값
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