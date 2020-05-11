#include "stdafx.h"
#include "..\Headers\SwordGenji.h"

//#include "DamegeNumUI.h"
#include "Get_ItemUI.h"

CSwordGenji::CSwordGenji(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
	ZeroMemory(m_matBones, sizeof(_mat*) * Bone_End);
}

CSwordGenji::CSwordGenji(const CSwordGenji & rhs)
	: CMonster(rhs)
{
	ZeroMemory(m_matBones, sizeof(_mat*) * Bone_End);
}

HRESULT CSwordGenji::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CSwordGenji::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	Ready_NF(pArg);
	Ready_Weapon();
	Ready_BoneMatrix();
	Ready_Collider();

	m_tObjParam.bCanRepel = true;			// 튕겨내기 가능성 (나의 공격이 적에게서)
	m_tObjParam.bCanCounter = true;			// 반격가능성
	m_tObjParam.bCanExecution = true;		// 처형
	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 9999.f;
	m_tObjParam.fHp_Max = m_tObjParam.fHp_Cur;

	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	// MonsterHP UI
	m_pMonsterUI = static_cast<CMonsterUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MonsterHPUI", pArg));
	m_pMonsterUI->Set_Target(this);
	m_pMonsterUI->Set_Bonmatrix(m_matBones[Bone_Head]);
	m_pMonsterUI->Ready_GameObject(pArg);

	//////////////////// 행동트리 init

	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL);

	if (nullptr == pPlayer)
		return E_FAIL;

	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create();

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(pPlayer)->Get_Pos());
	pBlackBoard->Set_Value(L"HP", m_tObjParam.fHp_Cur);
	pBlackBoard->Set_Value(L"MAXHP", m_tObjParam.fHp_Max);
	pBlackBoard->Set_Value(L"HPRatio", 100);
	pBlackBoard->Set_Value(L"Show", true);

	pBlackBoard->Set_Value(L"TrailOn", false);
	pBlackBoard->Set_Value(L"TrailOff", false);

	CBT_Selector* Start_Sel = Node_Selector("행동 시작"); // 찐
	//CBT_Sequence* Start_Sel = Node_Sequence("행동 시작"); // 테스트

	pBehaviorTree->Set_Child(Start_Sel);

	//CBT_CompareValue* Check_ShowValue = Node_BOOL_A_Equal_Value("시연회 변수 체크", L"Show", true);
	//Check_ShowValue->Set_Child(Start_Show());
	//Start_Sel->Add_Child(Check_ShowValue);

	Start_Sel->Add_Child(Start_Game());
	//Start_Sel->Add_Child(Normal_Cut2());

	//CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.2);
	//Start_Sel->Add_Child(Rotation0);

	//CBT_Wait* Wait1 = Node_Wait("RushWait1", 0.5, 0);
	//Start_Sel->Add_Child(Wait1);

	//CBT_MoveAround* aaaa = Node_MoveAround("MoveAround", L"Player_Pos", -3.f, 1.f, 0.f);
	//Start_Sel->Add_Child(aaaa);
	//CBT_MoveAround* bbbb = Node_MoveAround("MoveAround", L"Player_Pos", 3.f, 1.f, 0.f);
	//Start_Sel->Add_Child(bbbb);

	//그냥 재생
	/*CBT_RotationDir* TurnDir0 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir0);

	Start_Sel->Add_Child(Normal_Cut1());

	CBT_RotationDir* TurnDir1 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir1);

	Start_Sel->Add_Child(Normal_Cut2());

	CBT_RotationDir* TurnDir2 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir2);

	Start_Sel->Add_Child(Normal_Cut3());

	CBT_RotationDir* TurnDir3 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir3);

	Start_Sel->Add_Child(Strong_RightCut());

	CBT_RotationDir* TurnDir4 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir4);

	Start_Sel->Add_Child(Strong_LeftCut());

	CBT_RotationDir* TurnDir5 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir5);

	Start_Sel->Add_Child(ThreeCombo_Cut());

	CBT_RotationDir* TurnDir6 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir6);

	Start_Sel->Add_Child(Run_Straight_Cut());

	CBT_RotationDir* TurnDir7 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir7);

	Start_Sel->Add_Child(Horizon_Cut());

	CBT_RotationDir* TurnDir8 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir8);

	Start_Sel->Add_Child(Sting_Attack());

	CBT_RotationDir* TurnDir9 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir9);

	Start_Sel->Add_Child(TwoCombo_Cut());

	CBT_RotationDir* TurnDir10 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir10);

	Start_Sel->Add_Child(Throwing1());

	CBT_RotationDir* TurnDir11 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir11);

	Start_Sel->Add_Child(Throwing2());

	CBT_RotationDir* TurnDir12 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir12);

	Start_Sel->Add_Child(Throwing_BackStep());*/

	return NOERROR;
}

