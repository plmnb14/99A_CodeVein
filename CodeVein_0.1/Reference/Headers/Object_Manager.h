#pragma once

#include "Layer.h"
#include "RenderObject.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
private:
	explicit CObject_Manager();
	virtual ~CObject_Manager() = default;
public:
	CGameObject* Get_GameObjectBack(const _tchar* pLayerTag, _uint iSceneID);
	list<CGameObject*> Get_GameObjectList(const _tchar* pLayerTag, _uint iSceneID);


public:
	HRESULT		 Reserve_Container_Size(_uint iNumScenes);
	HRESULT		 Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype);
	HRESULT		 Add_GameObject_ToLayer(const _tchar* pPrototypeTag, _uint iSceneID, const _tchar* pLayerTag, void* pArg);

	CGameObject* Clone_GameObject_Return(const _tchar* pPrototypeTag, void* pArg);
	HRESULT		Add_GameOject_ToLayer_NoClone(CGameObject* _pGameObject , _uint iSceneID, const _tchar* pLayerTag, void* pArg);

	_int Update_Object_Manager(_double TimeDelta);
	_int Late_Update_Object_Manager(_double TimeDelta);
	HRESULT Clear_Instance(_uint iSceneIndex);

public:
	HRESULT LoadObjectPrototypes_FromPath(_Device pGraphicDev, const _tchar* szImgPath);

private:
	// 게임내에 생성될 수 있는 객체들의 원형.
	map<const _tchar*, CGameObject*>			m_Prototypes;
	typedef map<const _tchar*, CGameObject*>	PROTOTYPES;
private:
	map<const _tchar*, CLayer*>*				m_pLayers = nullptr;
	typedef map<const _tchar*, CLayer*>			LAYERS;

	_uint			m_iNumScenes = 0;
private:
	CGameObject* Find_Prototype(const _tchar* pPrototypeTag);
	CLayer*	Find_Layer(_uint iSceneID, const _tchar* pLayerTag);
public:
	virtual void Free();
};

END