#include "..\Headers\Mesh_Dynamic.h"
#include "Hierarchy.h"
#include "AniCtrl.h"


CMesh_Dynamic::CMesh_Dynamic(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{

}

CMesh_Dynamic::CMesh_Dynamic(const CMesh_Dynamic & rhs)
	: CComponent(rhs)
	, m_MeshContainerList(rhs.m_MeshContainerList)
	, m_pRootFrame(rhs.m_pRootFrame)
	, m_matPivot(rhs.m_matPivot)
	, m_pHierarchy(rhs.m_pHierarchy)
	, m_pAniCtrl_Lower(rhs.m_pAniCtrl_Lower->Clone_AniCtrl())
	, m_bIsSeperate(rhs.m_bIsSeperate)
{
	Safe_AddRef(m_pHierarchy);

	if (m_bIsSeperate)
	{
		m_pUpperFrame = rhs.m_pUpperFrame;
		m_pRightArmFrame = rhs.m_pRightArmFrame;
		m_pLeftArmFrame = rhs.m_pLeftArmFrame;

		m_pAniCtrl_Upper = (rhs.m_pAniCtrl_Upper->Clone_AniCtrl());
		m_pAniCtrl_RightArm = (rhs.m_pAniCtrl_RightArm->Clone_AniCtrl());
		m_pAniCtrl_LeftArm = (rhs.m_pAniCtrl_LeftArm->Clone_AniCtrl());
	}
}

_mat CMesh_Dynamic::Get_FrameMatrix(const char * pFrameName)
{
	_mat	matTmp;

	auto	iter = find_if(m_ReferenceBones.begin(), m_ReferenceBones.end(), [&](REFERENCEBONE::value_type Pair) { return !strcmp(pFrameName, Pair.first); });
	if (iter == m_ReferenceBones.end())
		return *D3DXMatrixIdentity(&matTmp);

	return _mat(*iter->second);
}

LPDIRECT3DTEXTURE9 CMesh_Dynamic::Get_MeshTexture(_uint iMeshContainerIndex, _uint iSubSetIndex, MESHTEXTURE::TYPE eType)
{
	return m_MeshContainerList[iMeshContainerIndex]->pMeshTexture[iSubSetIndex].pTextures[eType];
}

_ulong CMesh_Dynamic::Get_MaterialPass(_ulong iMeshConstainerIdx , _ulong dwSubsetIdx)
{
	return (_uint)m_MeshContainerList[iMeshConstainerIdx]->pMeshTexture[dwSubsetIdx].m_dwMaterialPass;
}

HRESULT CMesh_Dynamic::Ready_Component_Prototype(const _tchar * pFilePath, const _tchar * pFileName, _mat PivotMatrix)
{
	_tchar		szFullPath[STR_256] = L"";

	LPD3DXMESH		pMesh = nullptr;

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	m_pHierarchy = CHierarchy::Create(m_pGraphic_Dev, pFilePath);
	if (nullptr == m_pHierarchy)
		return E_FAIL;

	LPD3DXANIMATIONCONTROLLER		pAniCtrl = nullptr;

	if (FAILED(D3DXLoadMeshHierarchyFromX(szFullPath, D3DXMESH_MANAGED, m_pGraphic_Dev, m_pHierarchy, nullptr, &m_pRootFrame, &pAniCtrl)))
		return E_FAIL;

	m_pAniCtrl_Lower = CAniCtrl::Create(m_pGraphic_Dev, pAniCtrl);

	if (nullptr == m_pAniCtrl_Lower)
		return E_FAIL;

	m_matPivot = PivotMatrix;

	Update_CombinedTransformationMatrices(m_pRootFrame, m_matPivot);

	SetUp_CombinedTransformationMatricesPointer(m_pRootFrame, 0);

	if (!lstrcmp(pFileName, L"BodyInner_01.X") || 
		!lstrcmp(pFileName, L"BodyInner_02.X") || 
		!lstrcmp(pFileName, L"BodyInner_03.X") || 
		!lstrcmp(pFileName, L"BodyInner_04.X") ||
		!lstrcmp(pFileName, L"Drape_01.X") ||
		!lstrcmp(pFileName, L"Gauntlet_01.X") || 
		!lstrcmp(pFileName, L"Gauntlet_03.X") || 
		!lstrcmp(pFileName, L"LongCoat_01.X") || 
		!lstrcmp(pFileName, L"LongCoat_02.X") || 
		!lstrcmp(pFileName, L"LongCoat_03.X") ||
		!lstrcmp(pFileName, L"Muffler_01.X") || 
		!lstrcmp(pFileName, L"Muffler_02.X"))
	{
		m_bIsSeperate = true;

		m_pAniCtrl_Upper = CAniCtrl::Create(m_pGraphic_Dev, pAniCtrl);

		Set_BoneSeperate((D3DXFRAME_DERIVED*)m_pRootFrame, "Hips", 0);


		_mat		matTemp2;

		Update_CombinedTransformationMatrices((D3DXFRAME_DERIVED*)m_pUpperFrame, *D3DXMatrixIdentity(&matTemp2));

		SetUp_CombinedTransformationMatricesPointer((D3DXFRAME_DERIVED*)m_pUpperFrame, 1);

		//////////////////////

		m_pAniCtrl_RightArm = CAniCtrl::Create(m_pGraphic_Dev, pAniCtrl);

		Set_BoneSeperate((D3DXFRAME_DERIVED*)m_pUpperFrame, "LeftShoulder", 1);

		Update_CombinedTransformationMatrices((D3DXFRAME_DERIVED*)m_pRightArmFrame, *D3DXMatrixIdentity(&matTemp2));

		SetUp_CombinedTransformationMatricesPointer((D3DXFRAME_DERIVED*)m_pRightArmFrame, 2);


		m_pAniCtrl_LeftArm = CAniCtrl::Create(m_pGraphic_Dev, pAniCtrl);

		Set_BoneSeperate((D3DXFRAME_DERIVED*)m_pUpperFrame, "Neck", 2);

		Update_CombinedTransformationMatrices((D3DXFRAME_DERIVED*)m_pLeftArmFrame, *D3DXMatrixIdentity(&matTemp2));

		SetUp_CombinedTransformationMatricesPointer((D3DXFRAME_DERIVED*)m_pLeftArmFrame, 3);
	}

	Safe_Release(pAniCtrl);

	return NOERROR;
}

HRESULT CMesh_Dynamic::Ready_Component(void * pArg)
{
	list<const char*>* pFrameList = (list<const char*>*)pArg;

	if (nullptr == pFrameList)
		return NOERROR;

	for (auto& pName : *pFrameList)
		m_ReferenceBones.insert(REFERENCEBONE::value_type(pName, Find_CombinedTransformationMatrix(pName)));

	return NOERROR;
}

HRESULT CMesh_Dynamic::Update_CombinedTransformationMatrices(D3DXFRAME* pFrame, _mat ParentMatrix)
{
	LPD3DXFRAME_DERIVED	pFrame_Derived = (LPD3DXFRAME_DERIVED)pFrame;

	pFrame_Derived->CombinedTransformationMatrix = pFrame_Derived->TransformationMatrix * ParentMatrix;

	if (nullptr != pFrame_Derived->pFrameFirstChild)
		Update_CombinedTransformationMatrices(pFrame_Derived->pFrameFirstChild, pFrame_Derived->CombinedTransformationMatrix);

	if (nullptr != pFrame_Derived->pFrameSibling)
		Update_CombinedTransformationMatrices(pFrame_Derived->pFrameSibling, ParentMatrix);

	return NOERROR;
}

HRESULT CMesh_Dynamic::SetUp_CombinedTransformationMatricesPointer(D3DXFRAME* pFrame, _short _sSeperatePart)
{
	if (nullptr != pFrame->pMeshContainer)
	{
		D3DXMESHCONTAINER_DERIVED* pMeshContainer_Derived = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;

		for (_ulong i = 0; i < pMeshContainer_Derived->dwNumBones; ++i)
		{
			if (0 == _sSeperatePart)
			{
				D3DXFRAME_DERIVED*	pFrame_Derived = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pMeshContainer_Derived->pSkinInfo->GetBoneName(i));
				pMeshContainer_Derived->ppCombinedTransformationMatrices[i] = &pFrame_Derived->CombinedTransformationMatrix;
			}

			else if (1 == _sSeperatePart)
			{
				D3DXFRAME_DERIVED*	pFrame_Derived = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pUpperFrame, pMeshContainer_Derived->pSkinInfo->GetBoneName(i));
				pMeshContainer_Derived->ppCombinedTransformationMatrices[i] = &pFrame_Derived->CombinedTransformationMatrix;
			}

			else if (2 == _sSeperatePart)
			{
				D3DXFRAME_DERIVED*	pFrame_Derived = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRightArmFrame, pMeshContainer_Derived->pSkinInfo->GetBoneName(i));
				pMeshContainer_Derived->ppCombinedTransformationMatrices[i] = &pFrame_Derived->CombinedTransformationMatrix;
			}

			else if (3 == _sSeperatePart)
			{
				D3DXFRAME_DERIVED*	pFrame_Derived = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pLeftArmFrame, pMeshContainer_Derived->pSkinInfo->GetBoneName(i));
				pMeshContainer_Derived->ppCombinedTransformationMatrices[i] = &pFrame_Derived->CombinedTransformationMatrix;
			}
		}

		m_MeshContainerList.push_back(pMeshContainer_Derived);
	}

	if (nullptr != pFrame->pFrameFirstChild)
		SetUp_CombinedTransformationMatricesPointer(pFrame->pFrameFirstChild, _sSeperatePart);

	if (nullptr != pFrame->pFrameSibling)
		SetUp_CombinedTransformationMatricesPointer(pFrame->pFrameSibling, _sSeperatePart);

	return NOERROR;
}

