#include "QuadTree.h"

CQuadTree::CQuadTree(void)
{
	ZeroMemory(m_pChild, sizeof(CQuadTree*) * CHILD_END);
	ZeroMemory(m_pNeighbor, sizeof(CQuadTree*) * NEIGHBOR_END);
	ZeroMemory(m_dwCorner, sizeof(_ulong) * CORNER_END);
}

CQuadTree::~CQuadTree(void)
{

}

HRESULT CQuadTree::Ready_QuadTree(const _ulong& dwCntX, const _ulong& dwCntZ)
{
	m_dwCorner[CORNER_LT] = dwCntX * dwCntZ - dwCntX;
	m_dwCorner[CORNER_RT] = dwCntX * dwCntZ - 1;
	m_dwCorner[CORNER_LB] = 0;
	m_dwCorner[CORNER_RB] = dwCntX - 1;

	m_dwCenter = (m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_RT] + m_dwCorner[CORNER_LB] + m_dwCorner[CORNER_RB]) >> 2;

	m_fRadius = 0.f;
	m_bCulled = false;

	Make_ChildQuadTree();

	return S_OK;
}

_bool CQuadTree::Check_QuadTree_Frustum_Object(CFrustum_Culling* pFrustum, const _v3* pVtxPos, const _v3 pObjPos)
{
	// 반지름을 젭니다.
	// 좌측 상단 0 번째의 위치와 , 중앙 좌표로 부터의 거리를 젭니다.
	m_fRadius = D3DXVec3Length(&(pVtxPos[m_dwCorner[CORNER_LT]] - pVtxPos[m_dwCenter]));

	// 반지름 을 젠 후, 해당 
	// 절두체 안에 들어오는지 판단합니다.
	_bool bIsIn = pFrustum->Check_InFrustum(&pVtxPos[m_dwCenter], m_fRadius);

	// 안에 있다면,
	if (true == bIsIn)
	{
		//if (pFrustum->Check_InFrustumObj(&pObjPos, 10.f, this))
		//{
		//	return true;
		//}

		for (_ulong i = 0; i < CHILD_END; ++i)
		{
			if (nullptr != m_pChild[i])
				m_pChild[i]->Check_QuadTree_Frustum_Object(pFrustum, pVtxPos, pObjPos);
		}
	}

	return false;
}

void CQuadTree::Make_ChildQuadTree(void)
{
	if (1 == m_dwCorner[CORNER_RT] - m_dwCorner[CORNER_LT])
		return;

	for (_ulong i = 0; i < CHILD_END; ++i)
	{
		_ulong	dwLT, dwRT, dwLB, dwRB;
		Make_ChildCorner(CHILD(i),
			m_dwCorner[CORNER_LT],
			m_dwCorner[CORNER_RT],
			m_dwCorner[CORNER_LB],
			m_dwCorner[CORNER_RB],
			m_dwCenter,
			&dwLT, &dwRT, &dwLB, &dwRB);

		m_pChild[i] = CQuadTree::Create(dwLT, dwRT, dwLB, dwRB);

		if (nullptr != m_pChild[i])
			m_pChild[i]->Make_ChildQuadTree();
	}

}

