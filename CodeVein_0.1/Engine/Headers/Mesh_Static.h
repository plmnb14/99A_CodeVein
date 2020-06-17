#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CMesh_Static final : public CComponent
{
private:
	explicit CMesh_Static(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMesh_Static(const CMesh_Static& rhs);
	virtual ~CMesh_Static() = default;

public:
	const _v3		Get_VtxMin() const { return m_pVtxMin; }
	const _v3		Get_VtxMax() const { return m_pVtxMax; }
	_ulong			Get_VtxCnt() const { return m_dwVtxCnt; }
	_ulong			Get_Stride() const { return m_dwStride; }

public:
	_ulong Get_NumMaterials() const { return m_dwNumMaterials; }
	_ushort Get_MaterialPass(_ulong dwPass) { return m_sMaterialPass[dwPass]; }
	LPDIRECT3DBASETEXTURE9 Get_Texture(_ulong dwMaterialIndex, MESHTEXTURE::TYPE eType);

	LPD3DXMESH	Get_pMesh() { return m_pMesh; }
	_float Get_FrustumRadius() { return m_fHalfFristumRadius; }

public:
	virtual HRESULT Ready_Component_Prototype(const _tchar* pFilePath, const _tchar* pFileName, _mat PivotMatrix);
	virtual HRESULT Ready_Component(void* pArg);
public:
	HRESULT SetTexture_OnShader(LPD3DXEFFECT pEffect, D3DXHANDLE pConstantName, _uint iAttributeID, MESHTEXTURE::TYPE eType);
	HRESULT SetMaterial_OnShader(LPD3DXEFFECT pEffect, D3DXHANDLE pConstantName, _uint iAttributeID);
	HRESULT Render_Mesh(_uint iAttributeID);
private:
	LPD3DXMESH				m_pMesh = nullptr;
	LPD3DXBUFFER			m_pAdjacencyBuff = nullptr;
	LPD3DXBUFFER			m_pMaterialsBuff = nullptr;
	D3DXMATERIAL*			m_pMaterials = nullptr;
	MESHTEXTURE*			m_ppTextures = nullptr;
	_ulong					m_dwNumMaterials = 0;
	_mat					m_matPivot = {};

	_ulong					m_dwVtxCnt = 0;
	_ulong					m_dwStride = 0;

	_float					m_fHalfFristumRadius = 0.f;

	_v3						m_pVtxMin = {};
	_v3						m_pVtxMax = {};

	_ushort					m_sMaterialPass[STR_128] = {};

private:
	_bool					m_bIncludeMap[MESHTEXTURE::TYPE_END] = {};

private:
	HRESULT Change_TextureFileName(_tchar* pFilePath, _tchar * pSourMark, _tchar * pDestMark);
	HRESULT Change_TextureFileExtension(_tchar* pFilePath);

public:
	static CMesh_Static* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath, const _tchar* pFileName, _mat PivotMatrix);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
};

END