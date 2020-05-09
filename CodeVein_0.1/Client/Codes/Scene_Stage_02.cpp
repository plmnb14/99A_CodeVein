#include "stdafx.h"
#include "Scene_Stage_02.h"

#include "CameraMgr.h"
#include "Effect.h"
#include "UI.h"
#include "UI_Manager.h"
#include "ParticleMgr.h"
#include "ScriptManager.h"

#include "Player.h"

#include "Scene_Stage_Base.h"

CScene_Stage_02::CScene_Stage_02(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{

}

HRESULT CScene_Stage_02::Ready_Scene()
{
	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Environment(L"Layer_Environment")))
		return E_FAIL;

	//if (m_bLoadStaticMesh)
	g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Object_Stage_02.dat");

	CScriptManager::Get_Instance()->Set_StageIdx(1);
	CScriptManager::Get_Instance()->Ready_Script_DynamicObject(1);

	return S_OK;
}

_int CScene_Stage_02::Update_Scene(_double TimeDelta)
{
	CUI_Manager::Get_Instance()->Update_UI();

	return _int();
}

HRESULT CScene_Stage_02::Render_Scene()
{
	if (g_pInput_Device->Key_Down(DIK_H))
	{
		g_eSceneID_Cur = SCENE_STAGE_BASE;
		g_eSTeleportID_Cur = TeleportID_Home_1;

		CGameObject* pInstance = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);

		pInstance->Set_Enable(false);

		g_pManagement->Clear_LightList();

		if (FAILED(g_pManagement->Clear_Instance(SCENE_STAGE)))
			return -1;

		CScene* pScene = CScene_Stage_Base::Create(m_pGraphic_Device, m_bLoadStaticMesh);

		if (FAILED(g_pManagement->SetUp_CurrentScene(pScene)))
			return -1;
	}

	return S_OK;
}

HRESULT CScene_Stage_02::Ready_Layer_Player(const _tchar * pLayerTag)
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

HRESULT CScene_Stage_02::Ready_Layer_Environment(const _tchar* pLayerTag)
{
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Sky", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_BossMassageUI", SCENE_STAGE, L"Layer_BossMassageUI")))
		return E_FAIL;

	return S_OK;
}


HRESULT CScene_Stage_02::Ready_LightDesc()
{
	NEW_LIGHT		LightDesc;
	ZeroMemory(&LightDesc, sizeof(NEW_LIGHT));

	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 0.882f, 0.801f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	// In.WorldSpace
	_v3 vLightDir = _v3(0.2f, 1.f, 0.9f);

	LightDesc.fAlpha = 1.f;

	V3_NORMAL_SELF(&vLightDir);

	LightDesc.Direction = vLightDir;
	//LightDesc.Direction = _v3(0.0f, 0.f, 1.f);

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc, CLight_Manager::Static_Light)))
		return E_FAIL;

	return NOERROR;
}

CScene_Stage_02 * CScene_Stage_02::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _bool _bLoadStatic)
{
	CScene_Stage_02*	pInstance = new CScene_Stage_02(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("Failed To Creating CScene_Stage_02");
		Safe_Release(pInstance);
	}

	pInstance->m_bLoadStaticMesh = _bLoadStatic;

	return pInstance;
}

void CScene_Stage_02::Free()
{
	//Safe_Release(m_pNavMesh);

	CScene::Free();
}

