#include "stdafx.h"
#include "..\Headers\QueensKnight.h"
#include "..\Headers\BossHP.h"
#include "MassageUI.h"

CQueensKnight::CQueensKnight(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CQueensKnight::CQueensKnight(const CQueensKnight & rhs)
	: CMonster(rhs)
{ 
}

HRESULT CQueensKnight::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CQueensKnight::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_NF(pArg);
	Ready_Weapon();
	Ready_BoneMatrix();
	Ready_Collider();
	Ready_Sound();

	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 10000.f;
	m_tObjParam.fHp_Max = m_tObjParam.fHp_Cur;
	m_tObjParam.fDamage = 20.f;

	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	///////////////// 행동트리 init

	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL);

	if (nullptr == pPlayer)
		return E_FAIL;

	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create();	//인자에 true 주면 콘솔창에 디버깅정보 뜸, default = false

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", m_tObjParam.fHp_Cur);
	pBlackBoard->Set_Value(L"MAXHP", m_tObjParam.fHp_Max);
	pBlackBoard->Set_Value(L"Show", true);
	pBlackBoard->Set_Value(L"Show_Near", true);

	pBlackBoard->Set_Value(L"PushCol", true);	// 충돌여부 제어변수
	pBlackBoard->Set_Value(L"PhyCol", true);	// 피격판정 제어 변수
	pBlackBoard->Set_Value(L"TrailOn", false);	// 트레일 시작
	pBlackBoard->Set_Value(L"TrailOff", true);	// 트레일 끝
	pBlackBoard->Set_Value(L"LeakField_On", false);	// 리크필드 변수

	//소리 제어 변수
	pBlackBoard->Set_Value(L"SFX_01_Tag", 0);
	pBlackBoard->Set_Value(L"SFX_01_Play", false);
	pBlackBoard->Set_Value(L"SFX_01_Stop", false);
	pBlackBoard->Set_Value(L"SFX_02_Tag", 0);
	pBlackBoard->Set_Value(L"SFX_02_Play", false);
	pBlackBoard->Set_Value(L"SFX_02_Stop", false);
	pBlackBoard->Set_Value(L"Voice_Tag", 0);
	pBlackBoard->Set_Value(L"Voice_Play", false);
	pBlackBoard->Set_Value(L"Voice_Stop", false);

	pBlackBoard->Set_Value(L"CamShake1", false);
	pBlackBoard->Set_Value(L"CamShake2", false);
	pBlackBoard->Set_Value(L"CamShake4", false);

	CBT_Selector* Start_Sel = Node_Selector("행동 시작");
	//CBT_Sequence* Start_Sel = Node_Sequence("행동 시작");	//테스트

	pBehaviorTree->Set_Child(Start_Sel);


	//////////// 아래에 주석해놓은 4줄이 본게임에서 쓸 것임, 차례대로 공격함.

	CBT_CompareValue* Check_ShowValue = Node_BOOL_A_Equal_Value("시연회 변수 체크", L"Show", false);
	Check_ShowValue->Set_Child(Start_Game());
	Start_Sel->Add_Child(Check_ShowValue);
	Start_Sel->Add_Child(Start_Show());

	////////////

	// 패턴 확인용,  각 패턴 함수를 아래에 넣으면 재생됨

	//Start_Sel->Add_Child(LeakField());

	//CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.2);
	//Start_Sel->Add_Child(Rotation0);

	//CBT_Wait* Wait0 = Node_Wait("대기", 1, 0);
	//Start_Sel->Add_Child(Wait0);

	/////////////
	// UI 추가(지원)
	m_pBossUI = static_cast<CBossHP*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BossHP", nullptr));
	m_pBossUI->Set_UI_Pos(WINCX * 0.5f, WINCY * 0.1f);
	m_pBossUI->Set_BossName(CBossNameUI::Index_QueensKnight);
	if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBossUI, SCENE_STAGE, L"Layer_BossHP", nullptr)))
		return E_FAIL;

	m_pMeshCom->SetUp_Animation(Ani_Idle);

	return S_OK;
}

_int CQueensKnight::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if ( true == m_pAIControllerCom->Get_BoolValue(L"PushCol"))
		Push_Collider();
	
	CGameObject::Update_GameObject(TimeDelta);

	// 죽었을 경우
	if (m_bIsDead)
		m_bEnable = false;

	// 죽음 애니메이션
	if (m_bReadyDead)
	{
		// 죽으면서 UI 비활성화
		m_pBossUI->Set_Active(false);

		if (false == m_bFinishCamShake && m_pMeshCom->Is_Finish_Animation(0.5f))
		{
			m_bFinishCamShake = true;
			SHAKE_CAM_lv3;
		}

		return NO_EVENT;
	}
		

	// 플레이어 미발견
	if (false == m_bFight)
	{
		Update_NF();
	}
	// 플레이어 발견
	else
	{
		//// 어그로 관리
		Set_Target_Auto(true);

		// 뼈 위치 업데이트
		Update_Bone_Of_BlackBoard();
		// BB 직접 업데이트
		Update_Value_Of_BB();

		if (true == m_bAIController)
			m_pAIControllerCom->Update_AIController(TimeDelta);

		if (!m_bUITrigger)
		{
			m_bUITrigger = true;

			// 플레이어 발견 시, UI 활성화(지원)
			m_pBossUI->Set_Active(true);

			CMassageUI* pMassageUI = static_cast<CMassageUI*>(g_pManagement->Get_GameObjectBack(L"Layer_BossMassageUI", SCENE_STAGE));
			pMassageUI->Set_BossName(BOSS_NAME::Queens_Knight);
			pMassageUI->Set_Check_Play_BossnameUI(true);

			g_pSoundManager->Stop_Sound(CSoundManager::Background_01);
			g_pSoundManager->Play_BGM(L"Boss_QueensKnight_BGM.ogg");
		}

		// 보스UI 업데이트
		// 체력이 0이 되었을때 밀림현상 방지.
		if (0 >= m_tObjParam.fHp_Cur)
			m_pBossUI->Set_BossHPInfo(0, 100);
		else
			m_pBossUI->Set_BossHPInfo(m_tObjParam.fHp_Cur, m_tObjParam.fHp_Max);
	}

	if (false == m_bReadyDead && true == m_pAIControllerCom->Get_BoolValue(L"PhyCol"))
		Check_PhyCollider();

	OnCollisionEnter();

	m_pTransformCom->Set_Pos(m_pNavMeshCom->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	//====================================================================================================
	// 컬링
	//====================================================================================================
	m_bInFrustum = m_pOptimizationCom->Check_InFrustumforObject(&m_pTransformCom->Get_Pos(), 2.f);
	//====================================================================================================

	return NOERROR;
}

_int CQueensKnight::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == m_pRendererCom)
		return E_FAIL;

	//=============================================================================================
	// 그림자랑 모션블러는 프리스텀 안에 없으면 안그림
	//=============================================================================================
	if (!m_bDissolve)
	{
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
			return E_FAIL;
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_SHADOWTARGET, this)))
			return E_FAIL;
	}

	else
	{
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_ALPHA, this)))
			return E_FAIL;
	}

	if (m_bInFrustum)
	{
		if (false == m_bDissolve)
		{
			if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
				return E_FAIL;
		}
	}
	//=============================================================================================

	m_dTimeDelta = TimeDelta;

	m_pSword->Late_Update_GameObject(TimeDelta);
	m_pShield->Late_Update_GameObject(TimeDelta);

	return NOERROR;
}

HRESULT CQueensKnight::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	m_pMeshCom->Play_Animation(_float(m_dTimeDelta)); // * alpha

	if (m_bInFrustum)
	{
		if (FAILED(SetUp_ConstantTable(m_pShaderCom)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();

		_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

		for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
		{
			_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

			m_pMeshCom->Update_SkinnedMesh(i);

			for (_uint j = 0; j < iNumSubSet; ++j)
			{
				m_iPass = m_pMeshCom->Get_MaterialPass(i, j);

				if (m_bDissolve)
					m_iPass = 3;

				m_pShaderCom->Begin_Pass(m_iPass);

				m_pShaderCom->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

				m_pShaderCom->Commit_Changes();

				m_pMeshCom->Render_Mesh(i, j);

				m_pShaderCom->End_Pass();
			}
		}

		m_pShaderCom->End_Shader();
	}

	m_pSword->Update_GameObject(m_dTimeDelta);
	m_pShield->Update_GameObject(m_dTimeDelta);
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CQueensKnight::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	if (nullptr == pShader ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	m_pMeshCom->Play_Animation(DELTA_60 * m_dAniPlayMul);

	if (m_bInFrustum)
	{
		if (FAILED(SetUp_ConstantTable(pShader)))
			return E_FAIL;

		_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

		for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
		{
			_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

			m_pMeshCom->Update_SkinnedMesh(i);

			for (_uint j = 0; j < iNumSubSet; ++j)
			{
				m_iPass = m_pMeshCom->Get_MaterialPass(i, j);

				if (m_bDissolve)
					m_iPass = 3;

				pShader->Begin_Pass(m_iPass);

				pShader->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

				pShader->Commit_Changes();

				m_pMeshCom->Render_Mesh(i, j);

				pShader->End_Pass();
			}
		}

	}

	m_pSword->Update_GameObject(m_dTimeDelta);
	m_pShield->Update_GameObject(m_dTimeDelta);
	Update_Collider();
	Draw_Collider();

	return S_OK;
}

HRESULT CQueensKnight::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	if (false == m_bEnable)
		return S_OK;

	if (nullptr == pShader ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	//============================================================================================
	// 공통 변수
	//============================================================================================

	_mat	ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat	ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);
	_mat	WorldMatrix = m_pTransformCom->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &WorldMatrix, sizeof(_mat))))
		return E_FAIL;

	//============================================================================================
	// 모션 블러 상수
	//============================================================================================
	if (_bIsForMotionBlur)
	{
		if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
			return E_FAIL;

		m_matLastWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		_bool bMotionBlur = true;
		if (FAILED(pShader->Set_Bool("g_bMotionBlur", bMotionBlur)))
			return E_FAIL;
		_bool bDecalTarget = false;
		if (FAILED(pShader->Set_Bool("g_bDecalTarget", bDecalTarget)))
			return E_FAIL;
		_float fBloomPower = 0.5f;
		if (FAILED(pShader->Set_Value("g_fBloomPower", &fBloomPower, sizeof(_float))))
			return E_FAIL;
	}

	//============================================================================================
	// 기타 상수
	//============================================================================================
	else
	{
		_mat matWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		if (FAILED(pShader->Set_Value("g_matWVP", &matWVP, sizeof(_mat))))
			return E_FAIL;
	}

	//============================================================================================
	// 쉐이더 실행
	//============================================================================================
	_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			_int tmpPass = m_pMeshCom->Get_MaterialPass(i, j);

			pShader->Begin_Pass(iPass);
			pShader->Commit_Changes();

			pShader->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	//============================================================================================

	return NOERROR;
}

