#pragma once

// 컴포넌트 "원형" 을 보관한다.

#include "Buffer_RcTex.h"
#include "Buffer_Terrain.h"
#include "Buffer_CubeTex.h"
#include "Buffer_Sphere.h"

#include "Renderer.h"
#include "Transform.h"
#include "Collider.h"
#include "Shader.h"
#include "Mesh_Dynamic.h"
#include "Mesh_Static.h"
#include "Texture.h"
#include "RigidBody.h"
#include "NavMesh.h"
#include "AIController.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	explicit CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Ready_Component_Manager(_Device _pGraphicDev);
	HRESULT Reserve_Container_Size(_uint iNumScenes);
	HRESULT Add_Prototype(_uint iSceneID, const _tchar* pPrototypeTag, CComponent* pPrototype);
	CComponent* Clone_Component(_uint iSceneID, const _tchar* pPrototypeTag, void* pArg); // 원본을 찾고, 복제하여 리턴한다.
	HRESULT Clear_Instance(_uint iSceneIndex);

public:
	HRESULT LoadMesh_FilesFromPath(_Device pGraphicDev, const _tchar* szImgPath);
	HRESULT LoadTex_FilesFromPath(_Device pGraphicDev, const _tchar* szImgPath);

private:
	map<const _tchar*, CComponent*>*		m_pPrototypes = nullptr;
	typedef map<const _tchar*, CComponent*>	PROTOTYPES;
private:
	_uint		m_iNumScenes = 0;

private:
	list<Engine::PATH_INFO*> m_listMeshPathInfo;
	list<Engine::PATH_INFO*> m_listTexturePathInfo;

	_float					 m_fResCnt = 0.f;
	_float					 m_fMaxResCnt = 0.f;;
	_float					 m_fResPercent = 0.f;;

private:
	CComponent* Find_Prototype(_uint iSceneID, const _tchar* pPrototypeTag);
public:
	virtual void Free();
};

END