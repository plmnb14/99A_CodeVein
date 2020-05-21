#include "stdafx.h"
#include "..\Headers\PoisonButterfly.h"
#include "..\Headers\BossHP.h"
#include "MassageUI.h"

CPoisonButterfly::CPoisonButterfly(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
	ZeroMemory(m_matBones, sizeof(_mat*) * Bone_End);
}

CPoisonButterfly::CPoisonButterfly(const CPoisonButterfly & rhs)
	: CMonster(rhs)
{
	ZeroMemory(m_matBones, sizeof(_mat*) * Bone_End);
}

HRESULT CPoisonButterfly::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CPoisonButterfly::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_NF(pArg);
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

	//Start_Sel->Add_Child(OneTurn_Poison());

	//CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.2);
	//Start_Sel->Add_Child(Rotation0);

	/////////////
	// UI 추가(지원)
	m_pBossUI = static_cast<CBossHP*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BossHP", nullptr));
	m_pBossUI->Set_UI_Pos(WINCX * 0.5f, WINCY * 0.1f);
	m_pBossUI->Set_BossName(CBossNameUI::Index_PoisonButterfly);
	if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBossUI, SCENE_STAGE, L"Layer_BossHP", nullptr)))
		return E_FAIL;

	return NOERROR;
}

_int CPoisonButterfly::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (true == m_pAIControllerCom->Get_BoolValue(L"PushCol"))
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

		if ( false == m_bFinishCamShake && m_pMeshCom->Is_Finish_Animation(0.5f))
		{
			m_bFinishCamShake = true;
			SHAKE_CAM_lv3;

			g_pSoundManager->Stop_Sound(CSoundManager::Effect_SFX_01);
			g_pSoundManager->Play_Sound(L"Boss_DeadEff.ogg", CSoundManager::Effect_SFX_01, CSoundManager::Effect_Sound);
		}

		m_fBGMFade -= _float(TimeDelta) * 0.25f;
		if (m_fBGMFade <= 0.f)
			m_fBGMFade = 0.f;
		g_pSoundManager->Set_Volume(CSoundManager::BGM_Volume, m_fBGMFade);

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

		// 보스UI 업데이트
		// 체력이 0이 되었을때 밀림현상 방지.
		if (0 >= m_tObjParam.fHp_Cur)
			m_pBossUI->Set_BossHPInfo(0, 100);
		else
			m_pBossUI->Set_BossHPInfo(m_tObjParam.fHp_Cur, m_tObjParam.fHp_Max);
	}

	if (false == m_bReadyDead)
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

_int CPoisonButterfly::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

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

	return NO_EVENT;
}

