#ifndef TerrainTex_h__
#define TerrainTex_h__

#include "VIBuffer.h"

BEGIN(Engine)

class CTransform;
class ENGINE_DLL CBuffer_Terrain final : public CVIBuffer
{
private:
	explicit CBuffer_Terrain(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBuffer_Terrain(const CBuffer_Terrain& rhs);
	virtual ~CBuffer_Terrain(void) = default;

public:
	const _v3* Get_VtxPos(void) const { return m_pPosition; }
	_float Get_Height(CTransform* pTransform);
public:
	HRESULT	Ready_Component_Prototype(_ulong dwCntX, _ulong dwCntZ, _ulong dwVtxItv);
	HRESULT	Ready_Component_Prototype(const _tchar* pFilePath, _ulong dwVtxItv);
	virtual HRESULT Ready_Component(void* pArg);
	void Render_VIBuffer();
private:
	HANDLE				m_hFile;
	BITMAPFILEHEADER	m_fh;
	BITMAPINFOHEADER	m_ih;
	_v3*				m_pPosition = nullptr;
	_ulong				m_dwInterval = 0;
	_ulong				m_dwVtxCntX = 0;
	_ulong				m_dwVtxCntZ = 0;

public:
	static CBuffer_Terrain* Create(LPDIRECT3DDEVICE9 pGraphicDev, _ulong dwCntX, _ulong dwCntZ, _ulong dwVtxItv);
	static CBuffer_Terrain* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pFilePath, _ulong dwVtxItv);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free(void);
};

END
#endif // TerrainTex_h__
