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
	, m_pAniCtrl(rhs.m_pAniCtrl->Clone_AniCtrl())
{
	Safe_AddRef(m_pHierarchy);
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

HRESULT CMesh_Dynamic::Ready_Component_Prototype(const _tchar * pFilePath, const _tchar * pFileName, _mat PivotMatrix)
{
	_tchar		szFullPath[128] = L"";

	LPD3DXMESH		pMesh = nullptr;

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	m_pHierarchy = CHierarchy::Create(m_pGraphic_Dev, pFilePath);
	if (nullptr == m_pHierarchy)
		return E_FAIL;

	LPD3DXANIMATIONCONTROLLER		pAniCtrl = nullptr;

	if (FAILED(D3DXLoadMeshHierarchyFromX(szFullPath, D3DXMESH_MANAGED, m_pGraphic_Dev, m_pHierarchy, nullptr, &m_pRootFrame, &pAniCtrl)))
		return E_FAIL;

	m_pAniCtrl = CAniCtrl::Create(m_pGraphic_Dev, pAniCtrl);
	if (nullptr == m_pAniCtrl)
		return E_FAIL;

	Safe_Release(pAniCtrl);

	m_matPivot = PivotMatrix;

	Update_CombinedTransformationMatrices(m_pRootFrame, m_matPivot);

	SetUp_CombinedTransformationMatricesPointer(m_pRootFrame);

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

HRESULT CMesh_Dynamic::SetUp_CombinedTransformationMatricesPointer(D3DXFRAME* pFrame)
{
	if (nullptr != pFrame->pMeshContainer)
	{
		D3DXMESHCONTAINER_DERIVED* pMeshContainer_Derived = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;

		for (_ulong i = 0; i < pMeshContainer_Derived->dwNumBones; ++i)
		{
			D3DXFRAME_DERIVED*	pFrame_Derived = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pMeshContainer_Derived->pSkinInfo->GetBoneName(i));

			pMeshContainer_Derived->ppCombinedTransformationMatrices[i] = &pFrame_Derived->CombinedTransformationMatrix;
		}

		m_MeshContainerList.push_back(pMeshContainer_Derived);
	}

	if (nullptr != pFrame->pFrameFirstChild)
		SetUp_CombinedTransformationMatricesPointer(pFrame->pFrameFirstChild);

	if (nullptr != pFrame->pFrameSibling)
		SetUp_CombinedTransformationMatricesPointer(pFrame->pFrameSibling);

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

HRESULT CMesh_Dynamic::SetUp_Animation(_uint iIndex)
{
	if (nullptr == m_pAniCtrl)
		return E_FAIL;

	m_pAniCtrl->SetUp_Animation(iIndex);

	return NOERROR;
}

HRESULT CMesh_Dynamic::Play_Animation(_double TimeDelta)
{
	if (nullptr == m_pAniCtrl)
		return E_FAIL;

	m_pAniCtrl->Play_Animation(TimeDelta);

	Update_CombinedTransformationMatrices(m_pRootFrame, m_matPivot);

	return NOERROR;
}

_bool CMesh_Dynamic::Is_Finish_Animation(_float _fLerpValue)
{
	return m_pAniCtrl->Is_Finish_Animation(_fLerpValue);
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
		if (FAILED(m_pHierarchy->DestroyFrame(m_pRootFrame)))
			return;
	}

	m_MeshContainerList.clear();

	Safe_Release(m_pAniCtrl);
	Safe_Release(m_pHierarchy);

	CComponent::Free();
}
