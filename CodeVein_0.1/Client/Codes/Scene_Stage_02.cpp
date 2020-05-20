#include "stdafx.h"
#include "Scene_Stage_02.h"

#include "CameraMgr.h"
#include "Effect.h"
#include "UI.h"
#include "UI_Manager.h"
#include "ParticleMgr.h"
#include "ScriptManager.h"

#include "Player.h"

#include "Player_Colleague.h"

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

	m_pRenderer = static_cast<CRenderer*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Renderer"));
	m_pRenderer->Fog_On(true);
	m_pRenderer->Set_FogDestiny(0.04f);
	m_pRenderer->Set_UseLinearFog(false);

	g_pSoundManager->Stop_Sound(CSoundManager::Ambient_01);
	g_pSoundManager->Play_Sound(L"BGM_MT_SNOW.ogg", CSoundManager::Ambient_01, CSoundManager::Ambient_Sound);

	g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Object_Stage_02.dat");

	return S_OK;
}

_int CScene_Stage_02::Update_Scene(_double TimeDelta)
{
	CUI_Manager::Get_Instance()->Update_UI();

	Create_Fog(TimeDelta);
	Create_Dust(TimeDelta);
	Check_Effect_Fade();

	if (g_pInput_Device->Key_Down(DIK_B))
	{
		m_pRenderer->Start_FogFadeOut();
		m_bFadeOutStart = true;
	}

	if (g_pInput_Device->Key_Down(DIK_I))
	{
		CGameObject* pInstance = nullptr;

		pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Colleague",
			&CPlayer_Colleague::JACK_INFO(_v3(0.f, 0.f, 0.f), 0.f, 2));

		if (nullptr != pInstance)
			g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Colleague", nullptr);
		else
			cout << "소환할 수 없는 위치입니다" << endl;
	}

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
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Colleague")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Pet")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Item")))
		return E_FAIL;

	CPlayer* pInstance = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

	pInstance->Teleport_ResetOptions(g_eSceneID_Cur, g_eSTeleportID_Cur);
	pInstance->Set_Enable(true);

	pInstance = nullptr;

	return S_OK;
}

HRESULT CScene_Stage_02::Ready_Layer_Environment(const _tchar* pLayerTag)
{
	CGameObject* pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Sky", nullptr);
	TARGET_TO_TRANS(pInstance)->Set_Angle(AXIS_Y, D3DXToRadian(0.f));
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Sky", nullptr);

	pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Sky_Blur", nullptr);
	TARGET_TO_TRANS(pInstance)->Set_Angle(AXIS_Y, D3DXToRadian(0.f));
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Sky", nullptr);

	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_BossMassageUI", SCENE_STAGE, L"Layer_BossMassageUI")))
		return E_FAIL;

	//g_pManagement->Create_Effect(L"FloorPlane_Black", _v3(0.f, -6.f, 0.f));

	m_pSnowEffect_0 = static_cast<COrthoEffect*>(CParticleMgr::Get_Instance()->Create_EffectReturn(L"Snow_Ortho_0"));
	m_pSnowEffect_0->Set_Desc(_v3(0, 0, 0), nullptr);
	m_pSnowEffect_0->Set_UV_Speed(0.8f, -1.2f);
	m_pSnowEffect_0->Set_UI_Layer();
	//m_pSnowEffect_0->Set_ViewZ(0.f);
	m_pSnowEffect_0->Reset_Init();
	m_pSnowEffect_0->Set_Active(true);

	m_pSmokeEffect_0 = static_cast<COrthoEffect*>(CParticleMgr::Get_Instance()->Create_EffectReturn(L"Ortho_Smoke_0"));
	m_pSmokeEffect_0->Set_Desc(_v3(0, 0, 0), nullptr);
	m_pSmokeEffect_0->Set_UV_Speed(0.75f, 0.f);
	m_pSmokeEffect_0->Set_UI_Layer();
	m_pSmokeEffect_0->Reset_Init();
	m_pSmokeEffect_0->Set_Active(true);

	m_pSmokeEffect_1 = static_cast<COrthoEffect*>(CParticleMgr::Get_Instance()->Create_EffectReturn(L"Ortho_Smoke_1"));
	m_pSmokeEffect_1->Set_Desc(_v3(0, 0, 0), nullptr);
	m_pSmokeEffect_1->Set_UV_Speed(0.5f, 0.f);
	m_pSmokeEffect_1->Set_UI_Layer();
	m_pSmokeEffect_1->Reset_Init();
	m_pSmokeEffect_1->Set_Active(true);

	return S_OK;
}


