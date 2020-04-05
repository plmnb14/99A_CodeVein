#include "stdafx.h"
#include "Scene_Stage_01.h"

#include "CameraMgr.h"
#include "Effect.h"
#include "UI.h"
#include "UI_Manager.h"
#include "ParticleMgr.h"

CScene_Stage_01::CScene_Stage_01(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{

}

HRESULT CScene_Stage_01::Ready_Scene()
{
	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (m_bLoadStaticMesh)
		g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Object_Stage_01.dat");

	m_pNavMesh = static_cast<Engine::CNavMesh*>(g_pManagement->Clone_Component(SCENE_STATIC, L"NavMesh"));

	m_pNavMesh->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Stage_01.dat");

	return S_OK;
}

_int CScene_Stage_01::Update_Scene(_double TimeDelta)
{
	CUI_Manager::Get_Instance()->Update_UI();

	return _int();
}

HRESULT CScene_Stage_01::Render_Scene()
{
	IF_NOT_NULL(m_pNavMesh)
		m_pNavMesh->Render_NaviMesh();

	return S_OK;
}

HRESULT CScene_Stage_01::Ready_Layer_Player(const _tchar * pLayerTag)
{
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Player", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	return S_OK;
}


HRESULT CScene_Stage_01::Ready_LightDesc()
{
	D3DLIGHT9		LightDesc;
	ZeroMemory(&LightDesc, sizeof(D3DLIGHT9));

	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	// In.WorldSpace
	LightDesc.Direction = _v3(1.f, 1.f, -1.f);

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc)))
		return E_FAIL;

	return NOERROR;
}

CScene_Stage_01 * CScene_Stage_01::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _bool _bLoadStatic)
{
	CScene_Stage_01*	pInstance = new CScene_Stage_01(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("Failed To Creating CScene_Stage_01");
		Safe_Release(pInstance);
	}

	pInstance->m_bLoadStaticMesh = _bLoadStatic;

	return pInstance;
}

void CScene_Stage_01::Free()
{
	Safe_Release(m_pNavMesh);

	CScene::Free();
}

