#include "..\Headers\Shader.h"
#include "Mesh_Static.h"
#include "Mesh_Dynamic.h"

CShader::CShader(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
	, m_pEffect(nullptr)
{
	
}

CShader::CShader(const CShader & rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_pErrorBuffer(rhs.m_pErrorBuffer)
{
	Safe_AddRef(m_pEffect);
	Safe_AddRef(m_pErrorBuffer);
}

HRESULT CShader::Set_Bool(D3DXHANDLE Handle, _bool Bool)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetBool(Handle, Bool);	
}

HRESULT CShader::Set_MatrixArray(D3DXHANDLE Handle, const _mat* pData, _uint iByte)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetMatrixArray(Handle, pData, iByte);
}

HRESULT CShader::Set_Value(D3DXHANDLE Handle, const void* pData, _uint iByte)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetValue(Handle, pData, iByte);	
}

HRESULT CShader::Set_Texture(D3DXHANDLE Handle, LPDIRECT3DBASETEXTURE9 pTexture)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetTexture(Handle, pTexture);
}

HRESULT CShader::Set_StaticTexture_Auto(CMesh_Static * pMeshStatic, _ulong dwSubsetNum)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	_ulong dwSubSet = pMeshStatic->Get_MaterialPass(dwSubsetNum);

	switch (dwSubSet)
	{
	case 4:	// D N
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum , MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NORMAL_MAP));

		break;
	}

	case 5:	// D N E
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_EmissiveTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_EMISSIVE_MAP));

		break;
	}

	case 6:	// D N S E
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_SpecularTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_SPECULAR_MAP));
		m_pEffect->SetTexture("g_EmissiveTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_EMISSIVE_MAP));

		break;
	}

	case 7:	// D N U
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_UnionTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_UNION_MAP));

		break;
	}

	case 8:	// D N I
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_IDTexutre", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_ID_MAP));

		break;
	}

	case 9:	// D N T
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_TransperencyTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_TRANSPARENCY_MAP));

		break;
	}

	case 10:	// D N R
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_RoughnessTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_ROUGHNESS_MAP));

		break;
	}

	case 11:	// D N S U
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_SpecularTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_SPECULAR_MAP));
		m_pEffect->SetTexture("g_UnionTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_UNION_MAP));

		break;
	}

	case 12:	// D N S U E
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_SpecularTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_SPECULAR_MAP));
		m_pEffect->SetTexture("g_UnionTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_UNION_MAP));
		m_pEffect->SetTexture("g_EmissiveTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_EMISSIVE_MAP));

		break;
	}

	case 13:	// D N S
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_SpecularTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_SPECULAR_MAP));

		break;
	}

	case 14:	// D
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));

		break;
	}
	case 15:	// D _ Alpha
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));

		break;
	}

	case 16:	// D N ID
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_IDTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_ID_MAP));

		break;
	}

	case 17:	// D N E ID
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_EmissiveTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_EMISSIVE_MAP));
		m_pEffect->SetTexture("g_IDTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_ID_MAP));

		break;
	}

	case 18:	// D N E U
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum,MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_EmissiveTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_EMISSIVE_MAP));
		m_pEffect->SetTexture("g_UnionTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_UNION_MAP));

		break;
	}

	case 21:	// D Z
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_ZTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NR_MAP));

		break;
	}

	case 22:	// D N S H U
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_SpecularTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_SPECULAR_MAP));
		m_pEffect->SetTexture("g_HeightTexture", pMeshStatic->Get_Texture(dwSubsetNum,  MESHTEXTURE::TYPE_HEIGHT_MAP));
		m_pEffect->SetTexture("g_UnionTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_UNION_MAP));

		break;
	}

	case 24:	// D N E U ID
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_EmissiveTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_EMISSIVE_MAP));
		m_pEffect->SetTexture("g_UnionTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_UNION_MAP));
		m_pEffect->SetTexture("g_IDTexture", pMeshStatic->Get_Texture(dwSubsetNum, MESHTEXTURE::TYPE_ID_MAP));

		break;
	}

	}

	return S_OK;
}

