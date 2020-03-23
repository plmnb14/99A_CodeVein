// ResListPopup.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "ResListPopup.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "MyFormView.h"
#include "EffectToolView.h"

// CResListPopup ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CResListPopup, CDialog)

CResListPopup::CResListPopup(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_RESLISTPOPUP, pParent)
{

}

CResListPopup::~CResListPopup()
{
	for (auto& iter : m_listFileInfo)
		Safe_Delete(iter);
}

void CResListPopup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListResourses);
	DDX_Control(pDX, IDC_PIC, m_PictureControl);
}


BEGIN_MESSAGE_MAP(CResListPopup, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST1, &CResListPopup::OnLbnSelchangeList)
END_MESSAGE_MAP()


// CResListPopup �޽��� ó�����Դϴ�.


void CResListPopup::OnLbnSelchangeList()
{
	UpdateData(TRUE);

	CMainFrame* pMainFrm = dynamic_cast<CMainFrame*>(::AfxGetApp()->GetMainWnd());

	CMyFormView* pFormView = dynamic_cast<CMyFormView*>(pMainFrm->m_MainSplitter.GetPane(0, 0));

	CEffectToolView* pView = dynamic_cast<CEffectToolView*>(pMainFrm->m_MainSplitter.GetPane(0, 1));

	int iIndex = m_ListResourses.GetCurSel();

	if (-1 == iIndex)
		return;

	CString strFileName = L"";
	TCHAR szObjectKey[MAX_STR] = L"";
	TCHAR szFileName[MAX_STR] = L"";

	m_ListResourses.GetText(iIndex, strFileName);

	auto& iter_begin = m_listFileInfo.begin();

	for (int i = 0; i < iIndex; i++)
		iter_begin++;

	switch (m_eResType)
	{
	case CResListPopup::TYPE_MESH:
	{
		pFormView->m_ParticleTab.Set_MeshName((*iter_begin)->tPathInfo.sztrStateKey);
		break;
	}
	case CResListPopup::TYPE_TEX:
	{
		pFormView->m_ParticleTab.Set_TexName((*iter_begin)->tPathInfo.sztrStateKey);
		pFormView->m_ParticleTab.Set_MaxFrame(_float((*iter_begin)->iMaxCount));
		pFormView->m_ParticleTab.Set_Index((*iter_begin)->tPathInfo.szImgCnt);

		//CHAR szForPrint[MAX_STR] = "";
		//WideCharToMultiByte(CP_ACP, 0, (*iter_begin)->tPathInfo.sztrStateKey, MAX_STR, szForPrint, MAX_STR, NULL, NULL);
		//cout << szForPrint << endl;
		//cout << (*iter_begin)->iMaxCount << endl;
		//WideCharToMultiByte(CP_ACP, 0, (*iter_begin)->tPathInfo.szImgCnt, MAX_STR, szForPrint, MAX_STR, NULL, NULL);
		//cout << szForPrint << endl;
		//cout << "=======================================================" << endl;

		break;
	}
	case CResListPopup::TYPE_COLOR:
	{
		pFormView->m_ParticleTab.Set_ColorTexName((*iter_begin)->tPathInfo.sztrStateKey);
		pFormView->m_ParticleTab.Set_ColorIndex((*iter_begin)->tPathInfo.szImgCnt);
		break;
	}
	case CResListPopup::TYPE_GRADIENT:
		pFormView->m_ParticleTab.Set_GradientTexName((*iter_begin)->tPathInfo.sztrStateKey);
		break;
	}

	UpdateData(FALSE);
}

void CResListPopup::Set_ResType(RES_TYPE eType)
{
	m_eResType = eType;

	switch (eType)
	{
	case CResListPopup::TYPE_MESH:
	{
		Load_MeshFromPath();
		break;
	}
	case CResListPopup::TYPE_TEX:
	{
		Load_TextureFromPath();
		break;
	}
	case CResListPopup::TYPE_COLOR:
	{
		Load_TextureFromPath();
		break;
	}
	case CResListPopup::TYPE_GRADIENT:
	{
		Load_TextureFromPath();
		break;
	}
	}
}