void CQuadTree::Make_ChildCorner(CHILD eType,
	const _ulong& dwPLT,
	const _ulong& dwPRT,
	const _ulong& dwPLB,
	const _ulong& dwPRB,
	const _ulong& dwPCenter,
	_ulong* pLT, _ulong* pRT, _ulong* pLB, _ulong* pRB)
{
	_ulong	dwLT, dwRT, dwLB, dwRB;
	_ulong	dwLC, dwTC, dwRC, dwBC;

	dwLC = (dwPLT + dwPLB) >> 1;
	dwTC = (dwPLT + dwPRT) >> 1;
	dwRC = (dwPRT + dwPRB) >> 1;
	dwBC = (dwPLB + dwPRB) >> 1;

	switch (eType)
	{
	case CHILD_LT:
		dwLT = dwPLT;
		dwRT = dwTC;
		dwLB = dwLC;
		dwRB = dwPCenter;
		break;

	case CHILD_RT:
		dwLT = dwTC;
		dwRT = dwPRT;
		dwLB = dwPCenter;
		dwRB = dwRC;
		break;

	case CHILD_LB:
		dwLT = dwLC;
		dwRT = dwPCenter;
		dwLB = dwPLB;
		dwRB = dwBC;
		break;

	case CHILD_RB:
		dwLT = dwPCenter;
		dwRT = dwRC;
		dwLB = dwBC;
		dwRB = dwPRB;
		break;
	}

	*pLT = dwLT;
	*pRT = dwRT;
	*pLB = dwLB;
	*pRB = dwRB;
}

void CQuadTree::Set_Corner(const _ulong& dwLT, const _ulong& dwRT, const _ulong& dwLB, const _ulong& dwRB)
{
	m_dwCorner[CORNER_LT] = dwLT;
	m_dwCorner[CORNER_RT] = dwRT;
	m_dwCorner[CORNER_LB] = dwLB;
	m_dwCorner[CORNER_RB] = dwRB;

	m_dwCenter = (m_dwCorner[CORNER_LT] + m_dwCorner[CORNER_RT] + m_dwCorner[CORNER_LB] + m_dwCorner[CORNER_RB]) >> 2;
}

CQuadTree* CQuadTree::Create(const _ulong& dwCntX, const _ulong& dwCntZ)
{
	CQuadTree*		pInstance = new CQuadTree;

	if (FAILED(pInstance->Ready_QuadTree(dwCntX, dwCntZ)))
		Safe_Release(pInstance);

	if (FAILED(pInstance->Ready_Neighbor()))
		Safe_Release(pInstance);

	return pInstance;
}

CQuadTree* CQuadTree::Create(const _ulong& dwLT, const _ulong& dwRT, const _ulong& dwLB, const _ulong& dwRB)
{
	CQuadTree* pInstance = new CQuadTree;

	pInstance->Set_Corner(dwLT, dwRT, dwLB, dwRB);

	return pInstance;
}

void CQuadTree::Free(void)
{
	for (_ulong i = 0; i < CHILD_END; ++i)
		Safe_Release(m_pChild[i]);
}

void CQuadTree::Make_Neighbor(void)
{
	if (nullptr == m_pChild[0])
		return;

	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RT];
	m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_LB];

	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LT];
	m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pChild[CHILD_RB];

	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_LT];
	m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pChild[CHILD_RB];

	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_TOP] = m_pChild[CHILD_RT];
	m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pChild[CHILD_LB];


	if (nullptr != m_pNeighbor[NEIGHBOR_LEFT])
	{
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RT];
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_LEFT] = m_pNeighbor[NEIGHBOR_LEFT]->m_pChild[CHILD_RB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_TOP])
	{
		m_pChild[CHILD_LT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_LB];
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_TOP] = m_pNeighbor[NEIGHBOR_TOP]->m_pChild[CHILD_RB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_RIGHT])
	{
		m_pChild[CHILD_RT]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_RIGHT] = m_pNeighbor[NEIGHBOR_RIGHT]->m_pChild[CHILD_LB];
	}

	if (nullptr != m_pNeighbor[NEIGHBOR_BOTTOM])
	{
		m_pChild[CHILD_LB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_LT];
		m_pChild[CHILD_RB]->m_pNeighbor[NEIGHBOR_BOTTOM] = m_pNeighbor[NEIGHBOR_BOTTOM]->m_pChild[CHILD_RT];
	}

	for (_ulong i = 0; i < CHILD_END; ++i)
		m_pChild[i]->Make_Neighbor();

}

HRESULT CQuadTree::Ready_Neighbor(void)
{
	Make_Neighbor();

	return S_OK;
}
