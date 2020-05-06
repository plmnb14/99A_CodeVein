#include "stdafx.h"
#include "Scene_Logo.h"
#include "Scene_Title.h"
#include "Scene_Stage_Base.h"
#include "Scene_Stage_Training.h"
#include "Scene_Stage_01.h"
#include "Scene_Stage_02.h"
#include "Scene_Stage_03.h"
#include "Scene_Stage_04.h"

#include "BackGround.h"
#include "Management.h"
#include "CameraMgr.h"

#include "LogoBtn.h"
#include "Player.h"

#include "UI_Manager.h"
#include "LoadingScreen.h"
#include "LoadingBar.h"

CScene_Title::CScene_Title(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
	, m_pLoading(nullptr)
{

}

HRESULT CScene_Title::Ready_Scene()
{
	system("cls");

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Layer_LoadingUI(L"Layer_LoadingUI")))
		return E_FAIL;

	//디졸브 전역 텍스쳐
	g_pDissolveTexture = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../../Client/Resources/Texture/Effect/Noise/Noise_13.dds");

	m_pLoading = CLoading::Create(m_pGraphic_Device, SCENE_STAGE);

	if (nullptr == m_pLoading)
		return E_FAIL;

	CUI_Manager::Get_Instance()->SetUp_UILayer();
	
	CParticleMgr::Get_Instance()->Ready_Trail();

	if (FAILED(Ready_Player()))
		return E_FAIL;

	return S_OK;
}

_int CScene_Title::Update_Scene(_double TimeDelta)
{
	CUI_Manager::Get_Instance()->Update_UI();

	if (true == m_pLoading->Get_Finish())
	{
		static_cast<CLoadingBar*>(g_pManagement->Get_GameObjectBack(L"Layer_LoadingUI", SCENE_TITLE))->Set_Finish();
		
		cout << "로드 되었습니다!! 넘어가세요!!" << endl;
	}

	// =======================================================
	// UI 버튼 막아놓음.
	// =======================================================
	//_bool Coll_ToButton = static_cast<CLogoBtn*>(g_pManagement->Get_GameObjectBack(L"Layer_LogoBtn", SCENE_TITLE))->Get_CollMose();

	if (true == m_pLoading->Get_Finish() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		static_cast<CLoadingScreen*>(g_pManagement->Get_GameObjectBack(L"Layer_LoadingScreen", SCENE_STATIC))->Set_Active(false);
		static_cast<CLoadingScreen*>(g_pManagement->Get_GameObjectBack(L"Layer_LoadingScreen", SCENE_STATIC))->Set_UI_Index(1);
		
		// =======================================================
		// UI 버튼 막아놓음.
		// =======================================================
		//if (false == Coll_ToButton)
		//	return 0;
		//m_pLoadingScreen->Set_Active(false);	

		CScene* pScene = nullptr;
	
		switch (m_sStageOptionIdx)
		{
		case CScene_Logo::Stage_Base:
		{
			pScene = CScene_Stage_Base::Create(m_pGraphic_Device, m_bLoadStaticMesh);
			break;
		}
	
		case CScene_Logo::Stage_Training:
		{
			pScene = CScene_Stage_Training::Create(m_pGraphic_Device, m_bLoadStaticMesh);
			break;
		}
	
		case CScene_Logo::Stage_01:
		{
			pScene = CScene_Stage_01::Create(m_pGraphic_Device, m_bLoadStaticMesh);
			break;
		}
	
		case CScene_Logo::Stage_02:
		{
			pScene = CScene_Stage_02::Create(m_pGraphic_Device, m_bLoadStaticMesh);
			break;
		}
	
		case CScene_Logo::Stage_03:
		{
			pScene = CScene_Stage_03::Create(m_pGraphic_Device, m_bLoadStaticMesh);
			break;
		}

		case CScene_Logo::Stage_04:
		{
			pScene = CScene_Stage_04::Create(m_pGraphic_Device, m_bLoadStaticMesh);
			break;
		}
		}
	
		if (FAILED(g_pManagement->SetUp_CurrentScene(pScene)))
			return -1;
	
		if (FAILED(g_pManagement->Clear_Instance(SCENE_TITLE)))
			return -1;
	
		cout << "다음 스테이지로 이동 중 . . ." << endl;
		pScene = nullptr;
	
		return 0;
	}


	return NO_EVENT;
}

HRESULT CScene_Title::Render_Scene()
{

	return S_OK;
}

HRESULT CScene_Title::Ready_Prototype_GameObject()
{
	CCameraMgr::Get_Instance()->Reserve_ContainerSize(2);
	CCameraMgr::Get_Instance()->Ready_Camera(m_pGraphic_Device, DYNAMIC_CAM, L"Tool_FreeCam", TOOL_VIEW, DEFAULT_MODE);
	CCameraMgr::Get_Instance()->Set_MainCamera(DYNAMIC_CAM, L"Tool_FreeCam");
	CCameraMgr::Get_Instance()->Set_MainPos(_v3{ 0,3,-5 });


	CCameraMgr::Get_Instance()->Set_CamView(BACK_VIEW);

	g_bActiveCam = true;

	/*if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_LoadingScreen", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/LoadingScreen/LoadingScreen0.tga", 1))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_LoadingScreen", CLoadingScreen::Create(m_pGraphic_Device))))
		return E_FAIL;*/
	
	return S_OK;
}

HRESULT CScene_Title::Ready_Layer_LoadingUI(const _tchar * pLayerTag)
{
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_LoadingBar", SCENE_TITLE, pLayerTag)))
		return E_FAIL;

	static_cast<CLoadingScreen*>(g_pManagement->Get_GameObjectBack(L"Layer_LoadingScreen", SCENE_STATIC))->Set_UI_Index(0);
	return S_OK;
}


HRESULT CScene_Title::Temp_Stage_Loader(const _tchar * _DatPath)
{
	return S_OK;
}


HRESULT CScene_Title::Ready_Player()
{
	//========================================================================================
	// 레이어만 미리 준비
	//========================================================================================

	if (FAILED(g_pManagement->Add_Layer(SCENE_MORTAL, L"Layer_Player")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Monster")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Boss")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_MonsterProjectile")))
		return E_FAIL;

	//========================================================================================0

	// 플레이어 원형 생성
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Player", CPlayer::Create(m_pGraphic_Device))))
		return E_FAIL;

	// 플레이어 생성 하고
	CGameObject* pPlayer =  g_pManagement->Clone_GameObject_Return(L"GameObject_Player", nullptr);

	// 현재 스테이지가 아니니, 꺼둔다.
	pPlayer->Set_Enable(false);

	// Mortal 레이어는 스테틱보단 아래 단계이지만, 스테이지가 지나도 삭제되지 않습니다.
	g_pManagement->Add_GameOject_ToLayer_NoClone(pPlayer, SCENE_MORTAL, L"Layer_Player", nullptr);

	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_PlayerHP", SCENE_MORTAL, L"Layer_PlayerUI")))
	//	return E_FAIL;
	//
	//if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_PlayerST", SCENE_MORTAL, L"Layer_PlayerUI")))
	//	return E_FAIL;

	return S_OK;
}

CScene_Title * CScene_Title::Create(_Device pGraphic_Device , _short _sStageNum, _bool _bLoadStatic)
{
	CScene_Title*	pInstance = new CScene_Title(pGraphic_Device);

	pInstance->m_sStageOptionIdx = _sStageNum;
	pInstance->m_bLoadStaticMesh = _bLoadStatic;

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("Failed To Creating CScene_Title");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Title::Free()
{
	Safe_Release(m_pLoading);

	CScene::Free();
}

