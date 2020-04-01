#include "stdafx.h"
#include "Loading.h"
#include "Management.h"

#include "Sky.h"
#include "TexEffect.h"
#include "MeshEffect.h"
#include "Player.h"
#include "HPBack.h"
#include "TestMonster.h"
#include "Weapon.h"
#include "Dummy_Target.h"
#include "Trail_VFX.h"

#include "UI_Manager.h"

#include "TestMonster.h"
#include "PoisonButterfly.h"
#include "PoisonBullet.h"
#include "BlackUrchin.h"
#include "BlackWolf.h"
#include "GunGenji.h"
#include "SwordGenji.h"
#include "SwordShieldGenji.h"

#include "PlayerHP.h"
#include "PlayerST.h"
#include "BossDecoUI.h"
#include "BossHP.h"

#include "Item.h"


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
	// BT_Node 생성 중
	if (FAILED(g_pManagement->Ready_BT_Node()))
		return E_FAIL;

	_mat DefaultMat;
	D3DXMatrixIdentity(&DefaultMat);

	// 이펙트 원형 생성
	Ready_Effect();

	
	// 오브젝트 원형 생성
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// 플레이어
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Player", CPlayer::Create(m_pGraphicDev))))
		return E_FAIL;

	g_pManagement->LoadTex_FromPath(m_pGraphicDev, L"../../Data/Tex_Path.dat");


	g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Mesh_Path.dat");

	//몬스터

	if (FAILED(g_pManagement->Add_Prototype(L"Monster_TestMonster", CTestMonster::Create(m_pGraphicDev))))
		return E_FAIL;
	// 독나방
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_PoisonButterfly", CPoisonButterfly::Create(m_pGraphicDev))))
		return E_FAIL;
	// 검은 성게
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_BlackUrchin", CBlackUrchin::Create(m_pGraphicDev))))
		return E_FAIL;
	// 검은 늑대
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_BlackWolf", CBlackWolf::Create(m_pGraphicDev))))
		return E_FAIL;
	// 독나비 독 총알
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_PoisonBullet", CPoisonBullet::Create(m_pGraphicDev))))
		return E_FAIL;


	//무기
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Weapon", CWeapon::Create(m_pGraphicDev))))
		return E_FAIL;

	//더미
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Dummy", CDummy_Target::Create(m_pGraphicDev))))
		return E_FAIL;

	// 트레일
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_SwordTrail", Engine::CTrail_VFX::Create(m_pGraphicDev))))
		return E_FAIL;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	m_bFinish = true;

	cout << "로딩 완료" << endl;

	return 0;
}

unsigned int CALLBACK CLoading::Thread_Main(void* pArg)
{
	CLoading*	pLoading = (CLoading*)pArg;

	_uint		iFlag = 0;

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case SCENE_TITLE:
	{
		iFlag = pLoading->Loading_Title();
		break;
	}

	case SCENE_STAGE:
	{
		iFlag = pLoading->Loading_Stage();
		break;
	}
	}

	LeaveCriticalSection(pLoading->Get_Crt());

	return iFlag;
}

HRESULT CLoading::Ready_Effect(void)
{
	g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/EffectMesh_Path.dat"); // 임시
	

#pragma region ButterFly
	if (FAILED(Add_EffectPrototype(L"ButterFly_SoftSmoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_PointParticle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_RingLine", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_RingLine_Distortion", true)))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_Distortion")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_Distortion_Circle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_SoftSmoke_Bottom")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_Smoke_Red_Once")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_Smoke_Red_Particle")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_SoftSmoke_Floor")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_SoftSmoke_Mist")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_PointParticle_Plum")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_SoftSmoke_Ready")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"ButterFly_WaterSplash")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_GlitterSand")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_Distortion_Smoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_PopSand")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot_SubSmoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot_PointParticle")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot_DeadMist")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot_DeadSmoke")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_VenomShot_DeadSplash")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_BackStepSand01")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_BackStepSand02")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_BackStepSand_Floor")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"ButterFly_Distortion_SmokeGravity")))
		return E_FAIL;
#pragma endregion

	if (FAILED(Add_EffectPrototype(L"Boss_KnonkDown_Dust")))
		return E_FAIL;


	if (FAILED(Add_EffectPrototype(L"Player_FootSmoke")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"Hit_Blood_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Blood_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Blood_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_BloodMist_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_BloodMist_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_BloodParticle_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_BloodParticle_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_BloodParticle_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_BloodParticle_3")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"Hit_Slash_Particle_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Slash_Particle_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Slash_Particle_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Slash_Particle_3")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Slash_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Slash_1")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Slash_2")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Slash_3")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_SlashLine_0")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Particle_Red")))
		return E_FAIL;
	if (FAILED(Add_EffectPrototype(L"Hit_Particle_Yellow")))
		return E_FAIL;

	if (FAILED(Add_EffectPrototype(L"SpawnParticle")))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CLoading::Add_EffectPrototype(const _tchar* szName, _bool bIsMesh)
{
	_tchar szBuff[256] = L"../../Data/EffectData/";
	lstrcat(szBuff, szName);
	lstrcat(szBuff, L".dat");

	if (!bIsMesh)
	{
		if (FAILED(g_pManagement->Add_Prototype(szName, CTexEffect::Create(m_pGraphicDev, Read_EffectData(szBuff)))))
			return E_FAIL;
	}
	else
	{
		if (FAILED(g_pManagement->Add_Prototype(szName, CMeshEffect::Create(m_pGraphicDev, Read_EffectData(szBuff)))))
			return E_FAIL;
	}
	

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
		::ReadFile(hFile, &pInfo->fMaskIndex, sizeof(_float), &dwByte, nullptr);
		
		_bool bTemp;
		::ReadFile(hFile, &bTemp, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &bTemp, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &bTemp, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &bTemp, sizeof(_bool), &dwByte, nullptr);

		::ReadFile(hFile, &pInfo->bGravity, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bRandScale, sizeof(_bool), &dwByte, nullptr);
		::ReadFile(hFile, &pInfo->bMoveWithRot, sizeof(_bool), &dwByte, nullptr);

		break;
	}

	CloseHandle(hFile);

	return pInfo;
}

