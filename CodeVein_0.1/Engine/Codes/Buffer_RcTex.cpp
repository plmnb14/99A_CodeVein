#include "..\Headers\Buffer_RcTex.h"


D3DVERTEXELEMENT9 g_VertexElemHardware[] =
{ // 첫번째 멤버 : Stream
	{ 0, 0,     D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION,  0 },
	//{ 0, 3 * 4, D3DDECLTYPE_FLOAT3,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL,    0 },
	{ 0, 3 * 4	,D3DDECLTYPE_FLOAT2,     D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD,  0 },
	{ 1, 0,     D3DDECLTYPE_D3DCOLOR,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_COLOR,     0 },
	{ 1, 4,     D3DDECLTYPE_D3DCOLOR,   D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_COLOR,     1 },
	D3DDECL_END()
};

// 인스턴스 데이터
struct BOX_INSTANCEDATA_POS
{
	D3DCOLOR color;
	BYTE x;
	BYTE y;
	BYTE z;
	BYTE rotation;
};

_int g_iNumofInstance = 10;


CBuffer_RcTex::CBuffer_RcTex(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{

}

CBuffer_RcTex::CBuffer_RcTex(const CBuffer_RcTex & rhs)
	: CVIBuffer(rhs)
	, m_pVertexDeclHardware(rhs.m_pVertexDeclHardware)
	, m_pVBInstanceData(rhs.m_pVBInstanceData)
{
	
}

HRESULT CBuffer_RcTex::Ready_Component_Prototype()
{
	// 1) Create the vertex declaration
	m_pGraphic_Dev->CreateVertexDeclaration(g_VertexElemHardware, &m_pVertexDeclHardware);

	// 2) Create VB and IB
	m_iNumVertices = 4;
	m_iStride = sizeof(VTXTEX);
	m_dwFVF = D3DFVF_XYZ | D3DFVF_TEX1;
	m_pPosition = new _v3[4];

	m_iNumPolygons = 2;
	m_iPolygonSize = sizeof(POLYGON16);
	m_eFormat = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_Component_Prototype()))
		return E_FAIL;

	VTXTEX*		pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _v3(-0.5f, 0.5f, 0.f);
	pVertices[0].vTexUV = _v2(0.0f, 0.f);

	pVertices[1].vPosition = _v3(0.5f, 0.5f, 0.f);
	pVertices[1].vTexUV = _v2(1.f, 0.f);

	pVertices[2].vPosition = _v3(0.5f, -0.5f, 0.f);
	pVertices[2].vTexUV = _v2(1.f, 1.f);

	pVertices[3].vPosition = _v3(-0.5f, -0.5f, 0.f);
	pVertices[3].vTexUV = _v2(0.f, 1.f);	

	for (size_t i = 0; i < m_iNumVertices; ++i)
		m_pPosition[i] = pVertices[i].vPosition;	

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

	// 3)  Create a VB for the instancing data
	m_pGraphic_Dev->CreateVertexBuffer(g_iNumofInstance * sizeof(BOX_INSTANCEDATA_POS), 0, 0, D3DPOOL_MANAGED, &m_pVBInstanceData, 0);
	
	BOX_INSTANCEDATA_POS* pIPos;
	m_pVBInstanceData->Lock(0, NULL, (void**)&pIPos, 0);
	int nRemainingBoxes = g_iNumofInstance;
	for (BYTE iY = 0; iY < 1; iY++)
		for (BYTE iZ = 0; iZ < 1; iZ++)
			for (BYTE iX = 0; iX < 1 && nRemainingBoxes > 0; iX++, nRemainingBoxes--)
			{
				// 12 : Pos (4 * 3) 나중에 노말이 추가되면 24로 변경할 것.
				BOX_INSTANCEDATA_POS instanceBox;
				instanceBox.color = D3DCOLOR_ARGB(
					0xff,
					0x7f + 0x40 * ((g_iNumofInstance - nRemainingBoxes +iX) % 3),
					0x7f + 0x40 * ((g_iNumofInstance - nRemainingBoxes + iZ + 1) % 3),
					0x7f + 0x40 * ((g_iNumofInstance - nRemainingBoxes + iY + 2) % 3));
				instanceBox.x = iZ * 12;
				instanceBox.z = iX * 12;
				instanceBox.y = iY * 12;
				instanceBox.rotation = ((WORD)iX + (WORD)iZ + (WORD)iY) * 12 / 3;
				*pIPos = instanceBox, pIPos++;
			}
	m_pVBInstanceData->Unlock();


	return NOERROR;
}

HRESULT CBuffer_RcTex::Ready_Component(void* pArg)
{
	
	return NOERROR;
}

void CBuffer_RcTex::Render_VIBuffer()
{	
	m_pGraphic_Dev->SetStreamSource(0, m_pVB, 0, m_iStride);
	m_pGraphic_Dev->SetFVF(m_dwFVF);
	m_pGraphic_Dev->SetIndices(m_pIB);
	m_pGraphic_Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumPolygons);	
}

void CBuffer_RcTex::Render_Before_Instancing()
{
	m_pGraphic_Dev->SetVertexDeclaration(m_pVertexDeclHardware);

	m_pGraphic_Dev->SetStreamSource(0, m_pVB, 0, m_iStride);
	m_pGraphic_Dev->SetStreamSourceFreq(0, D3DSTREAMSOURCE_INDEXEDDATA | g_iNumofInstance);

	m_pGraphic_Dev->SetStreamSource(1, m_pVBInstanceData, 0, sizeof(BOX_INSTANCEDATA_POS));
	m_pGraphic_Dev->SetStreamSourceFreq(1, D3DSTREAMSOURCE_INSTANCEDATA | 1ul);

	m_pGraphic_Dev->SetIndices(m_pIB);
}

void CBuffer_RcTex::Render_DrawPrimitive_Instancing()
{
	m_pGraphic_Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumPolygons);
}

void CBuffer_RcTex::Render_After_Instancing()
{
	m_pGraphic_Dev->SetStreamSourceFreq(0, 1);
	m_pGraphic_Dev->SetStreamSourceFreq(1, 1);
}

CBuffer_RcTex * CBuffer_RcTex::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBuffer_RcTex*	pInstance = new CBuffer_RcTex(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		MessageBox(0, L"CBuffer_RcTex Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;

}


CComponent * CBuffer_RcTex::Clone_Component(void* pArg)
{
	CBuffer_RcTex*	pInstance = new CBuffer_RcTex(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MessageBox(0, L"CBuffer_RcTex Created Failed", L"System Error", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBuffer_RcTex::Free()
{
	if (!m_bisClone)
	{
		Safe_Release(m_pVBInstanceData);
		Safe_Release(m_pVertexDeclHardware);
	}
	
	CVIBuffer::Free();
}
