// MapTool_Page.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "MapTool_Page.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "Inspector_Sheet.h"
#include "Essential_Sheet.h"
#include "Inspector_Page.h"
#include "TreeFinder.h"
#include "CollisionMgr.h"

//#include "Scene_ObjList_Form.h"
#include "Management.h"
// CMapTool_Page 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMapTool_Page, CPropertyPage)

CMapTool_Page::CMapTool_Page()
	: CPropertyPage(IDD_MAPTOOL_PAGE),
	m_pRenderObj(nullptr)
	, m_pPickObj(nullptr)
	, m_iEditMode(0)
	, m_bCheckObj(false)
	, m_bDelete(false)
	, m_strPos_X(_T("0.000"))
	, m_strPos_Y(_T("0.000"))
	, m_strPos_Z(_T("0.000"))
	, m_strRot_X(_T("0.000"))
	, m_strRot_Y(_T("0.000"))
	, m_strRot_Z(_T("0.000"))
	, m_strScale_X(_T("0.000"))
	, m_strScale_Y(_T("0.000"))
	, m_strScale_Z(_T("0.000"))
	, m_strPosRate(_T("0.000"))
	, m_strRotRate(_T("0.000"))
	, m_strScaleRate(_T("0.000"))
	, m_iEditType(0)
	, m_bSelectAdvence(FALSE)
	, m_strLayer_Cur(_T("0"))
	, m_strLayer_Render(_T("0"))
	, m_CurObjLayerIdx(_T("0"))
{

}

CMapTool_Page::~CMapTool_Page()
{
	Free();
}

void CMapTool_Page::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	DDX_Radio(pDX, ID_EDIT_00, m_iEditMode);
	DDX_Control(pDX, IDC_TREE3, m_pObjectTree);
	DDX_Text(pDX, IDC_TRANS_X, m_strPos_X);
	DDX_Text(pDX, IDC_TRANS_Y, m_strPos_Y);
	DDX_Text(pDX, IDC_TRANS_Z, m_strPos_Z);
	DDX_Text(pDX, IDC_ROTATE_X, m_strRot_X);
	DDX_Text(pDX, IDC_ROTATE_Y, m_strRot_Y);
	DDX_Text(pDX, IDC_ROTATE_Z, m_strRot_Z);
	DDX_Text(pDX, IDC_SCALE_X, m_strScale_X);
	DDX_Text(pDX, IDC_SCALE_Y, m_strScale_Y);
	DDX_Text(pDX, IDC_SCALE_Z, m_strScale_Z);
	DDX_Text(pDX, IDC_TRANS_RATE, m_strPosRate);
	DDX_Text(pDX, IDC_ROTATE_RATE, m_strRotRate);
	DDX_Text(pDX, IDC_SCALE_RATE, m_strScaleRate);
	DDX_Radio(pDX, ID_TRANSFORM_00, m_iEditType);
	DDV_MinMaxInt(pDX, m_iEditType, 0, 2);
	DDX_Radio(pDX, IDC_CTRL_00, m_bSelectAdvence);
	DDX_Text(pDX, IDC_LAYER_CUR, m_strLayer_Cur);
	DDX_Text(pDX, IDC_LAYER_RENDER, m_strLayer_Render);
	DDX_Text(pDX, IDC_LAYER_CUR2, m_CurObjLayerIdx);
}

void CMapTool_Page::Delete_MouseObj()
{
	if (m_pRenderObj != nullptr)
	{
		Safe_Release(m_pRenderObj);
	}
}

void CMapTool_Page::Add_Tree()
{
}

void CMapTool_Page::Add_Branch()
{

}

void CMapTool_Page::Update()
{
	if (m_iEditMode != _CREATE)
	{
		if (m_pRenderObj != nullptr)
		{
			Safe_Release(m_pRenderObj);
		}
	}

	KeyInput();

	if (m_pRenderObj != nullptr)
	{
		if (m_iEditMode == _CREATE)
		{
			if (CInput_Device::Get_Instance()->Get_DIMouseState(CInput_Device::DIM_RB))
			{
				Safe_Release(m_pRenderObj);
			}
		}
	}
}

void CMapTool_Page::Render()
{
	if (m_pRenderObj != nullptr)
	{
		TARGET_TO_TRANS(m_pRenderObj)->Set_Pos(m_vMouse);

		m_pRenderObj->Update_GameObject(0);
		m_pRenderObj->Render_GameObject();
	}

	for (auto& iter : m_listObject)
	{
		if (false == m_bShowAll)
		{
			if(m_sLayerRenderIdx != iter->Get_LayerIdx())
				continue;
		}

		iter->Update_GameObject(0);
		iter->Render_GameObject();
	}
}

void CMapTool_Page::MyUpdate_Data()
{
}

void CMapTool_Page::KeyInput()
{
	IF_NULL_RETURN(m_pSelectedObj)

	if (m_bSelectAdvence == 1)
	{
		Key_Acting();

		//switch (m_iEditType)
		//{
		//case 0:
		//{
		//	Key_Pos();
		//	break;
		//}
		//
		//case 1:
		//{
		//	Key_Rot();
		//	break;
		//}
		//
		//case 2:
		//{
		//	Key_Scale();
		//	break;
		//}
		//}
	}

	else
	{
		Object_Movement();
		TARGET_TO_TRANS(m_pSelectedObj)->Set_Pos(g_MousePos);
	}
}