_int CSwordGenji::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	Push_Collider();

	CGameObject::Update_GameObject(TimeDelta);

	// 죽었을 경우
	if (m_bIsDead)
		m_bEnable = false;

	if (m_bReadyDead)
	{
		CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle", 0.1f, m_vRightToeBase, m_vHead);
		CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle_Sub", 0.1f, m_vRightToeBase, m_vHead);
	}

	// MonsterHP UI
	if (nullptr != m_pMonsterUI)
		m_pMonsterUI->Update_GameObject(TimeDelta);

	//m_pDamegeNumUI->Update_GameObject(TimeDelta);
	//m_pGet_Item->Update_GameObject(TimeDelta);

	// 처형이 아니고, 반격 당할 수 있을 경우
	if (true == m_tObjParam.bCanExecution && true == m_tObjParam.bCanRepel)
	{
		// 플레이어 미발견
		if (false == m_bFight)
		{
			Update_NF();
		}
		// 플레이어 발견
		else
		{
			// 가까운 녀석 어그로 끌림.
			Set_Target_Auto(true);

			// 뼈 위치 업데이트
			Update_Bone_Of_BlackBoard();
			// BB 직접 업데이트
			Update_Value_Of_BB();

			if (true == m_bAIController)
				m_pAIControllerCom->Update_AIController(TimeDelta);
		}
	}
	else
	{
		// 뼈만 업데이트
		Update_Bone_Of_BlackBoard();

		if (false == m_tObjParam.bCanExecution)
		{
			// 처형 체크
			Check_Execution();
		}

		else if (false == m_tObjParam.bCanRepel)
		{
			// 반격 체크
			Check_Repel();
		}
	}

	if (false == m_bReadyDead)
		Check_PhyCollider();
	else
		Check_DeadEffect(TimeDelta);

	m_pTransformCom->Set_Pos(m_pNavMeshCom->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	return NOERROR;
}

_int CSwordGenji::Late_Update_GameObject(_double TimeDelta)
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

	if (m_pOptimizationCom->Check_InFrustumforObject(&m_pTransformCom->Get_Pos(), 2.f))
	{
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
			return E_FAIL;
	}
	//=============================================================================================

	m_dTimeDelta = TimeDelta;

	m_pSword->Late_Update_GameObject(TimeDelta);

	return _int();
}

HRESULT CSwordGenji::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	m_pMeshCom->Play_Animation(_float(m_dTimeDelta)); // * alpha

	if (m_pOptimizationCom->Check_InFrustumforObject(&m_pTransformCom->Get_Pos(), 2.f))
	{
		if (FAILED(SetUp_ConstantTable()))
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
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CSwordGenji::Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur)
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

	return S_OK;
}

