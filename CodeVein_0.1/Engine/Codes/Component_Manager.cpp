#include "..\Headers\Component_Manager.h"
#include "Management.h"
#include "ActiveObject.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Ready_Component_Manager(_Device _pGraphicDev)
{
	if (FAILED(Add_Prototype(SCENE_STATIC, L"Shader_Default", CShader::Create(_pGraphicDev, L"../ShaderFiles/Shader_Default.fx"))))
		return E_FAIL;
	if (FAILED(Add_Prototype(SCENE_STATIC, L"Shader_Terrain", CShader::Create(_pGraphicDev, L"../ShaderFiles/Shader_Terrain.fx"))))
		return E_FAIL;
	if (FAILED(Add_Prototype(SCENE_STATIC, L"Shader_Mesh", CShader::Create(_pGraphicDev, L"../ShaderFiles/Shader_Mesh.fx"))))
		return E_FAIL;
	if (FAILED(Add_Prototype(SCENE_STATIC, L"Shader_Sky", CShader::Create(_pGraphicDev, L"../ShaderFiles/Shader_Sky.fx"))))
		return E_FAIL;
	if (FAILED(Add_Prototype(SCENE_STATIC, L"Shader_Effect", CShader::Create(_pGraphicDev, L"../ShaderFiles/Shader_Effect.fx"))))
		return E_FAIL;
	if (FAILED(Add_Prototype(SCENE_STATIC, L"Shader_Blur", CShader::Create(_pGraphicDev, L"../ShaderFiles/Shader_Blur.fx"))))
		return E_FAIL;
	if (FAILED(Add_Prototype(SCENE_STATIC, L"Shader_Gizmo", CShader::Create(_pGraphicDev, L"../ShaderFiles/Shader_Gizmo.fx"))))
		return E_FAIL;
	if (FAILED(Add_Prototype(SCENE_STATIC, L"Shader_UI", CShader::Create(_pGraphicDev, L"../ShaderFiles/Shader_UI.fx"))))
		return E_FAIL;
	if (FAILED(Add_Prototype(SCENE_STATIC, L"Shader_SSAO", CShader::Create(_pGraphicDev, L"../ShaderFiles/Shader_SSAO.fx"))))
		return E_FAIL;
	if (FAILED(Add_Prototype(SCENE_STATIC, L"Shader_3dUI", CShader::Create(_pGraphicDev, L"../ShaderFiles/Shader_3dUI.fx"))))
		return E_FAIL;
	if (FAILED(Add_Prototype(SCENE_STATIC, L"Shader_Skinned", CShader::Create(_pGraphicDev, L"../ShaderFiles/Shader_Skinned.fx"))))
		return E_FAIL;

	//===============================================

	if (FAILED(Add_Prototype(SCENE_STATIC, L"Transform", CTransform::Create(_pGraphicDev))))
		return E_FAIL;

	if (FAILED(Add_Prototype(SCENE_STATIC, L"Collider", CCollider::Create(_pGraphicDev))))
		return E_FAIL;

	if (FAILED(Add_Prototype(SCENE_STATIC, L"Rigidbody", CRigidBody::Create(_pGraphicDev))))
		return E_FAIL;

	if (FAILED(Add_Prototype(SCENE_STATIC, L"Renderer", CRenderer::Create(_pGraphicDev))))
		return E_FAIL;

	if (FAILED(Add_Prototype(SCENE_STATIC, L"NavMesh", CNavMesh::Create(_pGraphicDev))))
		return E_FAIL;

	if (FAILED(Add_Prototype(SCENE_STATIC, L"AIController", CAIController::Create(_pGraphicDev))))
		return E_FAIL;

	if (FAILED(Add_Prototype(SCENE_STATIC, L"Optimization", COptimization::Create(_pGraphicDev))))
		return E_FAIL;

	if (FAILED(Add_Prototype(SCENE_STATIC, L"BattleAgent", CBattleAgent::Create(_pGraphicDev))))
		return E_FAIL;

	if (FAILED(Add_Prototype(SCENE_STATIC, L"VIBuffer_Rect", CBuffer_RcTex::Create(_pGraphicDev))))
		return E_FAIL;

	if (FAILED(Add_Prototype(SCENE_STATIC, L"VIBuffer_Terrain", CBuffer_Terrain::Create(_pGraphicDev, 512, 512, 1 ))))
		return E_FAIL;

	if (FAILED(Add_Prototype(SCENE_STATIC, L"VIBuffer_Cube", CBuffer_CubeTex::Create(_pGraphicDev))))
		return E_FAIL;

	if (FAILED(Add_Prototype(SCENE_STATIC, L"VIBuffer_Sphere", CBuffer_Sphere::Create(_pGraphicDev))))
		return E_FAIL;

	//===============================================

	_mat		LocalMatrix, ScaleMatrix, RotationMatrix;

	D3DXMatrixIdentity(&LocalMatrix);

	return S_OK;
}

