// NaviMesh.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "NavMesh_Page.h"
#include "afxdialogex.h"

#include "Cell.h"
#include "TreeFinder.h"
#include "Management.h"

// CNaviMesh 대화 상자입니다.

IMPLEMENT_DYNAMIC(CNavMesh_Page, CPropertyPage)

CNavMesh_Page::CNavMesh_Page()
	: CPropertyPage(IDD_NAVIMESH_PAGE),
	m_iMax_VertexCnt(3), m_iCur_VertexCnt(0),
	m_eEditMode(_EDIT_END),
	m_pSelectCell(nullptr),
	m_iIndex(-3) , m_iVtxIndex(-3)
	, m_YRate(_T("0.000"))
	, m_YGap(_T("0.000"))
	, m_iGreedMode(1)
	, m_iEditMode(0)
	, m_iControlMode(0)
	, m_strLineAB(_T("-1"))
	, m_strLineBC(_T("-1"))
	, m_strLineCA(_T("-1"))
	, m_strCenter_X(_T("0.000"))
	, m_strCenter_Y(_T("0.000"))
	, m_strCenter_Z(_T("0.000"))
	, m_strGreedRate(_T("0.000"))
	, m_strSelectIdx(_T("-1"))
	, m_iSubsetIdx(0)
	, m_strSubsetIdx(_T("0"))
{

}

CNavMesh_Page::~CNavMesh_Page()
{

}

void CNavMesh_Page::DoDataExchange(CDataExchange* pDX) 
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, NaviNodeTree);
	DDX_Text(pDX, IDC_EDIT5, m_YRate);
	DDX_Text(pDX, IDC_EDIT6, m_YGap);
	DDX_Radio(pDX, IDC_GREED_00, m_iGreedMode);
	DDX_Radio(pDX, IDC_EMODE_00, m_iEditMode);
	DDX_Radio(pDX, IDC_CONTROL_00, m_iControlMode);

	DDX_Text(pDX, CELL_LINE_00, m_strLineAB);
	DDX_Text(pDX, CELL_LINE_01, m_strLineBC);
	DDX_Text(pDX, CELL_LINE_02, m_strLineCA);

	DDX_Text(pDX, CELL_CENTER_00, m_strCenter_X);
	DDX_Text(pDX, CELL_CENTER_01, m_strCenter_Y);
	DDX_Text(pDX, CELL_CENTER_02, m_strCenter_Z);
	DDX_Text(pDX, SELECT_INDEX, m_strSelectIdx);
	DDX_Control(pDX, IDC_CELL_SAVE, m_cSave);
	DDX_Control(pDX, IDC_AREA_LIST, m_cAreaList);
	DDX_Text(pDX, IDC_EDIT1, m_strSubsetIdx);
}

void CNavMesh_Page::Select_Vtx(RAY _tRay)
{
	for (auto& vecIter : m_vecSubset_Cell)
	{
		for (auto& iter : vecIter)
		{
			_float fU = 0.f, fV = 0.f, fDist = 0.f;
			_v3	vTargetPoint;
			_v3 * vVtxPos = iter->Get_VertexPos();

			if (D3DXIntersectTri(
				&vVtxPos[0],
				&vVtxPos[1],
				&vVtxPos[2],
				&_tRay.vRayPos,
				&_tRay.vRayDir,
				&fU,
				&fV,
				&fDist))
			{
				vTargetPoint = vVtxPos[0]
					+ fU * vVtxPos[1]
					- vVtxPos[0]
					+ fV * vVtxPos[2]
					- vVtxPos[0];

				Compare_Vertex(vTargetPoint, 0.3f);

				return;
			}
		}
	}
}

void CNavMesh_Page::Select_Tri(RAY _tRay)
{
	for (auto& vecIter : m_vecSubset_Cell)
	{
		for (auto& iter : vecIter)
		{
			_float fU = 0.f, fV = 0.f, fDist = 0.f;
			_v3	vTargetPoint;
			_v3 * vVtxPos = iter->Get_VertexPos();

			if (D3DXIntersectTri(
				&vVtxPos[0],
				&vVtxPos[1],
				&vVtxPos[2],
				&_tRay.vRayPos,
				&_tRay.vRayDir,
				&fU,
				&fV,
				&fDist))
			{

				return;
			}
		}
	}
}

void CNavMesh_Page::Select_Poly(RAY _tRay)
{
}