void CResListPopup::Load_TextureFromPath()
{
	wifstream fin;

	fin.open(L"../../Data/Tex_Path.dat");

	if (fin.fail())
		return;

	list<PATH_INFO*> tmpList;

	while (true)
	{
		PATH_INFO* tmpPath = new PATH_INFO;

		fin.getline(tmpPath->sztrStateKey, MAX_STR, '|');
		fin.getline(tmpPath->sztrFileName, MAX_STR, '|');
		fin.getline(tmpPath->sztrImgPath, MAX_STR, '|');
		fin.getline(tmpPath->szIsDynamic, MAX_STR, '|');
		fin.getline(tmpPath->szImgCnt, MAX_STR);

		if (fin.eof())
		{
			Safe_Delete(tmpPath);
			break;
		}

		tmpList.push_back(tmpPath);
	}

	fin.close();

	m_ListResourses.ResetContent();
	for (auto& iter : tmpList)
	{
		for (_int i = 0; i < stoi(iter->szImgCnt); ++i)
		{

			_tchar szFileName[256] = L"";
			wsprintf(szFileName, iter->sztrFileName, i);
			m_ListResourses.AddString(szFileName);

			_tchar szPath[256] = L"";
			lstrcpy(szPath, iter->sztrImgPath);
			lstrcat(szPath, szFileName);

			PATH_INFO_RE* tmpPath = new PATH_INFO_RE;

			lstrcpy(tmpPath->tPathInfo.sztrStateKey, iter->sztrStateKey);
			lstrcpy(tmpPath->tPathInfo.sztrFileName, szFileName);
			lstrcpy(tmpPath->tPathInfo.sztrImgPath, szPath);
			lstrcpy(tmpPath->tPathInfo.szIsDynamic, L"");
			_tchar szNum[128] = L"";
			wsprintf(szNum, L"%d", i);
			lstrcpy(tmpPath->tPathInfo.szImgCnt, szNum);
			tmpPath->iMaxCount = stoi(iter->szImgCnt);

			m_listFileInfo.push_back(tmpPath);
		}
	}

	for (auto& iter : tmpList)
		Safe_Delete(iter);
	tmpList.clear();
}

void CResListPopup::Load_MeshFromPath()
{
	wifstream fin;

	fin.open(L"../../Data/Mesh_Path.dat");

	if (fin.fail())
		return;

	list<PATH_INFO*> tmpList;

	while (true)
	{
		PATH_INFO* tmpPath = new PATH_INFO;

		fin.getline(tmpPath->sztrStateKey, MAX_STR, '|');
		fin.getline(tmpPath->sztrFileName, MAX_STR, '|');
		fin.getline(tmpPath->sztrImgPath, MAX_STR, '|');
		fin.getline(tmpPath->szIsDynamic, MAX_STR);

		if (fin.eof())
		{
			Safe_Delete(tmpPath);
			break;
		}

		tmpList.push_back(tmpPath);
	}

	fin.close();

	m_ListResourses.ResetContent();
	for (auto& iter : tmpList)
	{
		m_ListResourses.AddString(iter->sztrFileName);

		_tchar szPath[256] = L"";
		lstrcpy(szPath, iter->sztrImgPath);
		lstrcat(szPath, iter->sztrFileName);

		PATH_INFO_RE* tmpPath = new PATH_INFO_RE;

		lstrcpy(tmpPath->tPathInfo.sztrStateKey, iter->sztrStateKey);
		lstrcpy(tmpPath->tPathInfo.sztrFileName, iter->sztrFileName);
		lstrcpy(tmpPath->tPathInfo.sztrImgPath, szPath);
		lstrcpy(tmpPath->tPathInfo.szIsDynamic, L"");
		_tchar szNum[128] = L"0";
		lstrcpy(tmpPath->tPathInfo.szImgCnt, szNum);

		m_listFileInfo.push_back(tmpPath);
	}

	for (auto& iter : tmpList)
		Safe_Delete(iter);
	tmpList.clear();
}