void CMapTool_Page::Key_Acting()
{
	if (false == m_bDetailMode)
	{
		if (Engine::CInput_Device::Get_Instance()->Key_Pressing(DIK_LEFTARROW))
		{
			if (0 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Pos_Divide(-m_fPosRate, AXIS_X);
			}

			else if (1 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Angle(AXIS_Y, m_fRotRate);
			}

			Object_Movement();
		}

		else if (Engine::CInput_Device::Get_Instance()->Key_Pressing(DIK_RIGHTARROW))
		{
			if (0 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Pos_Divide(m_fPosRate, AXIS_X);
			}

			else if (1 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Angle(AXIS_Y, -m_fRotRate);
			}

			Object_Movement();
		}

		if (Engine::CInput_Device::Get_Instance()->Key_Pressing(DIK_UPARROW))
		{
			if (0 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Pos_Divide(m_fPosRate, AXIS_Z);
			}

			else if (1 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Angle(AXIS_X, m_fRotRate);
			}

			Object_Movement();
		}

		else if (Engine::CInput_Device::Get_Instance()->Key_Pressing(DIK_DOWNARROW))
		{
			if (0 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Pos_Divide(-m_fPosRate, AXIS_Z);
			}

			else if (1 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Angle(AXIS_X, -m_fRotRate);
			}

			Object_Movement();
		}

		if (Engine::CInput_Device::Get_Instance()->Key_Pressing(DIK_G))
		{
			if (0 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Pos_Divide(-m_fPosRate, AXIS_Y);
			}

			else if (1 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Angle(AXIS_Z, -m_fRotRate);
			}

			Object_Movement();
		}

		else if (Engine::CInput_Device::Get_Instance()->Key_Pressing(DIK_H))
		{
			if (0 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Pos_Divide(m_fPosRate, AXIS_Y);
			}

			else if (1 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Angle(AXIS_Z, m_fRotRate);
			}

			Object_Movement();
		}
	}

	else if (true == m_bDetailMode)
	{
		if (Engine::CInput_Device::Get_Instance()->Key_Down(DIK_LEFTARROW))
		{
			if (0 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Pos_Divide(-m_fPosRate, AXIS_X);
			}

			else if (1 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Angle(AXIS_Y, m_fRotRate);
			}

			Object_Movement();
		}

		else if (Engine::CInput_Device::Get_Instance()->Key_Down(DIK_RIGHTARROW))
		{
			if (0 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Pos_Divide(m_fPosRate, AXIS_X);
			}

			else if (1 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Angle(AXIS_Y, -m_fRotRate);
			}

			Object_Movement();
		}

		if (Engine::CInput_Device::Get_Instance()->Key_Down(DIK_UPARROW))
		{
			if (0 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Pos_Divide(m_fPosRate, AXIS_Z);
			}

			else if (1 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Angle(AXIS_X, m_fRotRate);
			}

			Object_Movement();
		}

		else if (Engine::CInput_Device::Get_Instance()->Key_Down(DIK_DOWNARROW))
		{
			if (0 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Pos_Divide(-m_fPosRate, AXIS_Z);
			}

			else if (1 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Angle(AXIS_X, -m_fRotRate);
			}

			Object_Movement();
		}

		if (Engine::CInput_Device::Get_Instance()->Key_Down(DIK_G))
		{
			if (0 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Pos_Divide(-m_fPosRate, AXIS_Y);
			}

			else if (1 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Angle(AXIS_Z, -m_fRotRate);
			}

			Object_Movement();
		}

		else if (Engine::CInput_Device::Get_Instance()->Key_Down(DIK_H))
		{
			if (0 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Pos_Divide(m_fPosRate, AXIS_Y);
			}

			else if (1 == m_iEditType)
			{
				TARGET_TO_TRANS(m_pSelectedObj)->Add_Angle(AXIS_Z, m_fRotRate);
			}

			Object_Movement();
		}
	}
}

void CMapTool_Page::Save_Object()
{
	if (m_listObject.size() == 0)
		return;

	wofstream fout;

	fout.open(L"../../Data/Stage_Test.dat");

	if (fout.fail())
		return;

	DWORD dwByte = 0;
	_tchar szObjdata[MAX_STR] = L"";
	_tchar szSlash[2] = L"|";
	wstring wstrCombined = L"";

	for (auto& pObj : m_listObject)
	{
		Engine::OBJ_INFO* ObjInfo = new Engine::OBJ_INFO;

		Engine::CRenderObject* tmpObj = nullptr;

		tmpObj = static_cast<Engine::CRenderObject*>(pObj);

		Engine::CTransform* ObjTrans = TARGET_TO_TRANS(tmpObj);

		// 정보 복사
		lstrcpy(ObjInfo->szName, tmpObj->Get_Name());
		_stprintf_s(ObjInfo->szLayerIdx, _T("%d"), tmpObj->Get_LayerIdx());
		_stprintf_s(ObjInfo->szIndex, _T("%d"), tmpObj->Get_Index());

		_stprintf_s(ObjInfo->szPos_X, _T("%.3f"), ObjTrans->Get_Pos().x);
		_stprintf_s(ObjInfo->szPos_Y, _T("%.3f"), ObjTrans->Get_Pos().y);
		_stprintf_s(ObjInfo->szPos_Z, _T("%.3f"), ObjTrans->Get_Pos().z);

		_stprintf_s(ObjInfo->szRot_X, _T("%.3f"), ObjTrans->Get_Angle().x);
		_stprintf_s(ObjInfo->szRot_Y, _T("%.3f"), ObjTrans->Get_Angle().y);
		_stprintf_s(ObjInfo->szRot_Z, _T("%.3f"), ObjTrans->Get_Angle().z);

		_stprintf_s(ObjInfo->szScale_X, _T("%.3f"), ObjTrans->Get_Size().x);
		_stprintf_s(ObjInfo->szScale_Y, _T("%.3f"), ObjTrans->Get_Size().y);
		_stprintf_s(ObjInfo->szScale_Z, _T("%.3f"), ObjTrans->Get_Size().z);


		// 정보 합치기
		lstrcpy(szObjdata, ObjInfo->szName);
		lstrcat(szObjdata, szSlash);
		lstrcat(szObjdata, ObjInfo->szLayerIdx);
		lstrcat(szObjdata, szSlash);
		lstrcat(szObjdata, ObjInfo->szIndex);
		lstrcat(szObjdata, szSlash);

		lstrcat(szObjdata, ObjInfo->szPos_X);
		lstrcat(szObjdata, szSlash);
		lstrcat(szObjdata, ObjInfo->szPos_Y);
		lstrcat(szObjdata, szSlash);
		lstrcat(szObjdata, ObjInfo->szPos_Z);
		lstrcat(szObjdata, szSlash);

		lstrcat(szObjdata, ObjInfo->szRot_X);
		lstrcat(szObjdata, szSlash);
		lstrcat(szObjdata, ObjInfo->szRot_Y);
		lstrcat(szObjdata, szSlash);
		lstrcat(szObjdata, ObjInfo->szRot_Z);
		lstrcat(szObjdata, szSlash);

		lstrcat(szObjdata, ObjInfo->szScale_X);
		lstrcat(szObjdata, szSlash);
		lstrcat(szObjdata, ObjInfo->szScale_Y);
		lstrcat(szObjdata, szSlash);
		lstrcat(szObjdata, ObjInfo->szScale_Z);
		lstrcat(szObjdata, szSlash);

		wstrCombined = szObjdata;

		fout << wstrCombined << endl;

		Engine::Safe_Delete(ObjInfo);
	}

	fout.close();
}

