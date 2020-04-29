#include "..\Headers\Buffer_CubeTex.h"

CBuffer_CubeTex::CBuffer_CubeTex(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CBuffer_CubeTex::CBuffer_CubeTex(const CBuffer_CubeTex & rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CBuffer_CubeTex::Ready_Component_Prototype()
{
	// For.Vertex Buffer
	m_iStride = sizeof(VTXCUBE);
	m_iNumVertices = 8;
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

	// For.Index Buffer
	m_iPolygonSize = sizeof(POLYGON16);
	m_iNumPolygons = 12;
	m_eFormat = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Component_Prototype()))
		return E_FAIL;

	// 할당된 공간에 값을 채운다.
	VTXCUBE*			pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _v3(-0.5f, 0.25f, -0.5f);
	pVertices[0].vTexUV = pVertices[0].vPosition;

	pVertices[1].vPosition = _v3(0.5f, 0.25f, -0.5f);
	pVertices[1].vTexUV = pVertices[1].vPosition;

	pVertices[2].vPosition = _v3(0.5f, -0.25f, -0.5f);
	pVertices[2].vTexUV = pVertices[2].vPosition;

	pVertices[3].vPosition = _v3(-0.5f, -0.25f, -0.5f);
	pVertices[3].vTexUV = pVertices[3].vPosition;

	pVertices[4].vPosition = _v3(-0.5f, 0.25f, 0.5f);
	pVertices[4].vTexUV = pVertices[4].vPosition;

	pVertices[5].vPosition = _v3(0.5f, 0.25f, 0.5f);
	pVertices[5].vTexUV = pVertices[5].vPosition;

	pVertices[6].vPosition = _v3(0.5f, -0.25f, 0.5f);
	pVertices[6].vTexUV = pVertices[6].vPosition;

	pVertices[7].vPosition = _v3(-0.5f, -0.25f, 0.5f);
	pVertices[7].vTexUV = pVertices[7].vPosition;

	m_pVB->Unlock();

	POLYGON16*		pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	// +X
	pIndices[0]._0 = 1;  pIndices[0]._1 = 5;  pIndices[0]._2 = 6;
	pIndices[1]._0 = 1;  pIndices[1]._1 = 6;  pIndices[1]._2 = 2;

	// -X
	pIndices[2]._0 = 4;  pIndices[2]._1 = 0;  pIndices[2]._2 = 3;
	pIndices[3]._0 = 4;  pIndices[3]._1 = 3;  pIndices[3]._2 = 7;

	// +Y
	pIndices[4]._0 = 4;  pIndices[4]._1 = 5;  pIndices[4]._2 = 1;
	pIndices[5]._0 = 4;  pIndices[5]._1 = 1;  pIndices[5]._2 = 0;

	// -Y
	pIndices[6]._0 = 3;  pIndices[6]._1 = 2;  pIndices[6]._2 = 6;
	pIndices[7]._0 = 3;  pIndices[7]._1 = 6;  pIndices[7]._2 = 7;

	// +Z
	pIndices[8]._0 = 5;  pIndices[8]._1 = 4;  pIndices[8]._2 = 7;
	pIndices[9]._0 = 5;  pIndices[9]._1 = 7;  pIndices[9]._2 = 6;

	// -Z
	pIndices[10]._0 = 0;  pIndices[10]._1 = 1;  pIndices[10]._2 = 2;
	pIndices[11]._0 = 0;  pIndices[11]._1 = 2;  pIndices[11]._2 = 3;


	m_pIB->Unlock();

	return NOERROR;
}

HRESULT CBuffer_CubeTex::Ready_Component(void * pArg)
{
	return NOERROR;
}

void CBuffer_CubeTex::Render_VIBuffer()
{
	if (nullptr == m_pGraphic_Dev)
		return;

	m_pGraphic_Dev->SetStreamSource(0, m_pVB, 0, m_iStride);
	m_pGraphic_Dev->SetFVF(m_dwFVF);
	m_pGraphic_Dev->SetIndices(m_pIB);
	m_pGraphic_Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumPolygons);

	

}

CBuffer_CubeTex * CBuffer_CubeTex::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBuffer_CubeTex*		pInstance = new CBuffer_CubeTex(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		MSG_BOX("CBuffer_CubeTex Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CBuffer_CubeTex::Clone_Component(void* pArg)
{
	CBuffer_CubeTex*		pInstance = new CBuffer_CubeTex(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MSG_BOX("CBuffer_CubeTex Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBuffer_CubeTex::Free()
{
	
	CVIBuffer::Free();
}