HRESULT CMesh_Dynamic::SetTexture_OnShader(LPD3DXEFFECT pEffect, D3DXHANDLE pConstantName, _uint iMeshContainerIdx, _uint iAttributeID, MESHTEXTURE::TYPE eType)
{
	if (nullptr == m_MeshContainerList[iMeshContainerIdx]->pMeshTexture ||
		m_MeshContainerList[iMeshContainerIdx]->NumMaterials <= iAttributeID)
		return E_FAIL;

	pEffect->SetTexture(pConstantName, m_MeshContainerList[iMeshContainerIdx]->pMeshTexture[iAttributeID].pTextures[eType]);

	return NOERROR;
}

HRESULT CMesh_Dynamic::SetMaterial_OnShader(LPD3DXEFFECT pEffect, D3DXHANDLE pConstantName, _uint iMeshContainerIdx, _uint iAttributeID)
{
	if (m_MeshContainerList[iMeshContainerIdx]->NumMaterials <= iAttributeID)
		return E_FAIL;

	m_MeshContainerList[iMeshContainerIdx]->pMaterials[iAttributeID].MatD3D.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	pEffect->SetValue(pConstantName, &m_MeshContainerList[iMeshContainerIdx]->pMaterials[iAttributeID].MatD3D, sizeof(D3DMATERIAL9));

	return NOERROR;
}

