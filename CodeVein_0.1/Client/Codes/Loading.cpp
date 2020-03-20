#include "stdafx.h"
#include "Loading.h"
#include "Management.h"

#include "Sky.h"
#include "Effect.h"
#include "Player.h"
#include "Terrain.h"
#include "TestMonster.h"
#include "HPBack.h"
#include "HPBar.h"


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

	// 오브젝트 원형
	lstrcpy(m_szString, L"게임오브젝트 원형 생성 중....");	
	if (FAILED(pManagement->Add_Prototype(L"GameObject_Player", CPlayer::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype(L"GameObject_Effect", CEffect::Create(m_pGraphicDev, nullptr))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype(L"GameObject_HPBack", CHPBack::Create(m_pGraphicDev))))
		return E_FAIL;

	if (FAILED(pManagement->Add_Prototype(L"GameObject_HPBar", CHPBar::Create(m_pGraphicDev))))
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

	// UI텍스쳐 (임시)
	if (FAILED(pManagement->Add_Prototype(SCENE_STAGE, L"Texture_HPBack", CTexture::Create(m_pGraphicDev, CTexture::TYPE_GENERAL, L"../Bin/Resources/Texture/TestUI/HPBarBack/HPBack.png", 1))))
		return E_FAIL;
	if (FAILED(pManagement->Add_Prototype(SCENE_STAGE, L"Texture_HPBar", CTexture::Create(m_pGraphicDev, CTexture::TYPE_GENERAL, L"../Bin/Resources/Texture/TestUI/HPBar/T_HPGaugeTexture_UI.tga", 1))))
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

