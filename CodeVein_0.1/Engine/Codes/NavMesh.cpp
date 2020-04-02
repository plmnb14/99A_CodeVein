#include "NavMesh.h"
#include "Management.h"

CNavMesh::CNavMesh(_Device pGraphicDev)
	: CComponent(pGraphicDev)
	, m_dwIndex(0)
	, m_dwSubsetIdx(0)
{
}


CNavMesh::CNavMesh(const CNavMesh & rhs)
	: CComponent(rhs)
{
}

CNavMesh::~CNavMesh()
{
}

void CNavMesh::Move_NaviMesh_Y(const _ulong _dwSubIdx, const _float _fYDist)
{
	return;

	for (auto& iter : *m_vecSubset_Cell[_dwSubIdx])
	{
		_v3* tmpPos = iter->Get_VertexPos();

		LOOP(3)
		{
			tmpPos[i].y += _fYDist;
		}

		iter->Set_VtxPos(tmpPos);
	}
}

HRESULT CNavMesh::Ready_NaviMesh(_Device _pGraphicDev, const _tchar * _NavMeshPath)
{
	m_vecSubset_Cell.reserve(1000);

	_tchar FullPathName[MAX_STR] = L"../../Data/";
	lstrcat(FullPathName, _NavMeshPath);

	wifstream fin;
	_int	iIndex;
	_int	iSubIdx;
	_float fA[3];
	_float fB[3];
	_float fC[3];
	_v3    vVtx[3];
	_int	iParam;

	_tchar tmpSubset[MAX_STR] = L"999";

	fin.open(FullPathName);

	if (fin.fail())
		return S_OK;

	while (true)
	{
		Engine::CELL_INFO* CellInfo = new Engine::CELL_INFO;

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
			iIndex = _wtoi(CellInfo->szIndex);
			iParam = _wtoi(CellInfo->szOption);
			iSubIdx = _wtoi(CellInfo->szSubset);

			if (iParam != 0)
			{
				//cout << iSubIdx << " :" << iIndex << " : " << iParam << endl;
			}
				 
			fA[0] = (_float)_wtof(CellInfo->szVtx_A_x);
			fA[1] = (_float)_wtof(CellInfo->szVtx_A_y);
			fA[2] = (_float)_wtof(CellInfo->szVtx_A_z);

			fB[0] = (_float)_wtof(CellInfo->szVtx_B_x);
			fB[1] = (_float)_wtof(CellInfo->szVtx_B_y);
			fB[2] = (_float)_wtof(CellInfo->szVtx_B_z);

			fC[0] = (_float)_wtof(CellInfo->szVtx_C_x);
			fC[1] = (_float)_wtof(CellInfo->szVtx_C_y);
			fC[2] = (_float)_wtof(CellInfo->szVtx_C_z);

			vVtx[0] = { fA[0], fA[1], fA[2] };
			vVtx[1] = { fB[0], fB[1], fB[2] };
			vVtx[2] = { fC[0], fC[1], fC[2] };


			CCell*	pInstance = CCell::Create(_pGraphicDev, vVtx);
			pInstance->Set_CellIdx(iIndex);
			pInstance->Set_CellParam(CELL_PARAM(iParam));
			pInstance->Calc_Line();

			if (lstrcmp(tmpSubset, CellInfo->szSubset))
			{
				lstrcpy(tmpSubset, CellInfo->szSubset);

				VEC_CELL* tmpVec = new VEC_CELL;
				m_vecSubset_Cell.push_back(tmpVec);
			}

			(*m_vecSubset_Cell[iSubIdx]).push_back(pInstance);
			

			// 값 지워주기
			pInstance = nullptr;
			Safe_Delete(CellInfo);
		}
	}

	fin.close();

	Link_Cell();

	return S_OK;
}

HRESULT CNavMesh::Ready_Prototype_NaviMesh()
{
	return S_OK;
}

void CNavMesh::Render_NaviMesh(void)
{
	//for (auto& iterVec : m_vecSubset_Cell)
	//{

	for (auto& iter : *m_vecSubset_Cell[m_dwSubsetIdx])
	{
		iter->Update();
		iter->Render();
	}
	//}
}