CBT_Composite_Node * CSwordGenji::Normal_Cut1()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("일반 베기1");
	CBT_Play_Ani* Show_Ani96 = Node_Ani("일반 베기1", 96, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("기본", 1, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.366, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 1.f, 0.584, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 2.f, 0.136, 0);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 2.f, 0.217, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);
	CBT_Wait* Wait1 = Node_Wait("대기1", 1.084, 0);
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("이동3", L"Monster_Speed", L"Monster_Dir", 0.6f, 0.633, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani96);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(TrailOff);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move3);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.167, 1, 0.233, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Normal_Cut2()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("일반 베기2");
	CBT_Play_Ani* Show_Ani95 = Node_Ani("일반 베기2", 95, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("기본", 1, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 0.5f, 1.033, 0);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 2.f, 0.3, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 2.f, 0.1, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani95);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(TrailOff);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.05, 1, 0.283, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Normal_Cut3()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("일반 베기3");
	CBT_Play_Ani* Show_Ani94 = Node_Ani("일반 베기3", 94, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("기본", 1, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.087, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.583, 0);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.617, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -0.6f, 0.533, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.417, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 0.6f, 0.65, 0);
	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani94);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(TrailOff);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.7, 1, 0.3, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Strong_RightCut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("강하게 오른쪽으로 베기");
	CBT_Play_Ani* Show_Ani92 = Node_Ani("강하게 오른쪽으로 베기", 92, 0.9f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("기본", 1, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.167, 0);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.4, 0);
	CBT_Wait* Wait1 = Node_Wait("대기0", 0.2, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani92);
	MainSeq->Add_Child(Show_Ani1);


	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(TrailOff);


	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.483, 1, 0.167, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Strong_LeftCut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("강하게 왼쪽으로 베기");
	CBT_Play_Ani* Show_Ani93 = Node_Ani("강하게 왼쪽으로 베기", 93, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("기본", 1, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.15, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.483, 0);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.584, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -0.6f, 0.35, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.416, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 0.6f, 0.704, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani93);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(TrailOff);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.667, 1, 0.183, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::ThreeCombo_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("3연속 베기");
	CBT_Play_Ani* Show_Ani90 = Node_Ani("3연속 베기", 90, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("기본", 1, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.333, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2, 0.417, 0);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_Wait* Wait1 = Node_Wait("대기", 1.05, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani90);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(TrailOff);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.767, 1, 0.133, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.117, 1, 0.15, 0);
	CBT_UpdateParam* pHitCol2 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.533, 1, 0.134, 0);

	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);
	Root_Parallel->Add_Service(pHitCol2);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Run_Straight_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("달리면서 직선베기");

	//메인 병렬
	CBT_Sequence* Run_Cut_Seq = Node_Sequence("베기seq");
	CBT_Play_Ani* Show_Ani83 = Node_Ani("달리기 시작", 83, 0.7f);
	CBT_Play_Ani* Show_Ani84 = Node_Ani("달리기", 84, 0.95f);
	CBT_Play_Ani* Show_Ani85 = Node_Ani("베기", 85, 0.8f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("기본", 1, 0.1f);

	//서브 병렬
	CBT_Sequence* MoveSeq = Node_Sequence("돌진");
	CBT_Wait* RunWaitF = Node_Wait("RunWait0", 0.367, 0);
	CBT_MoveDirectly* Move = Node_MoveDirectly_Rush("돌진", L"Monster_Speed", L"Monster_Dir", 6.4f, 0.7, 0);

	Root_Parallel->Set_Main_Child(Run_Cut_Seq);
	Run_Cut_Seq->Add_Child(Show_Ani83);
	Run_Cut_Seq->Add_Child(Show_Ani84);
	Run_Cut_Seq->Add_Child(Show_Ani85);
	Run_Cut_Seq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(MoveSeq);
	MoveSeq->Add_Child(RunWaitF);
	MoveSeq->Add_Child(Move);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.868, 1, 0.1, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Start_Game()
{
	CBT_Selector* Root_Seq = Node_Selector("게임 시작");

	CBT_DistCheck* DistCheck0 = Node_DistCheck("거리 체크", L"Player_Pos", 3);
	CBT_DistCheck* DistCheck1 = Node_DistCheck("거리 체크", L"Player_Pos", 5);
	CBT_DistCheck* DistCheck2 = Node_DistCheck("거리 체크", L"Player_Pos", 7);

	Root_Seq->Add_Child(DistCheck0);
	DistCheck0->Set_Child(RotationAndNearAttack());

	Root_Seq->Add_Child(DistCheck1);
	DistCheck1->Set_Child(ChaseAndNearAttack());

	Root_Seq->Add_Child(DistCheck2);
	DistCheck2->Set_Child(LookPlayer_FarAttack());

	Root_Seq->Add_Child(Chase_NearOrFar());

	return Root_Seq;
}

CBT_Composite_Node * CSwordGenji::RotationAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("돌고 랜덤 근접 공격");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);

	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordGenji::ChaseAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("추적 후 랜덤 공격");
	CBT_RotationDir* RotationDir0 = Node_RotationDir("플레이어 바라보기", L"Player_Pos", 0.2);

	Root_Seq->Add_Child(Chase(3.f));
	Root_Seq->Add_Child(RotationDir0);
	Root_Seq->Add_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordGenji::Chase_NearOrFar()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("근거리 또는 장거리 추적");

	Root_Sel->Add_Child(Chase(4));
	Root_Sel->Add_Child(Chase(6));

	return Root_Sel;
}