void CNavMesh_Page::Move_Position()
{
	if (m_pSelectCell)
	{
		if (m_iControlMode == _MOUSE)
		{
			if (m_iVtxIndex > -1)
			{
				static_cast<CCell*>(m_pSelectCell)->Set_IndexVtxPos(m_iVtxIndex, g_MousePos);
				static_cast<CCell*>(m_pSelectCell)->Set_VtxSelected(true);
			}

			else if (m_iVtxIndex == -1)
			{
				_v3 tmpCenter = static_cast<CCell*>(m_pSelectCell)->Get_CenterPos();

				_v3 CurVtx[3] = {};
				_v3 NewPostions[3] = {};


				LOOP(3)
				{
					CurVtx[i] = static_cast<CCell*>(m_pSelectCell)->Get_VertexPos()[i];
					NewPostions[i] = (CurVtx[i] - tmpCenter) + g_MousePos;
				}

				static_cast<CCell*>(m_pSelectCell)->Set_VtxPos(NewPostions);
				static_cast<CCell*>(m_pSelectCell)->Set_VtxSelected(false);
			}
		}

		else
			Key_Down();
	}
}

void CNavMesh_Page::Key_Down()
{
	// 값 증가
	if (CInput_Device::Get_Instance()->Key_Pressing(DIK_Z))
	{
		if (m_iVtxIndex > -1)
		{
			static_cast<CCell*>(m_pSelectCell)->Add_Vtx_Y(m_fYRate , m_iVtxIndex);
			static_cast<CCell*>(m_pSelectCell)->Set_VtxSelected(true);
		}

		else if (m_iVtxIndex == -1)
		{
			static_cast<CCell*>(m_pSelectCell)->Add_Vtx_Y(m_fYRate);
			static_cast<CCell*>(m_pSelectCell)->Add_Center_Y(m_fYRate);
			static_cast<CCell*>(m_pSelectCell)->Set_VtxSelected(false);
		}
	}

	// 값 감소
	else if (CInput_Device::Get_Instance()->Key_Pressing(DIK_X))
	{
		if (m_iVtxIndex > -1)
		{
			static_cast<CCell*>(m_pSelectCell)->Add_Vtx_Y(-m_fYRate, m_iVtxIndex);
			static_cast<CCell*>(m_pSelectCell)->Set_VtxSelected(true);
		}

		else if (m_iVtxIndex == -1)
		{
			static_cast<CCell*>(m_pSelectCell)->Add_Vtx_Y(-m_fYRate);
			static_cast<CCell*>(m_pSelectCell)->Add_Center_Y(-m_fYRate);
			static_cast<CCell*>(m_pSelectCell)->Set_VtxSelected(false);
		}

	}
}

void CNavMesh_Page::Create_Vertex(LPDIRECT3DDEVICE9 pGraphicDev, _v3 _vPos)
{
	if (m_iEditMode == _CREATE)
	{
		_v3 tmpVtx[3] = {};

		tmpVtx[0] = _vPos + _v3{ 0.f, 0.f , 0.5f };
		tmpVtx[1] = _vPos + _v3{ 0.5f, 0.f , -0.5f };
		tmpVtx[2] = _vPos + _v3{ -0.5f, 0.f , -0.5f };

		// 셀 생성
		Create_Cell(pGraphicDev, tmpVtx);
	}

	else if (m_iEditMode == _SELECT)
	{
		if (m_pSelectCell == nullptr)
		{
			for (auto& vecIter : m_vecSubset_Cell)
			{
				for (auto& iter : vecIter)
				{
					_float fU = 0.f, fV = 0.f, fDist = 0.f;
					_v3	vTargetPoint;
					_v3 * vVtxPos = iter->Get_VertexPos();

					if (D3DXIntersectTri(
						&vVtxPos[0],
						&vVtxPos[1],
						&vVtxPos[2],
						&g_Ray.vRayPos,
						&g_Ray.vRayDir,
						&fU,
						&fV,
						&fDist))
					{
						iter->Set_Selected(true);
						m_pSelectCell = iter;
						return;
					}
				}
			}
		}

		else if (m_pSelectCell != nullptr)
		{
			if (m_iVtxIndex != -3)
			{
				if (m_iGreedMode == 0)
				{
					if (m_iControlMode == _MOUSE)
					{
						if (m_iVtxIndex > -1)
						{
							_v3 tmpPos = g_MousePos;
							Greed_Vertex(&tmpPos , m_pSelectCell);

							static_cast<Engine::CCell*>(m_pSelectCell)->Set_IndexVtxPos(m_iVtxIndex, tmpPos);
						}
					}
				}

				m_pSelectCell->Set_VtxSelected(false);
				m_iVtxIndex = -3;
				return;
			}

			_float fU = 0.f, fV = 0.f, fDist = 0.f;
			_v3	vTargetPoint , vCrossVtx;
			_v3 * vVtxPos = m_pSelectCell->Get_VertexPos();

			if (D3DXIntersectTri(
				&vVtxPos[0],
				&vVtxPos[1],
				&vVtxPos[2],
				&g_Ray.vRayPos,
				&g_Ray.vRayDir,
				&fU,
				&fV,
				&fDist))
			{
				vCrossVtx = vVtxPos[0]+ fU * (vVtxPos[1]
							- vVtxPos[0])+ fV * (vVtxPos[2]
							- vVtxPos[0]);

				LOOP(3)
				{
					// 삼각형 내부 점과 가까울 경우 양수,
					if (0.5f > D3DXVec3Length(&(vVtxPos[i] - vCrossVtx)))
					{
						m_iVtxIndex = i;
						return;
					}

					// 충돌은 하는데 안가까우면 -1 ( 셀 자체의 선택 )
					else
						m_iVtxIndex = -1;
				}
			}

			else if (m_iVtxIndex == -3)
			{
				if (!m_pSelectCell->Get_VtxSelected())
				{
					m_pSelectCell->Set_Selected(false);
					m_pSelectCell = nullptr;
				} 
			}

		}
	}
}