CBT_Composite_Node * CQueensKnight::Normal_HorizontalCut1()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("일반 가로베기1");
	CBT_Play_Ani* Show_Ani37 = Node_Ani("일반 가로베기1", 37, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.166, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.567, 0);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 7.f, 0.333, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"	, 0.5, 13, 0.1, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"	, 0.5, 13, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos"	, 0.5, 13, 0.1, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos", 0.5, 13, 0.1, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"	, 0.5, 13, 0.1, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"	, 0.5, 13, 0.1, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_0", L"Sword_MidPos"	, 0.5, 25, 0.1, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_1", L"Sword_MidPos"	, 0.5, 25, 0.1, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani37);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(TrailOff);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.866, 1, 0.317, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Normal_VerticalCut1()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("일반 세로베기1");
	CBT_Play_Ani* Show_Ani36 = Node_Ani("일반 세로베기1", 36, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.333, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 6, 0.4, 0);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.067, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("카메라 흔들기", L"CamShake1", true);
	CBT_Wait* Wait2 = Node_Wait("대기2", 1.05, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -2, 0.216, 0);
	CBT_Wait* Wait3 = Node_Wait("대기3", 0.15, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 1, 0.484, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos", 0.3, 13, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos", 0.3, 13, 0.1, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_0", L"Sword_MidPos", 0.3, 20, 0.1, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_1", L"Sword_MidPos", 0.3, 20, 0.1, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos", 0.3, 13, 0.1, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos", 0.3, 13, 0.1, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("내려찍기 파티클", L"QueensKnight_SwordCrash_Particle", L"Sword_TopPos"		, 0.9, 5, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("내려찍기 파티클", L"QueensKnight_SwordCrash_Particle", L"Sword_MidPos"		, 0.9, 5, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("내려찍기 파티클", L"QueensKnight_SwordCrash_Particle_Orange", L"Sword_MidPos"	, 0.9, 5, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);
	Root_Parallel->Add_Service(Effect8);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani36);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(TrailOff);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait3);
	SubSeq->Add_Child(Move2);


	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.75, 1, 0.15, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::TwoCombo_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("2연속 베기");
	CBT_Play_Ani* Show_Ani37 = Node_Ani("일반 가로베기1", 37, 0.5);
	CBT_Play_Ani* Show_Ani35 = Node_Ani("일반 세로베기", 35, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.166, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.567, 0);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_SetValue* TrailOn0 = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 7.f, 0.333, 0);
	CBT_SetValue* TrailOff0 = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.45, 0);
	//50번애니 * 0.5 = 1.616
	CBT_RotationDir* Rotation1 = Node_RotationDir("플레이어 바라보기", L"Player_Pos", 0.09);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("플레이어 바라보기", L"Player_Pos", 0.01, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.15, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 6.f, 0.366, 0);
	CBT_SetValue* Sound2Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Stop", true);
	CBT_SetValue* Sound2Play = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Play", true);
	CBT_SetValue* Sound2Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_02_Tag", 0);
	CBT_SetValue* TrailOn1 = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_Wait* Wait3 = Node_Wait("대기3", 0.15, 0);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("카메라 흔들기", L"CamShake2", true);
	CBT_SetValue* TrailOff1 = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);
	CBT_Wait* Wait4 = Node_Wait("대기4", 0.784, 0);
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("이동3", L"Monster_Speed", L"Monster_Dir", -1.f, 0.483, 0);
	CBT_MoveDirectly* Move4 = Node_MoveDirectly_Rush("이동4", L"Monster_Speed", L"Monster_Dir", 1.f, 0.5, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"	, 0.6, 15, 0.5, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"	, 0.6, 15, 0.5, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos"	, 0.6, 15, 0.5, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_0", L"Sword_MidPos"	, 0.6, 15, 0.5, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_1", L"Sword_MidPos"	, 0.6, 15, 0.5, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"	, 2.1, 15, 0.5, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"	, 2.1, 15, 0.5, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos"	, 2.1, 15, 0.5, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_0", L"Sword_MidPos"	, 2.1, 15, 0.5, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_1", L"Sword_MidPos"	, 2.1, 15, 0.5, 0);
	CBT_CreateEffect* Effect8  = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos"	, 0.6, 10, 0.5, 0);
	CBT_CreateEffect* Effect9  = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"	, 0.6, 10, 0.5, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"	, 0.6, 10, 0.5, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos"	, 2.1, 10, 0.5, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"	, 2.1, 10, 0.5, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"	, 2.1, 10, 0.5, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("내려찍기 파티클", L"QueensKnight_SwordCrash_Particle", L"Sword_TopPos", 2.4, 5, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("내려찍기 파티클", L"QueensKnight_SwordCrash_Particle", L"Sword_MidPos", 2.4, 5, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);
	Root_Parallel->Add_Service(Effect8);
	Root_Parallel->Add_Service(Effect9);
	Root_Parallel->Add_Service(Effect10);
	Root_Parallel->Add_Service(Effect11);
	Root_Parallel->Add_Service(Effect12);
	Root_Parallel->Add_Service(Effect13);
	Root_Parallel->Add_Service(Effect14);
	Root_Parallel->Add_Service(Effect15);
	Root_Parallel->Add_Service(Effect16);
	Root_Parallel->Add_Service(Effect17);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani37);
	MainSeq->Add_Child(Show_Ani35);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(TrailOn0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(TrailOff0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Rotation1);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(Sound2Stop);
	SubSeq->Add_Child(Sound2Play);
	SubSeq->Add_Child(Sound2Tag);
	SubSeq->Add_Child(TrailOn1);
	SubSeq->Add_Child(Wait3);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(TrailOff1);
	SubSeq->Add_Child(Wait4);
	SubSeq->Add_Child(Move3);
	SubSeq->Add_Child(Move4);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.866, 1, 0.317, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.616 + 0.533, 1, 0.15, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);
	
	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::ThreeCombo_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("2연속 베기");
	CBT_Play_Ani* Show_Ani37 = Node_Ani("일반 가로베기1", 37, 0.5f);
	CBT_Play_Ani* Show_Ani35 = Node_Ani("일반 세로베기", 35, 0.4f);
	CBT_Play_Ani* Show_Ani32 = Node_Ani("방패치기", 32, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.166, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.567, 0);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_SetValue* TrailOn0 = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 7.f, 0.333, 0);
	CBT_SetValue* TrailOff0 = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.45, 0);
	//50번애니 * 0.5 = 1.616
	CBT_RotationDir* Rotation1 = Node_RotationDir("플레이어 바라보기1", L"Player_Pos", 0.09);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("플레이어 바라보기", L"Player_Pos", 0.01, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.15, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 6.f, 0.366, 0);
	CBT_SetValue* Sound2Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Stop", true);
	CBT_SetValue* Sound2Play = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Play", true);
	CBT_SetValue* Sound2Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_02_Tag", 0);
	CBT_SetValue* TrailOn1 = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_Wait* Wait3 = Node_Wait("대기3", 0.857, 0);
	CBT_SetValue* TrailOff1 = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);
	CBT_RotationDir* Rotation2 = Node_RotationDir("플레이어 바라보기2", L"Player_Pos", 0.09);
	CBT_ChaseDir* ChaseDir1 = Node_ChaseDir("플레이어 바라보기1", L"Player_Pos", 0.01, 0);
	//48번애니 * 0.4 = 1.473
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("이동3", L"Monster_Speed", L"Monster_Dir", 6.f, 0.45, 0);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("카메라 흔들기", L"CamShake2", true);
	CBT_SetValue* Sound1Stop1 = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play1 = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag1 = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 1);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"	, 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"	, 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos"	, 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_0", L"Sword_MidPos"	, 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_1", L"Sword_MidPos"	, 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"	, 2.1, 15, 0.1, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"	, 2.1, 15, 0.1, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos"	, 2.1, 15, 0.1, 0);
	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_0", L"Sword_MidPos"	, 2.1, 15, 0.1, 0);
	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_1", L"Sword_MidPos"	, 2.1, 15, 0.1, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos"	, 0.6, 12, 0.1, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"	, 0.6, 12, 0.1, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"	, 0.6, 12, 0.1, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos"	, 2.1, 12, 0.1, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"	, 2.1, 12, 0.1, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"	, 2.1, 12, 0.1, 0);
	
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("내려찍기 파티클", L"QueensKnight_SwordCrash_Particle", L"Sword_TopPos"	, 2.36, 5, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("내려찍기 파티클", L"QueensKnight_SwordCrash_Particle", L"Sword_MidPos"	, 2.36, 5, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("방패치기 파티클", L"QueensKnight_ShieldAttack_Particle", L"ShieldPos"		, 3.45, 5, 0.01, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("방패치기 왜곡", L"QueensKnight_ShieldAttack_Distortion", L"ShieldPos"		, 3.6, 2, 0.1, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("방패치기 붉은 빛", L"QueensKnight_ShieldAttack_RedLight", L"ShieldPos"	, 3.45, 1, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);
	Root_Parallel->Add_Service(Effect8);
	Root_Parallel->Add_Service(Effect9);
	Root_Parallel->Add_Service(Effect10);
	Root_Parallel->Add_Service(Effect11);
	Root_Parallel->Add_Service(Effect12);
	Root_Parallel->Add_Service(Effect13);
	Root_Parallel->Add_Service(Effect14);
	Root_Parallel->Add_Service(Effect15);
	Root_Parallel->Add_Service(Effect16);
	Root_Parallel->Add_Service(Effect17);
	Root_Parallel->Add_Service(Effect18);
	Root_Parallel->Add_Service(Effect19);
	Root_Parallel->Add_Service(Effect20);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani37);
	MainSeq->Add_Child(Show_Ani35);
	MainSeq->Add_Child(Show_Ani32);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);	
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(TrailOn0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(TrailOff0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Rotation1);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(Sound2Stop);
	SubSeq->Add_Child(Sound2Play);
	SubSeq->Add_Child(Sound2Tag);
	SubSeq->Add_Child(TrailOn1);
	SubSeq->Add_Child(Wait3);
	SubSeq->Add_Child(TrailOff1);
	SubSeq->Add_Child(Rotation2);
	SubSeq->Add_Child(ChaseDir1);
	SubSeq->Add_Child(Move3);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(Sound1Stop1);
	SubSeq->Add_Child(Sound1Play1);
	SubSeq->Add_Child(Sound1Tag1);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.866, 1, 0.317, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.616 + 0.533, 1, 0.15, 0);
	CBT_UpdateParam* pHitCol2 = Node_UpdateParam("무기 히트 On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.616 + 1.473 + 0.333, 1, 0.167, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);
	Root_Parallel->Add_Service(pHitCol2);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::BackStep_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("백스텝 베기");
	CBT_Play_Ani* Show_Ani42 = Node_Ani("백스텝 베기", 42, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);
	
	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.8, 0);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_SetValue* TrailOn0 = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.066, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", -8, 0.484, 0);
	CBT_SetValue* TrailOff0 = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_1", L"Sword_MidPos"			, 0.5, 17, 0.8, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_0", L"Sword_MidPos"			, 0.5, 17, 0.8, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"			, 0.5, 17, 0.8, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"			, 0.5, 17, 0.8, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"		, 0.5, 17, 0.8, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"		, 0.5, 17, 0.8, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos"	, 0.5, 17, 0.8, 0);
	
	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani42);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(TrailOn0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(TrailOff0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.9, 1, 0.183, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Sting()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("찌르기");
	CBT_Play_Ani* Show_Ani29 = Node_Ani("찌르기", 29, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.366, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 6, 0.317, 0);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("카메라 흔들기", L"CamShake1", true);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_SetValue* TrailOn0 = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.133, 0);
	CBT_SetValue* TrailOff0 = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.85, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -1, 0.317, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 1, 0.35, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2"			, L"Sword_TopPos"										, 0.6, 17, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2"			, L"Sword_MidPos"										, 0.6, 17, 0.1, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_0"			, L"Sword_MidPos"										, 0.6, 17, 0.1, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_1"			, L"Sword_MidPos"										, 0.6, 17, 0.1, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark"	, L"Sword_TopPos"										, 0.6, 7, 0.1, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark"	, L"Sword_MidPos"										, 0.6, 7, 0.1, 0);
	CBT_CreateEffect* Effect1 = Node_New_CreateEffect_Finite("피 소용돌이", L"QueensKnight_Sting_Tornade"		, L"Sword_TopPos"	, nullptr, L"V3_Null", L"Self_Angle", 0.75, 1, 0.01, 0);
	
	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani29);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(TrailOn0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(TrailOff0);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.683, 1, 0.133, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Wing_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("돌면서 날개치기");
	CBT_Play_Ani* Show_Ani30 = Node_Ani("돌면서 날개치기", 30, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.05, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", -6, 0.25, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.85, 0);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation1 = Node_RotationDir("방향 추적2", L"Player_Pos", 0.1);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_SetValue* TrailOn0 = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 16, 0.483, 0);
	CBT_SetValue* TrailOff0 = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("먼지", L"QueensKnight_WhirlWind_Smoke", L"Self_Pos", 0.63, 10, 0.5, 0);

	Root_Parallel->Add_Service(Effect0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani30);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation1);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(TrailOn0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(TrailOff0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 1.533, 1, 0.45, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Rush()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("돌진 찌르기");
	CBT_Play_Ani* Show_Ani29 = Node_Ani("돌진 찌르기", 29, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);
	
	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.276, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적2", L"Player_Pos", 0.2);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 30, 0.307, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.983, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -1, 0.317, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 1, 0.35, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos"		, 0.6, 16, 0.1, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"		, 0.6, 16, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"		, 0.6, 16, 0.1, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"	, 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"	, 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos", 0.6, 15, 0.1, 0);
	//CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("피 소용돌이", L"QueensKnight_Sting_Tornade", L"Sword_TopPos", 0.65, 1, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	//Root_Parallel->Add_Service(Effect6);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani29);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(PushColOn);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.456, 1, 0.36, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Shield_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("방패치기");
	CBT_Play_Ani* Show_Ani33 = Node_Ani("방패치기", 33, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.083, 0);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 4.f, 0.4, 0);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("카메라 흔들기", L"CamShake2", true);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 1);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 4.f, 0.2, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.617, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -2.f, 0.333, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("방패치기 파티클", L"QueensKnight_ShieldAttack_Particle", L"ShieldPos", 0.65, 5, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("방패치기 왜곡", L"QueensKnight_ShieldAttack_Distortion", L"ShieldPos", 0.7, 10, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("방패치기 붉은 빛", L"QueensKnight_ShieldAttack_RedLight", L"ShieldPos", 0.65, 1, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani33);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move2);


	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.683, 1, 0.133, 0);
	Root_Parallel->Add_Service(pHitCol0);


	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::LeakField()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("리크필드");
	CBT_Play_Ani* Show_Ani39 = Node_Ani("리크필드", 39, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("보호막 변수 On");
	CBT_SetValue* LeakFieldOn = Node_BOOL_SetValue("리크필드 변수 On", L"LeakField_On", true);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 2);


	CBT_CreateBuff* LeakField0 = Node_CreateBuff("리크필드 생성", L"Monster_LeakField", 13, 2.533, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(LeakField0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("손에 초록 기운", L"QueensKnight_LeakField_Hand", L"Bone_LeftHand", 0.55, 75, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("손에 초록 기운2", L"QueensKnight_LeakField_Hand_Aura", L"Bone_LeftHand", 0.55, 75, 0, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani39);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(LeakFieldOn);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Flash()
{
	CBT_Sequence* MainSeq = Node_Sequence("이동");

	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 3);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("이동", L"FlashPos", 0.2);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOn", L"PhyCol", true);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);

	MainSeq->Add_Child(Sound1Stop);
	MainSeq->Add_Child(Sound1Play);
	MainSeq->Add_Child(Sound1Tag);
	MainSeq->Add_Child(PhyColOff);
	MainSeq->Add_Child(PushColOff);
	MainSeq->Add_Child(MoveTo0);
	MainSeq->Add_Child(PhyColOn);
	MainSeq->Add_Child(PushColOn);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("점멸 왜곡", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.0, 1, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("점멸 왜곡", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.16, 1, 0, 0);

	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_0", L"Self_MidPos", 0.0, 1, 0, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_1", L"Self_MidPos", 0.0, 1, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_2", L"Self_MidPos", 0.0, 1, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_4", L"Self_MidPos", 0.0, 1, 0, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_5", L"Self_MidPos", 0.0, 1, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_8", L"Self_MidPos", 0.0, 1, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_9", L"Self_MidPos", 0.0, 1, 0, 0);

	CBT_StartDissolve* Dissolve0 = Node_StartDissolve("디졸브", this, 12.7f, false, 0.01);
	CBT_StartDissolve* Dissolve1 = Node_StartDissolve("디졸브", this, 12.7f, true, 0.17);
	CBT_StartDissolve* Dissolve2 = Node_StartDissolve("디졸브", m_pSword, 12.7f, false, 0.01);
	CBT_StartDissolve* Dissolve3 = Node_StartDissolve("디졸브", m_pSword, 12.7f, true, 0.17);
	CBT_StartDissolve* Dissolve4 = Node_StartDissolve("디졸브", m_pShield, 12.7f, false, 0.01);
	CBT_StartDissolve* Dissolve5 = Node_StartDissolve("디졸브", m_pShield, 12.7f, true, 0.17);

	MainSeq->Add_Service(Effect0);
	MainSeq->Add_Service(Effect1);
	MainSeq->Add_Service(Effect2);
	MainSeq->Add_Service(Effect3);
	MainSeq->Add_Service(Effect4);
	MainSeq->Add_Service(Effect5);
	MainSeq->Add_Service(Effect6);
	MainSeq->Add_Service(Effect7);
	MainSeq->Add_Service(Effect8);

	MainSeq->Add_Service(Dissolve0);
	MainSeq->Add_Service(Dissolve1);
	MainSeq->Add_Service(Dissolve2);
	MainSeq->Add_Service(Dissolve3);
	MainSeq->Add_Service(Dissolve4);
	MainSeq->Add_Service(Dissolve5);

	return MainSeq;
}

CBT_Composite_Node * CQueensKnight::Flash_Wing_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("돌면서 날개치기");
	CBT_Play_Ani* Show_Ani30 = Node_Ani("돌면서 날개치기", 30, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.15, 0);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", -6, 0.25, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.75, 0);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 3);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("점멸 이동", L"FlashPos", 0.1);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOn", L"PhyCol", true);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적2", L"Player_Pos", 0.1);
	CBT_SetValue* Sound2Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Stop", true);
	CBT_SetValue* Sound2Play = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Play", true);
	CBT_SetValue* Sound2Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_02_Tag", 0);
	CBT_SetValue* TrailOn0 = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 16, 0.483, 0);
	CBT_SetValue* TrailOff0 = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("점멸 파티클", L"QueensKnight_Teleport_Particle", L"Self_MidPos", 0, 150, 0.15, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("먼지", L"QueensKnight_WhirlWind_Smoke", L"Self_Pos", 1.3, 20, 0.1, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("점멸 왜곡", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.2, 1, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("점멸 왜곡", L"QueensKnight_DistortionCircle", L"Self_MidPos", 1.3, 1, 0, 0);
	
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_0", L"Self_MidPos", 0.2, 1, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_1", L"Self_MidPos", 0.2, 1, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_2", L"Self_MidPos", 0.2, 1, 0, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_4", L"Self_MidPos", 0.2, 1, 0, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_5", L"Self_MidPos", 0.2, 1, 0, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_8", L"Self_MidPos", 0.2, 1, 0, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_9", L"Self_MidPos", 0.2, 1, 0, 0);

	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_0", L"Self_MidPos",  1.3, 1, 0, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_1", L"Self_MidPos",  1.3, 1, 0, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_2", L"Self_MidPos",  1.3, 1, 0, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_4", L"Self_MidPos",  1.3, 1, 0, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_5", L"Self_MidPos", 1.3, 1, 0, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_8", L"Self_MidPos", 1.3, 1, 0, 0);
	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_9", L"Self_MidPos", 1.3, 1, 0, 0);

	CBT_StartDissolve* Dissolve0 = Node_StartDissolve("디졸브", this		, 3.7f, false, 0.2);
	CBT_StartDissolve* Dissolve1 = Node_StartDissolve("디졸브", this		, 3.7f, true, 1.3);
	CBT_StartDissolve* Dissolve2 = Node_StartDissolve("디졸브", m_pSword	, 3.7f, false, 0.2);
	CBT_StartDissolve* Dissolve3 = Node_StartDissolve("디졸브", m_pSword	, 3.7f, true, 1.3);
	CBT_StartDissolve* Dissolve4 = Node_StartDissolve("디졸브", m_pShield	, 3.7f, false, 0.2);
	CBT_StartDissolve* Dissolve5 = Node_StartDissolve("디졸브", m_pShield	, 3.7f, true, 1.3);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);
	Root_Parallel->Add_Service(Effect8);
	Root_Parallel->Add_Service(Effect9);
	Root_Parallel->Add_Service(Effect10);
	Root_Parallel->Add_Service(Effect11);
	Root_Parallel->Add_Service(Effect12);
	Root_Parallel->Add_Service(Effect13);
	Root_Parallel->Add_Service(Effect14);
	Root_Parallel->Add_Service(Effect15);
	Root_Parallel->Add_Service(Effect16);
	Root_Parallel->Add_Service(Effect17);
	Root_Parallel->Add_Service(Effect18);
	Root_Parallel->Add_Service(Effect19);
	Root_Parallel->Add_Service(Dissolve0);
	Root_Parallel->Add_Service(Dissolve1);
	Root_Parallel->Add_Service(Dissolve2);
	Root_Parallel->Add_Service(Dissolve3);
	Root_Parallel->Add_Service(Dissolve4);
	Root_Parallel->Add_Service(Dissolve5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani30);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(PhyColOn);
	SubSeq->Add_Child(PushColOn);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Sound2Stop);
	SubSeq->Add_Child(Sound2Play);
	SubSeq->Add_Child(Sound2Tag);
	SubSeq->Add_Child(TrailOn0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(TrailOff0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 1.533, 1, 0.45, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Flash_Rush()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("돌진 찌르기");
	CBT_Play_Ani* Show_Ani29 = Node_Ani("돌진 찌르기", 29, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.176, 0);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 3);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("점멸 이동", L"FlashPos", 0.1);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOn", L"PhyCol", true);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적2", L"Player_Pos", 0.1);
	CBT_SetValue* Sound2Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Stop", true);
	CBT_SetValue* Sound2Play = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Play", true);
	CBT_SetValue* Sound2Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_02_Tag", 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 30, 0.307, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.983, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -1, 0.317, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 1, 0.35, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("점멸 파티클", L"QueensKnight_Teleport_Particle", L"Self_MidPos", 0, 150, 0.15, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos"			, 0.2, 17, 0.1, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"			, 0.2, 17, 0.1, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"			, 0.2, 17, 0.1, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_0", L"Sword_MidPos"			, 0.2, 17, 0.1, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_1", L"Sword_MidPos"			, 0.2, 17, 0.1, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"		, 0.2, 13, 0.1, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"		, 0.2, 13, 0.1, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos"	, 0.2, 13, 0.1, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("점멸 왜곡", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.17, 1, 0, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("점멸 왜곡", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.6, 1, 0, 0);
	
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_0", L"Self_MidPos", 0.17, 1, 0, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_1", L"Self_MidPos", 0.17, 1, 0, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_2", L"Self_MidPos", 0.17, 1, 0, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_4", L"Self_MidPos", 0.17, 1, 0, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_5", L"Self_MidPos", 0.17, 1, 0, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_8", L"Self_MidPos", 0.17, 1, 0, 0);
	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_9", L"Self_MidPos", 0.17, 1, 0, 0);

	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_0", L"Self_MidPos", 0.6, 1, 0, 0);
	CBT_CreateEffect* Effect21 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_1", L"Self_MidPos", 0.6, 1, 0, 0);
	CBT_CreateEffect* Effect22 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_2", L"Self_MidPos", 0.6, 1, 0, 0);
	CBT_CreateEffect* Effect23 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_4", L"Self_MidPos", 0.6, 1, 0, 0);
	CBT_CreateEffect* Effect24 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_5", L"Self_MidPos", 0.6, 1, 0, 0);
	CBT_CreateEffect* Effect25 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_8", L"Self_MidPos", 0.6, 1, 0, 0);
	CBT_CreateEffect* Effect26 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_9", L"Self_MidPos", 0.6, 1, 0, 0);

	CBT_StartDissolve* Dissolve0 = Node_StartDissolve("디졸브", this,  3.7f, false, 0.17);
	CBT_StartDissolve* Dissolve1 = Node_StartDissolve("디졸브", this, 3.7f, true, 0.6);
	CBT_StartDissolve* Dissolve2 = Node_StartDissolve("디졸브", m_pSword, 3.7f, false, 0.17);
	CBT_StartDissolve* Dissolve3 = Node_StartDissolve("디졸브", m_pSword, 3.7f, true, 0.6);
	CBT_StartDissolve* Dissolve4 = Node_StartDissolve("디졸브", m_pShield, 3.7f, false, 0.17);
	CBT_StartDissolve* Dissolve5 = Node_StartDissolve("디졸브", m_pShield, 3.7f, true, 0.6);

	Root_Parallel->Add_Service(Effect0);
	//Root_Parallel->Add_Service(Effect1);
	//Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);
	Root_Parallel->Add_Service(Effect8);
	Root_Parallel->Add_Service(Effect9);
	Root_Parallel->Add_Service(Effect10);
	Root_Parallel->Add_Service(Effect11);
	Root_Parallel->Add_Service(Effect12);
	Root_Parallel->Add_Service(Effect13);
	Root_Parallel->Add_Service(Effect14);
	Root_Parallel->Add_Service(Effect15);
	Root_Parallel->Add_Service(Effect16);
	Root_Parallel->Add_Service(Effect17);
	Root_Parallel->Add_Service(Effect18);
	Root_Parallel->Add_Service(Effect19);
	Root_Parallel->Add_Service(Effect20);
	Root_Parallel->Add_Service(Effect21);
	Root_Parallel->Add_Service(Effect22);
	Root_Parallel->Add_Service(Effect23);
	Root_Parallel->Add_Service(Effect24);
	Root_Parallel->Add_Service(Effect25);
	Root_Parallel->Add_Service(Effect26);
	Root_Parallel->Add_Service(Dissolve0);
	Root_Parallel->Add_Service(Dissolve1);
	Root_Parallel->Add_Service(Dissolve2);
	Root_Parallel->Add_Service(Dissolve3);
	Root_Parallel->Add_Service(Dissolve4);
	Root_Parallel->Add_Service(Dissolve5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani29);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(PhyColOn);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Sound2Stop);
	SubSeq->Add_Child(Sound2Play);
	SubSeq->Add_Child(Sound2Tag);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(PushColOn);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.476, 1, 0.34, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Flash_Jump_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("위에서 내려찍기");
	CBT_Play_Ani* Show_Ani25 = Node_Ani("점프 시작", 25, 0.7f);
	CBT_Play_Ani* Show_Ani27 = Node_Ani("내려 찍기", 27, 0.9f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.871, 0);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 3);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("점멸 이동", L"FlashPos", 0.05);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("카메라 흔들기", L"CamShake4", true);
	CBT_SetValue* Sound2Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Stop", true);
	CBT_SetValue* Sound2Play = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Play", true);
	CBT_SetValue* Sound2Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_02_Tag", 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOff", L"PhyCol", true);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적2", L"Player_Pos", 0.1);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("점멸 파티클", L"QueensKnight_Teleport_Particle", L"Self_MidPos", 0, 150, 0.15, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("점멸 검은 연기", L"QueensKnight_Teleport_Smoke", L"Self_MidPos", 0, 10, 0.35, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("점멸 왜곡", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.42, 1, 0, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("점멸 왜곡", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.68, 1, 0, 0);
	//CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("점멸 왜곡 연기", L"QueensKnight_Teleport_DistortionSmoke", L"Self_MidPos", 0.3, 20, 0.1, 0);
	//CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("내려찍기 파장", L"QueensKnight_JumpDown_ShockWave", L"Self_Pos"			, 1.45, 1, 0.01, 0);
	//CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("내려찍기 덩어리", L"QueensKnight_2Phase_SwordCrash_Chunk", L"Sword_TopPos", 1.45, 2, 0.01, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("내려찍기 덩어리", L"QueensKnight_2Phase_SwordCrash_Chunk", L"Sword_MidPos", 1.45, 2, 0.01, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("내려찍기 덩어리", L"QueensKnight_2Phase_SwordCrash_Chunk_Dark", L"Sword_MidPos", 1.45, 2, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("내려찍기 파티클", L"QueensKnight_JumpDown_Particle_Red", L"Sword_TopPos"	, 1.45, 2, 0.01, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("내려찍기 파티클", L"QueensKnight_JumpDown_Particle_Red", L"Sword_MidPos"	, 1.45, 2, 0.01, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("내려찍기 파티클", L"QueensKnight_SwordCrash_Particle", L"Sword_TopPos"	, 1.45, 5, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("내려찍기 파티클", L"QueensKnight_SwordCrash_Particle", L"Sword_MidPos"	, 1.45, 5, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos", 1.12, 20, 0.01, 0);

	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_0", L"Self_MidPos", 0.43, 1, 0, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_1", L"Self_MidPos", 0.43, 1, 0, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_2", L"Self_MidPos", 0.43, 1, 0, 0);
	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_4", L"Self_MidPos", 0.43, 1, 0, 0);
	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_5", L"Self_MidPos", 0.43, 1, 0, 0);
	CBT_CreateEffect* Effect21 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_8", L"Self_MidPos", 0.43, 1, 0, 0);
	CBT_CreateEffect* Effect22 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_9", L"Self_MidPos", 0.43, 1, 0, 0);

	CBT_CreateEffect* Effect23 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_0", L"Self_MidPos", 0.7, 1, 0, 0);
	CBT_CreateEffect* Effect24 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_1", L"Self_MidPos", 0.7, 1, 0, 0);
	CBT_CreateEffect* Effect25 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_2", L"Self_MidPos", 0.7, 1, 0, 0);
	CBT_CreateEffect* Effect26 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_4", L"Self_MidPos", 0.7, 1, 0, 0);
	CBT_CreateEffect* Effect27 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_5", L"Self_MidPos", 0.7, 1, 0, 0);
	CBT_CreateEffect* Effect28 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_8", L"Self_MidPos", 0.7, 1, 0, 0);
	CBT_CreateEffect* Effect29 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_9", L"Self_MidPos", 0.7, 1, 0, 0);

	CBT_StartDissolve* Dissolve0 = Node_StartDissolve("디졸브", this		, 3.7f, false, 0.45);
	CBT_StartDissolve* Dissolve1 = Node_StartDissolve("디졸브", this		, 3.7f, true, 0.7);
	CBT_StartDissolve* Dissolve2 = Node_StartDissolve("디졸브", m_pSword	, 3.7f, false, 0.45);
	CBT_StartDissolve* Dissolve3 = Node_StartDissolve("디졸브", m_pSword	, 3.7f, true, 0.7);
	CBT_StartDissolve* Dissolve4 = Node_StartDissolve("디졸브", m_pShield	, 3.7f, false, 0.45);
	CBT_StartDissolve* Dissolve5 = Node_StartDissolve("디졸브", m_pShield	, 3.7f, true, 0.7);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	//Root_Parallel->Add_Service(Effect2);
	//Root_Parallel->Add_Service(Effect3);
	//Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);
	Root_Parallel->Add_Service(Effect8);
	Root_Parallel->Add_Service(Effect9);
	Root_Parallel->Add_Service(Effect10);
	Root_Parallel->Add_Service(Effect11);
	Root_Parallel->Add_Service(Effect14);
	Root_Parallel->Add_Service(Effect15);
	Root_Parallel->Add_Service(Effect16);
	Root_Parallel->Add_Service(Effect17);
	Root_Parallel->Add_Service(Effect18);
	Root_Parallel->Add_Service(Effect19);
	Root_Parallel->Add_Service(Effect20);
	Root_Parallel->Add_Service(Effect21);
	Root_Parallel->Add_Service(Effect22);
	Root_Parallel->Add_Service(Effect23);
	Root_Parallel->Add_Service(Effect24);
	Root_Parallel->Add_Service(Effect25);
	Root_Parallel->Add_Service(Effect26);
	Root_Parallel->Add_Service(Effect27);
	Root_Parallel->Add_Service(Effect28);
	Root_Parallel->Add_Service(Effect29);
	Root_Parallel->Add_Service(Dissolve0);
	Root_Parallel->Add_Service(Dissolve1);
	Root_Parallel->Add_Service(Dissolve2);
	Root_Parallel->Add_Service(Dissolve3);
	Root_Parallel->Add_Service(Dissolve4);
	Root_Parallel->Add_Service(Dissolve5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani25);
	MainSeq->Add_Child(Show_Ani27);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(Sound2Stop);
	SubSeq->Add_Child(Sound2Play);
	SubSeq->Add_Child(Sound2Tag);
	SubSeq->Add_Child(PushColOn);
	SubSeq->Add_Child(PhyColOn);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.921 + 0.416, 1, 0.15, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Flash_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("점멸 베기");
	CBT_Play_Ani* Show_Ani24 = Node_Ani("점멸 베기", 24, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.2, 0);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.533, 0);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 3);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("점멸 이동", L"FlashPos", 0.1);
	CBT_SetValue* Sound2Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Stop", true);
	CBT_SetValue* Sound2Play = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Play", true);
	CBT_SetValue* Sound2Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_02_Tag", 0);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOff", L"PhyCol", true);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 5.f, 0.267, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 1.55, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -0.3f, 0.667, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("점멸 파티클", L"QueensKnight_Teleport_Particle", L"Self_MidPos", 0, 50, 0.15, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("점멸 왜곡", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.2, 1, 0.35, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("점멸 왜곡", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.8, 1, 0.35, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("점멸 파티클 블랙", L"QueensKnight_Teleport_Particle_Black", L"Self_MidPos", 0, 2, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("점멸 파티클 블랙", L"QueensKnight_Teleport_Particle_Black", L"Self_MidPos", 0.8, 2, 0, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"		, 0.85, 17, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"		, 0.85, 17, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_TomPos"		, 0.85, 17, 0, 0);
	CBT_CreateEffect* Effect9  = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos"		, 0.85, 15, 0, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"		, 0.85, 15, 0, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TomPos"		, 0.85, 15, 0, 0);

	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_0", L"Self_MidPos", 0.0, 1, 0, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_1", L"Self_MidPos", 0.0, 1, 0, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_2", L"Self_MidPos", 0.0, 1, 0, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_4", L"Self_MidPos", 0.0, 1, 0, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_5", L"Self_MidPos", 0.0, 1, 0, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_8", L"Self_MidPos", 0.0, 1, 0, 0);
	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_9", L"Self_MidPos", 0.0, 1, 0, 0);

	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_0", L"Self_MidPos", 0.8, 1, 0, 0);
	CBT_CreateEffect* Effect21 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_1", L"Self_MidPos", 0.8, 1, 0, 0);
	CBT_CreateEffect* Effect22 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_2", L"Self_MidPos", 0.8, 1, 0, 0);
	CBT_CreateEffect* Effect23 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_4", L"Self_MidPos", 0.8, 1, 0, 0);
	CBT_CreateEffect* Effect24 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_5", L"Self_MidPos", 0.8, 1, 0, 0);
	CBT_CreateEffect* Effect25 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_8", L"Self_MidPos", 0.8, 1, 0, 0);
	CBT_CreateEffect* Effect26 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_9", L"Self_MidPos", 0.8, 1, 0, 0);

	CBT_StartDissolve* Dissolve0 = Node_StartDissolve("디졸브", this		, 3.7f, false, 0.2);
	CBT_StartDissolve* Dissolve1 = Node_StartDissolve("디졸브", this		, 3.7f, true, 0.8);
	CBT_StartDissolve* Dissolve2 = Node_StartDissolve("디졸브", m_pSword	, 3.7f, false, 0.2);
	CBT_StartDissolve* Dissolve3 = Node_StartDissolve("디졸브", m_pSword	, 3.7f, true, 0.8);
	CBT_StartDissolve* Dissolve4 = Node_StartDissolve("디졸브", m_pShield	, 3.7f, false, 0.2);
	CBT_StartDissolve* Dissolve5 = Node_StartDissolve("디졸브", m_pShield	, 3.7f, true, 0.8);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);
	Root_Parallel->Add_Service(Effect8);
	Root_Parallel->Add_Service(Effect9);
	Root_Parallel->Add_Service(Effect10);
	Root_Parallel->Add_Service(Effect11);
	Root_Parallel->Add_Service(Effect12);
	Root_Parallel->Add_Service(Effect13);
	Root_Parallel->Add_Service(Effect14);
	Root_Parallel->Add_Service(Effect15);
	Root_Parallel->Add_Service(Effect16);
	Root_Parallel->Add_Service(Effect17);
	Root_Parallel->Add_Service(Effect18);
	Root_Parallel->Add_Service(Effect19);
	Root_Parallel->Add_Service(Effect20);
	Root_Parallel->Add_Service(Effect21);
	Root_Parallel->Add_Service(Effect22);
	Root_Parallel->Add_Service(Effect23);
	Root_Parallel->Add_Service(Effect24);
	Root_Parallel->Add_Service(Effect25);
	Root_Parallel->Add_Service(Effect26);
	Root_Parallel->Add_Service(Dissolve0);
	Root_Parallel->Add_Service(Dissolve1);
	Root_Parallel->Add_Service(Dissolve2);
	Root_Parallel->Add_Service(Dissolve3);
	Root_Parallel->Add_Service(Dissolve4);
	Root_Parallel->Add_Service(Dissolve5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani24);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(Sound2Stop);
	SubSeq->Add_Child(Sound2Play);
	SubSeq->Add_Child(Sound2Tag);
	SubSeq->Add_Child(TrailOn);	
	SubSeq->Add_Child(PushColOn);
	SubSeq->Add_Child(PhyColOn);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(TrailOff);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.283, 1, 0.133, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Flash_Middle_Ground()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("중앙으로 점멸 후 내려찍기");
	CBT_Play_Ani* Show_Ani25 = Node_Ani("중앙으로 점프 시작", 25, 0.7f);
	CBT_Play_Ani* Show_Ani26 = Node_Ani("내려 찍기", 26, 0.9f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.821, 0);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 3);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("점멸 이동", L"Field_MidPos", 0.1);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.8, 0);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("카메라 흔들기", L"CamShake4", true);
	CBT_SetValue* Sound2Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Stop", true);
	CBT_SetValue* Sound2Play = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Play", true);
	CBT_SetValue* Sound2Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_02_Tag", 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOn", L"PhyCol", true);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("점멸 파티클", L"QueensKnight_Teleport_Particle", L"Self_MidPos", 0, 70, 0.0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("점멸 왜곡", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.2, 1, 0.35, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("내려찍기 스모크", L"QueensKnight_JumpDown_Smoke", L"Self_Pos", 1.6, 1, 0.35, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("내려찍기 장판", L"QueensKnight_Ultimate_Floor", L"Self_Pos", 1.6, 6, 0.35, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("내려찍기 콘1", L"QueensKnight_Ultimate_Cone_0", L"Self_Pos", 1.58, 1, 0.35, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("내려찍기 콘2", L"QueensKnight_Ultimate_Cone_1", L"Self_Pos", 1.65, 1, 0.35, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("내려찍기 콘3", L"QueensKnight_Ultimate_Cone_2", L"Self_Pos", 1.7, 1, 0.35, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("내려찍기 콘4", L"QueensKnight_Ultimate_Cone_3", L"Self_Pos", 1.75, 1, 0.35, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("내려찍기 콘5 - 바깥", L"QueensKnight_Ultimate_Cone_4", L"Self_Pos", 1.6, 1, 0.35, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("빨간 기운", L"QueensKnight_Ultimate_Smoke", L"Self_Pos", 1.6, 10, 0.35, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_0", L"Self_MidPos", 0.2, 1, 0, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_1", L"Self_MidPos", 0.2, 1, 0, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_2", L"Self_MidPos", 0.2, 1, 0, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_4", L"Self_MidPos", 0.2, 1, 0, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_5", L"Self_MidPos", 0.2, 1, 0, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_8", L"Self_MidPos", 0.2, 1, 0, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_9", L"Self_MidPos", 0.2, 1, 0, 0);
	
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_0", L"Self_MidPos", 1.27, 1, 0, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_1", L"Self_MidPos", 1.27, 1, 0, 0);
	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_2", L"Self_MidPos", 1.27, 1, 0, 0);
	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_4", L"Self_MidPos", 1.27, 1, 0, 0);
	CBT_CreateEffect* Effect21 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_5", L"Self_MidPos", 1.27, 1, 0, 0);
	CBT_CreateEffect* Effect22 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_8", L"Self_MidPos", 1.27, 1, 0, 0);
	CBT_CreateEffect* Effect23 = Node_CreateEffect_Finite("검은 잉크 연기", L"QueensKnight_Teleport_Ink_9", L"Self_MidPos", 1.27, 1, 0, 0);

	CBT_StartDissolve* Dissolve0 = Node_StartDissolve("디졸브", this, 3.7f, false		, 0.2);
	CBT_StartDissolve* Dissolve2 = Node_StartDissolve("디졸브", m_pSword, 3.7f, false	, 0.2);
	CBT_StartDissolve* Dissolve4 = Node_StartDissolve("디졸브", m_pShield, 3.7f, false	, 0.2);
	CBT_StartDissolve* Dissolve1 = Node_StartDissolve("디졸브", this, 3.7f, true		, 1.25);
	CBT_StartDissolve* Dissolve3 = Node_StartDissolve("디졸브", m_pSword, 3.7f, true	, 1.25);
	CBT_StartDissolve* Dissolve5 = Node_StartDissolve("디졸브", m_pShield, 3.7f, true	, 1.25);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);
	Root_Parallel->Add_Service(Effect8);
	Root_Parallel->Add_Service(Effect9);
	Root_Parallel->Add_Service(Effect10);
	Root_Parallel->Add_Service(Effect11);
	Root_Parallel->Add_Service(Effect12);
	Root_Parallel->Add_Service(Effect13);
	Root_Parallel->Add_Service(Effect14);
	Root_Parallel->Add_Service(Effect15);
	Root_Parallel->Add_Service(Effect16);
	Root_Parallel->Add_Service(Effect17);
	Root_Parallel->Add_Service(Effect18);
	Root_Parallel->Add_Service(Effect19);
	Root_Parallel->Add_Service(Effect20);
	Root_Parallel->Add_Service(Effect21);
	Root_Parallel->Add_Service(Effect22);
	Root_Parallel->Add_Service(Effect23);

	Root_Parallel->Add_Service(Dissolve0);
	Root_Parallel->Add_Service(Dissolve1);
	Root_Parallel->Add_Service(Dissolve2);
	Root_Parallel->Add_Service(Dissolve3);
	Root_Parallel->Add_Service(Dissolve4);
	Root_Parallel->Add_Service(Dissolve5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani25);
	MainSeq->Add_Child(Show_Ani26);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(Sound2Stop);
	SubSeq->Add_Child(Sound2Play);
	SubSeq->Add_Child(Sound2Tag);
	SubSeq->Add_Child(PushColOn);
	SubSeq->Add_Child(PhyColOn);

	
	CBT_CreateBullet* Col0 = Node_CreateBullet("바닥에서 찌릿 어둠총알", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir0", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col1 = Node_CreateBullet("바닥에서 찌릿 어둠총알", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir1", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col2 = Node_CreateBullet("바닥에서 찌릿 어둠총알", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir2", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col3 = Node_CreateBullet("바닥에서 찌릿 어둠총알", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir3", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col4 = Node_CreateBullet("바닥에서 찌릿 어둠총알", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir4", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col5 = Node_CreateBullet("바닥에서 찌릿 어둠총알", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir5", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col6 = Node_CreateBullet("바닥에서 찌릿 어둠총알", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir6", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col7 = Node_CreateBullet("바닥에서 찌릿 어둠총알", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir7", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	
	CBT_CreateBullet* ColMid = Node_CreateBullet("몬스터 주위에 결계", L"Monster_DarkMidCol", L"Self_Pos", L"", 0, 0.5f, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);

	Root_Parallel->Add_Service(Col0);
	Root_Parallel->Add_Service(Col1);
	Root_Parallel->Add_Service(Col2);
	Root_Parallel->Add_Service(Col3);
	Root_Parallel->Add_Service(Col4);
	Root_Parallel->Add_Service(Col5);
	Root_Parallel->Add_Service(Col6);
	Root_Parallel->Add_Service(Col7);

	Root_Parallel->Add_Service(ColMid);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Smart_ThreeCombo_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("2연속 베기");
	CBT_Play_Ani* Show_Ani37 = Node_Ani("일반 가로베기1", 37, 0.5f);
	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 5);
	CBT_Play_Ani* Show_Ani35 = Node_Ani("일반 세로베기", 35, 0.4f);
	CBT_DistCheck* Dist1 = Node_DistCheck("거리 체크", L"Player_Pos", 3);
	CBT_Play_Ani* Show_Ani32 = Node_Ani("방패치기", 32, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.166, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.567, 0);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_SetValue* TrailOn0 = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 7.f, 0.333, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.45, 0);
	CBT_SetValue* TrailOff0 = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);
	//50번애니 * 0.5 = 1.616
	CBT_RotationDir* Rotation1 = Node_RotationDir("플레이어 바라보기1", L"Player_Pos", 0.09);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("플레이어 바라보기", L"Player_Pos", 0.01, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.15, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 6.f, 0.366, 0);
	CBT_SetValue* Sound2Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Stop", true);
	CBT_SetValue* Sound2Play = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Play", true);
	CBT_SetValue* Sound2Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_02_Tag", 0);

	CBT_SetValue* TrailOn1 = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_Wait* Wait3 = Node_Wait("대기3", 0.857, 0);
	CBT_SetValue* TrailOff1 = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);
	CBT_RotationDir* Rotation2 = Node_RotationDir("플레이어 바라보기2", L"Player_Pos", 0.09);
	CBT_ChaseDir* ChaseDir1 = Node_ChaseDir("플레이어 바라보기1", L"Player_Pos", 0.01, 0);
	//48번애니 * 0.4 = 1.473
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("이동3", L"Monster_Speed", L"Monster_Dir", 6.f, 0.45, 0);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("카메라 흔들기", L"CamShake2", true);
	CBT_SetValue* Sound1Stop1 = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play1 = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag1 = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 1);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos", 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos", 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos", 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos", 2.1, 15, 0.1, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos", 2.1, 15, 0.1, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("붉은 번개", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos", 2.1, 15, 0.1, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos", 0.6, 12, 0.1, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos", 0.6, 12, 0.1, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos", 0.6, 12, 0.1, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos", 2.1, 12, 0.1, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos", 2.1, 12, 0.1, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("검붉은 번개", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos", 2.1, 12, 0.1, 0);

	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("내려찍기 파티클", L"QueensKnight_SwordCrash_Particle", L"Sword_TopPos", 2.36, 5, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("내려찍기 파티클", L"QueensKnight_SwordCrash_Particle", L"Sword_MidPos", 2.36, 5, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("방패치기 파티클", L"QueensKnight_ShieldAttack_Particle", L"ShieldPos", 3.45, 5, 0.01, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("방패치기 왜곡", L"QueensKnight_ShieldAttack_Distortion", L"ShieldPos", 3.5, 10, 0.1, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("방패치기 붉은 빛", L"QueensKnight_ShieldAttack_RedLight", L"ShieldPos", 3.45, 1, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);
	Root_Parallel->Add_Service(Effect8);
	Root_Parallel->Add_Service(Effect9);
	Root_Parallel->Add_Service(Effect10);
	Root_Parallel->Add_Service(Effect11);
	Root_Parallel->Add_Service(Effect12);
	Root_Parallel->Add_Service(Effect13);
	Root_Parallel->Add_Service(Effect14);
	Root_Parallel->Add_Service(Effect15);
	Root_Parallel->Add_Service(Effect16);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani37);
	MainSeq->Add_Child(Dist0);
	Dist0->Set_Child(Show_Ani35);
	MainSeq->Add_Child(Dist1);
	Dist1->Set_Child(Show_Ani32);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(TrailOn0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(TrailOff0);
	SubSeq->Add_Child(Rotation1);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(Sound2Stop);
	SubSeq->Add_Child(Sound2Play);
	SubSeq->Add_Child(Sound2Tag);
	SubSeq->Add_Child(TrailOn1);
	SubSeq->Add_Child(Wait3);
	SubSeq->Add_Child(TrailOff1);
	SubSeq->Add_Child(Rotation2);
	SubSeq->Add_Child(ChaseDir1);
	SubSeq->Add_Child(Move3);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(Sound1Stop1);
	SubSeq->Add_Child(Sound1Play1);
	SubSeq->Add_Child(Sound1Tag1);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.866, 1, 0.317, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.616 + 0.533, 1, 0.15, 0);
	CBT_UpdateParam* pHitCol2 = Node_UpdateParam("무기 히트 On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.616 + 1.473 + 0.333, 1, 0.167, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);
	Root_Parallel->Add_Service(pHitCol2);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Create_LeakField_Or_Not()
{
	CBT_Selector* Root_Sel = Node_Selector("리크필드 시전");

	CBT_CompareValue* Check_Ice_Barrier = Node_BOOL_A_Equal_Value("리크필드 사용중임?", L"LeakField_On", true);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본1", 0, 0.f);

	Root_Sel->Add_Child(Check_Ice_Barrier);
	Check_Ice_Barrier->Set_Child(Show_Ani0);

	Root_Sel->Add_Child(LeakField());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::Start_Game()
{
	CBT_Selector* Root_Sel = Node_Selector("게임 시작");

	CBT_CompareValue* Check_Upper_HPRatio40 = Node_FLOAT_A_Smaller_Than_Value("체력 40퍼 미만", L"HPRatio", 40.f);
	CBT_CompareValue* Check_Upper_HPRatio70 = Node_FLOAT_A_Smaller_Than_Value("체력 70퍼 미만", L"HPRatio", 70.f);

	Root_Sel->Add_Child(Check_Upper_HPRatio40);
	Check_Upper_HPRatio40->Set_Child(HP_Final());
	Root_Sel->Add_Child(Check_Upper_HPRatio70);
	Check_Upper_HPRatio70->Set_Child(More_Than_HP_40());
	Root_Sel->Add_Child(More_Than_HP_70());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::More_Than_HP_70()
{
	CBT_Selector* Root_Sel = Node_Selector("근거리 원거리 구분 공격");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_More_Than_HP70());

	Root_Sel->Add_Child(FarAttack_More_Than_HP70());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::NearAttack_Dist5_More_Than_HP70()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	Root_Sel->Add_Child(Normal_HorizontalCut1());
	Root_Sel->Add_Child(Normal_VerticalCut1());
	Root_Sel->Add_Child(TwoCombo_Cut());
	Root_Sel->Add_Child(ThreeCombo_Cut());
	Root_Sel->Add_Child(BackStep_Cut());
	Root_Sel->Add_Child(Sting());
	Root_Sel->Add_Child(Wing_Attack());
	Root_Sel->Add_Child(Shield_Attack());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::FarAttack_More_Than_HP70()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Rush());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::More_Than_HP_40()
{
	CBT_Selector* Root_Sel = Node_Selector("근거리 원거리 구분 공격");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_More_Than_HP40());

	Root_Sel->Add_Child(FarAttack_More_Than_HP40());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::NearAttack_Dist5_More_Than_HP40()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	//Root_Sel->Add_Child(Normal_HorizontalCut1());
	//Root_Sel->Add_Child(Normal_VerticalCut1());
	//Root_Sel->Add_Child(TwoCombo_Cut());
	//Root_Sel->Add_Child(ThreeCombo_Cut());
	Root_Sel->Add_Child(BackStep_Cut());
	Root_Sel->Add_Child(Sting());
	Root_Sel->Add_Child(Wing_Attack());
	Root_Sel->Add_Child(Flash());
	Root_Sel->Add_Child(Smart_ThreeCombo_Cut());
	Root_Sel->Add_Child(Shield_Attack());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::FarAttack_More_Than_HP40()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");
	
	Root_Sel->Add_Child(Rush());
	Root_Sel->Add_Child(Flash_Rush());
	Root_Sel->Add_Child(Flash_Jump_Attack());
	Root_Sel->Add_Child(Flash_Cut());
	Root_Sel->Add_Child(Flash());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::HP_Final()
{
	CBT_Selector* Root_Sel = Node_Selector("근거리 원거리 구분 공격");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_Final());

	Root_Sel->Add_Child(FarAttack_Fianl());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::NearAttack_Dist5_Final()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	//Root_Sel->Add_Child(Normal_HorizontalCut1());
	//Root_Sel->Add_Child(Normal_VerticalCut1());
	//Root_Sel->Add_Child(TwoCombo_Cut());
	//Root_Sel->Add_Child(ThreeCombo_Cut());
	Root_Sel->Add_Child(Smart_ThreeCombo_Cut());
	Root_Sel->Add_Child(BackStep_Cut());
	//Root_Sel->Add_Child(Sting());
	Root_Sel->Add_Child(Flash_Wing_Attack());
	Root_Sel->Add_Child(Flash());
	Root_Sel->Add_Child(Flash());
	Root_Sel->Add_Child(Shield_Attack());
	Root_Sel->Add_Child(Create_LeakField_Or_Not());
	
	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::FarAttack_Fianl()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Flash_Rush());
	Root_Sel->Add_Child(Flash_Jump_Attack());
	Root_Sel->Add_Child(Flash_Cut());
	Root_Sel->Add_Child(Flash());
	Root_Sel->Add_Child(Flash_Middle_Ground());
	Root_Sel->Add_Child(Create_LeakField_Or_Not());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::Start_Show()
{
	CBT_Selector* Root_Sel = Node_Selector("시연회");

	CBT_CompareValue* Check_ShowValueN = Node_BOOL_A_Equal_Value("시연회 근거리 변수 체크", L"Show_Near", true);

	Root_Sel->Add_Child(Check_ShowValueN);
	Check_ShowValueN->Set_Child(Show_ChaseAndNearAttack());

	Root_Sel->Add_Child(Show_FarAttack());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::Show_ChaseAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("추적 후 순서대로 공격");
	CBT_Play_Ani* Show_Ani4 = Node_Ani("추적모션", 4, 0.0f);
	CBT_MoveDirectly* Chase = Node_MoveDirectly_Chase("추적", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 5.f, 3.f);
	CBT_RotationDir* Rotation0 = Node_RotationDir("플레이어 바라보기", L"Player_Pos", 0.2);

	Root_Seq->Add_Child(Show_Ani4);
	Root_Seq->Add_Child(Chase);
	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(Show_NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CQueensKnight::Show_NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("순서대로 근거리 공격");

	CBT_Cooldown* Cool0 = Node_Cooldown("쿨0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("쿨1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("쿨2", 300);
	CBT_Cooldown* Cool3 = Node_Cooldown("쿨3", 300);
	CBT_Cooldown* Cool4 = Node_Cooldown("쿨4", 300);
	CBT_Cooldown* Cool5 = Node_Cooldown("쿨5", 300);
	CBT_Cooldown* Cool6 = Node_Cooldown("쿨6", 300);

	CBT_SetValue* Show_OffNearAttack = Node_BOOL_SetValue("시연회 OFF", L"Show_Near", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Normal_HorizontalCut1());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Normal_VerticalCut1());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(TwoCombo_Cut());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(ThreeCombo_Cut());
	Root_Sel->Add_Child(Cool4);
	Cool4->Set_Child(BackStep_Cut());
	Root_Sel->Add_Child(Cool5);
	Cool5->Set_Child(Sting());
	Root_Sel->Add_Child(Cool6);
	Cool6->Set_Child(Wing_Attack());

	Root_Sel->Add_Child(Show_OffNearAttack);

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::Show_FarAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("순서대로 원거리 공격");

	CBT_Cooldown* Cool0 = Node_Cooldown("쿨0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("쿨1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("쿨2", 300);
	CBT_Cooldown* Cool3 = Node_Cooldown("쿨3", 300);
	CBT_Cooldown* Cool4 = Node_Cooldown("쿨4", 300);
	CBT_Cooldown* Cool5 = Node_Cooldown("쿨5", 300);
	CBT_Cooldown* Cool6 = Node_Cooldown("쿨6", 300);

	CBT_Play_Ani* Show_Ani3 = Node_Ani("기본", Ani_Appearance_End, 0.95f);

	CBT_SetValue* Show_ValueOff = Node_BOOL_SetValue("시연회 OFF", L"Show", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Rush());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Flash_Wing_Attack());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Flash_Rush());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(Flash_Jump_Attack());
	Root_Sel->Add_Child(Cool4);
	Cool4->Set_Child(Flash_Cut());
	Root_Sel->Add_Child(Cool5);
	Cool5->Set_Child(Flash_Middle_Ground());
	Root_Sel->Add_Child(Cool6);
	Cool6->Set_Child(Show_Ani3);
	
	Root_Sel->Add_Child(Show_ValueOff);

	return Root_Sel;
}

void CQueensKnight::Down()
{
	m_dDownTime += DELTA_60;

	// down 시작함.
	if (true == m_bDown_StartAni)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			// 다운 끝나고 루프 시작
			m_pMeshCom->SetUp_Animation(Ani_Down_Loop);
			m_bDown_StartAni = false;
			m_bDown_LoopAni = true;
		}
	}

	// down 끝내기 시작,  누적 데미지로도 끝낼 수 있음
	if (m_dDownTime > 4.f)
	{
		m_bDown_LoopAni = false;

		m_pMeshCom->SetUp_Animation(Ani_Down_End);

		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_tObjParam.bDown = false;

			m_bDown_Start = false;
			m_bAIController = true;

			++m_iDownCount;

			// Down 안에서 쓰는 bool 리셋
			m_bDown_StartAni = true;
			m_dDownTime = 0;

			m_pMeshCom->SetUp_Animation(Ani_Idle);
		}
	}


	// 충돌처리, bCanHit를 무기가 false시켜줄것임.
	if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		m_dHitTime = 0;	// 피격가능 타임 초기화

		if (true == m_bDown_LoopAni)
		{
			m_pMeshCom->Reset_OldIndx();	//루프 애니 초기화
			m_pMeshCom->SetUp_Animation(Ani_Down_Loop);
		}

		m_pAIControllerCom->Reset_BT();
	}
	else
	{
		m_dHitTime += DELTA_60;

		if (m_dHitTime > 0.5)
		{
			m_tObjParam.bIsHit = false;		// 재충돌 가능
		}
	}

}

HRESULT CQueensKnight::Update_Bone_Of_BlackBoard()
{
	m_vWing = *(_v3*)(&(m_pWingFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Wing", m_vWing);

	m_vLeftHand = *(_v3*)(&(m_pLeftHandFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftHand", m_vLeftHand);
	
	return S_OK;
}

HRESULT CQueensKnight::Update_Value_Of_BB()
{
	CGameObject* pPlayer = CMonster::Get_pTargetObject();

	if (nullptr == pPlayer)
		return E_FAIL;

	CTransform* pPlayer_Trans = TARGET_TO_TRANS(pPlayer);


	// 1. 플레이어 좌표 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Player_Pos", pPlayer_Trans->Get_Pos());
	// 2. 체력 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HP", m_tObjParam.fHp_Cur);
	// 3. 체력 비율 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HPRatio", _float(m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max) * 100);


	////////
	// 1. 트레일 업데이트
	if (true == m_pAIControllerCom->Get_BoolValue(L"TrailOn"))
	{
		m_pSword->Set_Enable_Trail(true);
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"TrailOn", false);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"TrailOff"))
	{
		m_pSword->Set_Enable_Trail(false);
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"TrailOff", false);
	}

	// 사운드
	// SFX_01
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_01_Stop"))	// 멈춤
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_01_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::QueenKnight_SFX_01);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_01_Play"))	// 재생
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_01_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_01_Tag")]), CSoundManager::CHANNELID::QueenKnight_SFX_01, CSoundManager::SOUND::Effect_Sound);
	}

	// SFX_02
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_02_Stop"))	// 멈춤
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_02_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::QueenKnight_SFX_02);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_02_Play"))	// 재생
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_02_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_02_Tag")]), CSoundManager::CHANNELID::QueenKnight_SFX_02, CSoundManager::SOUND::Effect_Sound);
	}

	// Voice
	if (true == m_pAIControllerCom->Get_BoolValue(L"Voice_Stop"))	// 멈춤
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Voice_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::QueenKnight_Voice);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"Voice_Play"))	// 재생
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Voice_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"Voice_Tag")]), CSoundManager::CHANNELID::QueenKnight_Voice, CSoundManager::SOUND::Effect_Sound);
	}


	// 카메라 흔들기
	if (true == m_pAIControllerCom->Get_BoolValue(L"CamShake1"))
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"CamShake1", false);

		SHAKE_CAM_lv1;
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"CamShake2"))
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"CamShake2", false);

		SHAKE_CAM_lv2;
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"CamShake4"))
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"CamShake4", false);

		SHAKE_CAM_lv4;
	}




	////////

	// 몬스터 월드메트릭스
	_mat matSelf = m_pTransformCom->Get_WorldMat();
	_v3 vSelfPos = *(_v3*)&matSelf.m[3];
	_v3 vSelfLook = *D3DXVec3Normalize(&_v3(), (_v3*)&matSelf.m[2]);
	_v3 vSelfRight = *D3DXVec3Normalize(&_v3(), (_v3*)&matSelf.m[0]);

	// 1. 점멸을 위한 플레이어 4방 위치.
	_mat matPlayer = TARGET_TO_TRANS(pPlayer)->Get_WorldMat();
	_v3 vPlayerPos = *(_v3*)&matPlayer.m[3];
	_v3 vPlayerLook = *D3DXVec3Normalize(&_v3(), (_v3*)&matPlayer.m[2]);
	_v3 vPlayerRight = *D3DXVec3Normalize(&_v3(), (_v3*)&matPlayer.m[0]);

	_float fLength = 3.6f;

	switch (CALC::Random_Num(0, 3))
	{
	case 0:
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlashPos", vPlayerPos + fLength * vPlayerLook);
		break;
	case 1:
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlashPos", vPlayerPos - fLength * vPlayerLook);
		break;
	case 2:
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlashPos", vPlayerPos + fLength * vPlayerRight);
		break;
	case 3:
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlashPos", vPlayerPos - fLength * vPlayerRight);
		break;
	default:
		break;
	}

	// 2. 본인 좌표
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Pos", vSelfPos);

	// 2-1. 본인 좌표 + Y 가운데
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_MidPos", vSelfPos + _v3(0, 1.3f, 0.f));

	// 3. 칼 손잡이쪽
	CTransform* pSwordTrans = static_cast<CTransform*>(m_pSword->Get_Component(L"Com_Transform"));
	_mat matSword = pSwordTrans->Get_WorldMat();
	_v3 vSwordPos = _v3(matSword.m[3][0], matSword.m[3][1], matSword.m[3][2]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Sword_BottomPos", vSwordPos);

	// 3-1. 칼 끝 쪽
	vSwordPos = _v3(matSword.m[3][0], matSword.m[3][1], matSword.m[3][2]) + _v3(matSword.m[2][0], matSword.m[2][1], matSword.m[2][2]) * 3.75f;
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Sword_TopPos", vSwordPos);

	// 3-2. 칼 가운데
	vSwordPos = _v3(matSword.m[3][0], matSword.m[3][1], matSword.m[3][2]) + _v3(matSword.m[2][0], matSword.m[2][1], matSword.m[2][2]) * 2.35f;
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Sword_MidPos", vSwordPos);

	// 4. 방패
	CTransform* pShieldTrans = static_cast<CTransform*>(m_pShield->Get_Component(L"Com_Transform"));
	_mat matShield = pShieldTrans->Get_WorldMat();
	_v3 vShieldPos = _v3(matShield.m[3][0], matShield.m[3][1], matShield.m[3][2]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ShieldPos", vShieldPos);

	// 5. 중앙에서 내려찍기를 위한 필드 중앙 좌표
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Field_MidPos", _v3(0.f, 0.f, 0.f));

	// 5.1 내려찍기 어둠총알 방향
	_v3 vDir0 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(45)));
	_v3 vDir1 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(90)));
	_v3 vDir2 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(135)));
	_v3 vDir3 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(180)));
	_v3 vDir4 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(225)));
	_v3 vDir5 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(270)));
	_v3 vDir6 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(315)));

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir0", vSelfLook);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir1", vDir0);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir2", vDir1);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir3", vDir2);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir4", vDir3);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir5", vDir4);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir6", vDir5);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir7", vDir6);

	/////////////////
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"V3_Null", V3_NULL);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Dir", m_pTransformCom->Get_Axis(AXIS_Z));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Angle", D3DXToDegree(m_pTransformCom->Get_Angle()));
	/////////////////

	return S_OK;
}

