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
	pRenderer->Set_FogDestiny(0.02f);
	pRenderer->Set_UseLinearFog(false);
	Safe_Release(pRenderer);

	g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Object_Stage_03.dat");

	////인덱스 잘 찾는지 테스트
	//CScriptManager::Get_Instance()->Set_StageIdx(3);
	//CScriptManager::Get_Instance()->Ready_Script_DynamicObject(3);

	g_pSoundManager->Stop_Sound(CSoundManager::Background_Loop);
	g_pSoundManager->Stop_Sound(CSoundManager::Ambient_Loop);

	g_pSoundManager->Set_Volume(CSoundManager::BGM_Volume, 1.f);
	g_pSoundManager->Stop_Sound(CSoundManager::Ambient_Loop);
	g_pSoundManager->Play_BGM(L"SE_AMB_St01_Base_Out.ogg", true);

	CParticleMgr::Get_Instance()->Clear_Fog();

	return S_OK;
}

_int CScene_Stage_03::Update_Scene(_double TimeDelta)
{
	Create_Fog(TimeDelta);
	Create_Dust(TimeDelta);

	if (false == m_fCheck_ColleagueSummons && g_pInput_Device->Key_Down(DIK_L))
	{
		CGameObject* pInstance = nullptr;
	
		pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Colleague",
			&CPlayer_Colleague::JACK_INFO(_v3(0.f, 0.f, 0.f), 0.f, g_eSceneID_Cur));
	
		if (nullptr != pInstance)
			g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Colleague", nullptr);
		//else
		//	cout << "소환할 수 없는 위치입니다" << endl;

		m_fCheck_ColleagueSummons = true;
	}

	return S_OK;
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
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Colleague")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Pet")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Item")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Trap")))
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

	//g_pManagement->Create_Effect(L"FloorPlane_Black", _v3(0.f, -19.f, 0.f));

	return S_OK;
}

HRESULT CScene_Stage_03::Ready_Layer_Colleague(const _tchar * pLayerTag)
{
	//// 김재구 추가
	//CGameObject* pInstance = nullptr;

	//pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Colleague",
	//	&CPlayer_Colleague::JACK_INFO(_v3(-100.f, 50.f, 0.f), 0.f, 3));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Colleague", nullptr);

	return S_OK;
}

void CScene_Stage_03::Create_Fog(_double TimeDelta)
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
		g_pManagement->Create_Effect(L"MapFog_0", _v3(0.f, -17.5f, 0.f));
	}

	g_pManagement->Create_Effect_Offset(L"MapFog_0", 7.f, _v3(0.f, -18.2f, 0.f));
}

void CScene_Stage_03::Create_Dust(_double TimeDelta)
{
	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	if (!pPlayer)
		return;

	CTransform* pPlayerTrans = TARGET_TO_TRANS(pPlayer);
	_v3 vPlayerPos = pPlayerTrans->Get_Pos();

	const _float DUST_OFFSET = 1.f;

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

	float fAmbient = 0.1f;
	
	// 시작 겨우살이
	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(0.31f, 0.73f, 0.87f, 1.f) * 2.f;
	LightDesc.Ambient = D3DXCOLOR(fAmbient, fAmbient, fAmbient, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	LightDesc.Position = _v3(50.f, -10.f, 3.7f);
	LightDesc.Range = 5.f;
	LightDesc.fAlpha = 1.f;
	
	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc, CLight_Manager::Static_Light)))
		return E_FAIL;

	// 천장 막힌 건물 가운데
	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 0.75f, 0.5f, 1.f)* 2.f;
	LightDesc.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	LightDesc.Position = _v3(-2.9f, 2.f, -90.f);
	LightDesc.Range = 10.f;
	LightDesc.fAlpha = 1.f;
	
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