void CNavMesh_Page::Create_Polygon(LPDIRECT3DDEVICE9 pGraphicDev)
{
}

void CNavMesh_Page::Create_Vertex_Auto(LPDIRECT3DDEVICE9 pGraphicDev)
{
}

void CNavMesh_Page::Delete_SelectNav()
{
	IF_NOT_NULL(m_pSelectCell)
	{
			for (auto& iter : m_vecSubset_Cell[m_iSubsetIdx])
			{
				if (iter->Get_CellIdx() == m_pSelectCell->Get_CellIdx())
				{
					_int	iIndex = 0;
					_tchar	szIndex[32] = L"";

					iIndex = iter->Get_CellIdx();
					_stprintf_s(szIndex, _T("%d"), iIndex);

					iter->Free();
					iter = nullptr;

					m_pSelectCell = nullptr;

					m_vecSubset_Cell[m_iSubsetIdx].remove(iter);

					//HTREEITEM tmpNode = CTreeFinder::Find_Node_By_Name(NaviNodeTree, szIndex);
					//
					//NaviNodeTree.DeleteItem(tmpNode);

					return;
				}
			}
	}
}

void CNavMesh_Page::MyUpdateData()
{
	if(m_pSelectCell != nullptr)
	{
		//Index Update
		_int iIndex = static_cast<CCell*>(m_pSelectCell)->Get_CellIdx();

		m_strSelectIdx.Format(_T("%d"), iIndex);

		SetDlgItemText(SELECT_INDEX, m_strSelectIdx);

		// Nieghbor Update
		_int* tmpIndex = static_cast<CCell*>(m_pSelectCell)->Get_SiblingIdx();

		_int iAB = tmpIndex[0];
		_int iBC = tmpIndex[1];
		_int iCA = tmpIndex[2];

		m_strLineAB.Format(_T("%d"), iAB);
		m_strLineBC.Format(_T("%d"), iBC);
		m_strLineCA.Format(_T("%d"), iCA);

		//if(iAB ==999)
		//	m_strLineAB.Format(_T("%d") , 999);
		//
		//if (iBC == 999)
		//	m_strLineBC.Format(_T("%d"), 999);
		//
		//if (iCA == 999)
		//	m_strLineCA.Format(_T("%d"), 999);

		SetDlgItemText(CELL_LINE_00, m_strLineAB);
		SetDlgItemText(CELL_LINE_01, m_strLineBC);
		SetDlgItemText(CELL_LINE_02, m_strLineCA);

		// CenterPos Update
		_v3 vCenterPos = static_cast<CCell*>(m_pSelectCell)->Get_CenterPos();

		m_strCenter_X.Format(_T("%.3f"), vCenterPos.x);
		m_strCenter_Y.Format(_T("%.3f"), vCenterPos.y);
		m_strCenter_Z.Format(_T("%.3f"), vCenterPos.z);

		SetDlgItemText(CELL_CENTER_00, m_strCenter_X);
		SetDlgItemText(CELL_CENTER_01, m_strCenter_Y);
		SetDlgItemText(CELL_CENTER_02, m_strCenter_Z);
	}

	else
	{
		m_strSelectIdx.Format(_T("-%d"), 1);
		SetDlgItemText(SELECT_INDEX, m_strSelectIdx);

		m_strCenter_X.Format(_T("%.3f"), 0);
		m_strCenter_Y.Format(_T("%.3f"), 0);
		m_strCenter_Z.Format(_T("%.3f"), 0);

		SetDlgItemText(CELL_CENTER_00, m_strCenter_X);
		SetDlgItemText(CELL_CENTER_01, m_strCenter_Y);
		SetDlgItemText(CELL_CENTER_02, m_strCenter_Z);


		m_strLineAB.Format(_T("-%d"), 1);
		m_strLineBC.Format(_T("-%d"), 1);
		m_strLineCA.Format(_T("-%d"), 1);

		SetDlgItemText(CELL_LINE_00, m_strLineAB);
		SetDlgItemText(CELL_LINE_01, m_strLineBC);
		SetDlgItemText(CELL_LINE_02, m_strLineCA);

	}

}