HRESULT CQueensKnight::Update_NF()
{
	// 플레이어 발견 못 했을 때
	if (false == m_bFindPlayer)
	{
		// 플레이어 좌표 구함.
		_v3 vPlayer_Pos = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_Pos();

		// 동료 좌표 구함
		_v3 vColleague_Pos = _v3(10000.f, 10000.f, 10000.f);

		CGameObject* pColleague = g_pManagement->Get_GameObjectBack(L"Layer_Colleague", SCENE_STAGE);

		// 동료가 있으면 좌표 갱신
		if (nullptr != pColleague)
			vColleague_Pos = TARGET_TO_TRANS(pColleague)->Get_Pos();

		// 플레이어와 몬스터의 거리
		_v3 vP_LengthTemp = vPlayer_Pos - m_pTransformCom->Get_Pos();
		vP_LengthTemp.y = 0.f;
		_float fP_Length = D3DXVec3Length(&vP_LengthTemp);

		// 동료와 몬스터의 거리
		_v3 vC_LengthTemp = vColleague_Pos - m_pTransformCom->Get_Pos();
		vC_LengthTemp.y = 0.f;
		_float fC_Length = D3DXVec3Length(&vC_LengthTemp);



		// 플레이어가 최소거리안에 있는가?
		if (fP_Length < m_fMinLength || fC_Length < m_fMinLength)
		{
			// 플레이어 발견
			m_bFindPlayer = true;
		}
		// 플레이어가 최대거리 안에 있는가?
		else if (fP_Length < m_fMaxLength || fC_Length < m_fMaxLength)
		{
			// 플레이어가 시야각 안에 있는가?
			if (Is_InFov(m_fFov, m_pTransformCom, vPlayer_Pos) || Is_InFov(m_fFov, m_pTransformCom, vColleague_Pos))
			{
				// 플레이어 발견
				m_bFindPlayer = true;
			}
			else
			{
				m_pMeshCom->SetUp_Animation(Ani_Appearance);
			}
		}
		// 플레이어가 최대거리 밖에 있는가?
		else
			m_pMeshCom->SetUp_Animation(Ani_Appearance);

	}
	// 플레이어 발견
	else
	{
		m_pMeshCom->SetUp_Animation(Ani_Appearance_End);

		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_pMeshCom->SetUp_Animation(Ani_Idle);
			m_bFight = true;

			// 가까운 녀석 어그로 끌림.
			Set_Target_Auto();
		}
	}

	return S_OK;
}

