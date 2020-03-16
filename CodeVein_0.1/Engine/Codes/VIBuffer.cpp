#include "..\Headers\VIBuffer.h"

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{

}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_iStride(rhs.m_iStride)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_dwFVF(rhs.m_dwFVF)
	, m_iNumPolygons(rhs.m_iNumPolygons)
	, m_iPolygonSize(rhs.m_iPolygonSize)
	, m_eFormat(rhs.m_eFormat)	
	, m_pPosition(rhs.m_pPosition)
{
	m_pVB->AddRef();
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Ready_Component_Prototype()
{
	if (FAILED(m_pGraphic_Dev->CreateVertexBuffer(m_iStride * m_iNumVertices, 0, m_dwFVF, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	if (FAILED(m_pGraphic_Dev->CreateIndexBuffer(m_iPolygonSize * m_iNumPolygons, 0, m_eFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return NOERROR;

	return NOERROR;
}

HRESULT CVIBuffer::Ready_Component(void* pArg)
{
	return NOERROR;
}




void CVIBuffer::Free()
{
	if (false == m_bisClone)
		Safe_Delete_Array(m_pPosition);

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);

	CComponent::Free();
}