HRESULT CMesh_Dynamic::Update_SkinnedMesh(_int iMeshContainerIdx)
{
	for (_ulong i = 0; i < m_MeshContainerList[iMeshContainerIdx]->dwNumBones; ++i)
		m_MeshContainerList[iMeshContainerIdx]->pRenderMatrices[i] = m_MeshContainerList[iMeshContainerIdx]->pOffsetMatrices[i] * *m_MeshContainerList[iMeshContainerIdx]->ppCombinedTransformationMatrices[i];

	void	*pSourVtx = nullptr, *pDestVtx = nullptr;

	m_MeshContainerList[iMeshContainerIdx]->pOriginalMesh->LockVertexBuffer(0, &pSourVtx);
	m_MeshContainerList[iMeshContainerIdx]->MeshData.pMesh->LockVertexBuffer(0, &pDestVtx);

	m_MeshContainerList[iMeshContainerIdx]->pSkinInfo->UpdateSkinnedMesh(
		m_MeshContainerList[iMeshContainerIdx]->pRenderMatrices,
		nullptr,
		pSourVtx,
		pDestVtx);

	m_MeshContainerList[iMeshContainerIdx]->pOriginalMesh->UnlockVertexBuffer();
	m_MeshContainerList[iMeshContainerIdx]->MeshData.pMesh->UnlockVertexBuffer();

	return NOERROR;
}