HRESULT CQueensKnight::Update_Collider()
{
	_ulong matrixIdx = 0;

	for (auto& iter : m_vecPhysicCol)
	{
		_mat tmpMat;
		tmpMat = *m_matBones[matrixIdx] * m_pTransformCom->Get_WorldMat();

		_v3 ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);

		iter->Update(ColPos);

		++matrixIdx;
	}

	/////// 공격용 콜라이더
	m_vecAttackCol[0]->Update(m_vWing);

	m_pColliderCom->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pColliderCom->Get_Radius().y, 0.f));
	return S_OK;
}

void CQueensKnight::Check_PhyCollider()
{
	// 다운상태일 때는 이함수만 탄다.
	if (true == m_bDown_Start)
	{
		// 이 안에서 충돌처리도 할 것임.
		Down();
	}


	// 일반 충돌처리, bCanHit를 무기가 false시켜줄것임.
	else if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		//m_pMeshCom->Reset_OldIndx();	//애니 인덱스 초기화

		//m_bAIController = false;

		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		m_dHitTime = 0;	// 피격가능 타임 초기화

		//m_bFight = true;		// 싸움 시작

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			// 체력 비율 70 이하되면 스턴
			if (0 == m_iDownCount)
			{
				if (0.7 >= (m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max))
				{
					m_bDown_Start = true;

					m_tObjParam.bDown = true;

					m_pMeshCom->SetUp_Animation(Ani_Down_Start);
					m_bDown_StartAni = true;	//down 함수 내부에서 쓸 것임.
					m_pAIControllerCom->Reset_BT();
					m_bAIController = false;

					m_pAIControllerCom->Set_Value_Of_BlackBoard(L"PushCol", true);
					m_pAIControllerCom->Set_Value_Of_BlackBoard(L"TrailOff", true);
				}
			}

			if (1 == m_iDownCount)
			{
				if (0.4 >= (m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max))
				{
					m_bDown_Start = true;

					m_tObjParam.bDown = true;

					m_pMeshCom->SetUp_Animation(Ani_Down_Start);
					m_bDown_StartAni = true;	//down 함수 내부에서 쓸 것임.
					m_pAIControllerCom->Reset_BT();
					m_bAIController = false;

					m_pAIControllerCom->Set_Value_Of_BlackBoard(L"PushCol", true);
					m_pAIControllerCom->Set_Value_Of_BlackBoard(L"TrailOff", true);
				}
			}
		
			//cout << "QueensKnight - Check_PhyCollider : " << m_tObjParam.fHp_Cur << endl;

		}
		else
		{
			m_pMeshCom->SetUp_Animation(Ani_Death);	// 죽음처리 시작
			Start_Dissolve(				0.4f, false, true	, 2.7f);
			m_pSword->Start_Dissolve(	0.4f, false, false	, 2.7f);
			m_pShield->Start_Dissolve(	0.4f, false, false	, 2.7f);
			for (_int i = 0; i < 20; i++)
				CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(100.f, m_pTransformCom->Get_Pos(), 2.3f + (i * 0.1f)));
			CParticleMgr::Get_Instance()->Create_BossDeadParticle_Effect(m_pTransformCom->Get_Pos() + _v3(0.f, 1.3f, 0.f), 2.5f, 0.02f);
			g_pManagement->Create_ParticleEffect_Delay(L"SpawnParticle_ForBoss"	, 2.5f	,  3.f, m_pTransformCom->Get_Pos() + _v3(0.f, 0.5f, 0.f));
		}
	}
	else
	{
		m_dHitTime += DELTA_60;

		if (m_dHitTime > 0.5)
		{
			m_tObjParam.bIsHit = false;		// 재충돌 가능
		}
	}
}

