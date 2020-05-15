#include "Optimization.h"

COptimization::COptimization(_Device _pGraphicDev)
	:CComponent(_pGraphicDev)
	, m_pFrustum(nullptr)
	, m_pQuadTree(nullptr)
{
}

COptimization::COptimization(const COptimization& rhs)
	:CComponent(rhs)
	, m_pFrustum(rhs.m_pFrustum)
	, m_pQuadTree(rhs.m_pQuadTree)
{
}

COptimization::~COptimization(void)
{

}

HRESULT COptimization::Ready_Optimization(_bool bChoice, const _ulong dwCntX, const _ulong dwCntZ)
{
	m_pFrustum = CFrustum_Culling::Create();

	if (nullptr == m_pFrustum)
		return E_FAIL;

	//if (false == bChoice)
	//	return S_OK;
	//
	//m_pQuadTree = CQuadTree::Create(dwCntX, dwCntZ);
	//
	//if (nullptr == m_pFrustum)
	//	return E_FAIL;

	return S_OK;
}

_bool COptimization::Check_InFrustumforObject(const _v3 * pPos, const _float & fRadius)
{
	return m_pFrustum->Check_InFrustum(pPos, fRadius);
}

COptimization* COptimization::Create(_Device _pGraphicDev, _bool bChoice, _ulong dwCntX , _ulong dwCntZ)
{
	COptimization*	pInstance = new COptimization(_pGraphicDev);

	//if (FAILED(pInstance->Ready_Optimization(bChoice, dwCntX, dwCntZ)))
	//	Safe_Release(pInstance);

	return pInstance;
}

CComponent* COptimization::Clone_Component(void* pArg)
{
	COptimization*	pInstance = new COptimization(*this);

	if (FAILED(pInstance->Ready_Optimization(true, 0, 0)))
		Safe_Release(pInstance);

	return pInstance;
}

void COptimization::Free(void)
{
	CComponent::Free();

	Safe_Release(m_pFrustum);
	Safe_Release(m_pQuadTree);
}

