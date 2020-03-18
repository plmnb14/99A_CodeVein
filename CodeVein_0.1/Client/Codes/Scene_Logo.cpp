#include "stdafx.h"
#include "..\Headers\Scene_Logo.h"
#include "Scene_Stage.h"
#include "BackGround.h"
#include "Management.h"
#include "CameraMgr.h"
#include "LogoBtn.h"


CScene_Logo::CScene_Logo(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
	, m_pLoading(nullptr)
{

}

HRESULT CScene_Logo::Ready_Scene()
{
	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(L"Layer_BackGround")))
		return E_FAIL;

	if (FAILED(Ready_Layer_LogoBtn(L"Layer_LogoBtn")))
		return E_FAIL;

	m_pLoading = CLoading::Create(m_pGraphic_Device, SCENE_STAGE);
	if (nullptr == m_pLoading)
		return E_FAIL;

	return S_OK;
}

_int CScene_Logo::Update_Scene(_double TimeDelta)
{
	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);
	if (true == m_pLoading->Get_Finish() 
		&& GetKeyState(VK_LBUTTON) & 0x8000
		&& true == static_cast<CLogoBtn*>(pManagement->Get_GameObjectBack(L"Layer_LogoBtn", SCENE_LOGO))->Get_CollMose())
	{
		/*CManagement*		pManagement = CManagement::Get_Instance();
		if (nullptr == pManagement)
			return -1;*/

		//Safe_AddRef(pManagement);

		if (FAILED(pManagement->SetUp_CurrentScene(CScene_Stage::Create(m_pGraphic_Device))))
			return -1;

		if (FAILED(pManagement->Clear_Instance(SCENE_LOGO)))
			return -1;


		Safe_Release(pManagement);		

		return 0;
	}
	
	Safe_Release(pManagement);
	
	
	return _int();
}

HRESULT CScene_Logo::Render_Scene()
{
	
	return S_OK;
}

HRESULT CScene_Logo::Ready_Prototype_GameObject()
{
	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	// 원형객체를 생성해서 오브젝트매니져에 보관한다.

	if (FAILED(pManagement->Add_Prototype(L"GameObject_BackGround", CBackGround::Create(m_pGraphic_Device))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype(L"GameObject_LogoBtn", CLogoBtn::Create(m_pGraphic_Device))))
		return E_FAIL;

	//// 임시 로고 Texture
	
	if (FAILED(pManagement->Add_Prototype(SCENE_LOGO, L"Texture_LogoBtn", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Bin/Resources/Texture/TestUI/Button/TestBtn.png", 1))))
		return E_FAIL;

	Safe_Release(pManagement);

	CCameraMgr::Get_Instance()->Reserve_ContainerSize(2);
	CCameraMgr::Get_Instance()->Ready_Camera(m_pGraphic_Device, DYNAMIC_CAM, L"Tool_FreeCam", TOOL_VIEW, DEFAULT_MODE);
	CCameraMgr::Get_Instance()->Set_MainCamera(DYNAMIC_CAM, L"Tool_FreeCam");
	CCameraMgr::Get_Instance()->Set_MainPos(_v3{ 0,3,-5 });

	
	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	// 이미 오브젝트 매니져에 추가되어있는 객체를 찾아서 복제한다음. 
	// 적절한 레이어에 보관해라.

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	
	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_BackGround", SCENE_LOGO, pLayerTag)))
		return E_FAIL;


	Safe_Release(pManagement);

	return S_OK;
}

HRESULT CScene_Logo::Ready_Layer_LogoBtn(const _tchar * pLayerTag)
{
	CManagement* pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	if (FAILED(pManagement->Add_GameObject_ToLayer(L"GameObject_LogoBtn", SCENE_LOGO, pLayerTag)))
		return E_FAIL;

	Safe_Release(pManagement);

	return NOERROR;
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
	Safe_Release(m_pLoading);

	CScene::Free();
}