void CQueensKnight::Push_Collider()
{
	list<CGameObject*> tmpList[4];

	tmpList[0] = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);
	tmpList[1] = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
	tmpList[2] = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);
	tmpList[3] = g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE);

	for (auto& ListObj : tmpList)
	{
		if (ListObj.empty())
			continue;

		for (auto& iter : ListObj)
		{
			CCollider* pCollider = TARGET_TO_COL(iter);

			// 지금 속도값 임의로 넣었는데 구해서 넣어줘야함 - 완료
			if (m_pColliderCom->Check_Sphere(pCollider, m_pTransformCom->Get_Axis(AXIS_Z), m_pAIControllerCom->Get_FloatValue(L"Monster_Speed")))
			{
				CTransform* pTrans = TARGET_TO_TRANS(iter);
				CNavMesh*   pNav = TARGET_TO_NAV(iter);

				// 방향 구해주고
				_v3 vDir = m_pTransformCom->Get_Pos() - pTrans->Get_Pos();
				V3_NORMAL_SELF(&vDir);

				// y축 이동은 하지말자
				vDir.y = 0;

				// 네비 메쉬타게 끔 세팅
				pTrans->Set_Pos(pNav->Move_OnNaviMesh(NULL, &pTrans->Get_Pos(), &vDir, m_pColliderCom->Get_Length().x));
			}
		}
	}
}

