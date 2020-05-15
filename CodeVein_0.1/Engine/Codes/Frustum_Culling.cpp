#include "Frustum_Culling.h"
#include "Management.h"
#include "QuadTree.h"

CFrustum_Culling::CFrustum_Culling()
{

}

CFrustum_Culling::~CFrustum_Culling()
{
}

HRESULT CFrustum_Culling::Ready_Component()
{
	return S_OK;
}

//_bool CFrustum_Culling::Check_InFrustumObj(const _v3* pPos, const _float& fRadius , CQuadTree* pQuadTree)
//{
//	Ready_Frustum();
//
//	_mat		ViewMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
//	_mat		ProjMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);
//
//	D3DXMatrixInverse(&ProjMatrix, NULL, &ProjMatrix);
//	D3DXMatrixInverse(&ViewMatrix, NULL, &ViewMatrix);
//
//	for (_uint i = 0; i < 8; ++i)
//	{
//		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &ProjMatrix);
//		D3DXVec3TransformCoord(&m_vPoint[i], &m_vPoint[i], &ViewMatrix);
//	}
//
//	// x+
//	D3DXPlaneFromPoints(&m_Plane[0], &m_vPoint[1], &m_vPoint[5], &m_vPoint[6]);
//
//	// x-
//	D3DXPlaneFromPoints(&m_Plane[1], &m_vPoint[4], &m_vPoint[0], &m_vPoint[3]);
//
//	// y+
//	D3DXPlaneFromPoints(&m_Plane[2], &m_vPoint[4], &m_vPoint[5], &m_vPoint[1]);
//
//	// y-
//	D3DXPlaneFromPoints(&m_Plane[3], &m_vPoint[3], &m_vPoint[2], &m_vPoint[6]);
//
//	// z+
//	D3DXPlaneFromPoints(&m_Plane[4], &m_vPoint[7], &m_vPoint[6], &m_vPoint[5]);
//
//	// z-
//	D3DXPlaneFromPoints(&m_Plane[5], &m_vPoint[0], &m_vPoint[1], &m_vPoint[2]);
//
//	if (nullptr != pQuadTree)
//	{
//		//pQuadTree->Check_QuadTree_Frustum_Object(this, pVtxPos, pIndex, pTriCnt);
//	}
//
//	return Check_InFrustum(pPos , fRadius);
//}

_bool CFrustum_Culling::Check_InFrustum(const _v3* pPos, const _float fRadius)
{
	_float	fDistance = 0.f;

	for (_uint i = 0; i < 6; ++i)
	{
		fDistance = D3DXPlaneDotCoord(&m_Plane[i], pPos);

		if (fDistance > fRadius)
			return false;
	}

	return true;
}

_bool CFrustum_Culling::Check_InFrustum_Advenced(const _v3 * _Pos, const _float fRadius)
{
	_float fDist = 0.f;

	return true;
}

HRESULT CFrustum_Culling::Ready_Frustum(void)
{
	m_vPoint[0] = _v3(-1.f, 1.f, 0.f);
	m_vPoint[1] = _v3(1.f, 1.f, 0.f);
	m_vPoint[2] = _v3(1.f, -1.f, 0.f);
	m_vPoint[3] = _v3(-1.f, -1.f, 0.f);

	m_vPoint[4] = _v3(-1.f, 1.f, 1.f);
	m_vPoint[5] = _v3(1.f, 1.f, 1.f);
	m_vPoint[6] = _v3(1.f, -1.f, 1.f);
	m_vPoint[7] = _v3(-1.f, -1.f, 1.f);

	return S_OK;
}

CFrustum_Culling* CFrustum_Culling::Create()
{
	CFrustum_Culling* pComponent = new CFrustum_Culling();

	if (FAILED(pComponent->Ready_Component()))
	{
		Safe_Release(pComponent);
		return nullptr;
	}

	return pComponent;
}

void CFrustum_Culling::Free()
{
}