void CMapTool_Page::Load_Object()
{
	m_pObjectTree.DeleteAllItems();
	hObjectRoot = m_pObjectTree.InsertItem(TEXT("Object"), 0, 0, TVI_ROOT, TVI_LAST);

	for (auto& iter : m_listObject)
	{
		Safe_Release(iter);
	}
	m_listObject.clear();

	wifstream fin;

	_int	iIndex;
	_int	iTag;
	_int	iLayerIdx;

	_float fA[3];
	_float fB[3];
	_float fC[3];

	_v3    vVtx[3];

	fin.open(L"../../Data/Stage_Test.dat");

	if (fin.fail())
		return;

	while (true)
	{
		Engine::OBJ_INFO ObjInfo;

		fin.getline(ObjInfo.szName, MAX_STR, '|');
		fin.getline(ObjInfo.szLayerIdx, MAX_STR, '|');
		fin.getline(ObjInfo.szIndex, MAX_STR, '|');

		fin.getline(ObjInfo.szPos_X, MAX_STR, '|');
		fin.getline(ObjInfo.szPos_Y, MAX_STR, '|');
		fin.getline(ObjInfo.szPos_Z, MAX_STR, '|');
		
		fin.getline(ObjInfo.szRot_X, MAX_STR, '|');
		fin.getline(ObjInfo.szRot_Y, MAX_STR, '|');
		fin.getline(ObjInfo.szRot_Z, MAX_STR, '|');
		
		fin.getline(ObjInfo.szScale_X, MAX_STR, '|');
		fin.getline(ObjInfo.szScale_Y, MAX_STR, '|');
		fin.getline(ObjInfo.szScale_Z, MAX_STR);


		if (fin.eof())
			break;

		else
		{
			iIndex = _ttoi(ObjInfo.szIndex);
			iTag = _ttoi(ObjInfo.szTag);
			iLayerIdx = _ttoi(ObjInfo.szLayerIdx);

			fA[0] = (_float)_tstof(ObjInfo.szPos_X);
			fA[1] = (_float)_tstof(ObjInfo.szPos_Y);
			fA[2] = (_float)_tstof(ObjInfo.szPos_Z);

			fB[0] = (_float)_tstof(ObjInfo.szRot_X);
			fB[1] = (_float)_tstof(ObjInfo.szRot_Y);
			fB[2] = (_float)_tstof(ObjInfo.szRot_Z);

			fC[0] = (_float)_tstof(ObjInfo.szScale_X);
			fC[1] = (_float)_tstof(ObjInfo.szScale_Y);
			fC[2] = (_float)_tstof(ObjInfo.szScale_Z);

			vVtx[0] = { fA[0], fA[1], fA[2] };
			vVtx[1] = { fB[0], fB[1], fB[2] };
			vVtx[2] = { fC[0], fC[1], fC[2] };


			Engine::CRenderObject*	pInstance = Engine::CRenderObject::Create(g_pGraphicDev);
			pInstance->Set_OnTool(true);
			pInstance->Change_Mesh(ObjInfo.szName);
			pInstance->Set_Index(iIndex);
			pInstance->Set_LayerIdx(iLayerIdx);

			TARGET_TO_TRANS(pInstance)->Set_Pos(vVtx[0]);
			TARGET_TO_TRANS(pInstance)->Set_Angle(vVtx[1]);
			TARGET_TO_TRANS(pInstance)->Set_Scale(vVtx[2]);

			m_listObject.push_back(pInstance);
			
			_tchar szObjName[MAX_STR] = L"";
			lstrcpy(szObjName, ObjInfo.szName);
			lstrcat(szObjName, ObjInfo.szIndex);

			m_pObjectTree.InsertItem(szObjName, 0, 0, hObjectRoot);

			pInstance = nullptr;
		}
	}

	fin.close();
}

void CMapTool_Page::CreateObject()
{
	if (m_pRenderObj == nullptr)
		return;

	_int iIndex = 0;

	// 렌더 오브젝트를 복사 한다.
	Engine::CRenderObject* pInstace = Engine::CRenderObject::CreateClone(m_pRenderObj);
	pInstace->Set_OnTool(true);
	pInstace->Set_LayerIdx(m_sLayerCurIdx);

	// 현재 오브젝트 리스트에서 같은 이름이 있는지 찾는다.
	for (auto& iter : m_listObject)
	{
		Engine::CRenderObject* tmpRend = static_cast<Engine::CRenderObject*>(iter);

		// 이름 비교해서
		if (!lstrcmp(tmpRend->Get_Name(), pInstace->Get_Name()))
		{
			// 같으면 인덱스 + 1
			if (tmpRend->Get_Index() == iIndex)
			{
				++iIndex;
			}

			// 다르면 탈출
			else
				break;
		}
	}

	pInstace->Set_Index(iIndex);
	
	Add_Object_OnTree(pInstace);
	m_listObject.push_back(pInstace);
}