void CQueensKnight::OnCollisionEnter()
{
	// =============================================================================================
	// 충돌
	// =============================================================================================

	if (m_bPlayerFriendly)
	{
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE));
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_MonsterProjectile", SCENE_STAGE));
	}
	else
	{
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE));
	}

	// =============================================================================================

}

void CQueensKnight::OnCollisionEvent(list<CGameObject*> plistGameObject)
{
	// 공격 불가능이면 체크 안함
	if (false == m_tObjParam.bCanAttack)
		return;

	_bool bFirst = true;
	//게임 오브젝트를 받아와서
	for (auto& iter : plistGameObject)
	{
		// 맞을 수 없다면 리턴
		if (false == iter->Get_Target_CanHit())
			continue;

		// 내가 가진 Vec 콜라이더와 비교한다.
		for (auto& vecIter : m_vecAttackCol)
		{
			bFirst = true;

			// 피직콜라이더랑 비교
			for (auto& vecCol : iter->Get_PhysicColVector())
			{
				// 물체 전체를 대표할 콜라이더.
				if (vecIter->Check_Sphere(vecCol))
				{
					// 첫번째는 경계구 콜라이더니까 다음 콜라이더와 충돌처리 한다.
					if (bFirst)
					{
						bFirst = false;
						continue;
					}

					if (false == iter->Get_Target_IsDodge())
					{
						iter->Set_Target_CanHit(false);

						if (iter->Get_Target_IsHit())
							iter->Set_HitAgain(true);

						iter->Add_Target_Hp(-m_tObjParam.fDamage);
					}

					m_tObjParam.bCanAttack = false;

					break;

				}

				else
				{
					if (bFirst)
					{
						break;
					}
				}
			}
		}
	}
}

