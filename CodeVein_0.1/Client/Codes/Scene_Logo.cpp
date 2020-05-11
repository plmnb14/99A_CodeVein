#include "stdafx.h"
#include "Scene_Logo.h"
#include "Scene_Title.h"

#include "Management.h"
#include "BackGround.h"
#include "LogoBtn.h"
#include "LoadingScreen.h"
#include "LoadingBar.h"
#include "CursorEffect.h"
#include "MouseUI.h"

#include "OrthoEffect.h"

CScene_Logo::CScene_Logo(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
	, m_pLoading(nullptr)
{

}

HRESULT CScene_Logo::Ready_Scene()
{
	if (FAILED(Ready_Essential_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Logo(L"Layer_LogoUI")))
		return E_FAIL;

	m_pLoading = CLoading::Create(m_pGraphic_Device, SCENE_TITLE);
	if (nullptr == m_pLoading)
		return E_FAIL;
	
	return S_OK;
}

_int CScene_Logo::Update_Scene(_double TimeDelta)
{
	Logo_KeyInput();

	if (true == m_pLoading->Get_Finish())
	{
		m_pLogoBtn->Set_Active(true);

		if (g_pInput_Device->Key_Down(DIK_SPACE))
		{
			m_pLoadingScreen->Set_Active(true);
		}
		if (m_pLoadingScreen->Get_Load())
			m_bIsChangeStage = true;
	}
		
	if (true == m_pLoading->Get_Finish() && m_bIsChangeStage)
	{
		if (g_bReleaseMode)
		{
			// 베이스 스테이지 고정
			g_sStageIdx_Cur = 1;
			m_eSceneChange = Stage_Base;
		}
		
		if (FAILED(g_pManagement->SetUp_CurrentScene(CScene_Title::Create(m_pGraphic_Device, m_eSceneChange, m_bLoadStaticMesh))))
			return -1;
		
		if (FAILED(g_pManagement->Clear_Instance(SCENE_LOGO)))
			return -1;

		return 0;
	}

	return NO_EVENT;
}

HRESULT CScene_Logo::Render_Scene()
{
	return S_OK;
}

CScene_Logo * CScene_Logo::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CScene_Logo*	pInstance = new CScene_Logo(pGraphic_Device);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("Failed To Creating CScene_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Logo::Free()
{
	Safe_Release(m_pLogoBtn);
	Safe_Release(m_pLoading);

	m_pGlitterEffect_0->Set_Dead();
	m_pGlitterEffect_1->Set_Dead();
	m_pTitleBG->Set_Dead();

	CScene::Free();
}

HRESULT CScene_Logo::Ready_Essential_Prototype_GameObject()
{
	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_Logo(const _tchar * pLayerTag)
{
	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_Ortho_Title", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/Effect/Ortho/Ortho_Title/Ortho_Title_%d.dds", 9))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Ortho_Title_Glitter_0", COrthoEffect::Create(m_pGraphic_Device, Read_EffectData(L"../../Data/EffectData/Ortho_Title_Glitter_0.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Ortho_Title_Glitter_1", COrthoEffect::Create(m_pGraphic_Device, Read_EffectData(L"../../Data/EffectData/Ortho_Title_Glitter_1.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Ortho_Title_ShadowLine", COrthoEffect::Create(m_pGraphic_Device, Read_EffectData(L"../../Data/EffectData/Ortho_Title_ShadowLine.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Ortho_Title_ShadowText", COrthoEffect::Create(m_pGraphic_Device, Read_EffectData(L"../../Data/EffectData/Ortho_Title_ShadowText.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Ortho_Title_Smoke", COrthoEffect::Create(m_pGraphic_Device, Read_EffectData(L"../../Data/EffectData/Ortho_Title_Smoke.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Ortho_Title_BG", COrthoEffect::Create(m_pGraphic_Device, Read_EffectData(L"../../Data/EffectData/Ortho_Title_BG.dat")))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"Ortho_Title_WhiteFadeout", COrthoEffect::Create(m_pGraphic_Device, Read_EffectData(L"../../Data/EffectData/Ortho_Title_WhiteFadeout.dat")))))
		return E_FAIL;

	CParticleMgr::Get_Instance()->Ready_TitleEffect();


	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_LogoBackGround", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/LogoBack/LogoBack%d.dds", 5))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_LogoButton", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/Button/Button%d.png", 1))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_CursorEffect", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/CursorEffect/CursorEffect%d.png", 1))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_LoadingScreen", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/LoadingScreen/LoadingScreen%d.png", 2))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_LoadingBar", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/LoadingBar/LoadingBar%d.png", 10))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_MouseUI", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/MouseUI/MouseUI%d.dds", 1))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_LoadingBar", CLoadingBar::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_LoadingScreen", CLoadingScreen::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_LogoBackGround", CBackGround::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_LogoButton", CLogoBtn::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_CursorEffect", CCursorEffect::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_MouseUI", CMouseUI::Create(m_pGraphic_Device))))
		return E_FAIL;
	CMouseUI* pMouseUI = static_cast<CMouseUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MouseUI", nullptr));
	if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(pMouseUI, SCENE_STATIC, L"Layer_MouseUI", nullptr)))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_LogoBackGround", SCENE_LOGO, L"Layer_LogoBackGround")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_LogoButton", SCENE_LOGO, L"Layer_LogoButton")))
		return E_FAIL;

	m_pLogoBtn = static_cast<CLogoBtn*>(g_pManagement->Get_GameObjectBack(L"Layer_LogoButton", SCENE_LOGO));
	Safe_AddRef(m_pLogoBtn);

	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_LoadingScreen", SCENE_STATIC, L"Layer_LoadingScreen")))
		return E_FAIL;
	m_pLoadingScreen = static_cast<CLoadingScreen*>(g_pManagement->Get_GameObjectBack(L"Layer_LoadingScreen", SCENE_STATIC));
	m_pLoadingScreen->Set_FadeSpeed(0.6f);

	m_pTitleBG = static_cast<COrthoEffect*>(CParticleMgr::Get_Instance()->Create_EffectReturn(L"Ortho_Title_BG"));
	m_pTitleBG->Set_Desc(_v3(0, 0, 0), nullptr);
	m_pTitleBG->Reset_Init();
	
	m_pGlitterEffect_0 = static_cast<COrthoEffect*>(CParticleMgr::Get_Instance()->Create_EffectReturn(L"Ortho_Title_Glitter_0"));
	m_pGlitterEffect_0->Set_Desc(_v3(0, 0, 0), nullptr);
	m_pGlitterEffect_0->Set_UV_Speed(0.03f, 0.f);
	m_pGlitterEffect_0->Set_Mask(L"Tex_Ortho_Title", 6);
	m_pGlitterEffect_0->Reset_Init();
	
	m_pGlitterEffect_1 = static_cast<COrthoEffect*>(CParticleMgr::Get_Instance()->Create_EffectReturn(L"Ortho_Title_Glitter_1"));
	m_pGlitterEffect_1->Set_Desc(_v3(0, 0, 0), nullptr);
	m_pGlitterEffect_1->Set_UV_Speed(0.04f, 0.06f);
	m_pGlitterEffect_1->Set_Mask(L"Tex_Ortho_Title", 6);
	m_pGlitterEffect_1->Reset_Init();
	
	COrthoEffect* pEff = static_cast<COrthoEffect*>(CParticleMgr::Get_Instance()->Create_EffectReturn(L"Ortho_Title_WhiteFadeout"));
	pEff->Set_Desc(_v3(0, 0, 0), nullptr);
	pEff->Set_UI_Layer();
	pEff->Reset_Init();

	//CParticleMgr::Get_Instance()->Create_TitleEffect(L"Ortho_Title_WhiteFadeout");
	//CParticleMgr::Get_Instance()->Create_Effect(L"Ortho_Title_ShadowLine");
	//CParticleMgr::Get_Instance()->Create_Effect(L"Ortho_Title_ShadowText");
	//CParticleMgr::Get_Instance()->Create_Effect(L"Ortho_Title_Smoke");

	return S_OK;
}

