#include "stdafx.h"
#include "Scene_Logo.h"
#include "Scene_Title.h"

#include "Management.h"
#include "BackGround.h"
#include "LogoBtn.h"
#include "LoadingScreen.h"
#include "LoadingBar.h"
#include "CursorEffect.h"

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
		m_pLogoBtn->Set_Active(true);

	if (true == m_pLoading->Get_Finish() && g_pInput_Device->Key_Down(DIK_SPACE))
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

	CScene::Free();
}

HRESULT CScene_Logo::Ready_Essential_Prototype_GameObject()
{

	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_Logo(const _tchar * pLayerTag)
{
	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_LogoBackGround", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/LogoBack/LogoBack%d.png", 4))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_LogoButton", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/Button/Button%d.png", 4))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_CursorEffect", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/CursorEffect/CursorEffect%d.png", 1))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_LogoBackGround", CBackGround::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_LogoButton", CLogoBtn::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_CursorEffect", CCursorEffect::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_LogoBackGround", SCENE_LOGO, L"Layer_LogoBackGround")))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_LogoButton", SCENE_LOGO, L"Layer_LogoButton")))
		return E_FAIL;
	m_pLogoBtn = static_cast<CLogoBtn*>(g_pManagement->Get_GameObjectBack(L"Layer_LogoButton", SCENE_LOGO));
	Safe_AddRef(m_pLogoBtn);

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
