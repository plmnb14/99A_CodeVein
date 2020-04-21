#include "..\Headers\Mesh_Static.h"
#include "Texture.h"

CMesh_Static::CMesh_Static(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{

}

CMesh_Static::CMesh_Static(const CMesh_Static & rhs)
	: CComponent(rhs)
	, m_pMesh(rhs.m_pMesh)
	, m_dwNumMaterials(rhs.m_dwNumMaterials)
	, m_pAdjacencyBuff(rhs.m_pAdjacencyBuff)
	, m_pMaterialsBuff(rhs.m_pMaterialsBuff)
	, m_pMaterials(rhs.m_pMaterials)
	, m_ppTextures(rhs.m_ppTextures)
	, m_dwVtxCnt(rhs.m_dwVtxCnt)
	, m_dwStride(rhs.m_dwStride)
	, m_pVtxMin(rhs.m_pVtxMin)
	, m_pVtxMax(rhs.m_pVtxMax)
{
	Safe_AddRef(m_pMesh);
	Safe_AddRef(m_pAdjacencyBuff);
	Safe_AddRef(m_pMaterialsBuff);

	memcpy(m_dwMaterialPass, rhs.m_dwMaterialPass, sizeof(_ulong) * 10);

	for (size_t i = 0; i < m_dwNumMaterials; ++i)
	{
		for (size_t j = 0; j < MESHTEXTURE::TYPE_END; ++j)
		{
			Safe_AddRef(m_ppTextures[i].pTextures[j]);
		}
	}
}

LPDIRECT3DBASETEXTURE9 CMesh_Static::Get_Texture(_ulong dwMaterialIndex, MESHTEXTURE::TYPE eType)
{
	if (nullptr == m_ppTextures ||
		m_dwNumMaterials <= dwMaterialIndex)
		return nullptr;

	return m_ppTextures[dwMaterialIndex].pTextures[eType];
}

HRESULT CMesh_Static::Ready_Component_Prototype(const _tchar * pFilePath, const _tchar * pFileName, _mat PivotMatrix)
{
	// 파일에 저장되어있던 정점정보와 인덱스정보등을 읽어들인다.

	_tchar		szFullPath[128] = L"";

	LPD3DXMESH		pMesh = nullptr;

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	// 폴리곤에 세 변을 공유하는 이웃 폴리곤 세개의 인덱스를 정수 배열에 보관한 메모리.
	if (FAILED(D3DXLoadMeshFromX(szFullPath, D3DXMESH_MANAGED, m_pGraphic_Dev, &m_pAdjacencyBuff, &m_pMaterialsBuff, nullptr, &m_dwNumMaterials, &pMesh)))
		return E_FAIL;

	m_pMaterials = (D3DXMATERIAL*)m_pMaterialsBuff->GetBufferPointer();
	
	m_ppTextures = new MESHTEXTURE[m_dwNumMaterials];
	ZeroMemory(m_ppTextures, sizeof(MESHTEXTURE) * m_dwNumMaterials);

	for (size_t i = 0; i < m_dwNumMaterials; ++i)
	{
		lstrcpy(szFullPath, pFilePath);

		_tchar	szTextureFileName[MAX_PATH] = L"";

		MultiByteToWideChar(CP_ACP, 0, m_pMaterials[i].pTextureFilename, (_int)strlen(m_pMaterials[i].pTextureFilename)
			, szTextureFileName, MAX_PATH);

		lstrcat(szFullPath, szTextureFileName);

		//==================================================================================================================================
		// C - Color
		//==================================================================================================================================
		if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Dev, szFullPath, &m_ppTextures[i].pTextures[MESHTEXTURE::TYPE_DIFFUSE_MAP])))
			m_bIncludeMap[MESHTEXTURE::TYPE_DIFFUSE_MAP] = true;
		//==================================================================================================================================
		// N - Normal
		//==================================================================================================================================
		Change_TextureFileName(szFullPath, L"C", L"N");
		if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Dev, szFullPath, &m_ppTextures[i].pTextures[MESHTEXTURE::TYPE_NORMAL_MAP])))
			m_bIncludeMap[MESHTEXTURE::TYPE_NORMAL_MAP] = true;
		//==================================================================================================================================
		// S - Specular
		//==================================================================================================================================
		Change_TextureFileName(szFullPath, L"N", L"S");
		if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Dev, szFullPath, &m_ppTextures[i].pTextures[MESHTEXTURE::TYPE_SPECULAR_MAP])))
			m_bIncludeMap[MESHTEXTURE::TYPE_SPECULAR_MAP] = true;
		//==================================================================================================================================
		// E - Emissive
		//==================================================================================================================================
		Change_TextureFileName(szFullPath, L"S", L"E");
		if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Dev, szFullPath, &m_ppTextures[i].pTextures[MESHTEXTURE::TYPE_EMISSIVE_MAP])))
			m_bIncludeMap[MESHTEXTURE::TYPE_EMISSIVE_MAP] = true;
		//==================================================================================================================================
		// R - Roughness
		//==================================================================================================================================
		Change_TextureFileName(szFullPath, L"E", L"R");
		if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Dev, szFullPath, &m_ppTextures[i].pTextures[MESHTEXTURE::TYPE_ROUGHNESS_MAP])))
			m_bIncludeMap[MESHTEXTURE::TYPE_ROUGHNESS_MAP] = true;
		//==================================================================================================================================
		// U - Union
		//==================================================================================================================================
		Change_TextureFileName(szFullPath, L"R", L"U");
		if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Dev, szFullPath, &m_ppTextures[i].pTextures[MESHTEXTURE::TYPE_UNION_MAP])))
			m_bIncludeMap[MESHTEXTURE::TYPE_UNION_MAP] = true;
		//==================================================================================================================================
		// T - Transparency Map
		//==================================================================================================================================
		Change_TextureFileName(szFullPath, L"U", L"T");
		if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Dev, szFullPath, &m_ppTextures[i].pTextures[MESHTEXTURE::TYPE_TRANSPARENCY_MAP])))
			m_bIncludeMap[MESHTEXTURE::TYPE_TRANSPARENCY_MAP] = true;
		//==================================================================================================================================
		// ID - Material ID
		//==================================================================================================================================
		Change_TextureFileName(szFullPath, L"T", L"I");
		if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Dev, szFullPath, &m_ppTextures[i].pTextures[MESHTEXTURE::TYPE_ID_MAP])))
			m_bIncludeMap[MESHTEXTURE::TYPE_ID_MAP] = true;
		//==================================================================================================================================
		//// AO - Ambient Occlusion
		////==================================================================================================================================
		//Change_TextureFileName(szFullPath, L"D", L"O");
		//Change_TextureFileName(szFullPath, L"I", L"A");
		//if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Dev, szFullPath, &m_ppTextures[i].pTextures[MESHTEXTURE::TYPE_AO_MAP])))
		//	m_bIncludeMap[MESHTEXTURE::TYPE_AO_MAP] = true;

		m_dwMaterialPass[i] = 4;

		if (m_bIncludeMap[MESHTEXTURE::TYPE_NORMAL_MAP] == true)
		{
			// D N
			m_dwMaterialPass[i] = 4;

			if (m_bIncludeMap[MESHTEXTURE::TYPE_SPECULAR_MAP] == true)
			{
				// D N S
				m_dwMaterialPass[i] = 13;

				if (m_bIncludeMap[MESHTEXTURE::TYPE_EMISSIVE_MAP] == true)
				{
					// D N S E
					m_dwMaterialPass[i] = 6;
					continue;
				}

				continue;
			}

			else if (m_bIncludeMap[MESHTEXTURE::TYPE_EMISSIVE_MAP] == true)
			{
				// D N E
				m_dwMaterialPass[i] = 5;

				if (m_bIncludeMap[MESHTEXTURE::TYPE_ID_MAP] == true)
				{
					m_dwMaterialPass[i] = 17;
				}

				continue;
			}

			else if (m_bIncludeMap[MESHTEXTURE::TYPE_UNION_MAP] == true)
			{
				// D N U
				m_dwMaterialPass[i] = 7;
				continue;
			}

			else if (m_bIncludeMap[MESHTEXTURE::TYPE_TRANSPARENCY_MAP] == true)
			{
				// D N T
				m_dwMaterialPass[i] = 9;
				continue;
			}

			else if (m_bIncludeMap[MESHTEXTURE::TYPE_ROUGHNESS_MAP] == true)
			{
				// D N R
				m_dwMaterialPass[i] = 10;
				continue;
			}

			else if (m_bIncludeMap[MESHTEXTURE::TYPE_ID_MAP] == true)
			{
				m_dwMaterialPass[i] = 16;
			}
		}
	}

	_ulong dwFVF = pMesh->GetFVF();

	//if (false == (dwFVF & D3DFVF_NORMAL))
	//{
	//	pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF | D3DFVF_NORMAL, m_pGraphic_Dev, &m_pMesh);
	//
	//	D3DXComputeNormals(m_pMesh, (_ulong*)m_pAdjacencyBuff->GetBufferPointer());
	//}
	//else
	//{
	// D3DVERTEXELEMENT9구조체 하나가 정점의 fvf하나의 정보를 의미한다.
	D3DVERTEXELEMENT9		DeclCreate[MAX_FVF_DECL_SIZE] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		{ 0, 48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	//pMesh->GetDeclaration(DeclCreate);

	pMesh->CloneMesh(pMesh->GetOptions(), DeclCreate, m_pGraphic_Dev, &m_pMesh);
	//}

	Safe_Release(pMesh);

	m_matPivot = PivotMatrix;

	_byte*		pVertices = nullptr;

	m_pMesh->LockVertexBuffer(0, (void**)&pVertices);

	_ulong dwNumVertices = m_pMesh->GetNumVertices();

	_ulong dwStride = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

	D3DVERTEXELEMENT9		Decl[MAX_FVF_DECL_SIZE];

	m_pMesh->GetDeclaration(Decl);

	_ushort		nNormalOffset = 0;

	for (size_t i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (Decl[i].Usage == D3DDECLUSAGE_NORMAL)
		{
			nNormalOffset = Decl[i].Offset;
			break;
		}
	}

	for (_ulong i = 0; i < dwNumVertices; ++i)
	{
		D3DXVec3TransformCoord((_v3*)(pVertices + (i *  dwStride)), (_v3*)(pVertices + (i *  dwStride)), &m_matPivot);
		D3DXVec3TransformNormal((_v3*)(pVertices + (i *  dwStride) + nNormalOffset), (_v3*)(pVertices + (i *  dwStride) + nNormalOffset), &m_matPivot);
	}

	m_dwVtxCnt = m_pMesh->GetNumVertices();
	m_dwStride = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

	D3DXComputeBoundingBox((_v3*)pVertices, m_dwVtxCnt, m_dwStride, &m_pVtxMin, &m_pVtxMax);

	m_pMesh->UnlockVertexBuffer();


	return NOERROR;
}
HRESULT CMesh_Static::Ready_Component(void * pArg)
{
	return NOERROR;
}

