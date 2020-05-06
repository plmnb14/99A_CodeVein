#include "..\Headers\Object_Manager.h"
#include "Transform.h"
#include "Management.h"

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

list<CGameObject*>& CObject_Manager::Get_GameObjectList(const _tchar * pLayerTag, _uint iSceneID)
{
	CLayer* pLayer = Find_Layer(iSceneID, pLayerTag);

	return pLayer->Get_GameObjectList();
}

HRESULT CObject_Manager::Reserve_Container_Size(_uint iNumScenes)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumScenes];

	m_iNumScenes = iNumScenes;

	return S_OK;
}

HRESULT CObject_Manager::Add_Layer(_uint iSceneID, const _tchar * pLayerTag)
{
	CLayer*	pLayer = Find_Layer(iSceneID, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		m_pLayers[iSceneID].insert(LAYERS::value_type(pLayerTag, pLayer));
	}

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

CGameObject* CObject_Manager::Clone_GameObject_Return(const _tchar * pPrototypeTag, void * pArg)
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

	_int	iIndex = 0;

	_float fA[3] = {};
	_float fB[3] = {};
	_float fC[3] = {};

	_v3    vVtx[3];

	_tchar  szPath[STR_128] = L"../../Data/Load_StageData/";

	CManagement* pManagement = CManagement::Get_Instance();
	Safe_AddRef(pManagement);

	// 파일명과 경로를 조합하여 최종 파일경로 산출 
	lstrcat(szPath, szImgPath);

	fin.open(szPath);

	if (fin.fail())
	{
		Safe_Release(pManagement);
		return S_OK;
	}

	while (true)
	{
		Engine::OBJ_INFO* ObjInfo = new Engine::OBJ_INFO;

		fin.getline(ObjInfo->szName, STR_128, '|');
		fin.getline(ObjInfo->szIndex, STR_128, '|');
		fin.getline(ObjInfo->szTag, STR_128, '|');

		fin.getline(ObjInfo->szPos_X, STR_128, '|');
		fin.getline(ObjInfo->szPos_Y, STR_128, '|');
		fin.getline(ObjInfo->szPos_Z, STR_128, '|');

		fin.getline(ObjInfo->szRot_X, STR_128, '|');
		fin.getline(ObjInfo->szRot_Y, STR_128, '|');
		fin.getline(ObjInfo->szRot_Z, STR_128, '|');

		fin.getline(ObjInfo->szScale_X, STR_128, '|');
		fin.getline(ObjInfo->szScale_Y, STR_128, '|');
		fin.getline(ObjInfo->szScale_Z, STR_128);


		if (fin.eof())
		{
			Engine::Safe_Delete(ObjInfo);
			break;
		}

		else
		{
			iIndex = _wtoi(ObjInfo->szIndex);

			vVtx[0].x = (_float)_wtof(ObjInfo->szPos_X);
			vVtx[0].y = (_float)_wtof(ObjInfo->szPos_Y);
			vVtx[0].z = (_float)_wtof(ObjInfo->szPos_Z);

			vVtx[1].x = (_float)_wtof(ObjInfo->szRot_X);
			vVtx[1].y = (_float)_wtof(ObjInfo->szRot_Y);
			vVtx[1].z = (_float)_wtof(ObjInfo->szRot_Z);

			vVtx[2].x = (_float)_wtof(ObjInfo->szScale_X);
			vVtx[2].y = (_float)_wtof(ObjInfo->szScale_Y);
			vVtx[2].z = (_float)_wtof(ObjInfo->szScale_Z);


			_tchar szObjName[STR_128] = L"";
			lstrcpy(szObjName, ObjInfo->szName);

			// 프로토타입 이름으로 만들고
			_tchar tmpProtoName[STR_128] = L"RenderObject_";
			lstrcat(tmpProtoName, ObjInfo->szName);

			if (!(lstrcmp(szObjName, L"Mesh_Mistletoe")))
			{
				pManagement->Add_GameObject_ToLayer(tmpProtoName, SCENE_STAGE, L"Layer_Mistletoe", 
					&CActiveObject::ACTOBJ_INFO(ObjInfo->szName, vVtx[0], vVtx[1], vVtx[2], iIndex , 0));
			}

			else if (!(lstrcmp(szObjName, L"Mesh_Itembox")))
			{
				pManagement->Add_GameObject_ToLayer(tmpProtoName, SCENE_STAGE, L"Layer_ItemBox",
					&CActiveObject::ACTOBJ_INFO(ObjInfo->szName, vVtx[0], vVtx[1], vVtx[2], iIndex, 2));
			}

			else if (!(lstrcmp(szObjName, L"Mesh_Itembox_Lid")))
			{
				pManagement->Add_GameObject_ToLayer(tmpProtoName, SCENE_STAGE, L"Layer_ItemBox",
					&CActiveObject::ACTOBJ_INFO(ObjInfo->szName, vVtx[0], vVtx[1], vVtx[2], iIndex, 3));
			}
			else
			{
				pManagement->Add_GameObject_ToLayer(tmpProtoName, SCENE_STAGE, L"Layer_Render", 
					&CRenderObject::OBJ_INFO(ObjInfo->szName, vVtx[0], vVtx[1], vVtx[2], iIndex));
			}

			Engine::Safe_Delete(ObjInfo);
		}
	}

	fin.close();

	Safe_Release(pManagement);

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