HRESULT CMesh_Dynamic::Render_Mesh(_uint iMeshContainerIdx, _uint iAttributeID)
{
	m_MeshContainerList[iMeshContainerIdx]->MeshData.pMesh->DrawSubset(iAttributeID);

	return NOERROR;
}

void CMesh_Dynamic::Reset_OldIndx(_short _sAniCtrlNum)
{
	m_pAniCtrl_Lower->Reset_OldIdx();

	if (_sAniCtrlNum != 0)
	{
		m_pAniCtrl_Upper->Reset_OldIdx();
		m_pAniCtrl_RightArm->Reset_OldIdx();
		m_pAniCtrl_LeftArm->Reset_OldIdx();
	}
}

LPD3DXFRAME CMesh_Dynamic::Get_BonInfo(LPCSTR _bBoneName, _short _sCTRL_Type)
{
	// 현재 분리된 "상체" 의 정보만을 가져옴.
	return (_sCTRL_Type == 0 ? D3DXFrameFind(m_pRootFrame, _bBoneName) :
		_sCTRL_Type == 1 ? D3DXFrameFind(m_pUpperFrame, _bBoneName) :
		_sCTRL_Type == 2 ? D3DXFrameFind(m_pRightArmFrame, _bBoneName) : D3DXFrameFind(m_pLeftArmFrame, _bBoneName));

}

D3DXTRACK_DESC CMesh_Dynamic::Get_TrackInfo()
{
	return m_pAniCtrl_Lower->Get_TrackInfo();
}

D3DXTRACK_DESC CMesh_Dynamic::Get_TrackInfo_Upper()
{
	return m_pAniCtrl_Upper->Get_TrackInfo();
}

CAniCtrl * CMesh_Dynamic::Get_AniCtrl()
{
	return m_pAniCtrl_Lower;
}

_double CMesh_Dynamic::Get_AnimationFullTime()
{
	return m_pAniCtrl_Lower->Get_AnimationFullTime();
}

void CMesh_Dynamic::Set_BoneSeperate(D3DXFRAME_DERIVED * _frame, const char * _bodyName, _short _sSeperateNum)
{
	if (_frame->Name != nullptr)
	{
		if (!strcmp(_frame->Name, _bodyName))
		{
			if (0 == _sSeperateNum)
			{
				m_pUpperFrame = _frame->pFrameFirstChild;	// Spine
				_frame->pFrameFirstChild = m_pUpperFrame->pFrameSibling;			// 첫번째 자식 사라지고 이웃으로 채워줌
				m_pUpperFrame->pFrameSibling = NULL;
			}

			else if (1 == _sSeperateNum)
			{
				m_pRightArmFrame = _frame->pFrameSibling;
				_frame->pFrameSibling = m_pRightArmFrame->pFrameSibling;
				m_pRightArmFrame->pFrameSibling = NULL;
			}

			else if (2 == _sSeperateNum)
			{
				m_pLeftArmFrame = _frame->pFrameSibling;
				_frame->pFrameSibling = m_pLeftArmFrame->pFrameSibling;
				m_pLeftArmFrame->pFrameSibling = NULL;
			}
		}
	}

	if (_frame->pFrameSibling)
		Set_BoneSeperate((D3DXFRAME_DERIVED*)_frame->pFrameSibling, _bodyName, _sSeperateNum);

	if (_frame->pFrameFirstChild)
		Set_BoneSeperate((D3DXFRAME_DERIVED*)_frame->pFrameFirstChild, _bodyName, _sSeperateNum);
}