void CMapTool_Page::Delete_SelectObject()
{
	IF_NOT_NULL(m_pSelectedObj)
	{
		for (auto& iter : m_listObject)
		{
			if (static_cast<Engine::CRenderObject*>(iter)->Get_Index() == m_pSelectedObj->Get_Index())
			{
				if (!lstrcmp(static_cast<Engine::CRenderObject*>(iter)->Get_Name(), m_pSelectedObj->Get_Name()))
				{
					_int	iIndex = 0;
					_tchar	szIndex[32] = L"";
					_tchar	szName[MAX_STR] = L"";

					iIndex = m_pSelectedObj->Get_Index();
					_stprintf_s(szIndex, _T("%d"), iIndex);

					memcpy(&szName, m_pSelectedObj->Get_Name(), sizeof(_tchar[MAX_STR]));
					lstrcat(szName, szIndex);

					Safe_Release(iter);

					m_pSelectedObj = nullptr;

					m_listObject.remove(iter);

					HTREEITEM tmpNode = CTreeFinder::Find_Node_By_Name(m_pObjectTree, szName);

					m_pObjectTree.DeleteItem(tmpNode);
					return;
				}

				else
					continue;
			}
		}
	}
}

void CMapTool_Page::Object_Movement()
{
	IF_NOT_NULL(m_pRenderObj)
	{
		Engine::CTransform*	targetTrans = TARGET_TO_TRANS(m_pRenderObj);

		m_strPos_X.Format(_T("%.3f"), targetTrans->Get_Pos().x);
		m_strPos_Y.Format(_T("%.3f"), targetTrans->Get_Pos().y);
		m_strPos_Z.Format(_T("%.3f"), targetTrans->Get_Pos().z);

		SetDlgItemText(IDC_TRANS_X, m_strPos_X);
		SetDlgItemText(IDC_TRANS_Y, m_strPos_Y);
		SetDlgItemText(IDC_TRANS_Z, m_strPos_Z);

		m_strRot_X.Format(_T("%.3f"), D3DXToDegree(targetTrans->Get_Angle().x));
		m_strRot_Y.Format(_T("%.3f"), D3DXToDegree(targetTrans->Get_Angle().y));
		m_strRot_Z.Format(_T("%.3f"), D3DXToDegree(targetTrans->Get_Angle().z));

		SetDlgItemText(IDC_ROTATE_X, m_strRot_X);
		SetDlgItemText(IDC_ROTATE_Y, m_strRot_Y);
		SetDlgItemText(IDC_ROTATE_Z, m_strRot_Z);

		m_strScale_X.Format(_T("%.3f"), targetTrans->Get_Size().x);
		m_strScale_Y.Format(_T("%.3f"), targetTrans->Get_Size().y);
		m_strScale_Z.Format(_T("%.3f"), targetTrans->Get_Size().z);

		SetDlgItemText(IDC_SCALE_X, m_strScale_X);
		SetDlgItemText(IDC_SCALE_Y, m_strScale_Y);
		SetDlgItemText(IDC_SCALE_Z, m_strScale_Z);
	}

	else IF_NOT_NULL(m_pSelectedObj)
	{
		//cout << "선택됨!" << endl;

		Engine::CTransform*	targetTrans = TARGET_TO_TRANS(m_pSelectedObj);


		m_strPos_X.Format(_T("%.3f"), targetTrans->Get_Pos().x);
		m_strPos_Y.Format(_T("%.3f"), targetTrans->Get_Pos().y);
		m_strPos_Z.Format(_T("%.3f"), targetTrans->Get_Pos().z);

		SetDlgItemText(IDC_TRANS_X, m_strPos_X);
		SetDlgItemText(IDC_TRANS_Y, m_strPos_Y);
		SetDlgItemText(IDC_TRANS_Z, m_strPos_Z);

		m_strRot_X.Format(_T("%.3f"), D3DXToDegree(targetTrans->Get_Angle().x));
		m_strRot_Y.Format(_T("%.3f"), D3DXToDegree(targetTrans->Get_Angle().y));
		m_strRot_Z.Format(_T("%.3f"), D3DXToDegree(targetTrans->Get_Angle().z));

		SetDlgItemText(IDC_ROTATE_X, m_strRot_X);
		SetDlgItemText(IDC_ROTATE_Y, m_strRot_Y);
		SetDlgItemText(IDC_ROTATE_Z, m_strRot_Z);

		m_strScale_X.Format(_T("%.3f"), targetTrans->Get_Size().x);
		m_strScale_Y.Format(_T("%.3f"), targetTrans->Get_Size().y);
		m_strScale_Z.Format(_T("%.3f"), targetTrans->Get_Size().z);

		SetDlgItemText(IDC_SCALE_X, m_strScale_X);
		SetDlgItemText(IDC_SCALE_Y, m_strScale_Y);
		SetDlgItemText(IDC_SCALE_Z, m_strScale_Z);
	}

	else
	{
		//cout << "웅 암것도 읍스!" << endl;


		m_strPos_X.Format(_T("%.3f"), 0);
		m_strPos_Y.Format(_T("%.3f"), 0);
		m_strPos_Z.Format(_T("%.3f"), 0);

		SetDlgItemText(IDC_TRANS_X, L"0.000f");
		SetDlgItemText(IDC_TRANS_Y, L"0.000f");
		SetDlgItemText(IDC_TRANS_Z, L"0.000f");

		m_strRot_X.Format(_T("%.3f"), 0);
		m_strRot_Y.Format(_T("%.3f"), 0);
		m_strRot_Z.Format(_T("%.3f"), 0);

		SetDlgItemText(IDC_ROTATE_X, L"0.000f");
		SetDlgItemText(IDC_ROTATE_Y, L"0.000f");
		SetDlgItemText(IDC_ROTATE_Z, L"0.000f");

		m_strScale_X.Format(_T("%.3f"), 0);
		m_strScale_Y.Format(_T("%.3f"), 0);
		m_strScale_Z.Format(_T("%.3f"), 0);

		SetDlgItemText(IDC_SCALE_X, L"0.000f");
		SetDlgItemText(IDC_SCALE_Y, L"0.000f");
		SetDlgItemText(IDC_SCALE_Z, L"0.000f");
	}
}

