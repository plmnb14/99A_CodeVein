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
	vector<D3DXMESHCONTAINER_DERIVED*> &Get_MeshContainer() { return m_MeshContainerList; }
	_uint Get_NumMeshContainer() const {
		return (_uint)m_MeshContainerList.size();
	}
	_uint Get_NumMaterials(_uint iMeshConstainerIdx) {
		return (_uint)m_MeshContainerList[iMeshConstainerIdx]->NumMaterials;
	}
	_mat Get_FrameMatrix(const char* pFrameName);
	LPDIRECT3DTEXTURE9 Get_MeshTexture(_uint iMeshContainerIndex, _uint iSubSetIndex, MESHTEXTURE::TYPE eType);

	_ulong Get_MaterialPass(_ulong iMeshConstainerIdx , _ulong dwSubsetIdx);

public:
	virtual HRESULT Ready_Component_Prototype(const _tchar* pFilePath, const _tchar* pFileName, _mat PivotMatrix);
	virtual HRESULT Ready_Component(void* pArg);
public:
	HRESULT Update_CombinedTransformationMatrices(D3DXFRAME* pFrame, _mat ParentMatrix); // 뼈의 컴바인트 행렬을 갱신한다.
	HRESULT SetUp_CombinedTransformationMatricesPointer(D3DXFRAME* pFrame , _short _sSeperatePart); // 메시컨테이너에 있는 ppCombinedTransformationMAtrices에 주소를 채워주기위한 함수.
	HRESULT SetTexture_OnShader(LPD3DXEFFECT pEffect, D3DXHANDLE pConstantName, _uint iMeshContainerIdx, _uint iAttributeID, MESHTEXTURE::TYPE eType);
	HRESULT SetMaterial_OnShader(LPD3DXEFFECT pEffect, D3DXHANDLE pConstantName, _uint iMeshContainerIdx, _uint iAttributeID);
	HRESULT Update_SkinnedMesh(_int iMeshContainerIdx);
	HRESULT Render_Mesh(_uint iMeshContainerIdx, _uint iAttributeID);

public:
	void Reset_OldIndx(_short _sAniCtrlNum = 0);

public:
	LPD3DXFRAME		Get_BonInfo(LPCSTR _bBoneName, _short _sCTRL_Type = 0);
	D3DXTRACK_DESC	Get_TrackInfo();
	D3DXTRACK_DESC	Get_TrackInfo_Upper();
	CAniCtrl*		Get_AniCtrl();
	_double			Get_AnimationFullTime();

public:
	void	Set_BoneSeperate(D3DXFRAME_DERIVED* _frame, const char* _bodyName , _short _sSeperateNum);

public:
	HRESULT SetUp_Animation(_uint iIndex, _bool _bOffLerp = false);
	HRESULT SetUp_Animation_Lower(_uint iIndex, _bool _bOffLerp = false);
	HRESULT SetUp_Animation_Upper(_uint iIndex, _bool _bOffLerp = false);
	HRESULT SetUp_Animation_RightArm(_uint iIndex, _bool _bOffLerp = false);
	HRESULT SetUp_Animation_LeftArm(_uint iIndex, _bool _bOffLerp = false);


	HRESULT Play_Animation_Lower(_double TimeDelta);
	HRESULT Play_Animation_Upper(_double TimeDelta);
	HRESULT	Play_Animation_RightArm(_double TimeDelta, _bool _bTwoHanded = false);
	HRESULT	Play_Animation_LeftArm(_double TimeDelta, _bool _bTwoHanded = false);
	HRESULT Play_Animation(_double TimeDelta); // 애니메이션을 재생한다.

	_bool Is_Finish_Animation(_float _fLerpValue = 1.f);
	_bool Is_Finish_Animation_Lower(_float _fLerpValue = 1.f);
	_bool Is_Finish_Animation_Upper(_float _fLerpValue = 1.f);
	_bool Is_Finish_Animation_LeftArm(_float _fLerpValue = 1.f);
	_bool Is_Finish_Animation_RightArm(_float _fLerpValue = 1.f);


private:
	D3DXFRAME*			m_pRootFrame = nullptr;
	D3DXFRAME*			m_pUpperFrame = nullptr;
	D3DXFRAME*			m_pRightArmFrame = nullptr;
	D3DXFRAME*			m_pLeftArmFrame = nullptr;

private:
	_mat				m_matPivot;
	CHierarchy*			m_pHierarchy = nullptr;

private:
	CAniCtrl*			m_pAniCtrl_Upper = nullptr;
	CAniCtrl*			m_pAniCtrl_Lower = nullptr;
	CAniCtrl*			m_pAniCtrl_RightArm = nullptr;
	CAniCtrl*			m_pAniCtrl_LeftArm = nullptr;

private:
	_bool				m_bIsSeperate = false;

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