void CScene_Logo::Update_DebugStage_Console()
{
	system("cls");
	cout << "===============================================================================" << endl;
	cout << " 1. 위 숫자 키를 누르면 옵션이 활성화 됩니다." << endl;
	cout << " 2. (Load_StaticMesh) 가 (false) 이면 스테이지 들어가도 생성 안됩니다." << endl;
	cout << " 3. Space 를 누르면 다음 스테이지로 넘어갑니다." << endl;
	cout << " 4. 트레이닝 맵은 별도로 Load_StaticMesh 안해도 넘어갑니다." << endl;
	cout << " 5. 기본 설정된 맵은 Stage_Traing 입니다." << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	cout << " #  [ 릴리즈 모드 ] 는 메쉬로드와 스테이지 진행이 실제 게임처럼 됩니다." << endl;
	cout << " #  [ 릴리즈 모드 ] 는 강제로 [ Stage_Base ] 부터 시작하게 됩니다." << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	cout << "[1] Stage_Training = ";
	cout << (m_eSceneChange == CScene_Logo::Stage_Training ? "true" : "false") << endl;

	cout << "[2] Stage_Base = ";
	cout << (m_eSceneChange == CScene_Logo::Stage_Base ? "true" : "false") << endl;

	cout << "[3] Stage_01 = ";
	cout << (m_eSceneChange == CScene_Logo::Stage_01 ? "true" : "false") << endl;

	cout << "[4] Stage_02 = ";
	cout << (m_eSceneChange == CScene_Logo::Stage_02 ? "true" : "false") << endl;

	cout << "[5] Stage_03 = ";
	cout << (m_eSceneChange == CScene_Logo::Stage_03 ? "true" : "false") << endl;

	cout << "[6] Stage_04 = ";
	cout << (m_eSceneChange == CScene_Logo::Stage_04 ? "true" : "false") << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	cout << "[8] Load_StaticMesh = ";
	cout << (m_bLoadStaticMesh ? "true" : "false") << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	cout << "[9] # 릴리즈 모드 # ";
	cout << (g_bReleaseMode ? "true" : "false") << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
}

