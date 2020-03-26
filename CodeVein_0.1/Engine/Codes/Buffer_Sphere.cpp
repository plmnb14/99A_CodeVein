#include "..\Headers\Buffer_Sphere.h"

USING(Engine)

CBuffer_Sphere::CBuffer_Sphere(_Device pGraphicDev)
	: CVIBuffer(pGraphicDev)
{
}

CBuffer_Sphere::CBuffer_Sphere(const CBuffer_Sphere& rhs)
	: CVIBuffer(rhs)
{

}

HRESULT CBuffer_Sphere::Ready_Component_Prototype()
{
	return S_OK;
}

HRESULT CBuffer_Sphere::Ready_Component(void * pArg)
{
	m_iStride = sizeof(VTX_COL);
	m_dwFVF = VTXFVF_COL;
	m_iNumPolygons = 0;
	m_iNumVertices = 126;

	m_iPolygonSize = sizeof(POLYGON32);
	m_eFormat = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Component_Prototype()))
		return E_FAIL;

	VTX_COL* pVtxCol = nullptr;
	_v3	vVertex = { 0.f, 1.f, 0.f };
	_float fRadian = D3DXToRadian(360.f / 20);

	m_pVB->Lock(0, 0, (void**)&pVtxCol, 0);

	vVertex = { 0.f, 0.f, 1.f };
	LOOP(40)
	{
		pVtxCol[i].vPos = vVertex;
		pVtxCol[i].dwColor = COLOR_GREEN(1.f);
		CALC::V3_RotX(&vVertex, &vVertex, fRadian);
	}

	vVertex = { 0.f, 0.f, 1.f };
	LOOP(46)
	{
		pVtxCol[i + 40].vPos = vVertex;
		pVtxCol[i + 40].dwColor = COLOR_GREEN(1.f);
		CALC::V3_RotY(&vVertex, &vVertex, fRadian);
	}

	vVertex = { 1.f, 0.f, 0.f };
	LOOP(40)
	{
		pVtxCol[i + 86].vPos = vVertex;
		pVtxCol[i + 86].dwColor = COLOR_GREEN(1.f);
		CALC::V3_RotZ(&vVertex, &vVertex, fRadian);
	}

	pVtxCol = nullptr;

	m_pVB->Unlock();

	return S_OK;
}

void CBuffer_Sphere::Render_VIBuffer()
{
	m_pGraphic_Dev->SetFVF(m_dwFVF);

	m_pGraphic_Dev->DrawPrimitive(D3DPT_LINESTRIP, 0, 39);
	m_pGraphic_Dev->DrawPrimitive(D3DPT_LINESTRIP, 39, 46);
	m_pGraphic_Dev->DrawPrimitive(D3DPT_LINESTRIP, 86, 39);
}

void CBuffer_Sphere::Free()
{
	CVIBuffer::Free();
}

CBuffer_Sphere* CBuffer_Sphere::Create(_Device pGraphicDev)
{
	CBuffer_Sphere* pInstance = new CBuffer_Sphere(pGraphicDev);

	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CBuffer_Sphere::Clone_Component(void * pArg)
{
	CBuffer_Sphere*	pInstance = new CBuffer_Sphere(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MessageBox(0, L"CBuffer_Sphere Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}