HRESULT CQueensKnight::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_QueensKnight", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	// for.Com_AIController
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"AIController", L"Com_AIController", (CComponent**)&m_pAIControllerCom)))
		return E_FAIL;

	// for.Com_NavMesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMeshCom)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	//=================================================================================
	// for.Com_Optimaization
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Optimization", L"Com_Optimization", (CComponent**)&m_pOptimizationCom)))
		return E_FAIL;

	// for.Com_BattleAgent
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"BattleAgent", L"Com_BattleAgent", (CComponent**)&m_pBattleAgentCom)))
		return E_FAIL;
	//=================================================================================

	m_pColliderCom->Set_Radius(_v3{ 1.8f, 1.8f, 1.8f });
	m_pColliderCom->Set_Dynamic(true);
	m_pColliderCom->Set_Type(COL_SPHERE);
	m_pColliderCom->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pColliderCom->Get_Radius().y , 0.f });


	return NOERROR;
}

HRESULT CQueensKnight::SetUp_ConstantTable(CShader* pShader)
{
	if (nullptr == pShader)
		return E_FAIL;

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat		ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", pShader)))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 쉐이더 재질정보 수치 입력
	//=============================================================================================
	_float	fEmissivePower = 3.f;	// 이미시브 : 높을 수록, 자체 발광이 강해짐.
	_float	fSpecularPower = 5.f;	// 메탈니스 : 높을 수록, 정반사가 강해짐.
	_float	fRoughnessPower = 1.f;	// 러프니스 : 높을 수록, 빛 산란이 적어짐(빛이 응집됨).
	_float	fRimLightPower = 0.f;	// 림		: 높을 수록 빛이 퍼짐(림라이트의 범위가 넓어지고 , 밀집도가 낮아짐).
	_float	fMinSpecular = 0.5f;	// 최소 빛	: 높을 수록 빛이 퍼짐(림라이트의 범위가 넓어지고 , 밀집도가 낮아짐).

	if (FAILED(pShader->Set_Value("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fSpecularPower", &fSpecularPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fRoughnessPower", &fRoughnessPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fRimAlpha", &fRimLightPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fMinSpecular", &fMinSpecular, sizeof(_float))))
		return E_FAIL;
	//=============================================================================================


	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CQueensKnight::Ready_Weapon()
{
	// 오른손 무기
	m_pSword = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pSword->Change_WeaponData(WPN_QueenLance);

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pSword->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pSword->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	// Trail
	m_pSword->Set_Enable_Trail(true);
	m_pSword->Set_TrailIndex(7);
	m_pSword->Set_TrailUseMask(true, 0);

	// 왼손 방패
	m_pShield = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pShield->Change_WeaponData(WPN_QueenShield);

	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHandAttach");
	m_pShield->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pShield->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	return S_OK;
}

HRESULT CQueensKnight::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;


	// 뼈 주소 저장
	m_pWingFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine3_WingB4");
	m_pLeftHandFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHand");

	return S_OK;
}

HRESULT CQueensKnight::Ready_Collider()
{
	m_vecPhysicCol.reserve(10);

	//경계체크용
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.4f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Range]->_41, m_matBones[Bone_Range]->_42, m_matBones[Bone_Range]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//몸
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 0.95f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Body]->_41, m_matBones[Bone_Body]->_42, m_matBones[Bone_Body]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//머리
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 0.2f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Head]->_41, m_matBones[Bone_Head]->_42, m_matBones[Bone_Head]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//////////// 공격용 콜라이더
	m_vecAttackCol.reserve(5);

	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 1.f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(m_vWing);
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CQueensKnight::Ready_NF(void * pArg)
{
	INFO eTemp = *(INFO*)pArg;

	m_fFov = eTemp.fFov;
	m_fMaxLength = eTemp.fMaxLength;
	m_fMinLength = eTemp.fMinLength;

	//===================================================================
	// 림라이트 벨류 설정	
	//===================================================================
	m_pBattleAgentCom->Set_RimAlpha(0.5f);
	m_pBattleAgentCom->Set_RimValue(8.f);
	m_pBattleAgentCom->Set_OriginRimAlpha(0.5f);
	m_pBattleAgentCom->Set_OriginRimValue(8.f);
	//===================================================================

	//===================================================================
	// 트리거 소환용 설정
	//===================================================================
	// 트리거 소환되는 녀석이면
	if (eTemp.bSpawnOnTrigger)
	{
		// 스테이지 번호로 네비 불러옴
		_tchar szNavData[STR_128] = L"";

		lstrcpy(szNavData, (
			eTemp.sStageIdx == 0 ? L"Navmesh_Training.dat" :
			eTemp.sStageIdx == 1 ? L"Navmesh_Stage_01.dat" :
			eTemp.sStageIdx == 2 ? L"Navmesh_Stage_02.dat" :
			eTemp.sStageIdx == 3 ? L"Navmesh_Stage_03.dat" : L"Navmesh_Stage_04.dat"));

		m_pNavMeshCom->Set_Index(-1);
		m_pNavMeshCom->Ready_NaviMesh(m_pGraphic_Dev, szNavData);
		m_pNavMeshCom->Check_OnNavMesh(eTemp.vPos);
		m_pTransformCom->Set_Pos(eTemp.vPos);
		m_pTransformCom->Set_Angle(eTemp.vAngle);

		//m_pNavMeshCom->Set_SubsetIndex(eTemp.sSubSetIdx);
		//m_pNavMeshCom->Set_Index(eTemp.sCellIdx);
	}
	//===================================================================


	return S_OK;
}

HRESULT CQueensKnight::Ready_Sound()
{
	m_mapSound.emplace(0, L"BARK_Knight_TLanceGCS_AttackNormal01_N_GCS_000.ogg");
	m_mapSound.emplace(1, L"SE_QUEENS_KNIGHTS_ATTACK_GUARD_001.ogg");
	m_mapSound.emplace(2, L"SE_QUEENS_KNIGHTS_DRAIN_FIELD_000.ogg");
	m_mapSound.emplace(3, L"SE_QUEENS_KNIGHTS_WARP_001.ogg"); 

	
	return S_OK;
}

CQueensKnight * CQueensKnight::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CQueensKnight* pInstance = new CQueensKnight(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CQueensKnight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CQueensKnight::Clone_GameObject(void * pArg)
{
	CQueensKnight* pInstance = new CQueensKnight(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CQueensKnight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQueensKnight::Free()
{
	Safe_Release(m_pShield);
	Safe_Release(m_pSword);

	CGameObject::Free();
}