void CMapTool_Page::Check_SelectObj()
{
	Engine::CGameObject* pIstance = nullptr;

	pIstance = CCollisionMgr::Collision_Ray(m_listObject, g_Ray, &g_fCross, m_bShowAll, m_sLayerCurIdx);

	IF_NOT_NULL(pIstance)
	{
		IF_NOT_NULL(m_pSelectedObj)
			m_pSelectedObj->Set_Selected(false);

		m_pSelectedObj = static_cast<Engine::CRenderObject*>(pIstance);
		m_pSelectedObj->Set_Selected(true);

		m_CurObjLayerIdx.Format(_T("%d"), m_pSelectedObj->Get_LayerIdx());
		SetDlgItemText(IDC_LAYER_CUR2, m_CurObjLayerIdx);

		Object_Movement();

		//m_CTagBox.SetCurSel(m_pSelectedObj->Get_ObjTag());
	}

	else
	{
		IF_NOT_NULL(m_pSelectedObj)
			m_pSelectedObj->Set_Selected(false);

		m_pSelectedObj = nullptr;
		//Object_Movement();
	}
}

_bool CMapTool_Page::CheckObject()
{
	IF_NOT_NULL(m_pSelectedObj)
		return false;

	Engine::CGameObject* pIstance = nullptr;
	_float m_fCross = 0.f;

	if(m_listObject.size() > 0)
		pIstance = Engine::CCollisionMgr::Collision_Ray(m_listObject, g_Ray, &g_fCross);

	if (pIstance == nullptr)
	{
		m_vMouse = g_Ray.vRayPos + g_Ray.vRayDir * m_fCross;
		m_bCheckObj = false;
	}

	else
	{
		m_vMouse = g_Ray.vRayPos + g_Ray.vRayDir * g_fCross;
		m_bCheckObj = true;
	}

	pIstance = nullptr;

	return m_bCheckObj;
}

void CMapTool_Page::LoadFilePath(const wstring & wstrImgPath)
{
	HTREEITEM hStaticRoot, hDynamicRoot;

	wifstream fin;

	fin.open(wstrImgPath);

	if (fin.fail())
		return;

	hStaticRoot = m_Tree.InsertItem(TEXT("StaticMesh"), 0, 0, TVI_ROOT);
	hDynamicRoot = m_Tree.InsertItem(TEXT("DynamicMesh"), 0, 0, TVI_ROOT);

	cout << sizeof(PATH_INFO) << endl;

	while (true)
	{
		Engine::PATH_INFO* tmpPath = new Engine::PATH_INFO;

		fin.getline(tmpPath->sztrStateKey, MAX_STR, '|');
		fin.getline(tmpPath->sztrFileName, MAX_STR, '|');
		fin.getline(tmpPath->sztrImgPath, MAX_STR, '|');
		fin.getline(tmpPath->szIsDynamic, MAX_STR, '|');

		if (fin.eof())
		{
			Engine::Safe_Delete(tmpPath);
			break;
		}

		m_listMeshPathInfo.push_back(tmpPath);

		// 카테고리 이름
		TCHAR szBuf[MAX_STR] = L"";
		lstrcpy(szBuf, tmpPath->sztrImgPath);
		::PathRemoveBackslash(szBuf);
		CString tmpString = ::PathFindFileName(szBuf);
		
		_tchar* szBuf2 = new _tchar[MAX_STR];
		lstrcpy(szBuf2, tmpString);
		
		
		HTREEITEM* hItems  = new HTREEITEM;
		_bool	isEnd = true;
		_bool	bisDynamic = false;
		
		
		for (auto& iter : m_listStaticFolder)
		{
			if (!lstrcmp(iter, szBuf2))
			{
				m_Tree.InsertItem(tmpPath->sztrStateKey, 0, 0, *m_listStaticBranch.back());
				isEnd = false;
		
				Engine::Safe_Delete(szBuf2);
				Engine::Safe_Delete(hItems);
		
				break;
			}
		
			else
				isEnd = true;
		}
		
		for (auto& iter : m_listDynamicFolder)
		{
			if (!lstrcmp(iter, szBuf2))
			{
				m_Tree.InsertItem(tmpPath->sztrStateKey, 0, 0, *m_listDynamicBranch.back());
				isEnd = false;
		
				Engine::Safe_Delete(szBuf2);
				Engine::Safe_Delete(hItems);
		
				break;
			}
		
			else
				isEnd = true;
		}
		
		if (isEnd)
		{
			(lstrcmp(tmpPath->szIsDynamic, L"0") ? bisDynamic = false : bisDynamic = true);
		
			if (bisDynamic)
			{
				*hItems = m_Tree.InsertItem(szBuf2, 0, 0, hStaticRoot);
				m_listStaticFolder.push_back(szBuf2);
				m_listStaticBranch.push_back(hItems);
			}
		
			else
			{
				*hItems = m_Tree.InsertItem(szBuf2, 0, 0, hDynamicRoot);
				m_listDynamicFolder.push_back(szBuf2);
				m_listDynamicBranch.push_back(hItems);
			}
		
			m_Tree.InsertItem(tmpPath->sztrStateKey, 0, 0, *hItems);
		
			hItems = nullptr;
		}
	}

	fin.close();

	return;
}

void CMapTool_Page::Change_Mesh(const _tchar * _MeshName)
{
	m_pRenderObj->Change_Mesh(_MeshName);
}

void CMapTool_Page::Add_Object_OnTree(Engine::CRenderObject * _pObj)
{
	_tchar tmpName[MAX_STR] = L"";
	_tchar tmpNumber[MAX_STR] = L"";
	//HTREEITEM* newItem = new HTREEITEM;

	// 이름 복사하고,
	lstrcpy(tmpName, _pObj->Get_Name());

	// 폴더 이름에 있는지 찾는다.
	//for (auto& iter : m_listObjectTypeName)
	//{
	//	if (lstrcmp(iter, tmpName))
	//	{
	//		bCreateFolder = false;
	//		*newItem = hObjectRoot;
	//	}
	//}

	// 폴더가 없다면 
	//if (bCreateFolder == true)
	//{
	//	*newItem = m_pObjectTree.InsertItem(tmpName, 0, 0, hObjectRoot);
	//}

	//else
	//{
	//	*newItem = m_pObjectTree.
	//}

	_itot_s(_pObj->Get_Index(), tmpNumber, 10);

	lstrcat(tmpName, tmpNumber);

	m_pObjectTree.InsertItem(tmpName, 0, 0, hObjectRoot);
}

