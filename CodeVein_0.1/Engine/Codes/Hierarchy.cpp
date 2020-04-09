#include "..\Headers\Hierarchy.h"

CHierarchy::CHierarchy(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CHierarchy::Ready_HierarchyLoader(const _tchar* pFilePath)
{
	m_pFilePath = pFilePath;

	return NOERROR;
}

// 1. 뼈에 정보를 담을 공간을 생성한다.
// 2. 생성한 공간에 필요에 따라 인자로 전달된 값을들을 정리하여 보관한다.

STDMETHODIMP CHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME * ppNewFrame)
{
	LPD3DXFRAME_DERIVED		pFrame = new D3DXFRAME_DERIVED;
	ZeroMemory(pFrame, sizeof(D3DXFRAME_DERIVED));

	if (FAILED(SetUp_Name(&pFrame->Name, Name)))
		return E_FAIL;

	pFrame->CombinedTransformationMatrix = *D3DXMatrixIdentity(&pFrame->TransformationMatrix);

	*ppNewFrame = pFrame;

	return NOERROR;
}


// 1. 메시에 정보를 담을 공간을 생성한다.
// 2. 생성한 공간에 필요에 따라 인자로 전달된 값을들을 정리하여 보관한다.
STDMETHODIMP CHierarchy::CreateMeshContainer(LPCSTR Name, CONST D3DXMESHDATA * pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer)
{
	LPD3DXMESHCONTAINER_DERIVED	pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	ZeroMemory(pMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));

	if (FAILED(SetUp_Name(&pMeshContainer->Name, Name)))
		return E_FAIL;

	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;



	LPD3DXMESH		pMesh = pMeshData->pMesh;
	Safe_AddRef(pMesh);

	_ulong dwMeshContainerCnt = pMesh->GetNumFaces();

	pMeshContainer->pAdjacency = new _ulong[pMesh->GetNumFaces() * 3];
	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * pMesh->GetNumFaces() * 3);

	_ulong dwFVF = pMesh->GetFVF();

	D3DVERTEXELEMENT9		Decl[MAX_FVF_DECL_SIZE];
	pMesh->GetDeclaration(Decl);


	pMesh->CloneMesh(pMesh->GetOptions(), Decl, m_pGraphic_Device, &pMeshContainer->MeshData.pMesh);
	//	//// D3DVERTEXELEMENT9구조체 하나가 정점의 fvf하나의 정보를 의미한다.
	//D3DVERTEXELEMENT9		DeclCreate[MAX_FVF_DECL_SIZE] =
	//{
	//	{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	//	{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
	//	{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
	//	{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
	//	{ 0, 48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	//	{ 0, 56, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	//	D3DDECL_END()
	//};
	//
	//	//pMesh->CloneMesh(pMesh->GetOptions(), DeclCreate, m_pGraphic_Device, &pMeshContainer->MeshData.pMesh);
	//pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF, m_pGraphic_Device, &pMeshContainer->MeshData.pMesh);
	//}

	Safe_Release(pMesh);

	pMeshContainer->NumMaterials = NumMaterials;

	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * pMeshContainer->NumMaterials);

	pMeshContainer->pMeshTexture = new MESHTEXTURE[pMeshContainer->NumMaterials];
	ZeroMemory(pMeshContainer->pMeshTexture, sizeof(MESHTEXTURE) * pMeshContainer->NumMaterials);

	_tchar		szFullPath[MAX_PATH] = L"";

	for (_uint i = 0; i < MESHTEXTURE::TYPE_END; ++i)
		m_bIncludeMap[i] = false;

	for (size_t i = 0; i < pMeshContainer->NumMaterials; ++i)
	{
		lstrcpy(szFullPath, m_pFilePath);

		_tchar	szTextureFileName[MAX_PATH] = L"";

		MultiByteToWideChar(CP_ACP, 0, pMeshContainer->pMaterials[i].pTextureFilename, (_int)strlen(pMeshContainer->pMaterials[i].pTextureFilename)
			, szTextureFileName, MAX_PATH);

		lstrcat(szFullPath, szTextureFileName);

		if (FAILED(D3DXCreateTextureFromFile(m_pGraphic_Device, szFullPath, &pMeshContainer->pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_DIFFUSE_MAP])))
			return E_FAIL;

		pMeshContainer->pMeshTexture[i].m_dwMaterialPass = 14;

		if (!lstrcmp(szTextureFileName, L"T_Eyelash_Female1.tga"))
		{
			pMeshContainer->pMeshTexture[i].m_dwMaterialPass = 15;
		}

		//==================================================================================================================================
		// C - Color
		//==================================================================================================================================
		if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Device, szFullPath, &pMeshContainer->pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_DIFFUSE_MAP])))
			m_bIncludeMap[MESHTEXTURE::TYPE_DIFFUSE_MAP] = true;
		//==================================================================================================================================
		// N - Normal
		//==================================================================================================================================
		Change_TextureFileName(szFullPath, L"C", L"N");
		if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Device, szFullPath, &pMeshContainer->pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_NORMAL_MAP])))
			m_bIncludeMap[MESHTEXTURE::TYPE_NORMAL_MAP] = true;
		//==================================================================================================================================
		// S - Specular
		//==================================================================================================================================
		Change_TextureFileName(szFullPath, L"N", L"S");
		if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Device, szFullPath, &pMeshContainer->pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_SPECULAR_MAP])))
			m_bIncludeMap[MESHTEXTURE::TYPE_SPECULAR_MAP] = true;
		//==================================================================================================================================
		// E - Emissive
		//==================================================================================================================================
		Change_TextureFileName(szFullPath, L"S", L"E");
		if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Device, szFullPath, &pMeshContainer->pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_EMISSIVE_MAP])))
			m_bIncludeMap[MESHTEXTURE::TYPE_EMISSIVE_MAP] = true;
		//==================================================================================================================================
		// R - Roughness
		//==================================================================================================================================
		Change_TextureFileName(szFullPath, L"E", L"R");
		if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Device, szFullPath, &pMeshContainer->pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_ROUGHNESS_MAP])))
			m_bIncludeMap[MESHTEXTURE::TYPE_ROUGHNESS_MAP] = true;
		//==================================================================================================================================
		// U - Union
		//==================================================================================================================================
		Change_TextureFileName(szFullPath, L"R", L"U");
		if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Device, szFullPath, &pMeshContainer->pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_UNION_MAP])))
			m_bIncludeMap[MESHTEXTURE::TYPE_UNION_MAP] = true;
		//==================================================================================================================================
		// T - Transparency Map
		//==================================================================================================================================
		Change_TextureFileName(szFullPath, L"U", L"T");
		if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Device, szFullPath, &pMeshContainer->pMeshTexture[i].pTextures[MESHTEXTURE::TYPE_TRANSPARENCY_MAP])))
			m_bIncludeMap[MESHTEXTURE::TYPE_TRANSPARENCY_MAP] = true;
		////==================================================================================================================================
		//// ID - Material ID
		////==================================================================================================================================
		//Change_TextureFileName(szFullPath, L"T", L"I");
		//if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Device, szFullPath, &m_ppTextures[i].pTextures[MESHTEXTURE::TYPE_ID_MAP])))
		//	m_bIncludeMap[MESHTEXTURE::TYPE_ID_MAP] = true;
		////==================================================================================================================================
		//// AO - Ambient Occlusion
		////==================================================================================================================================
		//Change_TextureFileName(szFullPath, L"D", L"O");
		//Change_TextureFileName(szFullPath, L"I", L"A");
		//if (SUCCEEDED(D3DXCreateTextureFromFile(m_pGraphic_Device, szFullPath, &m_ppTextures[i].pTextures[MESHTEXTURE::TYPE_AO_MAP])))
		//	m_bIncludeMap[MESHTEXTURE::TYPE_AO_MAP] = true;

		if (m_bIncludeMap[MESHTEXTURE::TYPE_NORMAL_MAP] == true)
		{
			// D N
			pMeshContainer->pMeshTexture[i].m_dwMaterialPass = 4;

			if (m_bIncludeMap[MESHTEXTURE::TYPE_SPECULAR_MAP] == true)
			{
				// D N S
				pMeshContainer->pMeshTexture[i].m_dwMaterialPass = 13;

				if (m_bIncludeMap[MESHTEXTURE::TYPE_EMISSIVE_MAP] == true)
				{
					// D N S E
					pMeshContainer->pMeshTexture[i].m_dwMaterialPass = 6;
					continue;
				}

				continue;
			}

			else if (m_bIncludeMap[MESHTEXTURE::TYPE_EMISSIVE_MAP] == true)
			{
				// D N E
				pMeshContainer->pMeshTexture[i].m_dwMaterialPass = 5;
				continue;
			}

			else if (m_bIncludeMap[MESHTEXTURE::TYPE_UNION_MAP] == true)
			{
				// D N U
				pMeshContainer->pMeshTexture[i].m_dwMaterialPass = 7;
				continue;
			}

			else if (m_bIncludeMap[MESHTEXTURE::TYPE_TRANSPARENCY_MAP] == true)
			{
				// D N T
				pMeshContainer->pMeshTexture[i].m_dwMaterialPass = 9;
				continue;
			}

			else if (m_bIncludeMap[MESHTEXTURE::TYPE_ROUGHNESS_MAP] == true)
			{
				// D N R
				pMeshContainer->pMeshTexture[i].m_dwMaterialPass = 10;
				continue;
			}
		}

	}

	pMeshContainer->pSkinInfo = pSkinInfo;
	Safe_AddRef(pMeshContainer->pSkinInfo);

	pMeshContainer->dwNumBones = pMeshContainer->pSkinInfo->GetNumBones();

	pMeshContainer->pOffsetMatrices = new D3DXMATRIX[pMeshContainer->dwNumBones];
	pMeshContainer->ppCombinedTransformationMatrices = new D3DXMATRIX*[pMeshContainer->dwNumBones];
	pMeshContainer->pRenderMatrices = new D3DXMATRIX[pMeshContainer->dwNumBones];

	for (_ulong i = 0; i < pMeshContainer->dwNumBones; i++)
		pMeshContainer->pOffsetMatrices[i] = *pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i);

	//if (FAILED(pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), pMeshContainer->MeshData.pMesh->GetFVF(), m_pGraphic_Device, &pMeshContainer->pOriginalMesh)))
	//	return E_FAIL;

	if (FAILED(pMeshContainer->MeshData.pMesh->CloneMesh(pMesh->GetOptions(), Decl, m_pGraphic_Device, &pMeshContainer->pOriginalMesh)))
		return E_FAIL;

	//if (FAILED(pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), pMeshContainer->MeshData.pMesh->GetFVF(), m_pGraphic_Device, &pMeshContainer->pOriginalMesh)))
	//	return E_FAIL;

	*ppNewMeshContainer = pMeshContainer;

	return NOERROR;
}