CBT_Composite_Node * CSwordGenji::Chase(_float fAcceptable_Radius)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_MoveDirectly* pChase = Node_MoveDirectly_Chase("추적", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 3.f, fAcceptable_Radius);

	CBT_Play_Ani* Show_Ani16 = Node_Ani("추적", 16, 1.f);

	Root_Parallel->Set_Main_Child(pChase);

	Root_Parallel->Set_Sub_Child(Show_Ani16);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근거리 공격");

	Root_Sel->Add_Child(Normal_Cut1());
	Root_Sel->Add_Child(Normal_Cut2());
	Root_Sel->Add_Child(Normal_Cut3());
	Root_Sel->Add_Child(Strong_RightCut());
	Root_Sel->Add_Child(Strong_LeftCut());
	Root_Sel->Add_Child(ThreeCombo_Cut());
	Root_Sel->Add_Child(Horizontal_Cut());
	Root_Sel->Add_Child(Sting_Attack());
	Root_Sel->Add_Child(TwoCombo_Cut());
	Root_Sel->Add_Child(Run_Straight_Cut());
	Root_Sel->Add_Child(Left_Cut());
	
	return Root_Sel;
}

CBT_Composite_Node * CSwordGenji::LookPlayer_FarAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("플레이어 바라본 후 랜덤 근접 공격");

	CBT_RotationDir* Rotation0 = Node_RotationDir("플레이어 바라보기", L"Player_Pos", 0.1);

	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(FarAttack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordGenji::FarAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Throwing1());
	Root_Sel->Add_Child(Throwing2());
	Root_Sel->Add_Child(Throwing_BackStep());
	Root_Sel->Add_Child(Run_Straight_Cut());

	return Root_Sel;
}