void CMapTool_Page::Free()
{
	//m_pPivotPage = nullptr;

	if (m_pRenderObj != nullptr)
	{
		Engine::Safe_Release(m_pRenderObj);
	}

	for (auto& iter : m_listObject)
	{
		//iter->Free();
		//iter = nullptr;
		Engine::Safe_Release(iter);
	}
	m_listObject.clear();

	for (auto& iter : m_listStaticBranch)
	{
		Engine::Safe_Delete(iter);
	}
	m_listStaticBranch.clear();

	for (auto& iter : m_listStaticFolder)
	{
		Engine::Safe_Delete(iter);
	}
	m_listStaticFolder.clear();

	for (auto& iter : m_listDynamicBranch)
	{
		Engine::Safe_Delete(iter);
	}
	m_listDynamicBranch.clear();

	for (auto& iter : m_listDynamicFolder)
	{
		Engine::Safe_Delete(iter);
	}
	m_listDynamicFolder.clear();


	for (auto& iter : m_listMeshPathInfo)
	{
		Engine::Safe_Delete(iter);
	}
	m_listMeshPathInfo.clear();

	//m_Tree.DeleteAllItems();
}


BEGIN_MESSAGE_MAP(CMapTool_Page, CPropertyPage)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CMapTool_Page::OnTvnSelchangedTree1)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE3, &CMapTool_Page::OnTvnSelchangedTree3)
	ON_CONTROL_RANGE(BN_CLICKED, ID_EDIT_00, ID_EDIT_02, &CMapTool_Page::OnEditCtrl)
	ON_BN_CLICKED(ID_EDIT_00, &CMapTool_Page::OnBnClickedRadio1)
	ON_BN_CLICKED(ID_EDIT_01, &CMapTool_Page::OnBnClickedRadio2)
	ON_BN_CLICKED(ID_EDIT_02, &CMapTool_Page::OnBnClickedRadio3)
	ON_EN_CHANGE(IDC_TRANS_X, &CMapTool_Page::OnEnChangeTransX)
	ON_EN_CHANGE(IDC_TRANS_Y, &CMapTool_Page::OnEnChangeTransY)
	ON_EN_CHANGE(IDC_TRANS_Z, &CMapTool_Page::OnEnChangeTransZ)
	ON_EN_CHANGE(IDC_ROTATE_X, &CMapTool_Page::OnEnChangeRotateX)
	ON_EN_CHANGE(IDC_ROTATE_Y, &CMapTool_Page::OnEnChangeRotateY)
	ON_EN_CHANGE(IDC_ROTATE_Z, &CMapTool_Page::OnEnChangeRotateZ)
	ON_EN_CHANGE(IDC_SCALE_X, &CMapTool_Page::OnEnChangeScaleX)
	ON_EN_CHANGE(IDC_SCALE_Y, &CMapTool_Page::OnEnChangeScaleY)
	ON_EN_CHANGE(IDC_SCALE_Z, &CMapTool_Page::OnEnChangeScaleZ)
	ON_EN_CHANGE(IDC_TRANS_RATE, &CMapTool_Page::OnEnChangeTransRate)
	ON_EN_CHANGE(IDC_ROTATE_RATE, &CMapTool_Page::OnEnChangeRotateRate)
	ON_EN_CHANGE(IDC_SCALE_RATE, &CMapTool_Page::OnEnChangeScaleRate)
	ON_CONTROL_RANGE(BN_CLICKED, ID_TRANSFORM_00, ID_TRANSFORM_02, &CMapTool_Page::OnEditType)
	ON_BN_CLICKED(ID_TRANSFORM_00, &CMapTool_Page::OnBnClickedTransform00)
	ON_BN_CLICKED(ID_TRANSFORM_01, &CMapTool_Page::OnBnClickedTransform01)
	ON_BN_CLICKED(ID_TRANSFORM_02, &CMapTool_Page::OnBnClickedTransform02)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CTRL_00, IDC_CTRL_01, &CMapTool_Page::OnSelectMode)
	ON_BN_CLICKED(IDC_CTRL_01, &CMapTool_Page::OnBnClickedCtrl01)
	ON_BN_CLICKED(IDC_CTRL_00, &CMapTool_Page::OnBnClickedCtrl00)
	//ON_BN_CLICKED(IDC_DELETE, &CMapTool_Page::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_BUTTON5, &CMapTool_Page::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_SAVE_OBJ, &CMapTool_Page::OnBnClickedSaveObj)
	ON_BN_CLICKED(IDC_LOAD_OBJ, &CMapTool_Page::OnBnClickedLoadObj)
	ON_BN_CLICKED(IDC_DELETE_ALL, &CMapTool_Page::OnBnClickedDeleteAll)
	ON_EN_CHANGE(IDC_LAYER_CUR, &CMapTool_Page::OnEnChangeLayerCur)
	ON_EN_CHANGE(IDC_LAYER_RENDER, &CMapTool_Page::OnEnChangeLayerRender)
	ON_BN_CLICKED(IDC_CTRL_1, &CMapTool_Page::OnBnClickedCtrl1)
	ON_BN_CLICKED(IDC_CTRL_2, &CMapTool_Page::OnBnClickedCtrl2)
	ON_BN_CLICKED(IDC_CHECK3, &CMapTool_Page::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_BUTTON3, &CMapTool_Page::OnBnClickedButton3)
END_MESSAGE_MAP()


// CMapTool_Page 메시지 처리기입니다.


