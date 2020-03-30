#include "stdafx.h"
#include "Scene_Stage_Training.h"

#include "CameraMgr.h"
#include "Effect.h"
#include "UI.h"
#include "UI_Manager.h"
#include "ParticleMgr.h"

CScene_Stage_Training::CScene_Stage_Training(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{

}

HRESULT CScene_Stage_Training::Ready_Scene()
{
	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Dummy(L"Layer_Dummy")))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_Monster(L"Layer_Monster")))
		return E_FAIL;

	// Æ®·¹ÀÌ´× ¸ÊÀº ±×³É ·Îµå °¡´ÉÇØ¿è
	g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Stage_Training.dat");

	m_pNavMesh = static_cast<Engine::CNavMesh*>(g_pManagement->Clone_Component(SCENE_STATIC, L"NavMesh"));
	m_pNavMesh->Ready_NaviMesh(m_pGraphic_Device, L"Navmesh_Training.dat");

	CNavMesh* pNavMesh = static_cast<Engine::CNavMesh*>(g_pManagement->Get_GameObjectBack(L"Layer_Player" , SCENE_STAGE)->Get_Component(L"NavMesh"));
	pNavMesh->Reset_NaviMesh();
	pNavMesh->Ready_NaviMesh(m_pGraphic_Device , L"Navmesh_Training.dat");

	return S_OK;
}

_int CScene_Stage_Training::Update_Scene(_double TimeDelta)
{
	CUI_Manager::Get_Instance()->Update_UI();

	return _int();
}

HRESULT CScene_Stage_Training::Render_Scene()
{
	IF_NOT_NULL(m_pNavMesh)
		m_pNavMesh->Render_NaviMesh();

	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Player(const _tchar * pLayerTag)
{
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Player", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Dummy(const _tchar * pLayerTag)
{
	// µð¹ö±ë¿ë ´õ¹Ì Å¸°Ù
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Dummy", SCENE_STAGE, pLayerTag)))
		return E_FAIL;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////


	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_PoisonButterfly", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_BlackUrchin", SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;

	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_BlackWolf", SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;

	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"Monster_NormalGenji", SCENE_STAGE, pLayerTag)))
	//	return E_FAIL;

	CParticleMgr::Get_Instance()->Ready_ParticleManager();

	return S_OK;
}


HRESULT CScene_Stage_Training::Ready_LightDesc()
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

CScene_Stage_Training * CScene_Stage_Training::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _bool _bLoadStatic)
{
	CScene_Stage_Training*	pInstance = new CScene_Stage_Training(pGraphic_Device);

	pInstance->m_bLoadStaticMesh = _bLoadStatic;

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("Failed To Creating CScene_Stage_Training");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Stage_Training::Free()
{
	Safe_Release(m_pNavMesh);

	CScene::Free();
}