HRESULT CComponent_Manager::Reserve_Container_Size(_uint iNumScenes)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_pPrototypes = new PROTOTYPES[iNumScenes];

	m_iNumScenes = iNumScenes;

	return NOERROR;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iSceneID, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (m_iNumScenes <= iSceneID || 
		nullptr == pPrototype || 
		nullptr == m_pPrototypes ||
		nullptr != Find_Prototype(iSceneID, pPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iSceneID].insert(PROTOTYPES::value_type(pPrototypeTag, pPrototype));

	return NOERROR;
}

CComponent * CComponent_Manager::Clone_Component(_uint iSceneID, const _tchar * pPrototypeTag, void * pArg)
{
	if (m_iNumScenes <= iSceneID || 
		nullptr == m_pPrototypes)
		return nullptr;

	// ��ʺ�����ã�´�.
	CComponent* pPrototype = Find_Prototype(iSceneID, pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	// ������ �����Ѵ�.
	return pPrototype->Clone_Component(pArg);
}

HRESULT CComponent_Manager::Clear_Instance(_uint iSceneIndex)
{
	if (m_iNumScenes <= iSceneIndex ||
		nullptr == m_pPrototypes)
		return E_FAIL;

	for (auto& Pair : m_pPrototypes[iSceneIndex])
		Safe_Release(Pair.second);

	m_pPrototypes[iSceneIndex].clear();

	return NOERROR;	
}

HRESULT CComponent_Manager::LoadMesh_FilesFromPath(_Device pGraphicDev, const _tchar * wstrImgPath)
{
	HANDLE hFile = CreateFile(wstrImgPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		::MSG_BOX("Load Failed. [INVALID_HANDLE_VALUE]");

	DWORD dwByte = 0;

	list<PATH_INFO*> tmpList;

	while (true)
	{
		PATH_INFO* tmpPath = new PATH_INFO;

		::ReadFile(hFile, &tmpPath->sztrStateKey, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::ReadFile(hFile, &tmpPath->sztrFileName, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::ReadFile(hFile, &tmpPath->sztrImgPath, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::ReadFile(hFile, &tmpPath->bIsDynamic, sizeof(_bool), &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete(tmpPath);
			break;
		}

		m_fMaxResCnt += 1.f;
		tmpList.push_back(tmpPath);
	}

	CloseHandle(hFile);

	CManagement* pManagement = CManagement::Get_Instance();
	Safe_AddRef(pManagement);

	_mat matDefault;
	D3DXMatrixIdentity(&matDefault);

	for (auto& iter : tmpList)
	{
		CComponent* pMeshCom = (iter->bIsDynamic == true ?
			static_cast<CComponent*>(CMesh_Dynamic::Create(pGraphicDev, iter->sztrImgPath, iter->sztrFileName, matDefault)) :
			static_cast<CComponent*>(CMesh_Static::Create(pGraphicDev, iter->sztrImgPath, iter->sztrFileName, matDefault)));

		Add_Prototype(SCENE_STATIC, iter->sztrStateKey, pMeshCom);
		m_fResCnt += 1.f;

		m_listMeshPathInfo.push_back(iter);
	}

	tmpList.clear();

	m_fResCnt = 0.f;
	m_fMaxResCnt = 0.f;

	Safe_Release(pManagement);

	return S_OK;

	//wifstream fin;
	//
	//list<PATH_INFO*> tmpList;
	//
	//fin.open(wstrImgPath);
	//
	//if (fin.fail())
	//	return E_FAIL;
	//
	//while (true)
	//{
	//	PATH_INFO* tmpPath = new PATH_INFO;
	//
	//	fin.getline(tmpPath->sztrStateKey, STR_128, '|');
	//	fin.getline(tmpPath->sztrFileName, STR_128, '|');
	//	fin.getline(tmpPath->sztrImgPath, STR_128, '|');
	//	fin.getline(tmpPath->szIsDynamic, STR_128);
	//
	//	if (fin.eof())
	//	{
	//		Safe_Delete(tmpPath);
	//		break;
	//	}
	//
	//	m_fMaxResCnt += 1.f;
	//	tmpList.push_back(tmpPath);
	//}
	//
	//fin.close();
	//
	//CManagement* pManagement = CManagement::Get_Instance();
	//Safe_AddRef(pManagement);
	//
	//_mat matDefault;
	//D3DXMatrixIdentity(&matDefault);
	//
	//for (auto& iter : tmpList)
	//{
	//	// ���� ������Ʈ �����ְ�
	//	lstrcpy(iter->szProtoTypeName, L"RenderObject_");
	//
	//	// GameObject_Mesh_ObjectName �������� ������Ÿ�� Ű�� ����
	//	lstrcat(iter->szProtoTypeName, iter->sztrStateKey);
	//
	//	// ������Ÿ������ �ͱ۾����
	//	pManagement->Add_Prototype(iter->szProtoTypeName, CRenderObject::Create(pGraphicDev));
	//
	//	CComponent* pMeshCom = (lstrcmp(iter->szIsDynamic, L"0") ? 
	//		static_cast<CComponent*>(CMesh_Dynamic::Create(pGraphicDev, iter->sztrImgPath, iter->sztrFileName, matDefault)) :
	//		static_cast<CComponent*>(CMesh_Static::Create(pGraphicDev, iter->sztrImgPath, iter->sztrFileName, matDefault)));
	//
	//	Add_Prototype(SCENE_STATIC, iter->sztrStateKey, pMeshCom);
	//	m_fResCnt += 1.f;
	//
	//	m_listMeshPathInfo.push_back(iter);
	//}
	//
	//tmpList.clear();
	//
	//m_fResCnt = 0.f;
	//m_fMaxResCnt = 0.f;
	//
	//Safe_Release(pManagement);
	//
	//return S_OK;
}

HRESULT CComponent_Manager::LoadMesh_FilesFromPath_AddProtoRenderObj(_Device pGraphicDev, const _tchar * szImgPath)
{
	HANDLE hFile = CreateFile(szImgPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		::MSG_BOX("Load Failed. [INVALID_HANDLE_VALUE]");

	DWORD dwByte = 0;

	list<PATH_INFO*> tmpList;

	while (true)
	{
		PATH_INFO* tmpPath = new PATH_INFO;

		::ReadFile(hFile, &tmpPath->sztrStateKey, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::ReadFile(hFile, &tmpPath->sztrFileName, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::ReadFile(hFile, &tmpPath->sztrImgPath, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::ReadFile(hFile, &tmpPath->bIsDynamic, sizeof(_bool), &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete(tmpPath);
			break;
		}

		m_fMaxResCnt += 1.f;
		tmpList.push_back(tmpPath);
	}

	CloseHandle(hFile);

	CManagement* pManagement = CManagement::Get_Instance();
	Safe_AddRef(pManagement);

	_mat matDefault;
	D3DXMatrixIdentity(&matDefault);

	for (auto& iter : tmpList)
	{
		// ���� ������Ʈ �����ְ�
		lstrcpy(iter->szProtoTypeName, L"RenderObject_");

		// GameObject_Mesh_ObjectName �������� ������Ÿ�� Ű�� ����
		lstrcat(iter->szProtoTypeName, iter->sztrStateKey);

		// ���� �ܿ���� ���
		if (!lstrcmp(iter->sztrStateKey, L"Mesh_Mistletoe"))
		{
			// �ܿ���� �߰�
			pManagement->Add_Prototype(iter->szProtoTypeName, CActiveObject::Create(pGraphicDev));
		}

		else
		{
			// ������Ÿ������ �ͱ۾����
			pManagement->Add_Prototype(iter->szProtoTypeName, CRenderObject::Create(pGraphicDev));
		}

		// �ش� �̸����� �޽��� ����,
		CComponent* pMeshCom = static_cast<CComponent*>(CMesh_Static::Create(pGraphicDev, iter->sztrImgPath, iter->sztrFileName, matDefault));

		// �޽� ������ Ÿ�Ե� �������
		Add_Prototype(SCENE_STATIC, iter->sztrStateKey, pMeshCom);
		m_fResCnt += 1.f;

		m_listMeshPathInfo.push_back(iter);
	}

	tmpList.clear();

	m_fResCnt = 0.f;
	m_fMaxResCnt = 0.f;

	Safe_Release(pManagement);

	return S_OK;

	return S_OK;
}

HRESULT CComponent_Manager::LoadTex_FilesFromPath(_Device pGraphicDev, const _tchar * szImgPath)
{
	HANDLE hFile = CreateFile(szImgPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		::MSG_BOX("Load Failed. [INVALID_HANDLE_VALUE]");

	DWORD dwByte = 0;

	list<PATH_INFO*> tmpList;

	while (true)
	{
		PATH_INFO* tmpPath = new PATH_INFO;

		::ReadFile(hFile, &tmpPath->sztrStateKey, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::ReadFile(hFile, &tmpPath->sztrFileName, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::ReadFile(hFile, &tmpPath->sztrImgPath, sizeof(_tchar) * STR_128, &dwByte, nullptr);
		::ReadFile(hFile, &tmpPath->sImgCnt, sizeof(_ushort), &dwByte, nullptr);

		if (0 == dwByte)
		{
			Safe_Delete(tmpPath);
			break;
		}

		m_fMaxResCnt += 1.f;
		tmpList.push_back(tmpPath);
	}

	CloseHandle(hFile);

	for (auto& iter : tmpList)
	{
		_tchar szPath[STR_256] = L"";

		lstrcpy(szPath, iter->sztrImgPath);
		lstrcat(szPath, iter->sztrFileName);

		Add_Prototype(SCENE_STATIC, iter->sztrStateKey, CTexture::Create(pGraphicDev, CTexture::TYPE_GENERAL, szPath, iter->sImgCnt));
		++m_fResCnt;

		m_fResPercent = m_fResCnt / m_fMaxResCnt;

		m_listTexturePathInfo.push_back(iter);
	}

	tmpList.clear();

	m_fResCnt = 0.f;
	m_fMaxResCnt = 0.f;

	return S_OK;
}

HRESULT CComponent_Manager::LoadTex_FilesFromPath_Tool(_Device pGraphicDev, const _tchar * szImgPath)
{
	wifstream fin;
	
	list<PATH_INFO*> tmpList;
	
	
	fin.open(szImgPath);
	
	if (fin.fail())
		return E_FAIL;
	
	while (true)
	{
		PATH_INFO* tmpPath = new PATH_INFO;
	
		fin.getline(tmpPath->sztrStateKey, STR_128, '|');
		fin.getline(tmpPath->sztrFileName, STR_128, '|');
		fin.getline(tmpPath->sztrImgPath, STR_128, '|');
		fin.getline(tmpPath->szIsDynamic, STR_128, '|');
		fin.getline(tmpPath->szImgCnt, STR_128);
	
		if (fin.eof())
		{
			Safe_Delete(tmpPath);
			break;
		}
	
		++m_fMaxResCnt;
		tmpList.push_back(tmpPath);
	}
	
	fin.close();
	
	
	for (auto& iter : tmpList)
	{
		lstrcat(iter->sztrImgPath, iter->sztrFileName);
	
		Add_Prototype(SCENE_STATIC, iter->sztrStateKey, CTexture::Create(pGraphicDev, CTexture::TYPE_GENERAL, iter->sztrImgPath, _wtoi(iter->szImgCnt)));
		++m_fResCnt;
	
		m_fResPercent = m_fResCnt / m_fMaxResCnt;
	
		m_listTexturePathInfo.push_back(iter);
	}
	
	tmpList.clear();
	
	m_fResCnt = 0.f;
	m_fMaxResCnt = 0.f;

	return S_OK;
}

CComponent * CComponent_Manager::Find_Prototype(_uint iSceneID, const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_pPrototypes[iSceneID].begin(), m_pPrototypes[iSceneID].end(), CTag_Finder(pPrototypeTag));

	if (iter == m_pPrototypes[iSceneID].end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	for (size_t i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])		
			Safe_Release(Pair.second);

		m_pPrototypes[i].clear();		
	}

	Safe_Delete_Array(m_pPrototypes);

	for (auto& iter : m_listMeshPathInfo)
	{
		Safe_Delete(iter);
	}

	m_listMeshPathInfo.clear();

	for (auto& iter : m_listTexturePathInfo)
	{
		Safe_Delete(iter);
	}

	m_listTexturePathInfo.clear();
}