CBT_Composite_Node * CSwordGenji::Horizontal_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("수평베기");
	CBT_Play_Ani* Show_Ani88 = Node_Ani("수평베기", 88, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("기본", 1, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.417, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2, 0.416, 0);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_Wait* Wait1 = Node_Wait("대기", 0.4f, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani88);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(TrailOff);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.9, 1, 0.317, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Sting_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("찌르기");
	CBT_Play_Ani* Show_Ani87 = Node_Ani("찌르기", 87, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("기본", 1, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.917, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2, 0.716, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani87);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.6, 1, 0.117, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::TwoCombo_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("2연속 베기");
	CBT_Play_Ani* Show_Ani86 = Node_Ani("2연속 베기", 86, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("기본", 1, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.3, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2.5f, 0.433, 0);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_Wait* Wait1 = Node_Wait("대기", 0.4, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2.5f, 0.317, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani86);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(TrailOff);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.767, 1, 0.166, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.267, 1, 0.116, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Left_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("넘어지면서 왼쪽으로 베기");
	CBT_Play_Ani* Show_Ani91 = Node_Ani("왼쪽으로 베기", 91, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("기본", 1, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.983, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2.f, 0.284, 0);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("트레일 On", L"TrailOn", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2.4f, 0.466, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("트레일 Off", L"TrailOff", true);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 0.8f, 0.45, 0);
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 0.6f, 1.467, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani91);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(TrailOff);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(Move3);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.35, 1, 0.2, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Throwing1()
{
	CBT_Sequence* Root_Seq = Node_Sequence("옆에서 던지기");

	CBT_Play_Ani* Show_Ani81 = Node_Ani("옆에서 던지기", 81, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("기본", 1, 0.1f);

	CBT_CreateBullet* Bullet0 = Node_CreateBullet("수리검", L"Monster_SwordGenjiBullet", L"Bone_LeftHandAttach", L"ShotDir", 30, 5, 1.7, 1, 1, 0, CBT_Service_Node::Finite);

	Root_Seq->Add_Service(Bullet0);

	Root_Seq->Add_Child(Show_Ani81);
	Root_Seq->Add_Child(Show_Ani1);

	return Root_Seq;
}

CBT_Composite_Node * CSwordGenji::Throwing2()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("앞으로 던지기");
	CBT_Play_Ani* Show_Ani82 = Node_Ani("앞으로 던지기", 82, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("기본", 1, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 1.45, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.267, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 1.133, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -1.f, 0.333, 0);


	CBT_CreateBullet* Bullet0 = Node_CreateBullet("수리검", L"Monster_SwordGenjiBullet", L"Bone_LeftHandAttach", L"ShotDir", 30, 5, 1.733, 1, 0, 0, CBT_Service_Node::Finite);

	Root_Parallel->Add_Service(Bullet0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani82);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Throwing_BackStep()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("백스탭 던지기");
	CBT_Play_Ani* Show_Ani89 = Node_Ani("백스탭 던지기", 89, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("기본", 1, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.933, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", -2.5, 0.6, 0);

	CBT_CreateBullet* Bullet0 = Node_CreateBullet("수리검", L"Monster_SwordGenjiBullet", L"Bone_LeftHandAttach", L"ShotDir", 30, 5, 1, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Bullet0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani89);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Show_ChaseAndNeqrAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("공격 또는 추적");
	CBT_MoveDirectly* Chase = Node_MoveDirectly_Chase("추적", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 3.f, 2.f);

	Root_Seq->Add_Child(Chase);
	Root_Seq->Add_Child(Show_NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordGenji::Show_NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("근거리 공격");

	CBT_Cooldown* Cool0 = Node_Cooldown("쿨0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("쿨1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("쿨2", 300);
	CBT_Cooldown* Cool3 = Node_Cooldown("쿨3", 300);
	CBT_Cooldown* Cool4 = Node_Cooldown("쿨4", 300);
	CBT_Cooldown* Cool5 = Node_Cooldown("쿨5", 300);
	CBT_Cooldown* Cool6 = Node_Cooldown("쿨6", 300);
	CBT_Cooldown* Cool7 = Node_Cooldown("쿨7", 300);
	CBT_Cooldown* Cool8 = Node_Cooldown("쿨8", 300);
	CBT_Cooldown* Cool9 = Node_Cooldown("쿨9", 300);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Normal_Cut1());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Normal_Cut2());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Normal_Cut3());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(Strong_RightCut());
	Root_Sel->Add_Child(Cool4);
	Cool4->Set_Child(Strong_LeftCut());
	Root_Sel->Add_Child(Cool5);
	Cool5->Set_Child(ThreeCombo_Cut());

	Root_Sel->Add_Child(Cool6);
	Cool6->Set_Child(Run_Straight_Cut());
	Root_Sel->Add_Child(Cool7);
	Cool7->Set_Child(Horizontal_Cut());
	Root_Sel->Add_Child(Cool8);
	Cool8->Set_Child(Sting_Attack());
	Root_Sel->Add_Child(Cool9);
	Cool9->Set_Child(TwoCombo_Cut());

	return Root_Sel;
}

HRESULT CSwordGenji::Update_Bone_Of_BlackBoard()
{
	m_vLeftHand = *(_v3*)(&(m_pLeftHandAttachFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftHandAttach", m_vLeftHand);

	m_vHead = *(_v3*)(&(m_pHeadFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Head", m_vHead);

	m_vRightToeBase = *(_v3*)(&(m_pRightToeBaseFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_RightToeBase", m_vRightToeBase);

	return S_OK;
}

HRESULT CSwordGenji::Update_Value_Of_BB()
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

	// 1. 평상시 총 발사 방향
	_mat matGunDir = m_pTransformCom->Get_WorldMat();
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ShotDir", _v3(matGunDir.m[2][0], matGunDir.m[2][1], matGunDir.m[2][2]));



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


	return S_OK;
}

HRESULT CSwordGenji::Update_NF()
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
				m_pMeshCom->SetUp_Animation(m_eNF_Ani);
			}
		}
		// 플레이어가 최대거리 밖에 있는가?
		else
			m_pMeshCom->SetUp_Animation(m_eNF_Ani);
	}
	// 플레이어 발견
	else
	{
		switch (m_eNF_Ani)
		{
		case Client::CSwordGenji::Sit1:
			m_pMeshCom->SetUp_Animation(Ani_StandUp1);
			break;
		case Client::CSwordGenji::Sit2:
			m_pMeshCom->SetUp_Animation(Ani_StandUp2);
			break;
		default:
			m_bFight = true;
			break;
		}

		// 일어나는 동작들만 따로 관리
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_pMeshCom->SetUp_Animation(Ani_Idle);
			m_bFight = true;
		}
	}

	return S_OK;
}

HRESULT CSwordGenji::Update_Collider()
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

	m_pColliderCom->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pColliderCom->Get_Radius().y, 0.f));

	return S_OK;
}

void CSwordGenji::Skill_Movement(_float _fspeed, _v3 _vDir)
{
	_v3 tmpLook;
	_float fSpeed = _fspeed;

	tmpLook = _vDir;
	D3DXVec3Normalize(&tmpLook, &tmpLook);

	// 네비게이션 적용하면 
	m_pTransformCom->Set_Pos((m_pNavMeshCom->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &tmpLook, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));
}

void CSwordGenji::Decre_Skill_Movement(_float _fMutiply)
{
	m_fSkillMoveSpeed_Cur -= (0.3f - m_fSkillMoveAccel_Cur * m_fSkillMoveAccel_Cur * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60")) * _fMutiply;
	m_fSkillMoveAccel_Cur += g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

	if (m_fSkillMoveSpeed_Cur < 0.f)
	{
		m_fSkillMoveAccel_Cur = 0.5f;
		m_fSkillMoveSpeed_Cur = 0.f;
	}
}

void CSwordGenji::Check_PhyCollider()
{
	// 충돌처리, bCanHit를 무기가 false시켜줄것임.
	if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		//===========================================================
		// 맞을 때 마다 림라이트 값을 초기화 시킴
		m_pBattleAgentCom->Set_RimChangeData();
		//==============================

		m_bAIController = false;

		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		m_bFight = true;	// 싸움 시작

		m_fSkillMoveSpeed_Cur = 4.f;
		m_fSkillMoveAccel_Cur = 0.f;
		m_fSkillMoveMultiply = 0.5f;

		m_pAIControllerCom->Reset_BT();

		if (false == m_tObjParam.bIsExecution)
		{
			m_pMeshCom->Reset_OldIndx();	//애니 인덱스 초기화

			// 맞을때 플레이어의 룩을 받아와서 그 방향으로 밈.
			m_vPushDir_forHitting = (*(_v3*)&TARGET_TO_TRANS(CMonster::Get_pTargetObject())->Get_WorldMat().m[2]);
		}

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			_float fAngle = D3DXToDegree(m_pTransformCom->Chase_Target_Angle(&TARGET_TO_TRANS(CMonster::Get_pTargetObject())->Get_Pos()));

			if (0.f <= fAngle && fAngle < 90.f)
				m_pMeshCom->SetUp_Animation(Ani_Dmg01_FR);
			else if (90.f <= fAngle && fAngle < 180.f)
				m_pMeshCom->SetUp_Animation(Ani_Dmg01_BR);
			else if (-90.f <= fAngle && fAngle < 0)
				m_pMeshCom->SetUp_Animation(Ani_Dmg01_FL);
			else
				m_pMeshCom->SetUp_Animation(Ani_Dmg01_BL);
		}
		else
		{
			Ani eTmpAnim = (m_tObjParam.bIsExecution ? Ani_Death_F : Ani_Death);
			_float fDelay = (m_tObjParam.bIsExecution ? 0.5f : 0.1f);

			m_pMeshCom->SetUp_Animation(eTmpAnim);	// 죽음처리 시작
			Start_Dissolve(0.5f, false, true, fDelay);

			m_pSword->Start_Dissolve(0.45f, false, false, 0.5f);

			m_fDeadEffect_Delay = 0.5f;
			CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(100.f, m_pTransformCom->Get_Pos(), 0.5f));		
		}
	}
	// 맞았을 때
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_bAIController = true;
			m_tObjParam.bIsHit = false;

			//m_pMeshCom->SetUp_Animation(Ani_Idle);
		}
		else if (m_pMeshCom->Is_Finish_Animation(0.4f))	// 이때부터 재충돌 가능
		{
			m_tObjParam.bIsHit = false;
		}

		// 밀림 처리
		if (m_tObjParam.bIsHit == true)
		{
			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, m_vPushDir_forHitting);

			//cout << "밀리는 중" << endl;
		}
	}
}

