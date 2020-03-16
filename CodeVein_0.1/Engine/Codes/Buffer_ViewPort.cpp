#include "..\Headers\Buffer_ViewPort.h"

CBuffer_ViewPort::CBuffer_ViewPort(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{

}

CBuffer_ViewPort::CBuffer_ViewPort(const CBuffer_ViewPort & rhs)
	: CVIBuffer(rhs)	
{

}

HRESULT CBuffer_ViewPort::Ready_Component_Prototype(_float fStartX, _float fStartY, _float fSizeX, _float fSizeY)
{
	m_iNumVertices = 4;
	m_iStride = sizeof(VTXVIEWPORT);
	m_dwFVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
	m_pPosition = new _v3[4];

	m_iNumPolygons = 2;
	m_iPolygonSize = sizeof(POLYGON16);
	m_eFormat = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Component_Prototype()))
		return E_FAIL;

	VTXVIEWPORT*		pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _v4(fStartX, fStartY, 0.f, 1.f);
	pVertices[0].vTexUV = _v2(0.0f, 0.f);

	pVertices[1].vPosition = _v4(fStartX + fSizeX, fStartY, 0.f, 1.f);
	pVertices[1].vTexUV = _v2(1.f, 0.f);

	pVertices[2].vPosition = _v4(fStartX + fSizeX, fStartY + fSizeY, 0.f, 1.f);
	pVertices[2].vTexUV = _v2(1.f, 1.f);

	pVertices[3].vPosition = _v4(fStartX, fStartY + fSizeY, 0.f, 1.f);
	pVertices[3].vTexUV = _v2(0.f, 1.f);	

	for (size_t i = 0; i < m_iNumVertices; ++i)
		m_pPosition[i] = _v3(pVertices[i].vPosition.x, pVertices[i].vPosition.y, pVertices[i].vPosition.z);

	m_pVB->Unlock();
	
	POLYGON16*		pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_pIB->Unlock();


	return NOERROR;
}

HRESULT CBuffer_ViewPort::Ready_Component(void* pArg)
{
	
	return NOERROR;
}

void CBuffer_ViewPort::Render_VIBuffer()
{	
	m_pGraphic_Dev->SetStreamSource(0, m_pVB, 0, m_iStride);
	m_pGraphic_Dev->SetFVF(m_dwFVF);
	m_pGraphic_Dev->SetIndices(m_pIB);
	m_pGraphic_Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumPolygons);	
}


CBuffer_ViewPort * CBuffer_ViewPort::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _float fStartX, _float fStartY, _float fSizeX, _float fSizeY)
{
	CBuffer_ViewPort*	pInstance = new CBuffer_ViewPort(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype(fStartX, fStartY, fSizeX, fSizeY)))
	{
		MessageBox(0, L"CBuffer_ViewPort Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;

}


CComponent * CBuffer_ViewPort::Clone_Component(void* pArg)
{
	CBuffer_ViewPort*	pInstance = new CBuffer_ViewPort(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MessageBox(0, L"CBuffer_ViewPort Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBuffer_ViewPort::Free()
{
	CVIBuffer::Free();
}