void CMapTool_Page::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
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

	hItemParent = this->m_Tree.GetParentItem(hItemCur);

	hItemRoot = this->m_Tree.GetParentItem(hItemParent);



	strRoot = m_Tree.GetItemText(hItemRoot);

	strParent = m_Tree.GetItemText(hItemParent);

	strCur = m_Tree.GetItemText(hItemCur);

	if (m_iEditMode == _CREATE)
	{
		if (m_pRenderObj == nullptr)
		{
			m_pRenderObj = Engine::CRenderObject::Create(g_pGraphicDev);
			m_pRenderObj->Set_OnTool(true);
		}

		if (!lstrcmp(strCur, L"StaticMesh"))
			return;

		if (!lstrcmp(strCur, L"DynamicMesh"))
			return;

		for (auto& iter : m_listStaticFolder)
		{
			if (!lstrcmp(strCur, iter))
			{
				return;
			}
		}

		for (auto& iter : m_listDynamicFolder)
		{
			if (!lstrcmp(strCur, iter))
			{
				return;
			}
		}

		Change_Mesh(strCur);
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CMapTool_Page::OnTvnSelchangedTree3(NMHDR * pNMHDR, LRESULT * pResult)
{
	//cout << " 또 탐" << endl;

	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM hItemRoot;

	HTREEITEM hItemParent;

	HTREEITEM hItemCur;



	CString strRoot;

	CString strParent;

	CString strCur;

	CString strTxt;



	hItemCur = pNMTreeView->itemNew.hItem;

	hItemParent = this->m_pObjectTree.GetParentItem(hItemCur);

	hItemRoot = this->m_pObjectTree.GetParentItem(hItemParent);



	strRoot = m_pObjectTree.GetItemText(hItemRoot);

	strParent = m_pObjectTree.GetItemText(hItemParent);

	strCur = m_pObjectTree.GetItemText(hItemCur);


	if (m_iEditMode == _SELECT)
	{
		if (lstrcmp(strCur, L"Object"))
		{
			_tchar szNum[MAX_STR] = L"";
			_int tmpIndex = _ttoi(strCur);

			_itot_s(tmpIndex, szNum, 10);

			// 이름 + 숫자 같다면, 

			for (auto& iter : m_listObject)
			{
				_tchar szTmpName[MAX_STR] = L"";

				lstrcpy(szTmpName, static_cast<Engine::CRenderObject*>(iter)->Get_Name());
				lstrcat(szTmpName, szNum);

				if (!lstrcmp(strCur, szTmpName))
				{
					m_pSelectedObj = static_cast<Engine::CRenderObject*>(iter);
					m_pSelectedObj->Set_Selected(true);

					if(m_pSelectedObj->Get_LayerIdx() != m_sLayerCurIdx)
						m_pSelectedObj->Set_LayerIdx(m_sLayerCurIdx);
					break;
				}
			}

			m_pSelectedObj->Set_Selected(false);
			m_pSelectedObj = nullptr;
		}
	}

	if (m_iEditMode == _REMOVE)
	{
		if (lstrcmp(strCur, L"Object"))
		{
			_tchar szNum[MAX_STR] = L"";
			_int tmpIndex = _ttoi(strCur);

			_itot_s(tmpIndex, szNum, 10);

			for (auto& iter : m_listObject)
			{
				_tchar szNum2[MAX_STR] = L"";
				_tchar szTmpName[MAX_STR] = L"";

				_itot_s(static_cast<Engine::CRenderObject*>(iter)->Get_Index(), szNum2, 10);

				lstrcpy(szTmpName, static_cast<Engine::CRenderObject*>(iter)->Get_Name());
				lstrcat(szTmpName, szNum2);

				if (!lstrcmp(strCur, szTmpName))
				{
					iter->Free();
					iter = nullptr;
					m_listObject.remove(iter);

					m_pObjectTree.DeleteItem(hItemCur);

					Object_Movement();

					break;
				}
			}
		}
	}

	*pResult = 0;
}


BOOL CMapTool_Page::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	LoadFilePath(L"../../Data/Mesh_Path.dat");

	hObjectRoot = m_pObjectTree.InsertItem(TEXT("Object"), 0, 0, TVI_ROOT , TVI_LAST);

	return TRUE;
}


void CMapTool_Page::OnBnClickedRadio1()
{
}

void CMapTool_Page::OnBnClickedRadio2()
{
}

void CMapTool_Page::OnBnClickedRadio3()
{
}

void CMapTool_Page::OnSelectMode(_uint _Mode)
{
	UpdateData(true);

	switch (_Mode)
	{
	case 0:
	{
		m_bSelectAdvence = false;

		break;
	}
	case 1:
	{
		m_bSelectAdvence = true;
		break;
	}
	}

	UpdateData(false);
}

void CMapTool_Page::OnEditLayer(_uint _Mode)
{
	UpdateData(true);

	switch (_Mode)
	{
	case 0:
	{
		m_bShowAll = true;

		break;
	}
	case 1:
	{
		m_bShowAll = false;
		break;
	}
	}

	UpdateData(false);
}

void CMapTool_Page::OnEditCtrl(_uint _Mode)
{
	UpdateData(true);

	switch (_Mode)
	{
	case _SELECT:
	{
		m_iEditMode = _SELECT;

		break;
	}
	case _CREATE:
	{
		m_iEditMode = _CREATE;
		break;
	}
	case _REMOVE:
	{
		m_iEditMode = _REMOVE;
		break;
	}
	}

	UpdateData(false);
}

void CMapTool_Page::OnEditType(_uint _Mode)
{
	UpdateData(true);

	switch (_Mode)
	{
	case AXIS_X:
	{
		m_iEditType = 0;

		break;
	}
	case AXIS_Y:
	{
		m_iEditMode = 1;
		break;
	}

	case AXIS_Z:
	{
		m_iEditMode = 2;
		break;
	}
	}

	UpdateData(false);
}


void CMapTool_Page::OnEnChangeTransX()
{
	GetDlgItemText(IDC_TRANS_X, m_strPos_X);
	m_vPos.x = (_float)_tstof(m_strPos_X);

	IF_NOT_NULL(m_pSelectedObj)
	{
		TARGET_TO_TRANS(m_pSelectedObj)->Set_Pos_Axis(m_vPos.x, AXIS_X);
	}

}


