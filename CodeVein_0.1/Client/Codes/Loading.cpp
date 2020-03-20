#include "stdafx.h"
#include "Loading.h"
#include "Management.h"

#include "Sky.h"
#include "Effect.h"
#include "Player.h"
#include "Terrain.h"
#include "TestMonster.h"
#include "HPBack.h"
#include "PlayerHP.h"
#include "PlayerST.h"
#include "BossDecoUI.h"
#include "BossHP.h"
#include "ItemPalette.h"
#include "ItemSlot.h"
#include "RightArrow.h"
#include "LeftArrow.h"
#include "ItemIcon.h"
#include "MenuBaseUI.h"
#include "Menu_Btn.h"
#include "SlotCnt_UI.h"

USING(Client)

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bFinish(false)
{
	ZeroMemory(m_szString, sizeof(_tchar) * 128);
	Safe_AddRef(m_pGraphicDev);	
}

HRESULT CLoading::Ready_Loading(SCENEID eLoadingID)
{
	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex(NULL, 0, Thread_Main, this, 0, NULL);

	m_eLoadingID = eLoadingID;

	return S_OK;
}

_uint CLoading::Loading_ForStage(void)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	
	// 오브젝트 원형
	lstrcpy(m_szString, L"게임오브젝트 원형 생성 중....");	
	if (FAILED(pManagement->Add_Prototype(L"GameObject_Player", CPlayer::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype(L"GameObject_Effect", CEffect::Create(m_pGraphicDev, nullptr))))
		return E_FAIL;

	// UI
	if (FAILED(pManagement->Add_Prototype(L"GameObject_HPBack", CHPBack::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_PlayerHP", CPlayerHP::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_PlayerST", CPlayerST::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_BossDecoUI", CBossDecoUI::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_BossHP", CBossHP::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_ItemSlot", CItemSlot::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_RightArrow", CRightArrow::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_LeftArrow", CLeftArrow::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_ItemPalette", CItemPalette::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_ItemIcon", CItemIcon::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_MenuBase", CMenuBaseUI::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_MenuBtn", CMenu_Btn::Create(m_pGraphicDev))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"GameObject_CntSlotUI", CSlotCnt_UI::Create(m_pGraphicDev))))
		return E_FAIL;

	
	//몬스터
	if (FAILED(pManagement->Add_Prototype(L"Monster_TestMonster", CTestMonster::Create(m_pGraphicDev))))
		return E_FAIL;

	lstrcpy(m_szString, L"지형 생성 중....");
	if (FAILED(pManagement->Add_Prototype(L"GameObject_Terrain", CTerrain::Create(m_pGraphicDev))))
		return E_FAIL;

	//lstrcpy(m_szString, L"하늘 생성 중....");
	if (FAILED(pManagement->Add_Prototype(L"GameObject_Sky", CSky::Create(m_pGraphicDev))))
		return E_FAIL;

	// BT_Node 생성 중
	if (FAILED(pManagement->Ready_BT_Node()))
		return E_FAIL;

	// UI텍스쳐 (임시)
	if (FAILED(pManagement->Add_Prototype(SCENE_STAGE, L"Texture_HPBack", CTexture::Create(m_pGraphicDev, CTexture::TYPE_GENERAL, L"../Bin/Resources/Texture/TestUI/HPBarBack/HPBack.png", 1))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(SCENE_STAGE, L"Texture_PlayerHP", CTexture::Create(m_pGraphicDev, CTexture::TYPE_GENERAL, L"../Bin/Resources/Texture/TestUI/HPBar/T_HPGaugeTexture_UI.tga", 1))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(SCENE_STAGE, L"Texture_PlayerST", CTexture::Create(m_pGraphicDev, CTexture::TYPE_GENERAL, L"../Bin/Resources/Texture/TestUI/STBar/T_StaminaBar_UI.tga", 1))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(SCENE_STAGE, L"Texture_BossHP", CTexture::Create(m_pGraphicDev, CTexture::TYPE_GENERAL, L"../Bin/Resources/Texture/TestUI/BossUI/T_BossHpGaugeTexture_UI.tga", 1))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(SCENE_STAGE, L"Texture_BossDecoUI", CTexture::Create(m_pGraphicDev, CTexture::TYPE_GENERAL, L"../Bin/Resources/Texture/TestUI/BossUI/BossHpGauge_Deco.tga", 1))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(SCENE_STAGE, L"Texture_ItemIcon", CTexture::Create(m_pGraphicDev, CTexture::TYPE_GENERAL, L"../Bin/Resources/Texture/TestUI/ItemIcon/T_ItemIcon_00%d.tga", 5))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(SCENE_STAGE, L"Texture_Arrow", CTexture::Create(m_pGraphicDev, CTexture::TYPE_GENERAL, L"../Bin/Resources/Texture/TestUI/QuickSlotUI/T_Arrow%d.jpg", 2))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(SCENE_STAGE, L"Texture_ItemPalette", CTexture::Create(m_pGraphicDev, CTexture::TYPE_GENERAL, L"../Bin/Resources/Texture/TestUI/ItemPalette/ItemPalette.png", 1))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(SCENE_STAGE, L"Texture_MenuBase", CTexture::Create(m_pGraphicDev, CTexture::TYPE_GENERAL, L"../Bin/Resources/Texture/TestUI/MenuWindow/Window%d.png", 1))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(SCENE_STAGE, L"Texture_MenuBtn", CTexture::Create(m_pGraphicDev, CTexture::TYPE_GENERAL, L"../Bin/Resources/Texture/TestUI/Menu_Icon/MenuIcon00%d.tga", 10))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(SCENE_STAGE, L"Texture_Num", CTexture::Create(m_pGraphicDev, CTexture::TYPE_GENERAL, L"../Bin/Resources/Texture/TestUI/Number/Num%d.png", 4))))
		return E_FAIL;
	
	// UI Shader
	if (FAILED(pManagement->Add_Prototype(SCENE_STATIC, L"Shader_UI", CShader::Create(m_pGraphicDev, L"../Bin/ShaderFiles/Shader_UI.fx"))))
		return E_FAIL;

	m_bFinish = true;
	lstrcpy(m_szString, L"로딩 완료");

	Safe_Release(pManagement);

	return 0;
}

unsigned int CALLBACK CLoading::Thread_Main(void* pArg)
{
	CLoading*	pLoading = (CLoading*)pArg;

	_uint		iFlag = 0;

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case SCENE_STAGE:
		iFlag = pLoading->Loading_ForStage();
		break;
	}

	LeaveCriticalSection(pLoading->Get_Crt());

	return iFlag;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENEID eLoadingID)
{
	CLoading*	pInstance = new CLoading(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eLoadingID)))
		Engine::Safe_Release(pInstance);
	
	return pInstance;
}

void CLoading::Free(void)
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);

	Safe_Release(m_pGraphicDev);
}

