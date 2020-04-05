#include "stdafx.h"
#include "Scene_Logo.h"
#include "Scene_Title.h"
#include "Scene_Stage_Base.h"
#include "Scene_Stage_Training.h"
#include "Scene_Stage_01.h"
#include "Scene_Stage_02.h"
#include "Scene_Stage_03.h"

#include "BackGround.h"
#include "Management.h"
#include "CameraMgr.h"
#include "LogoBtn.h"

#include "UI_Manager.h"

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

	if (FAILED(Ready_Layer_LogoBtn(L"Layer_LogoBtn")))
		return E_FAIL;

	// 파티클
	//if (FAILED(CParticleMgr::Get_Instance()->Ready_ParticleManager()))
	//	return E_FAIL;

	m_pLoading = CLoading::Create(m_pGraphic_Device, SCENE_STAGE);
	if (nullptr == m_pLoading)
		return E_FAIL;

	m_pLoading->Set_LoadStaticMesh(m_bLoadStaticMesh);

	CUI_Manager::Get_Instance()->SetUp_UILayer();

	return S_OK;
}

_int CScene_Title::Update_Scene(_double TimeDelta)
{
	CUI_Manager::Get_Instance()->Update_UI();

	if (true == m_pLoading->Get_Finish())
	{
		cout << "로드 되었습니다!! 넘어가세요!!" << endl;
	}

	// =======================================================
	// UI 버튼 막아놓음.
	// =======================================================
	//_bool Coll_ToButton = static_cast<CLogoBtn*>(g_pManagement->Get_GameObjectBack(L"Layer_LogoBtn", SCENE_TITLE))->Get_CollMose();

	if (true == m_pLoading->Get_Finish() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		// =======================================================
		// UI 버튼 막아놓음.
		// =======================================================
		//if (false == Coll_ToButton)
		//	return 0;
	
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
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_LogoBtn", CLogoBtn::Create(m_pGraphic_Device))))
		return E_FAIL;

	CCameraMgr::Get_Instance()->Reserve_ContainerSize(2);
	CCameraMgr::Get_Instance()->Ready_Camera(m_pGraphic_Device, DYNAMIC_CAM, L"Tool_FreeCam", TOOL_VIEW, DEFAULT_MODE);
	CCameraMgr::Get_Instance()->Set_MainCamera(DYNAMIC_CAM, L"Tool_FreeCam");
	CCameraMgr::Get_Instance()->Set_MainPos(_v3{ 0,3,-5 });

	return S_OK;
}

HRESULT CScene_Title::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	return S_OK;
}

HRESULT CScene_Title::Ready_Layer_LogoBtn(const _tchar * pLayerTag)
{
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_LogoBtn", SCENE_TITLE, pLayerTag)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Title::Temp_Stage_Loader(const _tchar * _DatPath)
{
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

