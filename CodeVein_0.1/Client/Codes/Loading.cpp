#include "stdafx.h"
#include "Loading.h"
#include "Management.h"

#include "Sky.h"
#include "TexEffect.h"
#include "MeshEffect.h"
#include "Player.h"
#include "TestMonster.h"
#include "HPBack.h"
#include "Weapon.h"

#include "PlayerHP.h"
#include "PlayerST.h"
#include "BossDecoUI.h"
#include "BossHP.h"
#include "ItemSlot.h"
#include "RightArrow.h"
#include "LeftArrow.h"
#include "Item.h"
#include "MenuBaseUI.h"
#include "MenuIcon.h"
#include "SlotCnt_UI.h"
#include "Item_QuickSlot.h"
#include "Menu_Status.h"
#include "Menu_Item.h"
#include "ActiveSkill_UI.h"
#include "Menu_Active.h"

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

	// BT_Node 생성 중
	if (FAILED(pManagement->Ready_BT_Node()))
		return E_FAIL;

	_mat DefaultMat;
	D3DXMatrixIdentity(&DefaultMat);

	
	// 오브젝트 원형
	lstrcpy(m_szString, L"게임오브젝트 원형 생성 중....");
	if (FAILED(pManagement->Add_Prototype(L"GameObject_Player", CPlayer::Create(m_pGraphicDev))))
		return E_FAIL;

	// Sky
	if (FAILED(pManagement->Add_Prototype(L"GameObject_Sky", CSky::Create(m_pGraphicDev))))
		return E_FAIL;

	lstrcpy(m_szString, L"이펙트 생성 중....");
	Ready_Effect();

	// UI
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_HPBack", CHPBack::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_PlayerHP", CPlayerHP::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_PlayerST", CPlayerST::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_BossDecoUI", CBossDecoUI::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_BossHP", CBossHP::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_ItemSlot", CItemSlot::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_RightArrow", CRightArrow::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_LeftArrow", CLeftArrow::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_Item", CItem::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_MenuBase", CMenuBaseUI::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_MenuIcon", CMenuIcon::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_CntSlotUI", CSlotCnt_UI::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_ItemQuickSlot", CItem_QuickSlot::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_MenuStatus", CMenu_Status::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_MenuItem", CMenu_Item::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_ActionUI", CActiveSkill_UI::Create(m_pGraphicDev))))
	//	return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_MenuActive", CMenu_Active::Create(m_pGraphicDev))))
	//	return E_FAIL;

	//몬스터
	if (FAILED(pManagement->Add_Prototype(L"Monster_TestMonster", CTestMonster::Create(m_pGraphicDev))))
		return E_FAIL;

	//무기
	if (FAILED(pManagement->Add_Prototype(L"GameObject_Weapon", CWeapon::Create(m_pGraphicDev))))
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

HRESULT CLoading::Ready_Effect(void)
{
	CManagement*	pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return -1;

	Safe_AddRef(pManagement);

	if (FAILED(pManagement->Add_Prototype(L"Effect_TestSmoke", CTexEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/testSmoke.dat")))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"Effect_FootSmoke", CTexEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/Player_FootSmoke.dat")))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"Effect_ButterFly_SoftSmoke", CTexEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/ButterFly_SoftSmoke.dat")))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(L"Effect_ButterFly_Distortion", CTexEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/ButterFly_Distortion.dat")))))
		return E_FAIL;
	//if (FAILED(pManagement->Add_Prototype(L"GameObject_EffectTestMesh", CMeshEffect::Create(m_pGraphicDev, Read_EffectData(L"../../Data/EffectData/testMeshEff.dat")))))
	//	return E_FAIL;

	Safe_Release(pManagement);
	return S_OK;
}

Engine::EFFECT_INFO* CLoading::Read_EffectData(const _tchar* szPath)
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
		::ReadFile(hFile, &pInfo->szColorName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->szName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->szGradientName, sizeof(TCHAR) * MAX_STR, &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vEndColor, sizeof(_v4), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vMoveDirection, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vMoveScale, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vRandDirectionRange, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vRotDirection, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vStartColor, sizeof(_v4), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vStartPos, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->vStartScale, sizeof(_v3), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->fColorIndex, sizeof(_float), &dwByte, nullptr);

		break;
	}

	CloseHandle(hFile);

	return pInfo;
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