void CMapTool_Page::OnEnChangeTransY()
{
	GetDlgItemText(IDC_TRANS_Y, m_strPos_Y);
	m_vPos.y = (_float)_tstof(m_strPos_Y);

	IF_NOT_NULL(m_pSelectedObj)
	{
		TARGET_TO_TRANS(m_pSelectedObj)->Set_Pos_Axis(m_vPos.y, AXIS_Y);
	}
}


void CMapTool_Page::OnEnChangeTransZ()
{
	GetDlgItemText(IDC_TRANS_Z, m_strPos_Z);
	m_vPos.z = (_float)_tstof(m_strPos_Z);

	IF_NOT_NULL(m_pSelectedObj)
	{
		TARGET_TO_TRANS(m_pSelectedObj)->Set_Pos_Axis(m_vPos.z, AXIS_Z);
	}
}


void CMapTool_Page::OnEnChangeRotateX()
{
	GetDlgItemText(IDC_ROTATE_X, m_strRot_X);
	m_vRot.x = (_float)_tstof(m_strRot_X);

	IF_NOT_NULL(m_pSelectedObj)
	{
		TARGET_TO_TRANS(m_pSelectedObj)->Set_Angle(AXIS_X, D3DXToRadian(m_vRot.x));
	}
}


void CMapTool_Page::OnEnChangeRotateY()
{
	GetDlgItemText(IDC_ROTATE_Y, m_strRot_Y);
	m_vRot.y = (_float)_tstof(m_strRot_Y);

	IF_NOT_NULL(m_pSelectedObj)
	{
		TARGET_TO_TRANS(m_pSelectedObj)->Set_Angle(AXIS_Y, D3DXToRadian(m_vRot.y));
	}

}


void CMapTool_Page::OnEnChangeRotateZ()
{
	GetDlgItemText(IDC_ROTATE_Z, m_strRot_Z);
	m_vRot.z = (_float)_tstof(m_strRot_Z);

	IF_NOT_NULL(m_pSelectedObj)
	{
		TARGET_TO_TRANS(m_pSelectedObj)->Set_Angle(AXIS_Z, D3DXToRadian(m_vRot.z));
	}
}


void CMapTool_Page::OnEnChangeScaleX()
{
	GetDlgItemText(IDC_SCALE_X, m_strScale_X);
	m_vScale.x = (_float)_tstof(m_strScale_X);

	IF_NOT_NULL(m_pSelectedObj)
	{
		TARGET_TO_TRANS(m_pSelectedObj)->Set_Scale_Axis(m_vScale.x , AXIS_X);
	}
}


void CMapTool_Page::OnEnChangeScaleY()
{
	GetDlgItemText(IDC_SCALE_Y, m_strScale_Y);
	m_vScale.y = (_float)_tstof(m_strScale_Y);

	IF_NOT_NULL(m_pSelectedObj)
	{
		TARGET_TO_TRANS(m_pSelectedObj)->Set_Scale_Axis(m_vScale.y, AXIS_Y);
	}
}


void CMapTool_Page::OnEnChangeScaleZ()
{
	GetDlgItemText(IDC_SCALE_Z, m_strScale_Z);
	m_vScale.z = (_float)_tstof(m_strScale_Z);

	IF_NOT_NULL(m_pSelectedObj)
	{
		TARGET_TO_TRANS(m_pSelectedObj)->Set_Scale_Axis(m_vScale.z, AXIS_Z);
	}
}


void CMapTool_Page::OnEnChangeTransRate()
{
	GetDlgItemText(IDC_TRANS_RATE, m_strPosRate);
	m_fPosRate = (_float)_tstof(m_strPosRate);
}

void CMapTool_Page::OnEnChangeRotateRate()
{
	GetDlgItemText(IDC_ROTATE_RATE, m_strRotRate);
	m_fRotRate = (_float)_tstof(m_strRotRate);
}


void CMapTool_Page::OnEnChangeScaleRate()
{
	GetDlgItemText(IDC_SCALE_RATE, m_strScaleRate);
	m_fScaleRate = (_float)_tstof(m_strScaleRate);
}


void CMapTool_Page::OnBnClickedTransform00()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMapTool_Page::OnBnClickedTransform01()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMapTool_Page::OnBnClickedTransform02()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMapTool_Page::OnBnClickedCtrl00()
{
	m_bSelectAdvence = false;
}

void CMapTool_Page::OnBnClickedCtrl01()
{
	m_bSelectAdvence = true;
}

void CMapTool_Page::OnBnClickedButton5()
{
}


void CMapTool_Page::OnBnClickedSaveObj()
{
	Save_Object();
}


void CMapTool_Page::OnBnClickedLoadObj()
{
	Load_Object();
}


void CMapTool_Page::OnBnClickedDeleteAll()
{
}


void CMapTool_Page::OnEnChangeLayerCur()
{
	GetDlgItemText(IDC_LAYER_CUR, m_strLayer_Cur);
	m_sLayerCurIdx = (_short)_tstof(m_strLayer_Cur);
}


void CMapTool_Page::OnEnChangeLayerRender()
{
	GetDlgItemText(IDC_LAYER_RENDER, m_strLayer_Render);
	m_sLayerRenderIdx = (_short)_tstof(m_strLayer_Render);
}


void CMapTool_Page::OnBnClickedCtrl1()
{
	m_bShowAll = true;
}


void CMapTool_Page::OnBnClickedCtrl2()
{
	m_bShowAll = false;
}


void CMapTool_Page::OnBnClickedCheck3()
{
	m_bDetailMode = (m_bDetailMode == false ? true : false);
}


void CMapTool_Page::OnBnClickedButton3()
{
	IF_NOT_NULL(m_pSelectedObj)
	{
		m_pSelectedObj->Set_LayerIdx(m_sLayerCurIdx);

		m_CurObjLayerIdx.Format(_T("%d"), m_pSelectedObj->Get_LayerIdx());
		SetDlgItemText(IDC_LAYER_CUR2, m_CurObjLayerIdx);
	}

	cout << "탑니다" << endl;
}
