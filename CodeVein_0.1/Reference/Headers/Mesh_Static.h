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
	LPDIRECT3DBASETEXTURE9 Get_Texture(_ulong dwMaterialIndex, MESHTEXTURE::TYPE eType);
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
	_mat					m_matPivot;

	_ulong					m_dwVtxCnt;
	_ulong					m_dwStride;

	_v3						m_pVtxMin;
	_v3						m_pVtxMax;

private:
	_bool					m_bNoDiffuseMap = false;		// 디퓨즈가 없어요
	_bool					m_bNoNormalMap = false;			// 노말맵이 없어요
	_bool					m_bNoSpecularMap = false;		// 스펙큘러맵이 없어요
	_bool					m_bNoIDMap = false;				// ID 맵이 없어요
	_bool					m_bNoUnionMap = false;			// 유니온 맵이 없어요
	_bool					m_bNoEmissiveMap = false;		// 이미시브 맵이 없어요
	_bool					m_bNoRoughnessMap = false;		// 러프니스맵이 없어요
	_bool					m_bNoTransparencyMap = false;	// 반투명맵이 없어요


private:
	HRESULT Change_TextureFileName(_tchar* pFilePath, _tchar * pSourMark, _tchar * pDestMark);
public:
	static CMesh_Static* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath, const _tchar* pFileName, _mat PivotMatrix);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();
};

END