HRESULT CScene_Stage_02::Ready_LightDesc()
{
	NEW_LIGHT		LightDesc;
	ZeroMemory(&LightDesc, sizeof(NEW_LIGHT));

	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	// In.WorldSpace
	_v3 vLightDir = _v3(0.f, -100.f, 100.f);

	V3_NORMAL_SELF(&vLightDir);

	LightDesc.Direction = vLightDir;

	LightDesc.fAlpha = 1.f;
	//LightDesc.Direction = _v3(0.0f, 0.f, 1.f);

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc, CLight_Manager::Static_Light)))
		return E_FAIL;

	return NOERROR;
}

void CScene_Stage_02::Create_Fog(_double TimeDelta)
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

void CScene_Stage_02::Create_Dust(_double TimeDelta)
{
	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	if (!pPlayer)
		return;

	CTransform* pPlayerTrans = TARGET_TO_TRANS(pPlayer);
	_v3 vPlayerPos = pPlayerTrans->Get_Pos();

	const _float DUST_OFFSET = 0.5f;

	m_fMapWindDustDelay += _float(TimeDelta);
	if (m_fMapWindDustDelay > DUST_OFFSET)
	{
		m_fMapWindDustDelay = 0.f;

		for (_int i = 0; i < 10; ++i)
		{
			_mat matRotY;
			_v3 vDir = _v3(1.f, 0.f, 1.f);
			D3DXMatrixIdentity(&matRotY);

			D3DXMatrixRotationY(&matRotY, D3DXToRadian(_float(CCalculater::Random_Num_Double(0, 360))));
			D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
			D3DXVec3Normalize(&vDir, &vDir);

			_float fMinRange = 1.f;
			_float fRandRange = _float(CCalculater::Random_Num_Double(0, 30));
			_v3 vRandPos = vDir * (fMinRange + fRandRange);// +_v3(0.f, 0.2f, 0.f);

			g_pManagement->Create_Effect(L"MapDust", vPlayerPos + vRandPos + _v3(0.f, _float(CCalculater::Random_Num_Double(0, 0.5)), 0.f), nullptr);

			fRandRange = _float(CCalculater::Random_Num_Double(0, 20));
			vRandPos = vDir * (fMinRange + fRandRange);
			g_pManagement->Create_Effect(L"MapDust_2", vPlayerPos + vRandPos + _v3(0.f, _float(CCalculater::Random_Num_Double(0, 0.5)), 0.f), nullptr);
		}
	}
}

void CScene_Stage_02::Create_Snow(_double TimeDelta)
{
	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	if (!pPlayer)
		return; 

	const _float SNOW_OFFSET = 0.2f;

	m_fMapSnowDelay += _float(TimeDelta);
	if (m_fMapSnowDelay > SNOW_OFFSET)
	{
		m_fMapSnowDelay = 0.f;

		CTransform* pPlayerTrans = TARGET_TO_TRANS(pPlayer);
		_v3 vPlayerPos = pPlayerTrans->Get_Pos();

		for (_int i = 0; i < 10; i++)
			g_pManagement->Create_Effect(L"MapSnow", vPlayerPos + _v3(0.f, 10.f, 0.f), nullptr);
	}
}

void CScene_Stage_02::Check_Effect_Fade()
{
	if (m_bFadeInStart && m_fFade < 1.f)
	{
		m_bFadeOutStart = false;
		m_fFade += DELTA_60;
		m_pSmokeEffect_0->Set_Alpha(m_fFade);
		m_pSmokeEffect_1->Set_Alpha(m_fFade);
		m_pSnowEffect_0->Set_Alpha(m_fFade);
	}

	if (m_bFadeOutStart && m_fFade > 0.f)
	{
		m_bFadeInStart = false;
		m_fFade -= DELTA_60;
		m_pSmokeEffect_0->Set_Alpha(m_fFade);
		m_pSmokeEffect_1->Set_Alpha(m_fFade);
		m_pSnowEffect_0->Set_Alpha(m_fFade);
	}

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

	m_pSnowEffect_0->Set_Dead();
	m_pSmokeEffect_0->Set_Dead();
	m_pSmokeEffect_1->Set_Dead();
	
	CParticleMgr::Get_Instance()->Clear_Fog();

	Safe_Release(m_pRenderer);

	CScene::Free();
}

