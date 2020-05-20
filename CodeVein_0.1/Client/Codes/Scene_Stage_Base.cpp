#include "stdafx.h"
#include "Scene_Stage_Base.h"
#include "Scene_Stage_Training.h"
#include "Scene_Stage_01.h"
#include "Scene_Stage_02.h"
#include "Scene_Stage_03.h"
#include "Scene_Stage_04.h"

#include "UI_Manager.h"
#include "CameraMgr.h"
#include "Player.h"

#include "NPC_Yakumo.h"
#include "NPC_Yokumo.h"

#include "CustomCategory.h"
#include "CustomCategoryOption.h"
#include "CustomInven.h"
#include "CustomSlot.h"
#include "CustomSliderBar.h"

CScene_Stage_Base::CScene_Stage_Base(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{

}

HRESULT CScene_Stage_Base::Ready_Scene()
{
	// 빛 세팅
	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_Sky")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Custom(L"Layer_Custom")))
		return E_FAIL;

	CRenderer* pRenderer = static_cast<CRenderer*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Renderer"));
	pRenderer->Fog_On(true);
	pRenderer->Set_FogDestiny(0.01f);
	pRenderer->Set_UseLinearFog(true);
	Safe_Release(pRenderer);

	g_pSoundManager->Stop_Sound(CSoundManager::Ambient_01);
	g_pSoundManager->Play_Sound(L"AMB_BASE_000.ogg", CSoundManager::Ambient_01, CSoundManager::Ambient_Sound);

	// 플레이어의 네비 메쉬도 바꿔줍니다.
	Ready_Player();

	if (false == g_bOnStage[7])
	{
		m_pLoading = CLoading::Create(m_pGraphic_Device, SCENE_PREPARE_ALL);
	}

	g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Object_Stage_00.dat");

	//////////////////////////////////////////////////////////////////////////
	// NPC
	//////////////////////////////////////////////////////////////////////////
	// 야쿠모
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_NPC_Yakumo", SCENE_STAGE, L"Layer_NPC", &CNPC_Yakumo::NPC_INFO(_v3(-4.46f, -1.37f, -5.294f), D3DXToRadian(89.f)));
	
	// NPC 1 요쿠모
	//g_pManagement->Add_GameObject_ToLayer(L"GameObject_NPC_Yokumo", SCENE_STAGE, L"Layer_NPC", &CNPC_Yokumo::NPC_INFO(_v3(-10.5f, -1.37f, -14.3f), D3DXToRadian(45.f)));

	//
	//// NPC 2
	//g_pManagement->Add_GameObject_ToLayer(L"GameObject_NPC_Yakumo", SCENE_STAGE, L"Layer_NPC", &CNPC_Yakumo::NPC_INFO(_v3(6.283f, -1.37f, -14.75f), D3DXToRadian(-45.f)));

	return S_OK;
}

_int CScene_Stage_Base::Update_Scene(_double TimeDelta)
{
	Create_Fog(TimeDelta);

	//====================================================================================================
	// 만약에 하나의 사운드를 계속해서 재생하고 싶다면
	// Update 문이나, 순회가능한 곳에 Play 해둔다면 Loop 재생이 가능함.
	// 단, Update 가 끊기거나 접근이 불가능하면 해당 곡의 Lifetime 이 끝나면 멈춤
	//====================================================================================================
	//g_pSoundManager->Play_Sound(L"Gwan_Cchak.wav", CSoundManager::Background_01, CSoundManager::BGM_Sound);
	//====================================================================================================

	//CUI_Manager::Get_Instance()->Update_UI();

	return NO_EVENT;
}

HRESULT CScene_Stage_Base::Render_Scene()
{
	return S_OK;
}

HRESULT CScene_Stage_Base::Ready_Layer_Custom(const _tchar * pLayerTag)
{
	return S_OK;
}

HRESULT CScene_Stage_Base::Ready_Player()
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
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Colleague")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Pet")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Item")))
		return E_FAIL;

	CPlayer* pInstance = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

	pInstance->Set_Enable(true);
	pInstance->Teleport_ResetOptions(g_eSceneID_Cur , g_eSTeleportID_Cur);

	pInstance = nullptr;

	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_PlayerHP", SCENE_MORTAL, L"Layer_PlayerUI")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_PlayerST", SCENE_MORTAL, L"Layer_PlayerUI")))
		return E_FAIL;

	return S_OK;
}

