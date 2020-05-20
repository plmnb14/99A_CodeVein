#include "Cell.h"
#include "Management.h"

CCell::CCell(_Device pGraphicDev)
	:CGameObject(pGraphicDev)
	, m_pVertex()
	, m_pVertexPos()
	, m_pTransform(nullptr)
	, m_bSelected(false)
	, m_bVtxSelected(false)
	, m_bCam_Trig(false)
	, m_bSpawn_Trig(false)
	, m_bEvent_Trig(false)
{
}

CCell::CCell(const CCell& rhs)
	:CGameObject(rhs)
{
}

CCell::~CCell()
{
}

HRESULT CCell::Ready_Cell(_v3* _pVertex)
{
	return S_OK;
}

HRESULT CCell::Update_Cell(_v3 * _pVertex)
{

	return S_OK;
}

void CCell::Set_CellIdx(_int _iCellIndex)
{
	m_iCellIndex = _iCellIndex;
}

void CCell::Set_Vertex()
{
	m_pVertex[0].vPos = _v3(0.0f, 0.5f, 0.f);
	m_pVertex[0].dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	m_pVertex[1].vPos = _v3(0.5f, -0.5f, 0.f);
	m_pVertex[1].dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	m_pVertex[2].vPos = _v3(-0.5f, -0.5f, 0.f);
	m_pVertex[2].dwColor = D3DCOLOR_ARGB(255, 255, 0, 0);
}

void CCell::Set_Vertex(POINTS _ePoint, _v3 _vPos)
{
	m_pVertex[_ePoint].vPos = _vPos;
}

void CCell::Set_Vertex(_v3 * _vPos)
{
	LOOP(3)
	{
		m_pVertex[i].vPos = _vPos[i];
		m_pVertexPos[i] = _vPos[i];
	}

	LOOP(3)
		m_pVertex[i].dwColor = COLOR_TEAL(0.5f);
}

_v3 * CCell::Get_Line(LINES _eLineName)
{
	switch (_eLineName)
	{
	case LINE_AB:
	{
		return m_vAB;
	}

	case LINE_BC:
	{
		return m_vBC;
	}

	case LINE_CA:
	{
		return m_vCA;
	}
	}

	return nullptr;
}

_v2 * CCell::Get_NavLine(LINES _eLineName)
{
	return m_tNavLine->m_vPoint;
}

NAV_LINE CCell::Get_NavLineInfo(LINES _eLineName)
{
	return m_tNavLine[_eLineName];
}

HRESULT CCell::Initialize()
{
	Add_Components();

	return S_OK;
}

HRESULT CCell::LateInit()
{
	return S_OK;
}

HRESULT CCell::Add_Components()
{
	Engine::CComponent*	pComponent = nullptr;

	// Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	m_pTransform->Set_Scale({ 1,1,1 });

	return S_OK;
}

CGameObject * CCell::Clone_GameObject(void* pArg)
{
	return nullptr;
}

_int CCell::Update()
{
	CGameObject::Update_GameObject(0);

	if (m_bSelected)
	{
		if (m_bVtxSelected == false)
		{
			LOOP(3)
				m_pVertex[i].dwColor = COLOR_PINK(0.5f);
		}

		else
		{
			LOOP(3)
				m_pVertex[i].dwColor = COLOR_YELLOW(0.5f);
		}
	}

	else
	{

		switch (m_eCellParam)
		{
		case NORMAL:
		{
			LOOP(3)
				m_pVertex[i].dwColor = COLOR_TEAL(0.5f);
			break;
		}

		case WATER:
		{
			LOOP(3)
				m_pVertex[i].dwColor = COLOR_SKYBLUE(0.5f);
			break;
		}

		case EVENT_TRIGGER:
		{
			LOOP(3)
				m_pVertex[i].dwColor = COLOR_W_PURPLE(0.5f);
			break;
		}

		case CINEMA_TRIGGER:
		{
			LOOP(3)
				m_pVertex[i].dwColor = COLOR_ORANGE(0.5f);
			break;
		}

		case NEXT_SUBSET_FALL:
		{
			LOOP(3)
				m_pVertex[i].dwColor = COLOR_DARKBLUE(0.5f);
			break;
		}

		case NEXT_SUBSET:
		{
			LOOP(3)
				m_pVertex[i].dwColor = COLOR_DARKBLUE(0.5f);
			break;
		}

		case UPPER_CELL:
		{
			LOOP(3)
				m_pVertex[i].dwColor = COLOR_RED(0.5f);
			break;
		}

		case SPAWN_TRIGGER:
		{
			LOOP(3)
				m_pVertex[i].dwColor = COLOR_PURPLE(0.5f);
			break;
		}
		}
	}

	Calc_Line();

	return NO_EVENT;
}

