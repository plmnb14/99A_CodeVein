#include "Buffer_Terrain.h"
#include "Transform.h"

USING(Engine)

Engine::CBuffer_Terrain::CBuffer_Terrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: CVIBuffer(pGraphicDev)
	, m_pPosition(nullptr)
{

}

Engine::CBuffer_Terrain::CBuffer_Terrain(const CBuffer_Terrain& rhs)
	:CVIBuffer(rhs)
	, m_pPosition(rhs.m_pPosition)
	, m_dwInterval(rhs.m_dwInterval)
	, m_dwVtxCntX(rhs.m_dwVtxCntX)
	, m_dwVtxCntZ(rhs.m_dwVtxCntZ)
{

}


_float CBuffer_Terrain::Get_Height(CTransform * pTransform)
{
	_v3		vPosition = pTransform->Get_Pos();

	_uint	iIndex = _uint(vPosition.z / m_dwInterval) * m_dwVtxCntX + _uint(vPosition.x / m_dwInterval);

	_uint	iIndices[4] = { iIndex + m_dwVtxCntX, iIndex + m_dwVtxCntX + 1, iIndex + 1, iIndex };

	_float fRatioX = (vPosition.x - m_pPosition[iIndices[0]].x) / m_dwInterval;
	_float fRatioZ = (m_pPosition[iIndices[0]].z - vPosition.z) / m_dwInterval;

	if (fRatioX > fRatioZ)
	{
		return m_pPosition[iIndices[0]].y + (m_pPosition[iIndices[1]].y - m_pPosition[iIndices[0]].y) * fRatioX
			+ (m_pPosition[iIndices[2]].y - m_pPosition[iIndices[1]].y) * fRatioZ;
	}

	else
	{
		return m_pPosition[iIndices[0]].y + (m_pPosition[iIndices[3]].y - m_pPosition[iIndices[0]].y) * fRatioZ
			+ (m_pPosition[iIndices[2]].y - m_pPosition[iIndices[3]].y) * fRatioX;
	}
}

HRESULT Engine::CBuffer_Terrain::Ready_Component_Prototype(_ulong dwCntX, _ulong dwCntZ, _ulong dwVtxItv)
{
	m_dwVtxCntX = dwCntX;
	m_dwVtxCntZ = dwCntZ;

	m_iNumPolygons = (dwCntX - 1) * (dwCntZ - 1) * 2;
	m_iNumVertices = dwCntX * dwCntZ;

	m_pPosition = new _v3[m_iNumVertices];

	m_dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	m_iStride = sizeof(VTXNORTEX);

	m_iPolygonSize = sizeof(POLYGON32);
	m_eFormat = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Component_Prototype()))
		return E_FAIL;

	VTXNORTEX*		pVtxTex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVtxTex, 0);

	_ulong		dwIndex = 0;

	for (_ulong i = 0; i < dwCntZ; ++i)
	{
		for (_ulong j = 0; j < dwCntX; ++j)
		{
			dwIndex = i * dwCntX + j;

			pVtxTex[dwIndex].vPosition = _v3(_float(j * dwVtxItv), 0.0f, _float(i * dwVtxItv));

			m_pPosition[dwIndex] = pVtxTex[dwIndex].vPosition;

			pVtxTex[dwIndex].vTexUV = _v2(_float(j) / (dwCntX - 1),
				_float(i) / (dwCntZ - 1));
		}
	}



	POLYGON32*		pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	_ulong	dwTriIdx = 0;

	for (_ulong i = 0; i < dwCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < dwCntX - 1; ++j)
		{
			dwIndex = i * dwCntX + j;

			pIndex[dwTriIdx]._0 = dwIndex + dwCntX;
			pIndex[dwTriIdx]._1 = dwIndex + dwCntX + 1;
			pIndex[dwTriIdx]._2 = dwIndex + 1;
			++dwTriIdx;

			pIndex[dwTriIdx]._0 = dwIndex + dwCntX;
			pIndex[dwTriIdx]._1 = dwIndex + 1;
			pIndex[dwTriIdx]._2 = dwIndex;
			++dwTriIdx;
		}
	}

	m_pVB->Unlock();
	m_pIB->Unlock();

	m_dwInterval = dwVtxItv;

	return S_OK;
}