HRESULT CScene_Stage_Base::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	// For.Sky
	CGameObject* pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Sky", nullptr);
	TARGET_TO_TRANS(pInstance)->Set_Angle(AXIS_Y, D3DXToRadian(220.f));
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, pLayerTag, nullptr);

	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_Sky_Blur", SCENE_STAGE, pLayerTag)))
		return E_FAIL;

	return S_OK;
}


HRESULT CScene_Stage_Base::Ready_LightDesc()
{
	//디렉셔널 + 포인트 라이트로 바꿉니다.

	NEW_LIGHT		LightDesc;
	ZeroMemory(&LightDesc, sizeof(NEW_LIGHT));

	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 0.882f, 0.801f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	// In.WorldSpace
	_v3 vLightDir = _v3(-100.f, -50.f, 0.f);

	V3_NORMAL_SELF(&vLightDir);

	LightDesc.Direction = vLightDir;

	LightDesc.fAlpha = 1.f;

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc, CLight_Manager::Static_Light)))
		return E_FAIL;

	float fAmbient = 0.1f;

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(0.31f, 0.73f, 0.87f, 1.f) * 2.f;
	LightDesc.Ambient = D3DXCOLOR(fAmbient, fAmbient, fAmbient, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	LightDesc.Position = _v3(-0.f, 1.3f, 15.984f);
	LightDesc.Range = 5.f;

	LightDesc.fAlpha = 1.f;


	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc, CLight_Manager::Static_Light)))
		return E_FAIL;

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 0.75f, 0.5f, 1.f)* 2.f;
	LightDesc.Ambient = D3DXCOLOR(fAmbient, fAmbient, fAmbient, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	LightDesc.Position = _v3(-0.f, 2.5f, 6.319f);
	LightDesc.Range = 6.f;

	LightDesc.fAlpha = 1.f;

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc, CLight_Manager::Static_Light)))
		return E_FAIL;

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 0.75f, 0.5f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(fAmbient, fAmbient, fAmbient, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	LightDesc.Position = _v3(-0.f, 4.650f, 22.731f);
	LightDesc.Range = 6.f;

	LightDesc.fAlpha = 1.f;

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc, CLight_Manager::Static_Light)))
		return E_FAIL;

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 0.75f, 0.5f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(fAmbient, fAmbient, fAmbient, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	LightDesc.Position = _v3(-0.f, 2.5f, -24.138f);
	LightDesc.Range = 6.f;

	LightDesc.fAlpha = 1.f;

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc, CLight_Manager::Static_Light)))
		return E_FAIL;

	return NOERROR;
}

void CScene_Stage_Base::Create_Fog(_double TimeDelta)
{
	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	if (!pPlayer)
		return;

	CTransform* pPlayerTrans = TARGET_TO_TRANS(pPlayer);
	_v3 vPlayerPos = pPlayerTrans->Get_Pos();

	const _float FOG_OFFSET = 5.f;

	m_fMapFogDelay += _float(TimeDelta);
	if (m_fMapFogDelay > FOG_OFFSET)
	{
		m_fMapFogDelay = 0.f;
		g_pManagement->Create_Effect(L"MapFog_0", _v3(0.f, -4.f, 0.f));
	}

	g_pManagement->Create_Effect_Offset(L"MapFog_0", 7.f, _v3(0.f, -3.f, 0.f));
}

CScene_Stage_Base * CScene_Stage_Base::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _bool _bLoadStatic)
{
	CScene_Stage_Base*	pInstance = new CScene_Stage_Base(pGraphic_Device);

	//pInstance->m_bLoadStaticMesh = _bLoadStatic;

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("Failed To Creating CScene_Stage_Base");
		Safe_Release(pInstance);
	}

	//pInstance->m_bLoadStaticMesh = _bLoadStatic;

	return pInstance;
}

void CScene_Stage_Base::Free()
{
	if (nullptr != m_pLoading)
	{
		Safe_Release(m_pLoading);
	}

	CScene::Free();
}