void CSwordGenji::Push_Collider()
{
	list<CGameObject*> tmpList[4] = {};

	tmpList[0] = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);
	tmpList[1] = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
	tmpList[2] = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);
	tmpList[3] = g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE);

	for (auto& ListObj : tmpList)
	{
		if(ListObj.empty())
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

void CSwordGenji::Check_DeadEffect(_double TimeDelta)
{
	m_fDeadEffect_Delay -= _float(TimeDelta);
	if (m_fDeadEffect_Delay > 0.f)
		return;

	m_fDeadEffect_Offset -= _float(TimeDelta);
	if (m_fDeadEffect_Offset > 0.f)
		return;

	m_fDeadEffect_Offset = 0.1f;

	D3DXFRAME_DERIVED* pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Hips");
	_v3 vHipPos = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);

	CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle", 0.1f, m_vRightToeBase, m_vHead);
	CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle_Sub", 0.1f, m_vRightToeBase, m_vHead);

	CParticleMgr::Get_Instance()->Create_Effect(L"Monster_DeadSmoke_0", m_vHead);
	CParticleMgr::Get_Instance()->Create_Effect(L"Monster_DeadSmoke_0", vHipPos);
	CParticleMgr::Get_Instance()->Create_Effect(L"Monster_DeadSmoke_0", m_vRightToeBase);
}