HRESULT CPoisonButterfly::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	m_pMeshCom->Play_Animation(_float(m_dTimeDelta)); // * alpha

	if (m_bInFrustum)
	{
		m_pShaderCom->Begin_Shader();

		_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

		for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
		{
			if (FAILED(SetUp_ConstantTable(m_pShaderCom)))
				return E_FAIL;

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

	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CPoisonButterfly::Render_GameObject_Instancing_SetPass(CShader * pShader)
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

	Update_Collider();
	Draw_Collider();

	return S_OK;
}

HRESULT CPoisonButterfly::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
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

CBT_Composite_Node * CPoisonButterfly::Left_Eat(_float fWeight)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("왼쪽 냠");
	CBT_Play_Ani* Show_Ani25 = Node_Ani("왼쪽 냠", 25, fWeight);//27
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적", L"Player_Pos", 1.4, 0);
	CBT_RotationDir* RotationDir0 = Node_RotationDir("방향 회전", L"Player_Pos", 0.156);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("왜곡", L"ButterFly_Distortion_Circle", L"Bone_Tail6", 1.75f, 1, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("보라 파티클", L"ButterFly_PointParticle", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("반짝 보라 모래", L"ButterFly_GlitterSand", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("옅은 독안개", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("왜곡안개", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0.f, 60, 0.7, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani25);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(RotationDir0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 1.566, 1, 0.234, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Right_Eat(_float fWeight)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("오른쪽 냠");
	CBT_Play_Ani* Show_Ani20 = Node_Ani("오른쪽 냠", 20, fWeight);//22
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적", L"Player_Pos", 1.2, 0);
	CBT_RotationDir* RotationDir0 = Node_RotationDir("방향 회전", L"Player_Pos", 0.183);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("왜곡", L"ButterFly_Distortion_Circle", L"Bone_Tail6", 1.65f, 1, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("보라 파티클", L"ButterFly_PointParticle", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("옅은 독안개", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("반짝 보라 모래", L"ButterFly_GlitterSand", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("왜곡안개", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0.f, 60, 0.7, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani20);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(RotationDir0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 1.383, 1, 0.2, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Eat_TurnEat()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("냠 뒤돌아서 덥썩");

	CBT_Sequence* MainSeq = Node_Sequence("냠 뒤돌아서 냠");
	CBT_Play_Ani* Show_Ani20 = Node_Ani("좌회전 한바퀴 덥썩", 20, 0.6f);//22
	CBT_Play_Ani* Show_Ani18 = Node_Ani("뒤돌아서 덥썩", 18, 0.95f);//20
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적0", L"Player_Pos", 1.2, 0);
	CBT_RotationDir* RotationDir0 = Node_RotationDir("방향 회전0", L"Player_Pos", 0.183);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.417, 0);
	// 22번 애니 * 0.6 = 1.8
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.6, 0);
	CBT_RotationDir* RotationDir1 = Node_RotationDir("방향 회전1", L"Player_Pos", 0.216);
	CBT_SetValue* Sound1Stop1 = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play1 = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag1 = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 10, 0.2, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("왜곡", L"ButterFly_Distortion_Circle", L"Bone_Tail6_Tongue2", 1.5, 1, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("보라 파티클", L"ButterFly_PointParticle", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("옅은 독안개", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("반짝 보라 모래", L"ButterFly_GlitterSand", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("왜곡안개", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0.f, 60, 0.7, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("왜곡", L"ButterFly_Distortion_Circle", L"Bone_Tail6_Tongue2", 3.0, 1, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani20);
	MainSeq->Add_Child(Show_Ani18);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(RotationDir0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(RotationDir1);
	SubSeq->Add_Child(Sound1Stop1);
	SubSeq->Add_Child(Sound1Play1);
	SubSeq->Add_Child(Sound1Tag1);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 1.383, 1, 0.2, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 1.8 + 0.816, 1, 0.45, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);


	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Poison_Tornado_After_Charging()
{
	CBT_Sequence* Root_Seq = Node_Sequence("기모아서 독 소용돌이");

	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("소리1 재생", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("소리1 재생", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("소리1 이름 설정", L"Voice_Tag", 10);
	CBT_Play_Ani* Show_Ani26 = Node_Ani("기모아서 독 소용돌이", 26, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 0, 0.f);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("폭발할 때 처음 스모크 01", L"ButterFly_SoftSmoke_Ready_1", L"Self_Pos", 3.2, 1, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("폭발할 때 처음 스모크 02", L"ButterFly_SoftSmoke_Ready_2", L"Self_Pos", 3.2, 1, 0.01, 0);
	//CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("기 모을 때 메쉬이펙트", L"ButterFly_Distortion_Circle", L"Self_Pos", 0.3, 1, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("기 모을 때 스모크 01", L"ButterFly_SoftSmoke", L"Self_Pos", 0.3, 40, 1.2, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("기 모을 때 스모크 02", L"ButterFly_SoftSmoke_Bottom", L"Self_Pos", 0.8, 40, 1.5, 0);
	//CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("폭발할 때 처음 빨간 스모크", L"ButterFly_Smoke_Red_Once", L"Self_Pos"	, 0, 1, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("폭발하는 빨간 스모크", L"ButterFly_Smoke_Red_Particle", L"Self_Pos", 3, 15, 0.01, 0);
	//CBT_CreateEffect* Effect6 = Node_CreateParticle_Finite("바닥에 깔리는 빨간 스모크", L"ButterFly_SoftSmoke_Floor", L"Self_Pos", 10, 0.8, 150, 0, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("바닥에 깔리는 빨간 스모크", L"ButterFly_SoftSmoke_Floor", L"Self_Pos", 0.8, 60, 1.5, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("폭발할 때 자두색 동그라미 파티클", L"ButterFly_PointParticle_Plum", L"Self_Pos", 3.2, 20, 0.7, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("폭발할 때 회오리 메쉬이펙트 01", L"ButterFly_RingLine", L"Self_Pos", 3, 1, 0.01, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("폭발할 때 회오리 메쉬이펙트 02", L"ButterFly_RingLine", L"Self_Pos", 3.45, 1, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("회오리 메쉬 왜곡", L"ButterFly_RingLine_Distortion", L"Self_Pos", 3, 1, 0.01, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("기 모을 때 스모크 덩어리", L"ButterFly_SoftSmoke_Chunk", L"Self_Pos", 0.8, 20, 1.5, 0);


	Root_Seq->Add_Service(Effect0);
	Root_Seq->Add_Service(Effect1);
	Root_Seq->Add_Service(Effect2);
	Root_Seq->Add_Service(Effect3);
	//Root_Seq->Add_Service(Effect4);
	Root_Seq->Add_Service(Effect5);
	Root_Seq->Add_Service(Effect6);
	Root_Seq->Add_Service(Effect7);
	Root_Seq->Add_Service(Effect8);
	Root_Seq->Add_Service(Effect9);
	Root_Seq->Add_Service(Effect10);
	Root_Seq->Add_Service(Effect11);

	CBT_CreateBullet* PoisonBullet0 = Node_CreateBullet("독 총알", L"Monster_PoisonTornado", L"Self_Pos", L"", 0, 2, 3.36, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Seq->Add_Service(PoisonBullet0);

	Root_Seq->Add_Child(VoiceStop);
	Root_Seq->Add_Child(VoicePlay);
	Root_Seq->Add_Child(VoiceTag);
	Root_Seq->Add_Child(Show_Ani26);
	Root_Seq->Add_Child(Show_Ani6);

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::OneTurn_Poison()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("좁은 범위 독 발사");
	CBT_Play_Ani* Show_Ani22 = Node_Ani("좁은 범위 한바퀴 독 발사", 22, 0.95f);//24
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적0", L"Player_Pos", 1.2, 0);
	CBT_RotationDir* RotationDir0 = Node_RotationDir("방향 회전0", L"Player_Pos", 0.2);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("전체적으로 보라 동그라미 파티클", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("전체적으로 쓰이는 옅은 독안개", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("반짝이는 보라색 모래", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("안개와 같이 나오는 왜곡", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 150, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani22);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(RotationDir0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 1.25, 1, 1.816, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Eat_Turn()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("냠 돌기");
	CBT_Sequence* MainSeq = Node_Sequence("냠 돌기");
	CBT_Play_Ani* Show_Ani25 = Node_Ani("냠", 25, 0.5f);//27, 0.5
	CBT_Play_Ani* Show_Ani24 = Node_Ani("돌기", 24, 0.95f);//26
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적0", L"Player_Pos", 1.4, 0);
	CBT_RotationDir* RotationDir0 = Node_RotationDir("방향 회전0", L"Player_Pos", 0.156);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_Wait* Wait0 = Node_Wait("대기", 0.319, 0);
	// 27번 애니 * 0.5 = 1.875
	CBT_ChaseDir* ChaseDir1 = Node_ChaseDir("방향 추적1", L"Player_Pos", 1, 0);
	CBT_RotationDir* RotationDir1 = Node_RotationDir("방향 회전1", L"Player_Pos", 0.166);
	CBT_SetValue* Sound1Stop1 = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play1 = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag1 = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 10, 0.367, 0);
	CBT_SetValue* Sound2Stop1 = Node_BOOL_SetValue("소리2 재생", L"SFX_02_Stop", true);
	CBT_SetValue* Sound2Play1 = Node_BOOL_SetValue("소리2 재생", L"SFX_02_Play", true);
	CBT_SetValue* Sound2Tag1 = Node_INT_SetValue("소리2 이름 설정", L"SFX_02_Tag", 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 10, 0.367, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("전체적으로 보라 동그라미 파티클", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 300, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("전체적으로 쓰이는 옅은 독안개", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 300, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("반짝이는 보라색 모래", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 300, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("안개와 같이 나오는 왜곡", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 300, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("왜곡", L"ButterFly_Distortion_Circle", L"Bone_Tail6", 1.8, 1, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("왜곡", L"ButterFly_Distortion_Circle", L"Bone_Tail6", 3.7, 1, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani25);
	MainSeq->Add_Child(Show_Ani24);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(RotationDir0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(ChaseDir1);
	SubSeq->Add_Child(RotationDir1);
	SubSeq->Add_Child(Sound1Stop1);
	SubSeq->Add_Child(Sound1Play1);
	SubSeq->Add_Child(Sound1Tag1);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Sound2Stop1);
	SubSeq->Add_Child(Sound2Play1);
	SubSeq->Add_Child(Sound2Tag1);
	SubSeq->Add_Child(Move1);


	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 1.566, 1, 0.234, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 1.875 + 1.166, 1, 0.734, 0);

	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Rush()
{
	CBT_Simple_Parallel* RushParallel = Node_Parallel_Immediate("돌진 병렬");
	CBT_Sequence* RushAniSeq = Node_Sequence("RushSequence");
	CBT_Play_Ani* Show_Ani15 = Node_Ani("돌진", 15, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* MoveSequence = Node_Sequence("돌진 이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적0", L"Player_Pos", 0.2);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("뒤로 이동", L"Monster_Speed", L"Monster_Dir", -3.f, 1.083, 0);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적1", L"Player_Pos", 0.3, 0);
	CBT_RotationDir* Rotation1 = Node_RotationDir("방향 추적1", L"Player_Pos", 0.267);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_SetValue* Sound1Stop1 = Node_BOOL_SetValue("소리1 재생", L"Voice_Stop", true);
	CBT_SetValue* Sound1Play1 = Node_BOOL_SetValue("소리1 재생", L"Voice_Play", true);
	CBT_SetValue* Sound1Tag1 = Node_INT_SetValue("소리1 이름 설정", L"Voice_Tag", 11);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("돌진", L"Monster_Speed", L"Monster_Dir", 13, 1.1, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("전체적으로 보라 동그라미 파티클", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 150, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("전체적으로 옅은 독안개", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 200, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("보라색 투명한 물방울", L"ButterFly_WaterSplash", L"Bone_Tail6", 0, 150, 0, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("반짝이는 보라색 모래", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 150, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("안개와 같이 나오는 왜곡", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 150, 0, 0);

	RushParallel->Add_Service(Effect0);
	RushParallel->Add_Service(Effect1);
	RushParallel->Add_Service(Effect2);
	RushParallel->Add_Service(Effect3);
	RushParallel->Add_Service(Effect4);

	RushParallel->Set_Main_Child(RushAniSeq);
	RushAniSeq->Add_Child(Show_Ani15);
	RushAniSeq->Add_Child(Show_Ani6);

	RushParallel->Set_Sub_Child(MoveSequence);
	MoveSequence->Add_Child(Rotation0);
	MoveSequence->Add_Child(Sound1Stop);
	MoveSequence->Add_Child(Sound1Play);
	MoveSequence->Add_Child(Sound1Tag);
	MoveSequence->Add_Child(Move0);
	MoveSequence->Add_Child(ChaseDir0);
	MoveSequence->Add_Child(Rotation1);
	MoveSequence->Add_Child(PushColOff);
	MoveSequence->Add_Child(Sound1Stop1);
	MoveSequence->Add_Child(Sound1Play1);
	MoveSequence->Add_Child(Sound1Tag1);
	MoveSequence->Add_Child(Move1);
	MoveSequence->Add_Child(PushColOn);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 1.85, 1, 1.1, 0);
	RushParallel->Add_Service(pHitCol);

	return RushParallel;
}

CBT_Composite_Node * CPoisonButterfly::Fire_5Bullet()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("5발탄");
	CBT_Play_Ani* Show_Ani27 = Node_Ani("5발탄", 27, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("발사준비");
	CBT_ChaseDir* Chase0 = Node_ChaseDir("방향 추적", L"Player_Pos", 1.25, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 회전", L"Player_Pos", 0.2);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 3);

	CBT_CreateBullet* PoisonBullet0 = Node_CreateBullet("독 총알", L"Monster_PoisonBullet", L"Bone_Head", L"Self_PoisonDir0", 5, 5, 1.45, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet1 = Node_CreateBullet("독 총알", L"Monster_PoisonBullet", L"Bone_Head", L"Self_PoisonDir1", 5, 5, 1.45, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet2 = Node_CreateBullet("독 총알", L"Monster_PoisonBullet", L"Bone_Head", L"Self_PoisonDir2", 5, 5, 1.45, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet3 = Node_CreateBullet("독 총알", L"Monster_PoisonBullet", L"Bone_Head", L"Self_PoisonDir3", 5, 5, 1.45, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet4 = Node_CreateBullet("독 총알", L"Monster_PoisonBullet", L"Bone_Head", L"Self_PoisonDir4", 5, 5, 1.45, 1, 1, 0, CBT_Service_Node::Finite);

	Root_Parallel->Add_Service(PoisonBullet0);
	Root_Parallel->Add_Service(PoisonBullet1);
	Root_Parallel->Add_Service(PoisonBullet2);
	Root_Parallel->Add_Service(PoisonBullet3);
	Root_Parallel->Add_Service(PoisonBullet4);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani27);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Chase0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Fire_ChaseBullet()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("반바퀴 돌아서 독샷");

	CBT_Sequence* MainSeq = Node_Sequence("반바퀴 돌아서 독샷");
	CBT_Play_Ani* Show_Ani21 = Node_Ani("반바퀴 돌아서 독샷", 21, 0.95f);//23
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	//CBT_Wait* Wait0 = Node_Wait("대기", )
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("소리1 재생", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("소리1 재생", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("소리1 이름 설정", L"Voice_Tag", 10);


	CBT_CreateBullet* PoisonBullet0 = Node_CreateBullet("독 총알", L"Monster_PoisonChaseBullet", L"Bone_Tail6", L"Self_PoisonDir2", 5, 5, 2.5, 1, 1, 0, CBT_Service_Node::Finite);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("전체적으로 보라 동그라미 파티클", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("전체적으로 쓰이는 옅은 독안개", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("반짝이는 보라색 모래", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("안개와 같이 나오는 왜곡", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 150, 0.01, 0);

	Root_Parallel->Add_Service(PoisonBullet0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);


	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani21);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Turn_4PoisonShot()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("돌면서 사방으로 독 날리기");
	
	CBT_Sequence* MainSeq = Node_Sequence("돌면서 사방으로 독 날리기");
	CBT_Play_Ani* Show_Ani22 = Node_Ani("돌면서 사방으로 독 날리기", 22, 0.95f);//24
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait*	Wait0 = Node_Wait("대기", 1.5, 0);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);

	CBT_CreateBullet* PoisonBullet0 = Node_CreateBullet("독 총알", L"Monster_PoisonRotationBullet", L"Bone_Tail6", L"Self_Front", 1.5, 3, 1.53, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet1 = Node_CreateBullet("독 총알", L"Monster_PoisonRotationBullet", L"Bone_Tail6", L"Self_Right", 1.5, 3, 1.70, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet2 = Node_CreateBullet("독 총알", L"Monster_PoisonRotationBullet", L"Bone_Tail6", L"Self_Back", 1.5, 3, 1.87, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet3 = Node_CreateBullet("독 총알", L"Monster_PoisonRotationBullet", L"Bone_Tail6", L"Self_Left", 1.5, 3, 2.04, 1, 1, 0, CBT_Service_Node::Finite);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("전체적으로 보라 동그라미 파티클", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("전체적으로 쓰이는 옅은 독안개", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("반짝이는 보라색 모래", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("안개와 같이 나오는 왜곡", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 150, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);

	Root_Parallel->Add_Service(PoisonBullet0);
	Root_Parallel->Add_Service(PoisonBullet1);
	Root_Parallel->Add_Service(PoisonBullet2);
	Root_Parallel->Add_Service(PoisonBullet3);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani22);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Smart_Eat()
{
	CBT_Sequence* Root_Seq = Node_Sequence("똑똑한 왼쪽 냠, 오른쪽 냠");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 5);

	Root_Seq->Add_Child(Right_Eat(0.85f));
	Root_Seq->Add_Child(Dist0);
	Dist0->Set_Child(Left_Eat(0.85f));

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Rush_And_ChaseBullet()
{
	CBT_Simple_Parallel* RushParallel = Node_Parallel_Immediate("돌진 병렬");
	CBT_Sequence* RushAniSeq = Node_Sequence("RushSequence");
	CBT_Play_Ani* Show_Ani15 = Node_Ani("돌진", 15, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* MoveSequence = Node_Sequence("돌진 이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적0", L"Player_Pos", 0.2);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("뒤로 이동", L"Monster_Speed", L"Monster_Dir", -3.f, 1.083, 0);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적1", L"Player_Pos", 0.3, 0);
	CBT_RotationDir* Rotation1 = Node_RotationDir("방향 추적1", L"Player_Pos", 0.267);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_SetValue* Sound1Stop1 = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play1 = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag1 = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("돌진", L"Monster_Speed", L"Monster_Dir", 13, 1.1, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);

	CBT_CreateBullet* PoisonBullet0 = Node_CreateBullet("독 총알", L"Monster_PoisonChaseBullet", L"Bone_Tail6", L"Self_PoisonDir2", 5, 5, 1.85, 1, 1, 0, CBT_Service_Node::Finite);
	RushParallel->Add_Service(PoisonBullet0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("전체적으로 보라 동그라미 파티클", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 150, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("전체적으로 옅은 독안개", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 200, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("보라색 투명한 물방울", L"ButterFly_WaterSplash", L"Bone_Tail6", 0, 150, 0, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("반짝이는 보라색 모래", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 150, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("안개와 같이 나오는 왜곡", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 150, 0, 0);

	RushParallel->Add_Service(Effect0);
	RushParallel->Add_Service(Effect1);
	RushParallel->Add_Service(Effect2);
	RushParallel->Add_Service(Effect3);
	RushParallel->Add_Service(Effect4);

	RushParallel->Set_Main_Child(RushAniSeq);
	RushAniSeq->Add_Child(Show_Ani15);
	RushAniSeq->Add_Child(Show_Ani6);

	RushParallel->Set_Sub_Child(MoveSequence);
	MoveSequence->Add_Child(Rotation0);
	MoveSequence->Add_Child(Sound1Stop);
	MoveSequence->Add_Child(Sound1Play);
	MoveSequence->Add_Child(Sound1Tag);
	MoveSequence->Add_Child(Move0);
	MoveSequence->Add_Child(ChaseDir0);
	MoveSequence->Add_Child(Rotation1);
	MoveSequence->Add_Child(PushColOff);
	MoveSequence->Add_Child(Sound1Stop1);
	MoveSequence->Add_Child(Sound1Play1);
	MoveSequence->Add_Child(Sound1Tag1);
	MoveSequence->Add_Child(Move1);
	MoveSequence->Add_Child(PushColOn);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 1.85, 1, 1.1, 0);
	RushParallel->Add_Service(pHitCol);

	return RushParallel;
}

CBT_Composite_Node * CPoisonButterfly::WhirlWind()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("휠윈드");

	CBT_Sequence* MainSeq = Node_Sequence("휠윈드");
	CBT_Play_Ani* Show_Ani16 = Node_Ani("빙그르르", 16, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 0, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적", L"Player_Pos", 1.2, 0);
	CBT_SetValue* Sound1Stop1 = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play1 = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag1 = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 3, 0.5, 0);
	CBT_SetValue* Sound2Stop1 = Node_BOOL_SetValue("소리2 재생", L"SFX_02_Stop", true);
	CBT_SetValue* Sound2Play1 = Node_BOOL_SetValue("소리2 재생", L"SFX_02_Play", true);
	CBT_SetValue* Sound2Tag1 = Node_INT_SetValue("소리2 이름 설정", L"SFX_02_Tag", 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 3, 0.5, 0);


	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("전체적으로 보라 동그라미 파티클", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("전체적으로 쓰이는 옅은 독안개", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("반짝이는 보라색 모래", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("안개와 같이 나오는 왜곡", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 150, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	// 테스트 아직 안함
	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani16);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Sound1Stop1);
	SubSeq->Add_Child(Sound1Play1);
	SubSeq->Add_Child(Sound1Tag1);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Sound2Stop1);
	SubSeq->Add_Child(Sound2Play1);
	SubSeq->Add_Child(Sound2Tag1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 1, 1, 1.7, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 1.6, 1, 1.1, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Start_Game()
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

CBT_Composite_Node * CPoisonButterfly::More_Than_HP_70()
{
	CBT_Selector* Root_Sel = Node_Selector("근거리 원거리 구분 공격");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_More_Than_HP70());

	Root_Sel->Add_Child(FarAttack_More_Than_HP70());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::NearAttack_Dist5_More_Than_HP70()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	Root_Sel->Add_Child(Poison_Tornado_After_Charging());
	Root_Sel->Add_Child(OneTurn_Poison());
	Root_Sel->Add_Child(Eat_Turn());
	Root_Sel->Add_Child(Left_Eat());
	Root_Sel->Add_Child(Right_Eat());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::FarAttack_More_Than_HP70()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Rush());
	Root_Sel->Add_Child(Fire_5Bullet());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::More_Than_HP_40()
{
	CBT_Selector* Root_Sel = Node_Selector("근거리 원거리 구분 공격");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_More_Than_HP40());

	Root_Sel->Add_Child(FarAttack_More_Than_HP40());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::NearAttack_Dist5_More_Than_HP40()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	Root_Sel->Add_Child(Poison_Tornado_After_Charging());
	Root_Sel->Add_Child(Eat_TurnEat());
	Root_Sel->Add_Child(WhirlWind());
	Root_Sel->Add_Child(Turn_4PoisonShot());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::FarAttack_More_Than_HP40()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Rush());
	Root_Sel->Add_Child(Fire_5Bullet());
	Root_Sel->Add_Child(Fire_ChaseBullet());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::HP_Final()
{
	CBT_Selector* Root_Sel = Node_Selector("근거리 원거리 구분 공격");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_Final());

	Root_Sel->Add_Child(FarAttack_Fianl());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::NearAttack_Dist5_Final()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	Root_Sel->Add_Child(Poison_Tornado_After_Charging());
	Root_Sel->Add_Child(Eat_TurnEat());
	Root_Sel->Add_Child(WhirlWind());
	Root_Sel->Add_Child(Smart_Eat());
	Root_Sel->Add_Child(Turn_4PoisonShot());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::FarAttack_Fianl()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Fire_5Bullet());
	Root_Sel->Add_Child(Rush_And_ChaseBullet());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::Start_Show()
{
	CBT_Selector* Root_Sel = Node_Selector("시연회");

	CBT_CompareValue* Check_ShowValueN = Node_BOOL_A_Equal_Value("시연회 근거리 변수 체크", L"Show_Near", true);

	Root_Sel->Add_Child(Check_ShowValueN);
	Check_ShowValueN->Set_Child(Show_ChaseAndNearAttack());

	Root_Sel->Add_Child(Show_TurnAndFarAttack());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::Show_ChaseAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("추적 후 순서대로 공격");
	CBT_MoveDirectly* Chase = Node_MoveDirectly_Chase("추적", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 5.f, 3.f);
	CBT_RotationDir* Rotation0 = Node_RotationDir("플레이어 바라보기", L"Player_Pos", 0.2);

	Root_Seq->Add_Child(Chase);
	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(Show_NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Show_TurnAndFarAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("순서대로 원거리 공격");
	CBT_RotationDir* TurnDir = Node_RotationDir("Look 회전", L"Player_Pos", 0.2);

	Root_Seq->Add_Child(TurnDir);
	Root_Seq->Add_Child(Show_FarAttack());

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Show_NearAttack()
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
	Cool0->Set_Child(Left_Eat());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Right_Eat());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Eat_TurnEat());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(Poison_Tornado_After_Charging());
	Root_Sel->Add_Child(Cool4);
	Cool4->Set_Child(OneTurn_Poison());
	Root_Sel->Add_Child(Cool5);
	Cool5->Set_Child(Eat_Turn());
	Root_Sel->Add_Child(Cool6);
	Cool6->Set_Child(WhirlWind());

	Root_Sel->Add_Child(Show_OffNearAttack);

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::Show_FarAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("순서대로 원거리 공격");

	CBT_Cooldown* Cool0 = Node_Cooldown("쿨0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("쿨1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("쿨2", 300);
	CBT_Cooldown* Cool3 = Node_Cooldown("쿨3", 300);
	CBT_Cooldown* Cool4 = Node_Cooldown("쿨4", 300);

	CBT_Play_Ani* Show_Ani3 = Node_Ani("기본", Ani_Appearance_End, 0.95f);

	CBT_SetValue* Show_ValueOff = Node_BOOL_SetValue("시연회 OFF", L"Show", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Rush());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Fire_5Bullet());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Fire_ChaseBullet());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(Turn_4PoisonShot());
	Root_Sel->Add_Child(Cool4);
	Cool4->Set_Child(Show_Ani3);

	Root_Sel->Add_Child(Show_ValueOff);

	return Root_Sel;
}

void CPoisonButterfly::Down()
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

	//if (true == m_bDown_LoopAni)
	//{
	//	if (m_pMeshCom->Is_Finish_Animation(0.95f))
	//	{
	//		m_pMeshCom->Reset_OldIndx();
	//		m_pMeshCom->SetUp_Animation(Ani_Down_Loop);
	//	}
	//}
}

HRESULT CPoisonButterfly::Update_Bone_Of_BlackBoard()
{
	m_vTail6 = *(_v3*)(&(m_pTail6Frame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Tail6", m_vTail6);

	m_vTail4 = *(_v3*)(&(m_pTail4Frame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Tail4", m_vTail4);

	m_vTail2 = *(_v3*)(&(m_pTail2Frame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Tail2", m_vTail2);

	m_vHead = *(_v3*)(&(m_pHeadFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Head", m_vHead);

	m_vTail6_Tongue2 = *(_v3*)(&(m_pTail6_Tongue2Frame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Tail6_Tongue2", m_vTail6_Tongue2);

	return S_OK;
}

HRESULT CPoisonButterfly::Update_Value_Of_BB()
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

	// 사운드
	// SFX_01
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_01_Stop"))	// 멈춤
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_01_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::Butterfly_SFX_01);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_01_Play"))	// 재생
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_01_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_01_Tag")]), CSoundManager::CHANNELID::Butterfly_SFX_01, CSoundManager::SOUND::Effect_Sound);
	}

	// SFX_02
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_02_Stop"))	// 멈춤
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_02_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::Butterfly_SFX_02);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_02_Play"))	// 재생
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_02_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_02_Tag")]), CSoundManager::CHANNELID::Butterfly_SFX_02, CSoundManager::SOUND::Effect_Sound);
	}

	// SFX_03
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_03_Stop"))	// 멈춤
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_03_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::Butterfly_SFX_02);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_03_Play"))	// 재생
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_03_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_03_Tag")]), CSoundManager::CHANNELID::Butterfly_SFX_02, CSoundManager::SOUND::Effect_Sound);
	}

	// SFX_04
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_04_Stop"))	// 멈춤
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_04_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::Butterfly_SFX_01);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_04_Play"))	// 재생
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_04_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_04_Tag")]), CSoundManager::CHANNELID::Butterfly_SFX_01, CSoundManager::SOUND::Effect_Sound);
	}

	// SFX_05
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_05_Stop"))	// 멈춤
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_05_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::Butterfly_SFX_02);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_05_Play"))	// 재생
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_05_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_05_Tag")]), CSoundManager::CHANNELID::Butterfly_SFX_02, CSoundManager::SOUND::Effect_Sound);
	}

	// SFX_06
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_06_Stop"))	// 멈춤
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_06_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::Butterfly_SFX_01);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_06_Play"))	// 재생
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_06_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_06_Tag")]), CSoundManager::CHANNELID::Butterfly_SFX_01, CSoundManager::SOUND::Effect_Sound);
	}

	// Voice
	if (true == m_pAIControllerCom->Get_BoolValue(L"Voice_Stop"))	// 멈춤
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Voice_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::ButterFly_Voice);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"Voice_Play"))	// 재생
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Voice_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"Voice_Tag")]), CSoundManager::CHANNELID::ButterFly_Voice, CSoundManager::SOUND::Effect_Sound);
	}

	// 1. 5샷 방향
	_v3 pPlayerPos = pPlayer_Trans->Get_Pos();
	_v3 vDirTemp0 = *D3DXVec3Normalize(&_v3(), &_v3(pPlayerPos - m_vHead));

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_PoisonDir0", *D3DXVec3TransformNormal(&_v3(), &vDirTemp0, D3DXMatrixRotationY(&_mat(), D3DXToRadian(25))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_PoisonDir1", *D3DXVec3TransformNormal(&_v3(), &vDirTemp0, D3DXMatrixRotationY(&_mat(), D3DXToRadian(12.5f))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_PoisonDir2", vDirTemp0);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_PoisonDir3", *D3DXVec3TransformNormal(&_v3(), &vDirTemp0, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-12.5f))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_PoisonDir4", *D3DXVec3TransformNormal(&_v3(), &vDirTemp0, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-25))));

	// 2. 본인 좌표
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Pos", m_pTransformCom->Get_Pos());

	// 3. 로테이션 총알 4방향
	_v3 vFrontDir = *D3DXVec3Normalize(&_v3(), (_v3*)m_pTransformCom->Get_WorldMat().m[2]);
	_v3 vRightDir = *D3DXVec3Normalize(&_v3(), (_v3*)m_pTransformCom->Get_WorldMat().m[0]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Front", vFrontDir);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Back", -vFrontDir);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Right", vRightDir);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Left", -vRightDir);

	return S_OK;
}

HRESULT CPoisonButterfly::Update_NF()
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


		if (!m_bAppearanceEffect && m_pMeshCom->Is_Finish_Animation(0.472f))
		{
			g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"SE_GODDESS_BARK_004.ogg"), CSoundManager::CHANNELID::ButterFly_Voice, CSoundManager::SOUND::Effect_Sound);

			// 이펙트 나올 뼈 위치 업데이트
			Update_Bone_Of_BlackBoard();

			m_bAppearanceEffect = true;
			for (_int i = 0; i < 6; i++)
				g_pManagement->Create_Effect_Delay(L"ButterFly_Crying_Distortion", 0.02f * i, m_vTail6);
		}

		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::ButterFly_Voice);

			m_pMeshCom->SetUp_Animation(Ani_Idle);
			m_bFight = true;

			// 가까운 녀석 어그로 끌림.
			Set_Target_Auto();
		}

		if (!m_bUITrigger)
		{
			m_bUITrigger = true;

			// 플레이어 발견 시, UI 활성화(지원)
			m_pBossUI->Set_Active(true);

			CMassageUI* pMassageUI = static_cast<CMassageUI*>(g_pManagement->Get_GameObjectBack(L"Layer_BossMassageUI", SCENE_STAGE));
			pMassageUI->Set_BossName(BOSS_NAME::Poison_Butterfly);
			pMassageUI->Set_Check_Play_BossnameUI(true);

			g_pSoundManager->Stop_Sound(CSoundManager::Background_Loop);
			g_pSoundManager->Play_BGM(L"Boss_Butterfly_BGM.ogg");
		}
	}

	return S_OK;
}

HRESULT CPoisonButterfly::Update_Collider()
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
	m_vecAttackCol[0]->Update(m_vTail6);
	m_vecAttackCol[1]->Update(m_vTail4);
	m_vecAttackCol[2]->Update(m_vTail2);

	m_pColliderCom->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pColliderCom->Get_Radius().y, 0.f));
	return S_OK;
}

void CPoisonButterfly::Check_PhyCollider()
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
		m_tObjParam.bCanHit = true;		//일정시간 후에 true

		m_dHitTime = 0;	// 피격가능 타임 초기화

		//m_bFight = true;		// 싸움 시작

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			// 체력 비율 70 이하되면 스턴
			if (0 == m_iDownCount)
			{
				if (0.7 >= (m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max))
				{
					g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::ButterFly_Voice);
					g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"Los_BossDown_02_gate_f.ogg"), CSoundManager::CHANNELID::ButterFly_Voice, CSoundManager::SOUND::Effect_Sound);

					m_bDown_Start = true;

					m_tObjParam.bDown = true;

					m_pMeshCom->SetUp_Animation(Ani_Down_Start);
					m_bDown_StartAni = true;	//down 함수 내부에서 쓸 것임.
					m_pAIControllerCom->Reset_BT();
					m_bAIController = false;

					m_pAIControllerCom->Set_Value_Of_BlackBoard(L"PushCol", true);
				}
			}

			if (1 == m_iDownCount)
			{
				if (0.4 >= (m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max))
				{
					g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::ButterFly_Voice);
					g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"Los_BossDown_02_gate_f.ogg"), CSoundManager::CHANNELID::ButterFly_Voice, CSoundManager::SOUND::Effect_Sound);

					m_bDown_Start = true;

					m_tObjParam.bDown = true;

					m_pMeshCom->SetUp_Animation(Ani_Down_Start);
					m_bDown_StartAni = true;	//down 함수 내부에서 쓸 것임.
					m_pAIControllerCom->Reset_BT();
					m_bAIController = false;

					m_pAIControllerCom->Set_Value_Of_BlackBoard(L"PushCol", true);
				}
			}




		}
		else
		{
			g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::ButterFly_Voice);
			g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"Los_BossDead_01_gate_f.ogg"), CSoundManager::CHANNELID::ButterFly_Voice, CSoundManager::SOUND::Effect_Sound);
		
			m_pMeshCom->SetUp_Animation(Ani_Death);	// 죽음처리 시작
			Start_Dissolve(0.7f, false, true, 0.6f);
			for (_int i = 0; i < 20; i++)
				CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(100.f, m_pTransformCom->Get_Pos(), 0.3f + (i * 0.08f)));
			CParticleMgr::Get_Instance()->Create_BossDeadParticle_Effect(m_pTransformCom->Get_Pos() + _v3(0.f, 1.3f, 0.f), 0.6f, 0.5f);
			g_pManagement->Create_ParticleEffect_Delay(L"SpawnParticle_ForBoss", 1.f, 0.6f, m_pTransformCom->Get_Pos() + _v3(0.f, 1.3f, 0.f));
			 
			//CCameraMgr::Get_Instance()->MainCamera_Oscillatation_SetUp(2.f, 20.f, 0.5f, 0.9f, CCamera::CAM_OSC_TYPE::POS_OSC);
		}
	}
	else
	{
		m_dHitTime += DELTA_60;

		if (m_dHitTime > 0.5)
		{
			m_tObjParam.bIsHit = false;		// 재충돌 가능
			//m_tObjParam.bCanHit = true;
		}
	}
}

void CPoisonButterfly::Push_Collider()
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

void CPoisonButterfly::OnCollisionEnter()
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

void CPoisonButterfly::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

HRESULT CPoisonButterfly::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_PoisonButterFly", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
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

HRESULT CPoisonButterfly::SetUp_ConstantTable(CShader* pShader)
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
	_float	fSpecularPower = 10.f;	// 메탈니스 : 높을 수록, 정반사가 강해짐.
	_float	fRoughnessPower = 0.50f;	// 러프니스 : 높을 수록, 빛 산란이 적어짐(빛이 응집됨).
	_float	fRimLightPower = 0.f;	// 림		: 높을 수록 빛이 퍼짐(림라이트의 범위가 넓어지고 , 밀집도가 낮아짐).
	_float	fMinSpecular = 1.f;	// 최소 빛	: 높을 수록 빛이 퍼짐(림라이트의 범위가 넓어지고 , 밀집도가 낮아짐).

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

HRESULT CPoisonButterfly::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	// 뼈 주소 저장
	m_pTail6Frame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Tail6");
	m_pTail4Frame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Tail4");
	m_pTail2Frame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Tail2");
	m_pHeadFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head");
	m_pTail6_Tongue2Frame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Tail6_Tongue2");

	return S_OK;
}

HRESULT CPoisonButterfly::Ready_Collider()
{
	m_vecPhysicCol.reserve(10);

	//경계체크용
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Range]->_41, m_matBones[Bone_Range]->_42, m_matBones[Bone_Range]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//몸
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 1.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Body]->_41, m_matBones[Bone_Body]->_42, m_matBones[Bone_Body]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//머리
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 0.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Head]->_41, m_matBones[Bone_Head]->_42, m_matBones[Bone_Head]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);


	//////////// 공격용 콜라이더
	m_vecAttackCol.reserve(5);

	// 꼬리 끝
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 1.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(m_vTail6);
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	// 꼬리 중간
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 1.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(m_vTail4);
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	// 꼬리 처음
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 1.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(m_vTail2);
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CPoisonButterfly::Ready_NF(void * pArg)
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

HRESULT CPoisonButterfly::Ready_Sound()
{
	m_mapSound.emplace(0, L"SE_GODDESS_ATTACK_SWING_000.ogg");	// 꼬리 공격 바람소리
	m_mapSound.emplace(1, L"SE_GODDESS_KETSUGI_MOVE_000.ogg");	// 돌진
	m_mapSound.emplace(2, L"skybridge3.ogg");	// 회오리
	m_mapSound.emplace(3, L"SE_GODDESS_KETSUGI_001.ogg");	// 총알 발사
	m_mapSound.emplace(10, L"SE_GODDESS_BARK_004.ogg");	// 공격하기 전 소리지르기
	m_mapSound.emplace(11, L"SE_GODDESS_BARK_007.ogg");	// 돌진 소리지르기
	
	return S_OK;
}

CPoisonButterfly * CPoisonButterfly::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPoisonButterfly* pInstance = new CPoisonButterfly(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPoisonButterfly");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPoisonButterfly::Clone_GameObject(void * pArg)
{
	CPoisonButterfly* pInstance = new CPoisonButterfly(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPoisonButterfly");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPoisonButterfly::Free()
{

	CMonster::Free();
}
