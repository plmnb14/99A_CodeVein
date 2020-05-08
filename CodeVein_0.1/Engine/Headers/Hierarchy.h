#pragma once

#include "Base.h"

BEGIN(Engine)

class CHierarchy final : public ID3DXAllocateHierarchy, public CBase
{
private:
	explicit CHierarchy(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CHierarchy() = default;
public:
	HRESULT Ready_HierarchyLoader(const _tchar* pFilePath);
public:
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
	STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR Name, CONST D3DXMESHDATA *pMeshData, CONST D3DXMATERIAL *pMaterials, CONST D3DXEFFECTINSTANCE *pEffectInstances, DWORD NumMaterials, CONST DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER *ppNewMeshContainer);

	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree);
public:
	_ulong Get_MaterialPass(_ulong dwPass) { return m_dwMaterialPass[dwPass]; }

private:
	LPDIRECT3DDEVICE9	m_pGraphic_Device = nullptr;
private:
	const _tchar*		m_pFilePath = nullptr;
private:
	HRESULT SetUp_Name(char** ppSourName, const char* pDestName);
	HRESULT Change_TextureFileName(_tchar* pFilePath, _tchar * pSourMark, _tchar * pDestMark);
	HRESULT Change_TextureFileExtension(_tchar* pFilePath);

public:
	static CHierarchy* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath);
	virtual void Free();

	_ulong		m_dwMaterialPass[STR_32] = {};

private:
	_bool		m_bIncludeMap[MESHTEXTURE::TYPE_END] = {};

};

END