HRESULT CSwordGenji::Add_Component(void* pArg)
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

	_tchar name[STR_128] = { 0, };
	INFO eTemp = *(INFO*)pArg;

	if (nullptr == pArg)
		lstrcpy(name, L"Mesh_Genji_Normal");
	else
	{
		switch (eTemp.eColor)
		{
		case CSwordGenji::Jungle:
			lstrcpy(name, L"Mesh_Genji_Green");
			break;
		case CSwordGenji::Normal:
			lstrcpy(name, L"Mesh_Genji_Normal");
			break;
		case CSwordGenji::White:
			lstrcpy(name, L"Mesh_Genji_White");
			break;
		}
	}

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, name, L"Com_Mesh", (CComponent**)&m_pMeshCom)))
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

	m_pColliderCom->Set_Radius(_v3{ 1.f, 1.f, 1.f });
	m_pColliderCom->Set_Dynamic(true);
	m_pColliderCom->Set_Type(COL_SPHERE);
	m_pColliderCom->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pColliderCom->Get_Radius().y , 0.f });

	return NOERROR;
}

HRESULT CSwordGenji::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_mat		ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	//=============================================================================================
	// 기본 메트릭스
	//=============================================================================================

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;

	//=============================================================================================
	// 디졸브용 상수
	//=============================================================================================
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShaderCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 쉐이더 재질정보 수치 입력
	//=============================================================================================
	_float	fEmissivePower = 5.f;	// 이미시브 : 높을 수록, 자체 발광이 강해짐.
	_float	fSpecularPower = 1.f;	// 메탈니스 : 높을 수록, 정반사가 강해짐.
	_float	fRoughnessPower = 1.f;	// 러프니스 : 높을 수록, 빛 산란이 적어짐(빛이 응집됨).
	_float	fMinSpecular = 0.0f;	// 최소 빛	: 높을 수록 빛이 퍼짐(림라이트의 범위가 넓어지고 , 밀집도가 낮아짐).
	_float	fID_R = 1.0f;	// ID_R : R채널 ID 값 , 1이 최대
	_float	fID_G = 0.f;	// ID_G : G채널 ID 값 , 1이 최대
	_float	fID_B = 0.f;	// ID_B	: B채널 ID 값 , 1이 최대

	if (FAILED(m_pShaderCom->Set_Value("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fSpecularPower", &fSpecularPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fRoughnessPower", &fRoughnessPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fMinSpecular", &fMinSpecular, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fID_R_Power", &fID_R, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fID_G_Power", &fID_G, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fID_B_Power", &fID_B, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 림라이트 값들을 쉐이더에 등록시킴
	//=============================================================================================
	m_pBattleAgentCom->Update_RimParam_OnShader(m_pShaderCom);
	//=============================================================================================

	return NOERROR;
}

HRESULT CSwordGenji::Ready_Weapon()
{
	m_pSword = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pSword->Change_WeaponData(Wpn_SSword_Military);

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pSword->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pSword->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	return S_OK;
}

HRESULT CSwordGenji::Ready_Collider()
{
	m_vecPhysicCol.reserve(10);

	//경계체크용
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.2f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Range]->_41, m_matBones[Bone_Range]->_42, m_matBones[Bone_Range]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//몸
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 0.5f;

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

	return S_OK;
}

HRESULT CSwordGenji::Ready_NF(void * pArg)
{
	INFO eTemp = *(INFO*)pArg;

	m_eNF_Ani = eTemp.eNF_Ani;
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

void CSwordGenji::Check_Execution()
{
	// 만약 처형이 가능하다면, 리턴
	if (true == m_tObjParam.bCanExecution)
		return;

	// 처형이 불가능한데, 처형중이 아닐 경우,
	if (false == m_tObjParam.bIsExecution)
	{
		// 처형중인 상태로 만들어주고,
		m_tObjParam.bIsExecution = true;

		// 처형 종류에 따라 처형 애니메이션을 선택한다.
		Check_ExecutionAnim();

		m_pMeshCom->Reset_OldIndx();
	}

	// 처형 중일 경우,
	else if (true == m_tObjParam.bIsExecution)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.8f))
		{
			// 처형이 끝나고 죽을꺼기 땜에 초기화는 딱히 안해줌
			//m_tObjParam.bIsExecution = false;
			//m_tObjParam.bCanExecution = true;

			//죽게 세팅한다.
			//m_bIsDead = true;

			// 혹시 모르니 HP 도  음수로
			m_tObjParam.bCanHit = false;
			//m_tObjParam.fHp_Cur = -1.0f;

			return;
		}
	}

	// 처형 모션을 AniCtrl에 세팅해준다.
	if (m_tObjParam.fHp_Cur > 0.f)
		m_pMeshCom->SetUp_Animation(m_eExecutionAnim);
}