HRESULT CNavMesh::Link_Cell()
{
	{
		for (auto& iterVec : m_vecSubset_Cell)
		{
			for (auto& iter_Dst : *iterVec)
			{
				if (iter_Dst == nullptr)
					return S_OK;

				_v3* tmpDstAB = iter_Dst->Get_Line(CCell::LINE_AB);
				_v3* tmpDstBC = iter_Dst->Get_Line(CCell::LINE_BC);
				_v3* tmpDstCA = iter_Dst->Get_Line(CCell::LINE_CA);

				LOOP(2)
				{
					tmpDstAB[i].y = 0.f;
					tmpDstBC[i].y = 0.f;
					tmpDstCA[i].y = 0.f;
				}

				for (auto& iter_Rsc : *iterVec)
				{
					if (iter_Rsc == nullptr)
						return S_OK;

					if (iter_Dst == iter_Rsc)
						continue;

					Check_Line(tmpDstAB, iter_Dst, iter_Rsc, CCell::LINE_AB);
					Check_Line(tmpDstBC, iter_Dst, iter_Rsc, CCell::LINE_BC);
					Check_Line(tmpDstCA, iter_Dst, iter_Rsc, CCell::LINE_CA);
				}
			}
		}
	}

	return S_OK;
}

void CNavMesh::Check_Line(_v3* _DstLine, CCell* _DstCell, CCell* _RscCell, _int _LineNum)
{
	CCell*	tmpCell = _RscCell;

	// 라인 3개의 인덱스
	_int iIndex = 0;
	_bool bCheck = false;
	_int iLineNum = _LineNum;

	_v3* tmpRscAB = _RscCell->Get_Line(CCell::LINE_AB);
	_v3* tmpRcsBC = _RscCell->Get_Line(CCell::LINE_BC);
	_v3* tmpRcsCA = _RscCell->Get_Line(CCell::LINE_CA);

	LOOP(2)
	{
		tmpRscAB[i].y = 0.f;
		tmpRcsBC[i].y = 0.f;
		tmpRcsCA[i].y = 0.f;
	}

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

void CNavMesh::Check_OnNavMesh(const _v3* pTargetPos)
{
	if (m_dwIndex == -1)
	{
		while (true)
		{
			_int iSubsetIdx = 0;

			for (auto& iter : *m_vecSubset_Cell[iSubsetIdx])
			{
				if (iter->Compare_Inner_Only(*pTargetPos))
				{
					m_dwIndex = iter->Get_CellIdx();
					m_dwSubsetIdx = iSubsetIdx;
					return;
				}
			}

			++iSubsetIdx;
		}
	}
}

_v3 CNavMesh::Move_OnNaviMesh(CRigidBody* _pRigid, const _v3* pTargetPos, const _v3* pTargetDir , const _float _fSpeedDelta)
{
	m_bCanMove = false;

	_v3		vEndPos = *pTargetPos + *pTargetDir * _fSpeedDelta;
	_v2		vDirection = {};
	_ulong	iIndex = 0;
	_bool	bIsUpper = false;

	if ((*m_vecSubset_Cell[m_dwSubsetIdx])[m_dwIndex]->Get_CellParam() == UPPER_CELL)
		bIsUpper = true;

	if (CCell::COMPARE_FALL == (*m_vecSubset_Cell[m_dwSubsetIdx])[m_dwIndex]->Compare(&vEndPos, &m_dwIndex, &vDirection, &iIndex, bIsUpper))
	{
		//if (CCell::COMPARE_FALL == m_vecSubset_Cell[m_dwSubsetIdx][m_dwIndex]->Compare(&*pTargetPos, &m_dwIndex, &vDirection, &iIndex, bIsUpper))
		//{
		if (_pRigid != nullptr)
		{
			_pRigid->Set_IsFall(true);
			_pRigid->Set_IsGround(false);
		}
		//}

		m_bCanMove = true;
		return vEndPos;
	}

	else if (CCell::COMPARE_MOVE == (*m_vecSubset_Cell[m_dwSubsetIdx])[m_dwIndex]->Compare(&vEndPos, &m_dwIndex , &vDirection, &iIndex , bIsUpper))
	{
		//if (m_vecSubset_Cell[m_dwSubsetIdx][m_dwIndex]->Get_CellParam() == ENGINE::UPPER_CELL)
		//{
		//	if (CCell::COMPARE_STAY == m_vecSubset_Cell[m_dwSubsetIdx][m_dwIndex]->Compare(&vEndPos, &m_dwIndex, &vDirection, &iIndex))
		//	{
		//		_v3		vIncident, vSliding, vLineNor, vNewEndPos;
		//		_v3		vCross_01, vCross_02;
		//
		//		vLineNor = { vDirection.x , 0 , vDirection.y };
		//		V3_NORMAL_SELF(&vLineNor);
		//
		//		// 입사 Vector
		//		vIncident = *pTargetDir;
		//
		//		vSliding = vIncident - D3DXVec3Dot(&vIncident, &vLineNor) * vLineNor;
		//
		//		vNewEndPos = *pTargetPos + vSliding * _fSpeedDelta;
		//
		//
		//		return vNewEndPos;
		//	}
		//}

		m_bCanMove = true;
		return vEndPos;
	}

	else if (CCell::COMPARE_STAY == (*m_vecSubset_Cell[m_dwSubsetIdx])[m_dwIndex]->Compare(&vEndPos, &m_dwIndex, &vDirection, &iIndex , bIsUpper))
	{
		_v3		vIncident, vSliding, vLineNor, vNewEndPos;
		_v3		vCross_01, vCross_02;

		vLineNor = { vDirection.x , 0 , vDirection.y };
		V3_NORMAL_SELF(&vLineNor);

		// 입사 Vector
		vIncident = *pTargetDir;

		vSliding = vIncident - D3DXVec3Dot(&vIncident, &vLineNor) * vLineNor;

		vNewEndPos = *pTargetPos + vSliding * _fSpeedDelta;

		if (CCell::COMPARE_FALL == (*m_vecSubset_Cell[m_dwSubsetIdx])[m_dwIndex]->Compare(&vEndPos, &m_dwIndex, &vDirection, &iIndex, bIsUpper))
		{
			//if (CCell::COMPARE_FALL == m_vecSubset_Cell[m_dwSubsetIdx][m_dwIndex]->Compare(&*pTargetPos, &m_dwIndex, &vDirection, &iIndex, bIsUpper))
			//{
				_pRigid->Set_IsFall(true);
				_pRigid->Set_IsGround(false);
			//}

			m_bCanMove = true;
			return vEndPos;
		}

		else if (CCell::COMPARE_MOVE == (*m_vecSubset_Cell[m_dwSubsetIdx])[m_dwIndex]->Compare(&vNewEndPos, &m_dwIndex, &vDirection, &iIndex , bIsUpper))
		{
			//if (m_vecSubset_Cell[m_dwSubsetIdx][m_dwIndex]->Get_CellParam() == NEXT_SUBSET)
			//{
			//	m_dwSubsetIdx += 1;
			//	m_dwIndex = 0;
			//}

			m_bCanMove = true;
			return vNewEndPos;
		}

		else if (CCell::COMPARE_STAY == (*m_vecSubset_Cell[m_dwSubsetIdx])[m_dwIndex]->Compare(&vNewEndPos, &m_dwIndex, &vDirection, &iIndex , bIsUpper))
		{
			m_bCanMove = false;
			return  *pTargetPos - *pTargetDir * _fSpeedDelta;

			// 슬라이딩 벡터 보류
			// 보류
			//_v2* tmpPoint = m_vecCell[m_dwIndex]->Get_NavLine(CCell::LINES(iIndex));
			//_v2	 vresultPoint;
			//_v2	 TargetPoint;
			//
			//_v3 tmpPos = *pTargetPos;
			//_v3 newDir;
			//_float TmpALen = fabs(D3DXVec2Length(&(tmpPoint[0] - _v2(tmpPos.x, tmpPos.z))));
			//_float TmpBLen = fabs(D3DXVec2Length(&(tmpPoint[1] - _v2(tmpPos.x, tmpPos.z))));
			//_float TmpAngle = 0.f;
			//
			//// 가까운 점을 찾았다.
			//vresultPoint = (TmpALen > TmpBLen ? tmpPoint[1] : tmpPoint[0]);
			//
			//for (auto& iter : m_vecCell)
			//{
			//	//cout << ">?" << endl;
			//
			//	LOOP(3)
			//	{
			//		for (int j = 0; j < 2; ++j)
			//		{
			//			if (iter->Get_NavLine(CCell::LINES(i))[j] == vresultPoint)
			//			{
			//				if (j == 0)
			//					TargetPoint = iter->Get_NavLine(CCell::LINES(i))[1];
			//
			//				else
			//					TargetPoint = iter->Get_NavLine(CCell::LINES(i))[0];
			//
			//				_v2 vTmpDir = TargetPoint - vresultPoint;
			//				_float innerAngle = 0.f;
			//
			//				innerAngle = D3DXVec3Dot(&-vSliding, &_v3(vTmpDir.x , 0 , vTmpDir.y));
			//
			//				if (innerAngle == 0)
			//					TmpAngle = innerAngle;
			//
			//				else if (TmpAngle < innerAngle)
			//				{
			//					// 만약 각이 더 크면
			//					TmpAngle = innerAngle;
			//					_v2 TmpDirection = iter->Get_NavLineInfo(CCell::LINES(i)).m_vNor;
			//
			//					newDir = { -TmpDirection.x , 0 , -TmpDirection.y };
			//				}
			//
			//				else
			//					continue;
			//			}
			//		}
			//	}
			//
			//
			//}
			//
			//V3_NORMAL_SELF(&newDir);
			//
			//// 입사 Vector
			//vIncident = *pTargetDir;
			//
			//vSliding = vIncident - D3DXVec3Dot(&vIncident, &newDir) * newDir;
			//
			//vNewEndPos = *pTargetPos + vSliding * _fSpeedDelta;
			//
			//return *pTargetPos - vSliding * _fSpeedDelta;
		}
	}

	return _v3(0.f, 0.f, 0.f);
}

_v3 CNavMesh::Axis_Y_OnNavMesh(const _v3 _vPos)
{

	if ((*m_vecSubset_Cell[m_dwSubsetIdx])[m_dwIndex] == nullptr)
		return _vPos;

	D3DXPLANE plane;

	////cout << m_vecSubset_Cell[m_dwSubsetIdx][m_dwIndex]->Get_VertexPos()[1].y << endl;

	D3DXPlaneFromPoints(&plane,
		&(*m_vecSubset_Cell[m_dwSubsetIdx])[m_dwIndex]->Get_VertexPos()[0],
		&(*m_vecSubset_Cell[m_dwSubsetIdx])[m_dwIndex]->Get_VertexPos()[1],
		&(*m_vecSubset_Cell[m_dwSubsetIdx])[m_dwIndex]->Get_VertexPos()[2]);

	_v3 vPos = _vPos;

	vPos.y = -(plane.a * vPos.x
		+ plane.c * vPos.z + plane.d) / plane.b;

	return vPos;
}

_bool CNavMesh::Goto_Next_Subset(const _v3 _vPos, CRigidBody* _pRigid)
{
	if ((*m_vecSubset_Cell[m_dwSubsetIdx])[m_dwIndex]->Get_CellParam() == NEXT_SUBSET || (*m_vecSubset_Cell[m_dwSubsetIdx])[m_dwIndex]->Get_CellParam() == NEXT_SUBSET_FALL)
	{
		if ((*m_vecSubset_Cell[m_dwSubsetIdx])[m_dwIndex]->Get_CellParam() == NEXT_SUBSET_FALL)
		{
			_pRigid->Set_IsFall(true);
			_pRigid->Set_IsGround(false);
		}

		++m_dwSubsetIdx;
		m_dwIndex = 0;

		//LOOP(3)
		//{
		//	if (_vPos.y <= m_vecSubset_Cell[m_dwSubsetIdx][m_dwIndex]->Get_VertexPos()[i].y)
		//	{
		//		++m_dwSubsetIdx;
		//		m_dwIndex = 0;
		//
		//		if (m_vecSubset_Cell[m_dwSubsetIdx][m_dwIndex]->Get_CenterPos().y < _vPos.y)
		//		{
		//			_pRigid->Set_IsFall(true);
		//			_pRigid->Set_IsGround(false);
		//		}
		//
		//		return TRUE;
		//	}
		//}

		return TRUE;
	}

	return FALSE;
}

void CNavMesh::Trigger_Camera()
{
}

void CNavMesh::Trigger_Spawn()
{
}

void CNavMesh::Trigger_Event()
{
}

CELL_PARAM CNavMesh::Get_CellParam()
{
	return (*m_vecSubset_Cell[m_dwSubsetIdx])[m_dwIndex]->Get_CellParam();
}

void CNavMesh::Reset_NaviMesh()
{
	for (auto& iter : m_vecSubset_Cell)
	{
		for (auto& Veciter : *iter)
		{
			Safe_Release(Veciter);
		}

		iter->clear();
		iter->shrink_to_fit();
		Safe_Delete(iter);
	}

	m_vecSubset_Cell.clear();
	m_vecSubset_Cell.shrink_to_fit();
}

CNavMesh * CNavMesh::Create(_Device pGraphicDev)
{
	CNavMesh*	pInstance = new CNavMesh(pGraphicDev);

	if (FAILED(pInstance->Ready_Prototype_NaviMesh()))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent * CNavMesh::Clone_Component(void * pArg)
{
	CNavMesh*	pInstance = new CNavMesh(*this);

	return pInstance;
}

void CNavMesh::Free()
{
	CComponent::Free();

	for (auto& iter : m_vecSubset_Cell)
	{
		for (auto& Veciter : *iter)
		{
			Safe_Release(Veciter);
		}

		iter->clear();
		iter->shrink_to_fit();
		Safe_Delete(iter);
	}

	m_vecSubset_Cell.clear();
	m_vecSubset_Cell.shrink_to_fit();

}