HRESULT CMesh_Static::SetTexture_OnShader(LPD3DXEFFECT pEffect, D3DXHANDLE ConstantName, _uint iAttributeID, MESHTEXTURE::TYPE eType)
{
	if (nullptr == m_ppTextures ||
		m_dwNumMaterials <= iAttributeID)
		return E_FAIL;

	pEffect->SetTexture(ConstantName, m_ppTextures[iAttributeID].pTextures[eType]);

	return NOERROR;
}

HRESULT CMesh_Static::SetMaterial_OnShader(LPD3DXEFFECT pEffect, D3DXHANDLE pConstantName, _uint iAttributeID)
{
	if (m_dwNumMaterials <= iAttributeID)
		return E_FAIL;

	m_pMaterials[iAttributeID].MatD3D.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);

	pEffect->SetValue(pConstantName, &m_pMaterials[iAttributeID].MatD3D, sizeof(D3DMATERIAL9));

	return NOERROR;
}

HRESULT CMesh_Static::Render_Mesh(_uint iAttributeID)
{
	if (nullptr == m_pMesh)
		return E_FAIL;

	m_pMesh->DrawSubset(iAttributeID);

	return NOERROR;
}

HRESULT CMesh_Static::Change_TextureFileName(_tchar* pFilePath, _tchar * pSourMark, _tchar * pDestMark)
{
	_int iLength = lstrlen(pFilePath);

	for (_int i = iLength; i >= 0; --i)
	{
		if (pFilePath[i] == *pSourMark)
		{
			pFilePath[i] = *pDestMark;

			if (!lstrcmp(pDestMark, L"I"))
			{
				lstrcpy(&pFilePath[i], L"ID.tga");
			}

			break;
		}
	}
	return S_OK;
}

CMesh_Static * CMesh_Static::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pFilePath, const _tchar * pFileName, _mat PivotMatrix)
{
	CMesh_Static*		pInstance = new CMesh_Static(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype(pFilePath, pFileName, PivotMatrix)))
	{
		MSG_BOX("Failed To Create CMesh_Static Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMesh_Static::Clone_Component(void * pArg)
{
	CMesh_Static*		pInstance = new CMesh_Static(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MSG_BOX("Failed To Create CMesh_Static Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh_Static::Free()
{
	for (size_t i = 0; i < m_dwNumMaterials; ++i)
	{
		for (size_t j = 0; j < MESHTEXTURE::TYPE_END; j++)
		{
			Safe_Release(m_ppTextures[i].pTextures[j]);
		}
	}


	if (false == m_bisClone)
		Safe_Delete_Array(m_ppTextures);

	Safe_Release(m_pAdjacencyBuff);
	Safe_Release(m_pMaterialsBuff);
	Safe_Release(m_pMesh);

	CComponent::Free();
}