void CCell::LateUpdate()
{
}

void CCell::Render()
{
	D3DXCOLOR tmpColor = m_pVertex[0].dwColor;

	CManagement::Get_Instance()->Gizmo_Draw_Triangle(m_pVertex , tmpColor);
	CManagement::Get_Instance()->Gizmo_Draw_Triangle_Line(m_pVertexPos , m_bSelected , m_eCellParam);

	//LOOP(3)
	//	CGizmoMgr::GetInstance()->Draw_Vertex(m_pVertex[i].vPos);
}

void CCell::DrawTriangle(_v3* _pVertex)
{
	LOOP(3)
		m_pVertex[i].vPos = _pVertex[i];
}

_bool CCell::Compare_Inner_Only (const _v3 pPos)
{
	for (_ulong i = 0; i < LINE_END; ++i)
	{
		if (COMPARE_LEFT == Compare_Line(&_v2(pPos.x, pPos.z), i))
		{
			return false;
		}
	}

	return true;
}

CCell::COMPARE CCell::Compare(const _v3 * pEndPos, _long * pIndex , _v2* _pDirection , _ulong* _iIndex, _bool _bIsUpper)
{
	for (_ulong i = 0; i < LINE_END; ++i)
	{
		if (COMPARE_LEFT == Compare_Line(&_v2(pEndPos->x, pEndPos->z), i , _pDirection))
		{
			if (nullptr == m_pSibling[i])
			{
				*_pDirection = m_tNavLine[i].m_vNor;

				IF_NOT_NULL(_iIndex)
					*_iIndex = i;

				return COMPARE_STAY;
			}
			else
			{
					// 전진하는 타일이 Upper 라면
				if (m_pSibling[i]->Get_CellParam() == EVENT_TRIGGER)
				{
					return COMPARE_FALL;
				}

				// 어퍼 아니면
				*pIndex = m_pSibling[i]->Get_CellIdx();
				return COMPARE_MOVE;
			}
		}
	}

	////cout << "움직임 " << endl;
	return COMPARE_MOVE;
}

CCell::COMPARE_LR CCell::Compare_Line(const _v2 * pEndPos , _int _iLine, _v2* _pDirection, _ulong* _iIndex)
{
	_v2		vDest = *pEndPos - m_tNavLine[_iLine].m_vPoint[0];

	_float	fResult = D3DXVec2Dot(D3DXVec2Normalize(&vDest, &vDest), &m_tNavLine[_iLine].m_vNor);

	if (0 <= fResult)
	{
		// 양수
		return COMPARE_LEFT;
	}
	else
	{
		//음수
		return COMPARE_RIGHT;
	}
}