void CSwordGenji::Check_ExecutionAnim()
{
	switch (m_tObjParam.eExecutionWpn)
	{
	case EXE_WPN::EXECUTION_Wpn_Orge:
	{
		Check_OrgeExecution();
		break;
	}
	case EXE_WPN::EXECUTION_Wpn_Stinger:
	{
		Check_StingerExecution();
		break;
	}

	case EXE_WPN::EXECUTION_Wpn_Hounds:
	{
		Check_HoundsExecution();
		break;
	}

	case EXE_WPN::EXECUTION_Wpn_Ivy:
	{
		Check_IvyExecution();
		break;
	}

	}
}

void CSwordGenji::Check_OrgeExecution()
{
}

void CSwordGenji::Check_IvyExecution()
{
}

void CSwordGenji::Check_StingerExecution()
{
	switch (m_tObjParam.eExecutionType)
	{
	case EXE_TYPE::EXECUTION_Back:
	{
		m_eExecutionAnim = Ani_Execution_LongCoat_B_S;
		break;
	}
	case EXE_TYPE::EXECUTION_BackCinema:
	{
		m_eExecutionAnim = Ani_Execution_LongCoat_B;
		break;
	}
	case EXE_TYPE::EXECUTION_Front:
	{
		break;
	}
	case EXE_TYPE::EXECUTION_FrontCinema:
	{
		break;
	}
	case EXE_TYPE::EXECUTION_ComboFront:
	{
		break;
	}
	case EXE_TYPE::EXECUTION_ComboBack:
	{
		break;
	}
	case EXE_TYPE::EXECUTION_ComboCinema:
	{
		break;
	}
	}
}

void CSwordGenji::Check_HoundsExecution()
{
}

void CSwordGenji::Check_Repel()
{
	if (m_tObjParam.bCanRepel)
		return;

	if (false == m_tObjParam.bIsRepel)
	{
		// 패링당한 상태
		m_tObjParam.bIsRepel = true;

		// 반격 애니메이션으로 세팅
		m_pMeshCom->SetUp_Animation(Ani_DmgRepel);

		m_fSkillMoveAccel_Cur = 0.f;
		m_fSkillMoveSpeed_Cur = 2.f;
		m_fSkillMoveMultiply = 0.5;
	}

	else if (m_tObjParam.bIsRepel)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_tObjParam.bIsRepel = false;
			m_tObjParam.bCanRepel = true;
		}

		Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_Z));
		Decre_Skill_Movement(m_fSkillMoveMultiply);
	}
}

HRESULT CSwordGenji::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	// 뼈 주소 저장
	m_pLeftHandAttachFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHandAttach");
	m_pHeadFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head");
	m_pRightToeBaseFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightToeBase");


	return S_OK;
}

CSwordGenji * CSwordGenji::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSwordGenji* pInstance = new CSwordGenji(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CSwordGenji");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSwordGenji::Clone_GameObject(void * pArg)
{
	CSwordGenji* pInstance = new CSwordGenji(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CSwordGenji");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwordGenji::Free()
{
	//Safe_Release(m_pDamegeNumUI);
	//Safe_Release(m_pGet_Item);

	Safe_Release(m_pSword);

	CMonster::Free();
}