void CNavMesh_Page::Calc_Neighbor()
{
	for (auto& vecIter : m_vecSubset_Cell)
	{
		for (auto& iter_Dst : vecIter)
		{
			if (iter_Dst == nullptr)
				return;

			_v3* tmpDstAB = iter_Dst->Get_Line(CCell::LINE_AB);
			_v3* tmpDstBC = iter_Dst->Get_Line(CCell::LINE_BC);
			_v3* tmpDstCA = iter_Dst->Get_Line(CCell::LINE_CA);

			for (auto& iter_Rsc : vecIter)
			{
				if (iter_Rsc == nullptr)
					return;

				if (iter_Dst == iter_Rsc)
					continue;

				Check_Line(tmpDstAB, iter_Dst, iter_Rsc, CCell::LINE_AB);
				Check_Line(tmpDstBC, iter_Dst, iter_Rsc, CCell::LINE_BC);
				Check_Line(tmpDstCA, iter_Dst, iter_Rsc, CCell::LINE_CA);
			}
		}
	}
}

void CNavMesh_Page::Set_AreaParam()
{
	if (m_pSelectCell == nullptr)
		return;

	_int iAreaindex = m_cAreaList.GetCurSel();

	m_pSelectCell->Set_CellParam(CELL_PARAM(iAreaindex));

	_int iAreaindex2 = m_pSelectCell->Get_CellParam();
}

void CNavMesh_Page::Get_AreaParam()
{
	_int iAreaindex = m_pSelectCell->Get_CellParam();

	m_cAreaList.SetCurSel(iAreaindex);
}

void CNavMesh_Page::Check_Line(_v3* _DstLine, CCell* _DstCell, CCell* _RscCell, _int _LineNum)
{
	CCell*	tmpCell = _RscCell;

	// 라인 3개의 인덱스
	_int iIndex = 0;
	_bool bCheck = false;
	_int iLineNum = _LineNum;

	_v3* tmpRscAB = _RscCell->Get_Line(CCell::LINE_AB);
	_v3* tmpRcsBC = _RscCell->Get_Line(CCell::LINE_BC);
	_v3* tmpRcsCA = _RscCell->Get_Line(CCell::LINE_CA);

	// Dst AB 검사 , Rsc
	if (_DstLine[0] + _DstLine[1] == tmpRscAB[1] + tmpRscAB[0])
	{
		//iIndex = 0;
		bCheck = true;
	}

	// AB 검사 BC
	else if (_DstLine[0] + _DstLine[1] == tmpRcsBC[1] + tmpRcsBC[0])
	{
		//iIndex = 1;
		bCheck = true;
	}

	// AB 검사 CA
	else if (_DstLine[0] + _DstLine[1] == tmpRcsCA[1] + tmpRcsCA[0])
	{
		//iIndex = 2;
		bCheck = true;
	}

	else
	{
		//iIndex   = -1;
		tmpCell = nullptr;
	}

	_DstCell->Set_Sibling(tmpCell, iLineNum, iIndex);
}

void CNavMesh_Page::Add_Subset()
{
	//HTREEITEM* tmpItem = new HTREEITEM;

	_tchar szSubset[MAX_STR] = L"Subset_";
	_tchar szSubsetIndex[MAX_STR] = L"";
	_int   iCurIndex = m_iSubsetIdx;

	_stprintf_s(szSubsetIndex, _T("%d"), iCurIndex);

	lstrcat(szSubset, szSubsetIndex);

	NaviNodeTree.InsertItem(szSubset, 0, 0, TVI_ROOT, TVI_LAST);

	CELL_LIST* tmpCellList = new CELL_LIST;
	m_vecSubset_Cell.push_back(*tmpCellList);
}

void CNavMesh_Page::Render()
{
	Key_Down();

	MyUpdateData();
	Move_Position();

	LOOP(3)
	{
		if (m_arrVertex[i] != nullptr)
		{
			m_arrVertex[i]->Update_GameObject(0);
			m_arrVertex[i]->Render_GameObject();
		}
	}


	//for (auto& vecIter : m_vecSubset_Cell)
	//{
	if (m_iSubsetIdx <= m_vecSubset_Cell.size() - 1)
	{
		for (auto& iter : m_vecSubset_Cell[m_iSubsetIdx])
		{
			iter->Update();
			iter->Render();
		}
	}
	//}
}