void CScene_Logo::Logo_KeyInput()
{
	if (g_pInput_Device->Key_Down(DIK_1))
	{
		cout << "트레이닝" << endl;
		m_eSceneChange = Stage_Training;
		Update_DebugStage_Console();
		g_sStageIdx_Cur = 0;
	}

	if (g_pInput_Device->Key_Down(DIK_2))
	{
		m_eSceneChange = Stage_Base;
		Update_DebugStage_Console();
		g_sStageIdx_Cur = 1;
	}

	else if (g_pInput_Device->Key_Down(DIK_3))
	{
		m_eSceneChange = Stage_01;
		Update_DebugStage_Console();
		g_sStageIdx_Cur = 2;
	}

	else if (g_pInput_Device->Key_Down(DIK_4))
	{
		m_eSceneChange = Stage_02;
		Update_DebugStage_Console();
		g_sStageIdx_Cur = 3;
	}

	else if (g_pInput_Device->Key_Down(DIK_5))
	{
		m_eSceneChange = Stage_03;
		Update_DebugStage_Console();
		g_sStageIdx_Cur = 4;
	}

	else if (g_pInput_Device->Key_Down(DIK_6))
	{
		m_eSceneChange = Stage_04;
		Update_DebugStage_Console();
		g_sStageIdx_Cur = 5;
	}

	else if (g_pInput_Device->Key_Down(DIK_8))
	{
		m_bLoadStaticMesh = (m_bLoadStaticMesh ? false : true);

		Update_DebugStage_Console();
	}

	else if (g_pInput_Device->Key_Down(DIK_9))
	{
		g_bReleaseMode = (g_bReleaseMode ? false : true);

		Update_DebugStage_Console();
	}
}


Engine::EFFECT_INFO* CScene_Logo::Read_EffectData(const _tchar* szPath)
{
	HANDLE hFile = CreateFile(szPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
		::MSG_BOX("Load Failed. [INVALID_HANDLE_VALUE]");

	DWORD dwByte = 0;
	Engine::EFFECT_INFO* pInfo = new Engine::EFFECT_INFO;

	_bool bIsTex = false;
	::ReadFile(hFile, &bIsTex, sizeof(_bool), &dwByte, nullptr);

	while (true)
	{
		::ReadFile(hFile, &pInfo->bBillBoard, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bOnlyYRot, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bDistortion, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bStaticFrame, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bUseColorTex, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bUseRGBA, sizeof(_bool), &dwByte, nullptr);

		::ReadFile(hFile, &pInfo->bColorMove, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bDirMove, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bFadeIn, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bFadeOut, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bLinearMove, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bRandomMove, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bRandomRot, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bRandStartPos, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bRevColor, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bRotMove, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bScaleMove, sizeof(_bool), &dwByte, nullptr);

		::ReadFile(hFile, &pInfo->fAlphaSpeed, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fAlphaSpeed_Max, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fAlphaSpeed_Min, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fColorSpeed, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fCreateDelay, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fCreateDelay_Max, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fCreateDelay_Min, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fLifeTime, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fMaxAlpha, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fAnimSpeed, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fMaxFrame, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fMoveScaleSpeed, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fMoveSpeed, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fMoveSpeed_Max, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fMoveSpeed_Min, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fRandStartPosRange_Max, sizeof(_float) * 3, &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fRandStartPosRange_Min, sizeof(_float) * 3, &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fRotSpeed, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fRotSpeed_Max, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fRotSpeed_Min, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->iMaxCount, sizeof(_int), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->szColorName, sizeof(TCHAR) * STR_256, &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->szName, sizeof(TCHAR) * STR_256, &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->szGradientName, sizeof(TCHAR) * STR_256, &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vEndColor, sizeof(_v4), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vMoveDirection, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vMoveScale, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vRandDirectionRange, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vRotDirection, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vStartColor, sizeof(_v4), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vStartPos, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vStartScale, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fColorIndex, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fMaskIndex, sizeof(_float), &dwByte, nullptr);

		_bool bTemp;
		::ReadFile(hFile, &bTemp, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &bTemp, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &bTemp, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &bTemp, sizeof(_bool), &dwByte, nullptr);

		::ReadFile(hFile, &pInfo->bGravity, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bRandScale, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bMoveWithRot, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bSlowly, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fDistortionPower, sizeof(_float), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bDissolve, sizeof(_bool), &dwByte, nullptr);

		break;
	}

	CloseHandle(hFile);

	return pInfo;
}