void CCell::Calc_Line()
{
	m_tNavLine[LINE_AB].m_vPoint[0] = { m_pVertexPos[POINT_A].x ,m_pVertexPos[POINT_A].z };
	m_tNavLine[LINE_AB].m_vPoint[1] = { m_pVertexPos[POINT_B].x ,m_pVertexPos[POINT_B].z };

	m_tNavLine[LINE_AB].m_vDir = m_tNavLine[LINE_AB].m_vPoint[1] - m_tNavLine[LINE_AB].m_vPoint[0];
	m_tNavLine[LINE_AB].m_vNor = _v2(m_tNavLine[LINE_AB].m_vDir.y * -1.f, m_tNavLine[LINE_AB].m_vDir.x);
	D3DXVec2Normalize(&m_tNavLine[LINE_AB].m_vDir, &m_tNavLine[LINE_AB].m_vDir);

	m_tNavLine[LINE_BC].m_vPoint[0] = { m_pVertexPos[POINT_B].x ,m_pVertexPos[POINT_B].z };
	m_tNavLine[LINE_BC].m_vPoint[1] = { m_pVertexPos[POINT_C].x ,m_pVertexPos[POINT_C].z };

	m_tNavLine[LINE_BC].m_vDir = m_tNavLine[LINE_BC].m_vPoint[1] - m_tNavLine[LINE_BC].m_vPoint[0];
	m_tNavLine[LINE_BC].m_vNor = _v2(m_tNavLine[LINE_BC].m_vDir.y * -1.f, m_tNavLine[LINE_BC].m_vDir.x);
	D3DXVec2Normalize(&m_tNavLine[LINE_BC].m_vDir, &m_tNavLine[LINE_BC].m_vDir);

	m_tNavLine[LINE_CA].m_vPoint[0] = { m_pVertexPos[POINT_C].x ,m_pVertexPos[POINT_C].z };
	m_tNavLine[LINE_CA].m_vPoint[1] = { m_pVertexPos[POINT_A].x ,m_pVertexPos[POINT_A].z };

	m_tNavLine[LINE_CA].m_vDir = m_tNavLine[LINE_CA].m_vPoint[1] - m_tNavLine[LINE_CA].m_vPoint[0];
	m_tNavLine[LINE_CA].m_vNor = _v2(m_tNavLine[LINE_CA].m_vDir.y * -1.f, m_tNavLine[LINE_CA].m_vDir.x);
	D3DXVec2Normalize(&m_tNavLine[LINE_CA].m_vDir, &m_tNavLine[LINE_CA].m_vDir);

	m_vAB[0] = m_pVertexPos[POINT_A];
	m_vAB[1] = m_pVertexPos[POINT_B];
	
	m_vBC[0] = m_pVertexPos[POINT_B];
	m_vBC[1] = m_pVertexPos[POINT_C];

	m_vCA[0] = m_pVertexPos[POINT_C];
	m_vCA[1] = m_pVertexPos[POINT_A];
};

void CCell::Set_CenterPos(_v3 * _pVertex)
{
	m_vCenter = (_pVertex[0] + _pVertex[1] + _pVertex[2]) * 0.33333334f;
}

void CCell::Set_Selected(_bool _bSelected)
{
	m_bSelected = _bSelected;
}

void CCell::Set_IndexVtxPos(_int _iIndex , _v3 _vPos)
{

	m_pVertexPos[_iIndex] = _vPos;
	m_pVertex[_iIndex].vPos = _vPos;

}

void CCell::Set_VtxPos(_v3* _vPos)
{
	LOOP(3)
	{
		m_pVertexPos[i] = _vPos[i];
		m_pVertex[i].vPos = _vPos[i];
	}

}

_v3* CCell::Get_VertexPos()
{
	return m_pVertexPos;
}

_v3 CCell::Get_CenterPos()
{
	Set_CenterPos(m_pVertexPos);

	return m_vCenter;
}

_int* CCell::Get_SiblingIdx()
{
	return m_pSiblingIdx;
}

void CCell::Add_Vtx_Y(_float _fHight)
{
	LOOP(3)
	{
		m_pVertex[i].vPos += _v3{ 0.f ,_fHight, 0.f };
		m_pVertexPos[i] += _v3{ 0.f ,_fHight, 0.f };
	}
}

void CCell::Add_Vtx_Y(_float _fHight, _int iIndex)
{
	m_pVertex[iIndex].vPos += _v3{ 0.f ,_fHight, 0.f };
	m_pVertexPos[iIndex] += _v3{ 0.f ,_fHight, 0.f };
}

void CCell::Add_Center_Y(_float _fHight)
{
	m_vCenter += _v3{ 0.f ,_fHight, 0.f };

	LOOP(3)
	{
		m_pVertex[i].vPos += _v3{ 0.f ,_fHight, 0.f };
		m_pVertexPos[i] += _v3{ 0.f ,_fHight, 0.f };
	}
}

void CCell::Set_Sibling(CCell * _Sibling, _int _LineNum , _int _iLineIdx)
{
	IF_NOT_NULL(_Sibling)
	{
		m_pSibling[_LineNum] = _Sibling;
		m_pSiblingIdx[_LineNum] = m_pSibling[_LineNum]->Get_CellIdx();
	}

	else
	{
		IF_NULL(m_pSibling[_LineNum])
		{
			m_pSibling[_LineNum] = nullptr;
			m_pSiblingIdx[_LineNum] = -1;
		}
	}
}

CCell * CCell::Create(_Device pGraphicDev, _v3* _vPos)
{
	CCell*	pInstance = new CCell(pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		pInstance = nullptr;
	}

	pInstance->Set_Vertex(_vPos);

	return pInstance;
}

void CCell::Free()
{
	CGameObject::Free();

	Safe_Release(m_pTransform);

	LOOP(3)
	{
		m_pSibling[i] = nullptr;
	}
}