void CNavMesh_Page::Create_Cell(LPDIRECT3DDEVICE9 pGraphicDev, _v3* _vPos)
{
	_int iIndex = 0;
	_bool bFind = false;

	// 셀을 생성한다.
	CCell*	pInstance = CCell::Create(pGraphicDev , _vPos);


	// 인덱스 탐색
	LOOP(MAX_STR * 2)
	{
		bFind = false;

		// 해당하는 Index 의 Subset 에서
		for (auto& iter : m_vecSubset_Cell[m_iSubsetIdx])
		{
			// i 랑 인덱스랑 같다면,
			if (i == iter->Get_CellIdx())
			{
				// 계속 탐색한다.
				bFind = true;
				continue;
			}
		}

		// 만약 다 돌았는데도 false 이면,
		if (bFind == false)
		{
			iIndex = i;
			break;
		}
	}

	pInstance->Set_CellIdx(iIndex);
	m_vecSubset_Cell[m_iSubsetIdx].push_back(pInstance);

	_tchar	tmpSubsetIndex[MAX_STR] = L"";
	_tchar	tmpIndex[MAX_STR] = L"";

	_itot_s(iIndex, tmpIndex, 10);
	_itot_s(m_iSubsetIdx, tmpSubsetIndex, 10);

	_tchar szSubset[MAX_STR] = L"Subset_";

	lstrcat(szSubset , tmpSubsetIndex);

	HTREEITEM tmpNode = CTreeFinder::Find_Node_By_Name(NaviNodeTree, szSubset);

	if (tmpNode == nullptr)
	{
		NaviNodeTree.InsertItem(tmpIndex, 0, 0, TVI_ROOT, TVI_LAST);
	}

	else
	{
		NaviNodeTree.InsertItem(tmpIndex, 0, 0, tmpNode);
	}
}
void CNavMesh_Page::Save_CellData()
{
	if (m_vecSubset_Cell[0].size() == 0)
		return;

	wofstream fout;

	fout.open(L"../../Data/Navmesh_Test.dat");

	if (fout.fail())
		return;

	DWORD dwByte = 0;
	_tchar szCelldata[MAX_STR] = L"";
	_tchar szSlash[2] = L"|";
	wstring wstrCombined = L"";

	m_vecSubset_Cell.capacity();

	for (size_t vecIter = 0; vecIter < m_iSubsetIdx + 1; ++vecIter)
	{
		for (auto& iter : m_vecSubset_Cell[vecIter])
		{
			CELL_INFO* CellInfo = new CELL_INFO;

			// 정보 복사
			_stprintf_s(CellInfo->szVtx_A_x, _T("%.3f"), iter->Get_VertexPos()[0].x);
			_stprintf_s(CellInfo->szVtx_A_y, _T("%.3f"), iter->Get_VertexPos()[0].y);
			_stprintf_s(CellInfo->szVtx_A_z, _T("%.3f"), iter->Get_VertexPos()[0].z);

			_stprintf_s(CellInfo->szVtx_B_x, _T("%.3f"), iter->Get_VertexPos()[1].x);
			_stprintf_s(CellInfo->szVtx_B_y, _T("%.3f"), iter->Get_VertexPos()[1].y);
			_stprintf_s(CellInfo->szVtx_B_z, _T("%.3f"), iter->Get_VertexPos()[1].z);

			_stprintf_s(CellInfo->szVtx_C_x, _T("%.3f"), iter->Get_VertexPos()[2].x);
			_stprintf_s(CellInfo->szVtx_C_y, _T("%.3f"), iter->Get_VertexPos()[2].y);
			_stprintf_s(CellInfo->szVtx_C_z, _T("%.3f"), iter->Get_VertexPos()[2].z);

			_stprintf_s(CellInfo->szAB_Index, _T("%d"), iter->Get_SiblingIdx()[0]);
			_stprintf_s(CellInfo->szBC_Index, _T("%d"), iter->Get_SiblingIdx()[1]);
			_stprintf_s(CellInfo->szCA_Index, _T("%d"), iter->Get_SiblingIdx()[2]);

			_stprintf_s(CellInfo->szIndex, _T("%d"), iter->Get_CellIdx());
			_stprintf_s(CellInfo->szOption, _T("%d"), iter->Get_CellParam());

			_stprintf_s(CellInfo->szSubset, _T("%d"), (_int)vecIter);

			// 정보 합치기

			lstrcpy(szCelldata, CellInfo->szSubset);
			lstrcat(szCelldata, szSlash);
			lstrcat(szCelldata, CellInfo->szIndex);
			lstrcat(szCelldata, szSlash);

			lstrcat(szCelldata, CellInfo->szVtx_A_x);
			lstrcat(szCelldata, szSlash);
			lstrcat(szCelldata, CellInfo->szVtx_A_y);
			lstrcat(szCelldata, szSlash);
			lstrcat(szCelldata, CellInfo->szVtx_A_z);
			lstrcat(szCelldata, szSlash);

			lstrcat(szCelldata, CellInfo->szVtx_B_x);
			lstrcat(szCelldata, szSlash);
			lstrcat(szCelldata, CellInfo->szVtx_B_y);
			lstrcat(szCelldata, szSlash);
			lstrcat(szCelldata, CellInfo->szVtx_B_z);
			lstrcat(szCelldata, szSlash);

			lstrcat(szCelldata, CellInfo->szVtx_C_x);
			lstrcat(szCelldata, szSlash);
			lstrcat(szCelldata, CellInfo->szVtx_C_y);
			lstrcat(szCelldata, szSlash);
			lstrcat(szCelldata, CellInfo->szVtx_C_z);
			lstrcat(szCelldata, szSlash);

			lstrcat(szCelldata, CellInfo->szOption);
			lstrcat(szCelldata, szSlash);

			//wstrCombined = szCelldata;

			fout << szCelldata << endl;

			Safe_Delete(CellInfo);
		}
	}

	fout.close();
}
void CNavMesh_Page::Load_CellData()
{
	NaviNodeTree.DeleteAllItems();

	for (auto& vecIter : m_vecSubset_Cell)
	{
		for (auto& iter : vecIter)
		{
			Safe_Release(iter);
		}

		vecIter.clear();
	}

	m_vecSubset_Cell.clear();
	m_vecSubset_Cell.shrink_to_fit();

	wifstream fin;
	_int	iIndex;
	_int	iSubIdx;
	_float fA[3];
	_float fB[3];
	_float fC[3];
	_v3    vVtx[3];
	_int	iParam;
	_int	iCellIndex = 0;

	_tchar tmpSubset[MAX_STR] = L"999";
	HTREEITEM newTreeNode = NULL;

	fin.open(L"../../Data/Navmesh_Test.dat");

	if (fin.fail())
		return;

	while (true)
	{
		CELL_INFO* CellInfo = new CELL_INFO;

		fin.getline(CellInfo->szSubset, MAX_STR, '|');
		fin.getline(CellInfo->szIndex, MAX_STR, '|');

		fin.getline(CellInfo->szVtx_A_x, MAX_STR, '|');
		fin.getline(CellInfo->szVtx_A_y, MAX_STR, '|');
		fin.getline(CellInfo->szVtx_A_z, MAX_STR, '|');

		fin.getline(CellInfo->szVtx_B_x, MAX_STR, '|');
		fin.getline(CellInfo->szVtx_B_y, MAX_STR, '|');
		fin.getline(CellInfo->szVtx_B_z, MAX_STR, '|');

		fin.getline(CellInfo->szVtx_C_x, MAX_STR, '|');
		fin.getline(CellInfo->szVtx_C_y, MAX_STR, '|');
		fin.getline(CellInfo->szVtx_C_z, MAX_STR, '|');

		fin.getline(CellInfo->szOption, MAX_STR);


		if (fin.eof())
		{
			Safe_Delete(CellInfo);
			break;
		}

		else
		{
			iIndex = _ttoi(CellInfo->szIndex);
			iParam = _ttoi(CellInfo->szOption);
			iSubIdx = _ttoi(CellInfo->szSubset);

			fA[0] = (_float)_tstof(CellInfo->szVtx_A_x);
			fA[1] = (_float)_tstof(CellInfo->szVtx_A_y);
			fA[2] = (_float)_tstof(CellInfo->szVtx_A_z);

			fB[0] = (_float)_tstof(CellInfo->szVtx_B_x);
			fB[1] = (_float)_tstof(CellInfo->szVtx_B_y);
			fB[2] = (_float)_tstof(CellInfo->szVtx_B_z);

			fC[0] = (_float)_tstof(CellInfo->szVtx_C_x);
			fC[1] = (_float)_tstof(CellInfo->szVtx_C_y);
			fC[2] = (_float)_tstof(CellInfo->szVtx_C_z);

			vVtx[0] = { fA[0], fA[1], fA[2] };
			vVtx[1] = { fB[0], fB[1], fB[2] };
			vVtx[2] = { fC[0], fC[1], fC[2] };


			CCell*	pInstance = CCell::Create(g_pGraphicDev, vVtx);
			pInstance->Set_CellIdx(iCellIndex);
			pInstance->Set_CellParam(CELL_PARAM(iParam));

			_tchar SubsetName[MAX_STR] = L"Subset_";

			if (lstrcmp(tmpSubset, CellInfo->szSubset))
			{
				iCellIndex = 0;

				lstrcpy(tmpSubset, CellInfo->szSubset);
				lstrcat(SubsetName, tmpSubset);
				newTreeNode = NaviNodeTree.InsertItem(SubsetName, 0, 0, TVI_ROOT, TVI_LAST);

				CELL_LIST* tmpList = new CELL_LIST;
				m_vecSubset_Cell.push_back(*tmpList);
			}


			if (newTreeNode != NULL)
			{
				_stprintf_s(CellInfo->szIndex, _T("%d"), iCellIndex);

				NaviNodeTree.InsertItem(CellInfo->szIndex, 0, 0, newTreeNode);
				m_vecSubset_Cell[iSubIdx].push_back(pInstance);

				++iCellIndex;
			}

			//HTREEITEM tmpNode = CTreeFinder::Find_Node_By_Name(NaviNodeTree, SubsetName);

			//if (tmpNode == NULL)
			//{
			//	HTREEITEM newTree = NaviNodeTree.InsertItem(SubsetName, 0, 0, TVI_ROOT, TVI_LAST);
			//	NaviNodeTree.InsertItem(CellInfo->szIndex, 0, 0, newTree);
			//}
			//
			//else
			//{
			//	NaviNodeTree.InsertItem(CellInfo->szIndex, 0, 0, newTreeNode);
			//}


			pInstance = nullptr;
			Safe_Delete(CellInfo);
		}
	}

	fin.close();
}
void CNavMesh_Page::Clear_CellList()
{
	for (auto& vecIter : m_vecSubset_Cell)
	{
		for (auto& iter : vecIter)
		{
			Safe_Release(iter);
		}
		vecIter.clear();
	}

	m_vecSubset_Cell.clear();
	m_vecSubset_Cell.shrink_to_fit();

	NaviNodeTree.DeleteAllItems();
}
bool CNavMesh_Page::Compare_Vertex(_v3 _vPos, _float _fLength)
{
	_v3 tmpPos = _vPos;

	for (auto& iter : m_vecSubset_Cell[m_iSubsetIdx])
	{
		LOOP(3)
		{
			_float fLength = V3_LENGTH(&(iter->Get_VertexPos()[i] - tmpPos));

			if (fLength <= _fLength)
			{
				iter->Set_Selected(true);
				m_pSelectCell = iter;
				return true;
			}
		}

		iter->Set_Selected(false);
	}

	return false;
}