_uint CLoading::Loading_Title()
{
	cout << "Title_Loading . . ." << endl;
	cout << "===============================================================================" << endl;

	// 필수 메쉬 불러오는중
	//============================================================================================================
	cout << "필수 메쉬 불러오는중" << endl;
	g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Mesh_Essential_Path.dat");

	// 텍스쳐 불러오는 중
	//============================================================================================================
	cout << "텍스쳐 불러오는중" << endl;
	g_pManagement->LoadTex_FromPath(m_pGraphicDev, L"../../Data/Tex_Path.dat");

	//============================================================================================================
	
	// 이펙트 원형 생성
	//============================================================================================================
	cout << "이펙트 원형 생성중" << endl;
	Ready_Effect();

	// UI 원형 생성
	//============================================================================================================
	cout << " UI 원형 생성중" << endl;
	CUI_Manager::Get_Instance()->Add_UI_Prototype(m_pGraphicDev);

	//============================================================================================================

	m_bFinish = true;

	system("cls");
	cout << "===============================================================================" << endl;
	cout << "타이틀 로딩 완료" << endl;
	cout << "===============================================================================" << endl;
	cout << " 1. 위 숫자 키를 누르면 옵션이 활성화 됩니다." << endl;
	cout << " 2. (Load_StaticMesh) 가 (false) 이면 스테이지 들어가도 생성 안됩니다." << endl;
	cout << " 3. Space 를 누르면 다음 스테이지로 넘어갑니다." << endl;
	cout << " 4. 현재 Stage_(n) 은 실행되지 않고 자동으로 Training Stage 로 넘어갑니다." << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	cout << "[1] Stage_Base = true " << endl;
	cout << "[2] Stage_Training = false " << endl;
	cout << "[3] Stage_01 = false " << endl;
	cout << "[4] Stage_02 = false " << endl;
	cout << "[5] Stage_03 = false " << endl;
	cout << "-------------------------------------------------------------------------------" << endl;
	cout << "[6] Load_StaticMesh = false " << endl;
	cout << "-------------------------------------------------------------------------------" << endl;


	return NO_EVENT;
}

_uint CLoading::Loading_Stage()
{
	// 스태틱 메쉬 불러오는 중
	//============================================================================================================
	if(m_bLoadStaticMesh)
		g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Mesh_Static_Path.dat");


	// 다이나믹 메쉬 불러오는 중
	//============================================================================================================
	g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Mesh_Dynamic_Path.dat");


	// 무기 불러오는 중
	//============================================================================================================
	g_pManagement->LoadMesh_FromPath(m_pGraphicDev, L"../../Data/Mesh_Weapon_Path.dat");

	// BT_Node 생성 중
	//============================================================================================================
	if (FAILED(g_pManagement->Ready_BT_Node()))
		return E_FAIL;


	// 오브젝트 원형 생성
	//============================================================================================================

	// 플레이어
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Player", CPlayer::Create(m_pGraphicDev))))
		return E_FAIL;

	// 몬스터
	//============================================================================================================

	if (FAILED(g_pManagement->Add_Prototype(L"Monster_TestMonster", CTestMonster::Create(m_pGraphicDev))))
		return E_FAIL;
	// 독나방
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_PoisonButterfly", CPoisonButterfly::Create(m_pGraphicDev))))
		return E_FAIL;
	// 검은 성게
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_BlackUrchin", CBlackUrchin::Create(m_pGraphicDev))))
		return E_FAIL;
	// 검은 늑대
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_BlackWolf", CBlackWolf::Create(m_pGraphicDev))))
		return E_FAIL;
	// 총겐지
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_GunGenji", CGunGenji::Create(m_pGraphicDev))))
		return E_FAIL;
	// 검겐지
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_SwordGenji", CSwordGenji::Create(m_pGraphicDev))))
		return E_FAIL;
	// 검방패겐지
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_SwordShieldGenji", CSwordShieldGenji::Create(m_pGraphicDev))))
		return E_FAIL;
	// 독나비 독 총알
	if (FAILED(g_pManagement->Add_Prototype(L"Monster_PoisonBullet", CPoisonBullet::Create(m_pGraphicDev))))
		return E_FAIL;

	// 기타
	//============================================================================================================

	//무기
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Weapon", CWeapon::Create(m_pGraphicDev))))
		return E_FAIL;
	//더미
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_Dummy", CDummy_Target::Create(m_pGraphicDev))))
		return E_FAIL;
	// 트레일
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_SwordTrail", Engine::CTrail_VFX::Create(m_pGraphicDev))))
		return E_FAIL;

	//============================================================================================================
	m_bFinish = true;

	cout << "로딩 완료" << endl;

	return NO_EVENT;
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