HRESULT CMesh_Dynamic::SetUp_Animation(_uint iIndex, _bool _bOffLerp)
{
	if (nullptr == m_pAniCtrl_Lower)
		return E_FAIL;

	m_pAniCtrl_Lower->SetUp_Animation(iIndex, _bOffLerp);

	return NOERROR;
}

HRESULT CMesh_Dynamic::SetUp_Animation_Lower(_uint iIndex, _bool _bOffLerp)
{
	if (nullptr == m_pAniCtrl_Lower)
		return E_FAIL;

	m_pAniCtrl_Lower->SetUp_Animation(iIndex, _bOffLerp);

	return S_OK;
}

HRESULT CMesh_Dynamic::SetUp_Animation_Upper(_uint iIndex, _bool _bOffLerp)
{
	if (nullptr == m_pAniCtrl_Upper)
		return E_FAIL;

	m_pAniCtrl_Upper->SetUp_Animation(iIndex, _bOffLerp);

	return S_OK;
}

HRESULT CMesh_Dynamic::SetUp_Animation_RightArm(_uint iIndex, _bool _bOffLerp)
{
	if (nullptr == m_pAniCtrl_RightArm)
		return E_FAIL;

	m_pAniCtrl_RightArm->SetUp_Animation(iIndex, _bOffLerp);

	return S_OK;
}

HRESULT CMesh_Dynamic::SetUp_Animation_LeftArm(_uint iIndex, _bool _bOffLerp)
{
	if (nullptr == m_pAniCtrl_LeftArm)
		return E_FAIL;

	m_pAniCtrl_LeftArm->SetUp_Animation(iIndex , _bOffLerp);

	return S_OK;
}

HRESULT CMesh_Dynamic::Play_Animation_Lower(_double TimeDelta)
{
	if (nullptr == m_pAniCtrl_Lower)
		return E_FAIL;

	m_pAniCtrl_Lower->Play_Animation(TimeDelta);

	_mat tmp;
	_mat topMatrix;

	D3DXMatrixIdentity(&topMatrix);

	Update_CombinedTransformationMatrices((D3DXFRAME_DERIVED*)m_pRootFrame, topMatrix);

	return E_NOTIMPL;
}

HRESULT CMesh_Dynamic::Play_Animation_Upper(_double TimeDelta)
{
	if (nullptr == m_pAniCtrl_Upper)
		return E_FAIL;

	m_pAniCtrl_Upper->Play_Animation(TimeDelta);

	_mat tmpMat;
	D3DXMatrixIdentity(&tmpMat);

	tmpMat = ((D3DXFRAME_DERIVED*)(m_pRootFrame->pFrameFirstChild->pFrameFirstChild))->CombinedTransformationMatrix;

	Update_CombinedTransformationMatrices((D3DXFRAME_DERIVED*)m_pUpperFrame, tmpMat);

	return E_NOTIMPL;
}

HRESULT CMesh_Dynamic::Play_Animation_RightArm(_double TimeDelta, _bool _bTwoHanded)
{
	if (nullptr == m_pAniCtrl_RightArm)
		return E_FAIL;

	m_pAniCtrl_RightArm->Play_Animation(TimeDelta);

	_mat tmpMat, PosMat;
	D3DXMatrixIdentity(&tmpMat);
	D3DXMatrixIdentity(&PosMat);

	if (_bTwoHanded)
	{
		tmpMat = ((D3DXFRAME_DERIVED*)
			(m_pUpperFrame->
				pFrameFirstChild->	// Spine1
				pFrameFirstChild))->CombinedTransformationMatrix;
	}

	else
	{
		tmpMat = ((D3DXFRAME_DERIVED*)
			(m_pUpperFrame->
				pFrameFirstChild->	// Spine1
				pFrameFirstChild->
				pFrameFirstChild))->CombinedTransformationMatrix;
	}

	Update_CombinedTransformationMatrices(m_pRightArmFrame, tmpMat);

	return S_OK;
}

