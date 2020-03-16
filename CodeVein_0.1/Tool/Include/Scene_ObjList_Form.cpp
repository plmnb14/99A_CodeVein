// Scene_ObjList_Form.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "Scene_ObjList_Form.h"

// CScene_ObjList_Form

IMPLEMENT_DYNCREATE(CScene_ObjList_Form, CFormView)

CScene_ObjList_Form::CScene_ObjList_Form()
	: CFormView(IDD_SCENE_OBJLIST_FORM)
{

}

CScene_ObjList_Form::~CScene_ObjList_Form()
{
	Free();
}

_bool CScene_ObjList_Form::Add_Object_OnTree(Engine::CRenderObject* _pRendObj)
{
	_tchar* tmpName = new _tchar[MAX_STR];
	_tchar tmpNumber[MAX_STR] = L"";

	// 이름 복사하고,
	lstrcpy(tmpName, _pRendObj->Get_Name());

	//_pRendObj->Get_Index()
	//_wtoi(tmpName)
	_itot_s(_pRendObj->Get_Index(), tmpNumber, 10);

	lstrcat(tmpName, tmpNumber);

	LOOP(MAX_STR)
	{
		if (m_arrObject[i] == nullptr)
		{
			m_arrObject[i] = tmpName;
			break;
		}

		else
			continue;
	}

	return true;
}

void CScene_ObjList_Form::Add_On_List(_tchar * _name)
{
	m_listNewObject.push_back(_name);
}

void CScene_ObjList_Form::Free()
{
	for (auto& iter : m_listObject)
	{
		Engine::Safe_Delete(iter);
	}
	m_listObject.clear();
}

void CScene_ObjList_Form::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE2, m_pTreeCtrl);
	DDX_Control(pDX, IDC_BUTTON2, m_eButton);

	//cout << "여 옵니다" << endl;
}

BEGIN_MESSAGE_MAP(CScene_ObjList_Form, CFormView)
	ON_BN_CLICKED(IDC_BUTTON2, &CScene_ObjList_Form::DataChange)
END_MESSAGE_MAP()


// CScene_ObjList_Form 진단입니다.

#ifdef _DEBUG
void CScene_ObjList_Form::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CScene_ObjList_Form::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CScene_ObjList_Form 메시지 처리기입니다.

void CScene_ObjList_Form::OnTvnSelchangedTree2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);


	HTREEITEM hItemRoot;

	HTREEITEM hItemParent;

	HTREEITEM hItemCur;



	CString strRoot;

	CString strParent;

	CString strCur;

	CString strTxt;



	hItemCur = pNMTreeView->itemNew.hItem;

	hItemParent = this->m_pTreeCtrl.GetParentItem(hItemCur);

	hItemRoot = this->m_pTreeCtrl.GetParentItem(hItemParent);



	strRoot = m_pTreeCtrl.GetItemText(hItemRoot);

	strParent = m_pTreeCtrl.GetItemText(hItemParent);

	strCur = m_pTreeCtrl.GetItemText(hItemCur);


	*pResult = 0;
}

void CScene_ObjList_Form::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	//_tchar tmpChar[MAX_STR] = L"Object";
	//
	//hObjectRoot = m_pTreeCtrl.InsertItem(tmpChar, 0, 0, TVI_ROOT);
}

void CScene_ObjList_Form::DataChange()
{
	//cout << "여 옴? " << endl;
	m_arrObject;
}


void CScene_ObjList_Form::OnBnClickedButton2()
{
	UpdateData(false);

	DataChange();
	m_arrObject;

	UpdateData(true);
	//for (auto& iter : m_arrObject)
	//{
	//	m_listObject.push_back(iter);
	//	m_pTreeCtrl.InsertItem(iter, 0, 0, hObjectRoot);
	//
	//	iter = nullptr;
	//}
	//m_listNewObject.clear();
}