HRESULT CShader::Set_DynamicTexture_Auto(CMesh_Dynamic * pMeshDynamic, _ulong dwSubsetNum , _ulong dwMeshContainer)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	_ulong dwSubSet = pMeshDynamic->Get_MaterialPass(dwSubsetNum , dwMeshContainer);

	switch (dwSubSet)
	{
	case 4:	// D N
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_NORMAL_MAP));

		break;
	}

	case 5:	// D N E
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_EmissiveTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_EMISSIVE_MAP));

		break;
	}

	case 6:	// D N S E
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_SpecularTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_SPECULAR_MAP));
		m_pEffect->SetTexture("g_EmissiveTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_EMISSIVE_MAP));

		break;
	}

	case 7:	// D N U
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_UnionTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_UNION_MAP));

		break;
	}

	case 8:	// D N I
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_IDTexutre", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_ID_MAP));

		break;
	}

	case 9:	// D N T
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_TransperencyTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_TRANSPARENCY_MAP));

		break;
	}

	case 10:	// D N R
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_RoughnessTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_ROUGHNESS_MAP));

		break;
	}

	case 11:	// D N S U
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_SpecularTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_SPECULAR_MAP));
		m_pEffect->SetTexture("g_UnionTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_UNION_MAP));

		break;
	}

	case 12:	// D N S U ID
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_SpecularTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_SPECULAR_MAP));
		m_pEffect->SetTexture("g_UnionTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_UNION_MAP)); 
		m_pEffect->SetTexture("g_IDTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_ID_MAP));

		break;
	}

	case 13:	// D N S
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_SpecularTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_SPECULAR_MAP));

		break;
	}

	case 14:	// D
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));

		break;
	}
	case 15:	// D _ Alpha
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));

		break;
	}

	case 16:	// D N ID
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_IDTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_ID_MAP));

		break;
	}

	case 17:	// D N E ID
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_EmissiveTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_EMISSIVE_MAP));
		m_pEffect->SetTexture("g_IDTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_ID_MAP));

		break;
	}

	case 18:	// D N E U
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_EmissiveTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_EMISSIVE_MAP));
		m_pEffect->SetTexture("g_UnionTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_UNION_MAP));

		break;
	}

	case 19:	// D N S H
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_SpecularTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_SPECULAR_MAP));
		m_pEffect->SetTexture("g_HeightTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_HEIGHT_MAP));

		break;
	}

	case 20:	// D N S E U
	{
		m_pEffect->SetTexture("g_DiffuseTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_DIFFUSE_MAP));
		m_pEffect->SetTexture("g_NormalTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_NORMAL_MAP));
		m_pEffect->SetTexture("g_EmissiveTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_EMISSIVE_MAP));
		m_pEffect->SetTexture("g_UnionTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_UNION_MAP));
		m_pEffect->SetTexture("g_SpecularTexture", pMeshDynamic->Get_MeshTexture(dwSubsetNum, dwMeshContainer, MESHTEXTURE::TYPE_SPECULAR_MAP));

		break;
	}

	}

	return S_OK;
}

HRESULT CShader::Commit_Changes()
{
	if (nullptr == m_pEffect)
		return E_FAIL;
	
	return m_pEffect->CommitChanges();;
}

HRESULT CShader::Ready_Component_Prototype(const _tchar * pFilePath)
{
	if (FAILED(D3DXCreateEffectFromFile(m_pGraphic_Dev, pFilePath, nullptr, nullptr, 0, nullptr, &m_pEffect, &m_pErrorBuffer)))
	{
		// MessageBoxA(0, (char*)m_pErrorBuffer->GetBufferPointer(), "System Error", MB_OK);
		return E_FAIL;
	}

	return NOERROR;
}

HRESULT CShader::Ready_Component(void * pArg)
{
	

	return NOERROR;
}

HRESULT CShader::Begin_Shader()
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	_uint iNumPass = 0;

	m_pEffect->Begin(nullptr, 0);

	return NOERROR;
}

HRESULT CShader::Begin_Pass(_uint iPassIdx)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->BeginPass(iPassIdx);

	return NOERROR;
}

HRESULT CShader::End_Pass()
{
	if (nullptr == m_pEffect)
		return E_FAIL;	

	m_pEffect->EndPass();

	return NOERROR;
}

HRESULT CShader::End_Shader()
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->End();

	return NOERROR;
}

CShader * CShader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pFilePath)
{
	CShader*		pInstance = new CShader(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype(pFilePath)))
	{
		MSG_BOX("CShader Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CShader::Clone_Component(void * pArg)
{
	CShader*		pInstance = new CShader(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MSG_BOX("CTexture Cloned Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	Safe_Release(m_pErrorBuffer);
	Safe_Release(m_pEffect);

	CComponent::Free();
}
