#include "..\Headers\Object_Manager.h"
#include "Transform.h"
//#include "Management.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{

}

CGameObject* CObject_Manager::Get_GameObjectBack(const _tchar* pLayerTag, _uint iSceneID)
{
	CLayer* pLayer = Find_Layer(iSceneID, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_GameObjectBack();
}

HRESULT CObject_Manager::Reserve_Container_Size(_uint iNumScenes)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumScenes];

	m_iNumScenes = iNumScenes;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == pPrototype)
		return E_FAIL;

	if (nullptr != Find_Prototype(pPrototypeTag))
		return E_FAIL;

	m_Prototypes.insert(PROTOTYPES::value_type(pPrototypeTag, pPrototype));		

	return NOERROR;
}

HRESULT CObject_Manager::Add_GameObject_ToLayer(const _tchar * pPrototypeTag, _uint iSceneID, const _tchar * pLayerTag, void * pArg)
{
	CGameObject*	pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject*	pGameObject = pPrototype->Clone_GameObject(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer*	pLayer = Find_Layer(iSceneID, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iSceneID].insert(LAYERS::value_type(pLayerTag, pLayer));
	}
	else	
		pLayer->Add_GameObject(pGameObject);	

	return NOERROR;
}

CGameObject * CObject_Manager::Clone_GameObject_Return(const _tchar * pPrototypeTag, void * pArg)
{
	CGameObject*	pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject*	pGameObject = pPrototype->Clone_GameObject(pArg);
	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;
}

HRESULT CObject_Manager::Add_GameOject_ToLayer_NoClone(CGameObject * _pGameObject, _uint iSceneID, const _tchar * pLayerTag, void * pArg)
{
	CLayer*	pLayer = Find_Layer(iSceneID, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		pLayer->Add_GameObject(_pGameObject);

		m_pLayers[iSceneID].insert(LAYERS::value_type(pLayerTag, pLayer));
	}
	else
		pLayer->Add_GameObject(_pGameObject);

	return NOERROR;
}

_int CObject_Manager::Update_Object_Manager(_double TimeDelta)
{
	_int	iProgress = 0;

	for (size_t i = 0; i < m_iNumScenes; ++i)
	{		
		for (auto& Pair : m_pLayers[i])
		{
			iProgress = Pair.second->Update_Layer(TimeDelta);

			if (0 > iProgress)
				return iProgress;
		}
	}

	return _int(iProgress);
}

_int CObject_Manager::Late_Update_Object_Manager(_double TimeDelta)
{
	_int	iProgress = 0;

	for (size_t i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			iProgress = Pair.second->Late_Update_Layer(TimeDelta);

			if (0 > iProgress)
				return iProgress;
		}
	}

	return _int(iProgress);
}

HRESULT CObject_Manager::Clear_Instance(_uint iSceneIndex)
{
	if (m_iNumScenes <= iSceneIndex || 
		nullptr == m_pLayers)
		return E_FAIL;

	for (auto& Pair : m_pLayers[iSceneIndex])
		Safe_Release(Pair.second);

	m_pLayers[iSceneIndex].clear();	

	return NOERROR;
}

HRESULT CObject_Manager::LoadObjectPrototypes_FromPath(_Device pGraphicDev, const _tchar * szImgPath)
{
	wifstream fin;

	_int	iIndex;

	_float fA[3];
	_float fB[3];
	_float fC[3];

	_v3    vVtx[3];

	_tchar  szPath[MAX_STR] = L"../../Data/";

	// 파일명과 경로를 조합하여 최종 파일경로 산출 
	lstrcat(szPath, szImgPath);

	fin.open(szPath);

	if (fin.fail())
		return S_OK;

	while (true)
	{
		Engine::OBJ_INFO* ObjInfo = new Engine::OBJ_INFO;

		fin.getline(ObjInfo->szName, MAX_STR, '|');
		fin.getline(ObjInfo->szIndex, MAX_STR, '|');
		fin.getline(ObjInfo->szTag, MAX_STR, '|');

		fin.getline(ObjInfo->szPos_X, MAX_STR, '|');
		fin.getline(ObjInfo->szPos_Y, MAX_STR, '|');
		fin.getline(ObjInfo->szPos_Z, MAX_STR, '|');

		fin.getline(ObjInfo->szRot_X, MAX_STR, '|');
		fin.getline(ObjInfo->szRot_Y, MAX_STR, '|');
		fin.getline(ObjInfo->szRot_Z, MAX_STR, '|');

		fin.getline(ObjInfo->szScale_X, MAX_STR, '|');
		fin.getline(ObjInfo->szScale_Y, MAX_STR, '|');
		fin.getline(ObjInfo->szScale_Z, MAX_STR);


		if (fin.eof())
		{
			Engine::Safe_Delete(ObjInfo);
			break;
		}

		else
		{
			iIndex = _wtoi(ObjInfo->szIndex);
			
			fA[0] = (_float)_wtof(ObjInfo->szPos_X);
			fA[1] = (_float)_wtof(ObjInfo->szPos_Y);
			fA[2] = (_float)_wtof(ObjInfo->szPos_Z);

			fB[0] = (_float)_wtof(ObjInfo->szRot_X);
			fB[1] = (_float)_wtof(ObjInfo->szRot_Y);
			fB[2] = (_float)_wtof(ObjInfo->szRot_Z);

			fC[0] = (_float)_wtof(ObjInfo->szScale_X);
			fC[1] = (_float)_wtof(ObjInfo->szScale_Y);
			fC[2] = (_float)_wtof(ObjInfo->szScale_Z);

			vVtx[0] = { fA[0], fA[1], fA[2] };
			vVtx[1] = { fB[0], fB[1], fB[2] };
			vVtx[2] = { fC[0], fC[1], fC[2] };


			Engine::CRenderObject*	pInstance = Engine::CRenderObject::Create(pGraphicDev);

			pInstance->Change_Mesh(ObjInfo->szName);
			pInstance->Set_Index(iIndex);

			TARGET_TO_TRANS(pInstance)->Set_Pos(vVtx[0]);
			TARGET_TO_TRANS(pInstance)->Set_Angle(vVtx[1]);
			TARGET_TO_TRANS(pInstance)->Set_Scale(vVtx[2]);

			Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Render", NULL);


			_tchar szObjName[MAX_STR] = L"";
			lstrcpy(szObjName, ObjInfo->szName);
			lstrcat(szObjName, ObjInfo->szIndex);

			pInstance = nullptr;
			Engine::Safe_Delete(ObjInfo);
		}
	}

	fin.close();

	return S_OK;
}

CGameObject * CObject_Manager::Find_Prototype(const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTag_Finder(pPrototypeTag));

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iSceneID, const _tchar * pLayerTag)
{
	auto	iter = find_if(m_pLayers[iSceneID].begin(), m_pLayers[iSceneID].end(), CTag_Finder(pLayerTag));
	if (iter == m_pLayers[iSceneID].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	for (auto& Pair : m_Prototypes)
	{
		Safe_Release(Pair.second);
	}
	m_Prototypes.clear();

	for (size_t i = 0; i < m_iNumScenes; ++i)
	{
		for (auto& Pair : m_pLayers[i])		
			Safe_Release(Pair.second);

		m_pLayers[i].clear();		
	}

	Safe_Delete_Array(m_pLayers);	
}
