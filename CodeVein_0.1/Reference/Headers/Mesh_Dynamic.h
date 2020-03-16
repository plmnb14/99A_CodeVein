#pragma once

#include "Component.h"

BEGIN(Engine)

class CAniCtrl;
class CHierarchy;
class ENGINE_DLL CMesh_Dynamic final : public CComponent
{
private:
	explicit CMesh_Dynamic(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMesh_Dynamic(const CMesh_Dynamic& rhs);
	virtual  ~CMesh_Dynamic() = default;
public:
	_uint Get_NumMeshContainer() const {
		return (_uint)m_MeshContainerList.size(); }
	_uint Get_NumMaterials(_uint iMeshConstainerIdx) {
		return (_uint)m_MeshContainerList[iMeshConstainerIdx]->NumMaterials; }
	_mat Get_FrameMatrix(const char* pFrameName);
	LPDIRECT3DTEXTURE9 Get_MeshTexture(_uint iMeshContainerIndex, _uint iSubSetIndex, MESHTEXTURE::TYPE eType);
	
public:
	virtual HRESULT Ready_Component_Prototype(const _tchar* pFilePath, const _tchar* pFileName, _mat PivotMatrix);
	virtual HRESULT Ready_Component(void* pArg);
public:
	HRESULT Update_CombinedTransformationMatrices(D3DXFRAME* pFrame, _mat ParentMatrix); // 뼈의 컴바인트 행렬을 갱신한다.
	HRESULT SetUp_CombinedTransformationMatricesPointer(D3DXFRAME* pFrame); // 메시컨테이너에 있는 ppCombinedTransformationMAtrices에 주소를 채워주기위한 함수.
	HRESULT SetTexture_OnShader(LPD3DXEFFECT pEffect, D3DXHANDLE pConstantName, _uint iMeshContainerIdx, _uint iAttributeID, MESHTEXTURE::TYPE eType);
	HRESULT SetMaterial_OnShader(LPD3DXEFFECT pEffect, D3DXHANDLE pConstantName, _uint iMeshContainerIdx, _uint iAttributeID);
	HRESULT Update_SkinnedMesh(_int iMeshContainerIdx);
	HRESULT Render_Mesh(_uint iMeshContainerIdx, _uint iAttributeID);
public:
	HRESULT SetUp_Animation(_uint iIndex); // 특정 애니메이션을 재생할 수 있도록 준비시킨다.
	HRESULT Play_Animation(_double TimeDelta); // 애니메이션을 재생한다.

	_bool Is_Finish_Animation(_float _fLerpValue = 1.f);
private:
	D3DXFRAME*			m_pRootFrame = nullptr;
	_mat				m_matPivot;
	CHierarchy*			m_pHierarchy = nullptr;
private:
	CAniCtrl*			m_pAniCtrl = nullptr;

private:
	vector<LPD3DXMESHCONTAINER_DERIVED>			m_MeshContainerList;
	typedef vector<LPD3DXMESHCONTAINER_DERIVED>	MESHCONTAINERLIST;
private:
	// 해시테이블. (공간을 팔아 시간을 산다.)
	unordered_map<const char*, const _mat*>			m_ReferenceBones;
	typedef unordered_map<const char*, const _mat*>	REFERENCEBONE;
private:
	const _mat* Find_CombinedTransformationMatrix(const char* pFrameName);
public:
	static CMesh_Dynamic* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath, const _tchar* pFileName, _mat PivotMatrix);
	virtual CComponent* Clone_Component(void* pArg);
	virtual void Free();

};

END