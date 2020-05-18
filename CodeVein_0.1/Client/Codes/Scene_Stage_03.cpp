#include "stdafx.h"
#include "Scene_Stage_03.h"
#include "Scene_Stage_Base.h"

#include "CameraMgr.h"
#include "Effect.h"
#include "UI.h"
#include "UI_Manager.h"
#include "ParticleMgr.h"
#include "ScriptManager.h"

#include "Player.h"

#include "Player_Colleague.h"

CScene_Stage_03::CScene_Stage_03(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{

}

HRESULT CScene_Stage_03::Ready_Scene()
{
	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	/*if (FAILED(Ready_Layer_Colleague(L"Layer_Colleague")))
		return E_FAIL;*/

	if (FAILED(Ready_Layer_Environment(L"Layer_Environment")))
		return E_FAIL;

	CRenderer* pRenderer = static_cast<CRenderer*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Renderer"));
	pRenderer->Fog_On(true);
	Safe_Release(pRenderer);

	g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Object_Stage_03.dat");

	return S_OK;
}

_int CScene_Stage_03::Update_Scene(_double TimeDelta)
{
	CUI_Manager::Get_Instance()->Update_UI();

	return _int();
}

HRESULT CScene_Stage_03::Render_Scene()
{
	return S_OK;
}

HRESULT CScene_Stage_03::Ready_Layer_Player(const _tchar * pLayerTag)
{
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Mistletoe")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Monster")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Boss")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_MonsterProjectile")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_BossUI")))
		return E_FAIL;

	CPlayer* pInstance = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

	pInstance->Teleport_ResetOptions(g_eSceneID_Cur, g_eSTeleportID_Cur);
	pInstance->Set_Enable(true);

	pInstance = nullptr;

	return S_OK;
}

HRESULT CScene_Stage_03::Ready_Layer_Environment(const _tchar* pLayerTag)
{
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Sky", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Sky_Blur", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_BossMassageUI", SCENE_STAGE, L"Layer_BossMassageUI")))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage_03::Ready_Layer_Colleague(const _tchar * pLayerTag)
{
	// ±èÀç±¸ Ãß°¡
	CGameObject* pInstance = nullptr;

	pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Colleague",
		&CPlayer_Colleague::JACK_INFO(_v3(-100.f, 50.f, 0.f), 0.f, 3));
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Colleague", nullptr);

	return S_OK;
}


HRESULT CScene_Stage_03::Ready_LightDesc()
{
	NEW_LIGHT		LightDesc;
	ZeroMemory(&LightDesc, sizeof(NEW_LIGHT));

	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	// In.WorldSpace
	_v3 vLightDir = _v3(1.f, -1.f, 0.f);

	LightDesc.fAlpha = 1.f;

	V3_NORMAL_SELF(&vLightDir);

	LightDesc.Direction = vLightDir;
	//LightDesc.Direction = _v3(0.0f, 0.f, 1.f);

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc, CLight_Manager::Static_Light)))
		return E_FAIL;

	return NOERROR;
}

CScene_Stage_03 * CScene_Stage_03::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _bool _bLoadStatic)
{
	CScene_Stage_03*	pInstance = new CScene_Stage_03(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("Failed To Creating CScene_Stage_03");
		Safe_Release(pInstance);
	}

	pInstance->m_bLoadStaticMesh = _bLoadStatic;

	return pInstance;
}

void CScene_Stage_03::Free()
{
	//Safe_Release(m_pNavMesh);

	CScene::Free();
}