void CNavMesh_Page::Greed_Vertex(_v3* _vPos ,CGameObject* _target , _float _fLength)
{
	_v3 tmpPos = *_vPos;

	for (auto& iter : m_vecSubset_Cell[m_iSubsetIdx])
	{
		if (_target != nullptr)
		{
			if(iter == _target)
				continue;
		}

		LOOP(3)
		{
			if (V3_LENGTH(&(iter->Get_VertexPos()[i] - tmpPos)) <= _fLength)
			{
				tmpPos = iter->Get_VertexPos()[i];
				*_vPos = tmpPos;
				return;
			}
		}
	}
}

void CNavMesh_Page::Free()
{
	LOOP(3)
	{
		if (m_arrVertex[i] != nullptr)
		{
			Safe_Release(m_arrVertex[i]);
		}
	}


	for (auto& vecIter : m_vecSubset_Cell)
	{
		for (auto& iter : vecIter)
		{
			Safe_Release(iter);
		}
		vecIter.clear();
	}
	m_vecSubset_Cell.clear();
	m_vecSubset_Cell.shrink_to_fit();
}


BEGIN_MESSAGE_MAP(CNavMesh_Page, CPropertyPage)
	ON_EN_CHANGE(IDC_EDIT11, &CNavMesh_Page::OnEnChangeEdit11)
	ON_EN_CHANGE(IDC_EDIT12, &CNavMesh_Page::OnEnChangeEdit12)
	ON_EN_CHANGE(IDC_EDIT13, &CNavMesh_Page::OnEnChangeEdit13)

	ON_EN_CHANGE(IDC_EDIT2, &CNavMesh_Page::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, &CNavMesh_Page::OnEnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT4, &CNavMesh_Page::OnEnChangeEdit4)

	ON_EN_CHANGE(IDC_EDIT6, &CNavMesh_Page::OnEnChangeEdit6)
	ON_EN_CHANGE(IDC_EDIT5, &CNavMesh_Page::OnEnChangeEdit5)

	ON_CONTROL_RANGE(BN_CLICKED, IDC_GREED_00, IDC_GREED_01, &CNavMesh_Page::OnGreedMode)
	ON_BN_CLICKED(IDC_GREED_00, &CNavMesh_Page::OnBnClickedGreed00)
	ON_BN_CLICKED(IDC_GREED_01, &CNavMesh_Page::OnBnClickedGreed01)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_EMODE_00, IDC_EMODE_02, &CNavMesh_Page::OnEditMode)
	ON_BN_CLICKED(IDC_EMODE_00, &CNavMesh_Page::OnBnClickedEmode00)
	ON_BN_CLICKED(IDC_EMODE_01, &CNavMesh_Page::OnBnClickedEmode01)
	ON_BN_CLICKED(IDC_EMODE_02, &CNavMesh_Page::OnBnClickedEmode02)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CONTROL_00, IDC_CONTROL_01, &CNavMesh_Page::OnCtrlMode)
	ON_BN_CLICKED(IDC_CONTROL_00, &CNavMesh_Page::OnBnClickedControl00)
	ON_BN_CLICKED(IDC_CONTROL_01, &CNavMesh_Page::OnBnClickedControl01)
	ON_BN_CLICKED(IDC_BUTTON7, &CNavMesh_Page::OnBnClickedButton7)
	ON_EN_CHANGE(SELECT_INDEX, &CNavMesh_Page::OnEnChangeIndex)
	ON_BN_CLICKED(IDC_CELL_SAVE, &CNavMesh_Page::OnBnClickedCellSave)
	ON_BN_CLICKED(IDC_CELL_LOAD, &CNavMesh_Page::OnBnClickedCellLoad)
	ON_BN_CLICKED(IDC_BUTTON1, &CNavMesh_Page::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON5, &CNavMesh_Page::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON2, &CNavMesh_Page::OnBnClickedButton2)
	ON_EN_CHANGE(IDC_EDIT1, &CNavMesh_Page::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CNaviMesh 메시지 처리기입니다.


void CNavMesh_Page::OnEnChangeEdit11()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CNavMesh_Page::OnEnChangeEdit12()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CNavMesh_Page::OnEnChangeEdit13()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CNavMesh_Page::OnEnChangeEdit2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CNavMesh_Page::OnEnChangeEdit3()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CNavMesh_Page::OnEnChangeEdit4()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CPropertyPage::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


BOOL CNavMesh_Page::OnInitDialog()
{
	CPropertyPage::OnInitDialog();


	HTREEITEM* tmpItem = new HTREEITEM;

	NaviNodeTree.InsertItem(L"Subset_0" , 0, 0 , TVI_ROOT, TVI_LAST);

	CELL_LIST* tmpCellList = new CELL_LIST;
	m_vecSubset_Cell.push_back(*tmpCellList);

	m_cAreaList.AddString(L"0_Normal");
	m_cAreaList.AddString(L"1_Water");
	m_cAreaList.AddString(L"2_Event_Trigger");
	m_cAreaList.AddString(L"3_Cinema_Trigger");
	m_cAreaList.AddString(L"4_Next_Subset");
	m_cAreaList.AddString(L"5_Next_Subset_Fall");
	m_cAreaList.AddString(L"6_Upper_Cell");
	m_cAreaList.AddString(L"7_Spawn_Trigger");

	m_cAreaList.SetCurSel(0);


	return TRUE;
}


void CNavMesh_Page::OnEnChangeEdit6()
{
	GetDlgItemText(IDC_EDIT6, m_YGap);
	m_fYGap = (_float)_tstof(m_YGap);
}


void CNavMesh_Page::OnEnChangeEdit5()
{
	GetDlgItemText(IDC_EDIT5, m_YRate);
	m_fYRate = (_float)_tstof(m_YRate);

	//cout << m_fYRate << endl;
}


void CNavMesh_Page::OnBnClickedGreed00()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CNavMesh_Page::OnBnClickedGreed01()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CNavMesh_Page::OnBnClickedEmode00()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CNavMesh_Page::OnBnClickedEmode01()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CNavMesh_Page::OnBnClickedEmode02()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CNavMesh_Page::OnBnClickedControl00()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CNavMesh_Page::OnBnClickedControl01()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CNavMesh_Page::OnGreedMode(_uint _Mode)
{
	UpdateData(true);

	switch (_Mode)
	{
	case _NONE:
	{
		m_iGreedMode = _NONE;

		break;
	}
	case _GREED:
	{
		m_iGreedMode = _GREED;
		break;
	}
	}

	UpdateData(false);
}

void CNavMesh_Page::OnEditMode(_uint _Mode)
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

void CNavMesh_Page::OnCtrlMode(_uint _Mode)
{
	UpdateData(true);

	switch (_Mode)
	{
	case _MOUSE:
	{
		m_iControlMode = _MOUSE;

		break;
	}
	case _KEYBOARD:
	{
		m_iControlMode = _KEYBOARD;
		break;
	}
	}

	UpdateData(false);
}


void CNavMesh_Page::OnBnClickedButton7()
{

}


void CNavMesh_Page::OnEnChangeIndex()
{
	GetDlgItemText(SELECT_INDEX, m_strSelectIdx);
	m_iSelectIdx = (_int)_tstof(m_strSelectIdx);
}


void CNavMesh_Page::OnBnClickedCellSave()
{
	Save_CellData();
}


void CNavMesh_Page::OnBnClickedCellLoad()
{
	Load_CellData();
}


void CNavMesh_Page::OnBnClickedButton1()
{
	Calc_Neighbor();
}


void CNavMesh_Page::OnBnClickedButton5()
{
	Set_AreaParam();
}


void CNavMesh_Page::OnBnClickedButton2()
{
	Add_Subset();
}


void CNavMesh_Page::OnEnChangeEdit1()
{
	GetDlgItemText(IDC_EDIT1, m_strSubsetIdx);
	m_iSubsetIdx = (_int)_tstof(m_strSubsetIdx);
}