STDMETHODIMP CHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	Safe_Delete_Array(pFrameToFree->Name);

	if (nullptr != pFrameToFree->pMeshContainer)
		DestroyMeshContainer(pFrameToFree->pMeshContainer);

	if (pFrameToFree->pFrameSibling)
		DestroyFrame(pFrameToFree->pFrameSibling);

	if (pFrameToFree->pFrameFirstChild)
		DestroyFrame(pFrameToFree->pFrameFirstChild);

	Safe_Delete(pFrameToFree);

	return NOERROR;
}

STDMETHODIMP CHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	D3DXMESHCONTAINER_DERIVED*		pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerToFree;

	Safe_Delete_Array(pMeshContainer->Name);
	Safe_Release(pMeshContainer->MeshData.pMesh);
	Safe_Delete_Array(pMeshContainer->pMaterials);
	Safe_Delete_Array(pMeshContainer->pAdjacency);
	Safe_Release(pMeshContainer->pSkinInfo);
	Safe_Release(pMeshContainer->pOriginalMesh);

	for (size_t i = 0; i < pMeshContainer->NumMaterials; ++i)
	{
		for (size_t j = 0; j < MESHTEXTURE::TYPE_END; ++j)
		{
			Safe_Release(pMeshContainer->pMeshTexture[i].pTextures[j]);
		}
	}

	Safe_Delete_Array(pMeshContainer->pMeshTexture);
	Safe_Delete_Array(pMeshContainer->pOffsetMatrices);
	Safe_Delete_Array(pMeshContainer->ppCombinedTransformationMatrices);
	Safe_Delete_Array(pMeshContainer->pRenderMatrices);

	Safe_Delete_Array(pMeshContainer);

	return NOERROR;
}

HRESULT CHierarchy::SetUp_Name(char ** ppSourName, const char * pDestName)
{
	if (nullptr == pDestName)
		return NOERROR;

	_uint iLength = (_uint)strlen(pDestName);

	*ppSourName = new char[iLength + 1];

	memcpy(*ppSourName, pDestName, iLength + 1);

	return NOERROR;
}

HRESULT CHierarchy::Change_TextureFileName(_tchar * pFilePath, _tchar * pSourMark, _tchar * pDestMark)
{
	_int iLength = lstrlen(pFilePath);

	for (_int i = iLength; i >= 0; --i)
	{
		if (pFilePath[i] == *pSourMark)
		{
			pFilePath[i] = *pDestMark;
			break;
		}
	}
	return NOERROR;
}

CHierarchy * CHierarchy::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath)
{
	CHierarchy*		pInstance = new CHierarchy(pGraphic_Device);

	if (FAILED(pInstance->Ready_HierarchyLoader(pFilePath)))
	{
		MSG_BOX("Failed To Create Hierarchy Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHierarchy::Free()
{
	Safe_Release(m_pGraphic_Device);
}