HRESULT CBuffer_Terrain::Ready_Component_Prototype(const _tchar * pFilePath, _ulong dwVtxItv)
{
	_ulong	dwByte = 0;

	m_dwVtxCntX = 128;
	m_dwVtxCntZ = 128;

	m_iNumPolygons = (m_dwVtxCntX - 1) * (m_dwVtxCntZ - 1) * 2;
	m_iNumVertices = m_dwVtxCntX * m_dwVtxCntZ;

	m_pPosition = new _v3[m_iNumVertices];

	m_dwFVF = VTXFVF_COL;
	m_iStride = sizeof(VTX_COL);

	m_iPolygonSize = sizeof(POLYGON32);
	m_eFormat = D3DFMT_INDEX32;

	if (FAILED(CVIBuffer::Ready_Component_Prototype()))
		return E_FAIL;

	VTX_COL*		pVtxTex = NULL;

	m_pVB->Lock(0, 0, (void**)&pVtxTex, 0);

	_ulong		dwIndex = 0;

	for (_ulong i = 0; i < m_dwVtxCntZ; ++i)
	{
		for (_ulong j = 0; j < m_dwVtxCntX; ++j)
		{
			dwIndex = i * m_dwVtxCntX + j;

			pVtxTex[dwIndex].vPos = { _float(j * dwVtxItv) - (m_dwVtxCntX / 2)
				, 0.f,
				_float(i * dwVtxItv) - (m_dwVtxCntX / 2) };
			pVtxTex[dwIndex].dwColor = D3DCOLOR_ARGB(255, 255, 255, 255);
		}
	}



	POLYGON32*		pIndex = NULL;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	_ulong	dwTriIdx = 0;

	_v3	vNormal, vSour, vDest;


	for (_ulong i = 0; i < m_dwVtxCntZ - 1; ++i)
	{
		for (_ulong j = 0; j < m_dwVtxCntX - 1; ++j)
		{
			dwIndex = i * m_dwVtxCntX + j;

			pIndex[dwTriIdx]._0 = dwIndex + m_dwVtxCntX;
			pIndex[dwTriIdx]._1 = dwIndex + m_dwVtxCntX + 1;
			pIndex[dwTriIdx]._2 = dwIndex + 1;
			++dwTriIdx;

			pIndex[dwTriIdx]._0 = dwIndex + m_dwVtxCntX;
			pIndex[dwTriIdx]._1 = dwIndex + 1;
			pIndex[dwTriIdx]._2 = dwIndex;
			++dwTriIdx;
		}
	}

	m_pVB->Unlock();
	m_pIB->Unlock();

	m_dwInterval = dwVtxItv;

	return S_OK;
}



HRESULT CBuffer_Terrain::Ready_Component(void * pArg)
{
	return S_OK;
}

void CBuffer_Terrain::Render_VIBuffer()
{
	m_pGraphic_Dev->SetStreamSource(0, m_pVB, 0, m_iStride);
	m_pGraphic_Dev->SetFVF(m_dwFVF);
	m_pGraphic_Dev->SetIndices(m_pIB);
	m_pGraphic_Dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertices, 0, m_iNumPolygons);
}

CBuffer_Terrain* Engine::CBuffer_Terrain::Create(LPDIRECT3DDEVICE9 pGraphicDev,
	_ulong dwCntX,
	_ulong dwCntZ,
	_ulong dwVtxItv)
{
	CBuffer_Terrain*		pInstance = new CBuffer_Terrain(pGraphicDev);

	if (FAILED(pInstance->Ready_Component_Prototype(dwCntX, dwCntZ, dwVtxItv)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

CBuffer_Terrain * CBuffer_Terrain::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pFilePath, _ulong dwVtxItv)
{
	CBuffer_Terrain*		pInstance = new CBuffer_Terrain(pGraphicDev);

	if (FAILED(pInstance->Ready_Component_Prototype(pFilePath, dwVtxItv)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}


Engine::CComponent* Engine::CBuffer_Terrain::Clone_Component(void* pArg)
{
	CBuffer_Terrain*	pInstance = new CBuffer_Terrain(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}


void Engine::CBuffer_Terrain::Free(void)
{
	if (false == m_bisClone)
		Safe_Delete_Array(m_pPosition);

	CVIBuffer::Free();
}