HRESULT CMesh_Dynamic::Play_Animation_LeftArm(_double TimeDelta, _bool _bTwoHanded)
{
	if (nullptr == m_pAniCtrl_LeftArm)
		return E_FAIL;

	m_pAniCtrl_LeftArm->Play_Animation(TimeDelta);

	_mat tmpMat, PosMat;
	D3DXMatrixIdentity(&tmpMat);
	D3DXMatrixIdentity(&PosMat);

	if (_bTwoHanded)
	{
		tmpMat = ((D3DXFRAME_DERIVED*)
			(m_pUpperFrame->
				pFrameFirstChild->	// Spine1
				pFrameFirstChild))->CombinedTransformationMatrix;
	}

	else
	{
		tmpMat = ((D3DXFRAME_DERIVED*)
			(m_pUpperFrame->
				pFrameFirstChild->	// Spine1
				pFrameFirstChild->
				pFrameFirstChild))->CombinedTransformationMatrix;
	}

	Update_CombinedTransformationMatrices(m_pLeftArmFrame, tmpMat);

	return S_OK;
}

HRESULT CMesh_Dynamic::Play_Animation(_double TimeDelta)
{
	if (nullptr == m_pAniCtrl_Lower)
		return E_FAIL;

	m_pAniCtrl_Lower->Play_Animation(TimeDelta);

	Update_CombinedTransformationMatrices(m_pRootFrame, *D3DXMatrixIdentity(&m_matPivot));

	return NOERROR;
}

_bool CMesh_Dynamic::Is_Finish_Animation(_float _fLerpValue)
{
	return m_pAniCtrl_Lower->Is_Finish_Animation(_fLerpValue);
}

_bool CMesh_Dynamic::Is_Finish_Animation_Lower(_float _fLerpValue)
{
	return m_pAniCtrl_Lower->Is_Finish_Animation(_fLerpValue);
}

_bool CMesh_Dynamic::Is_Finish_Animation_Upper(_float _fLerpValue)
{
	return m_pAniCtrl_Upper->Is_Finish_Animation(_fLerpValue);
}

_bool CMesh_Dynamic::Is_Finish_Animation_LeftArm(_float _fLerpValue)
{
	return m_pAniCtrl_LeftArm->Is_Finish_Animation(_fLerpValue);
}

_bool CMesh_Dynamic::Is_Finish_Animation_RightArm(_float _fLerpValue)
{
	return m_pAniCtrl_RightArm->Is_Finish_Animation(_fLerpValue);
}

const _mat* CMesh_Dynamic::Find_CombinedTransformationMatrix(const char* pFrameName)
{
	D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pFrameName);
	if (nullptr == pFrame)
		return nullptr;

	return &pFrame->CombinedTransformationMatrix;
}

CMesh_Dynamic * CMesh_Dynamic::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pFilePath, const _tchar * pFileName, _mat PivotMatrix)
{
	CMesh_Dynamic*		pInstance = new CMesh_Dynamic(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype(pFilePath, pFileName, PivotMatrix)))
	{
		MSG_BOX("Failed To Create CMesh_Dynamic Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMesh_Dynamic::Clone_Component(void * pArg)
{
	CMesh_Dynamic*		pInstance = new CMesh_Dynamic(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MSG_BOX("Failed To Create CMesh_Dynamic Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh_Dynamic::Free()
{
	if (false == m_bisClone)
	{
		if (m_bIsSeperate)
		{
			if (FAILED(m_pHierarchy->DestroyFrame(m_pLeftArmFrame)))
				return;

			if (FAILED(m_pHierarchy->DestroyFrame(m_pRightArmFrame)))
				return;

			if (FAILED(m_pHierarchy->DestroyFrame(m_pUpperFrame)))
				return;
		}

		if (FAILED(m_pHierarchy->DestroyFrame(m_pRootFrame)))
			return;
	}

	m_MeshContainerList.clear();

	Safe_Release(m_pAniCtrl_LeftArm);
	Safe_Release(m_pAniCtrl_RightArm);
	Safe_Release(m_pAniCtrl_Lower);
	Safe_Release(m_pAniCtrl_Upper);
	Safe_Release(m_pHierarchy);

	CComponent::Free();
}
