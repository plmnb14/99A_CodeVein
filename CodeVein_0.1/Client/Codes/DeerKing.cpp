#include "stdafx.h"
#include "..\Headers\DeerKing.h"
#include "..\Headers\Weapon.h"

CDeerKing::CDeerKing(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CDeerKing::CDeerKing(const CDeerKing & rhs)
	: CMonster(rhs)
{
}

HRESULT CDeerKing::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CDeerKing::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_NF(pArg);
	Ready_Weapon();
	Ready_BoneMatrix();
	Ready_Collider();

	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 1000.f;
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

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", m_tObjParam.fHp_Cur);
	pBlackBoard->Set_Value(L"MAXHP", m_tObjParam.fHp_Max);

	pBlackBoard->Set_Value(L"Show", true);
	pBlackBoard->Set_Value(L"Show_Near", true);

	pBlackBoard->Set_Value(L"PushCol", true);	// 충돌여부 제어변수
	//pBlackBoard->Set_Value(L"PhyCol", true); // 피격판정 제어 변수

	//CBT_Selector* Start_Sel = Node_Selector("행동 시작");
	CBT_Sequence* Start_Sel = Node_Sequence("행동 시작");	//테스트

	pBehaviorTree->Set_Child(Start_Sel);

	//////////// 아래에 주석해놓은 4줄이 본게임에서 쓸 것임, 차례대로 공격함.

	//CBT_CompareValue* Check_ShowValue = Node_BOOL_A_Equal_Value("시연회 변수 체크", L"Show", true);
	//Check_ShowValue->Set_Child(Start_Show());
	//Start_Sel->Add_Child(Check_ShowValue);
	//Start_Sel->Add_Child(Start_Game());

	////////////

	// 패턴 확인용,  각 패턴 함수를 아래에 넣으면 재생됨

	Start_Sel->Add_Child(Jump_Fist());
	Start_Sel->Add_Child(Jump_In_Place());
	
	//CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.2);
	//Start_Sel->Add_Child(Rotation0);

	//CBT_Wait* Wait0 = Node_Wait("대기", 1, 0);
	//Start_Sel->Add_Child(Wait0);

	return S_OK;
}

_int CDeerKing::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL))
		return E_FAIL;

	if (true == m_pAIControllerCom->Get_BoolValue(L"PushCol"))
		Push_Collider();

	CGameObject::Update_GameObject(TimeDelta);

	// 죽었을 경우
	if (m_bIsDead)
		return DEAD_OBJ;

	// 죽음 애니메이션
	if (m_bReadyDead)
	{
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
		// 뼈 위치 업데이트
		Update_Bone_Of_BlackBoard();
		// BB 직접 업데이트
		Update_Value_Of_BB();

		if (false == m_bThrow_Shield)
		{	// 2페이지 돌입직전에 방패가 던져질 방향 업데이트
			Update_Dir_Shield_Throwing();
		}

		if (true == m_bAIController)
			m_pAIControllerCom->Update_AIController(TimeDelta);

	}

	if (false == m_bReadyDead)
		Check_PhyCollider();

	OnCollisionEnter();

	m_pTransformCom->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	return NOERROR;
}

_int CDeerKing::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;
	//if (FAILED(m_pRendererCom->Add_RenderList(RENDER_SHADOWTARGET, this)))
	//	return E_FAIL;
	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	m_pShield->Late_Update_GameObject(TimeDelta);

	return NOERROR;
}

HRESULT CDeerKing::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	m_pMeshCom->Play_Animation(_float(m_dTimeDelta)); // * alpha

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
			if (false == m_bReadyDead)
				m_iPass = m_pMeshCom->Get_MaterialPass(i, j);

			m_pShaderCom->Begin_Pass(m_iPass);

			m_pShaderCom->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

			m_pShaderCom->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			m_pShaderCom->End_Pass();
		}
	}

	m_pShaderCom->End_Shader();

	if (false == m_bThrow_Shield)
		m_pShield->Update_GameObject(m_dTimeDelta);
	else
		Update_Shield();

	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CDeerKing::Render_GameObject_SetPass(CShader * pShader, _int iPass)
{
	if (nullptr == pShader ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat		ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);
	if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
		return E_FAIL;

	m_matLastWVP = m_pTransformCom->Get_WorldMat() * ViewMatrix * ProjMatrix;

	//_mat matLightVP = g_pManagement->Get_LightViewProj();
	//if (FAILED(pShader->Set_Value("g_LightVP_Close", &matLightVP, sizeof(_mat))))
	//	return E_FAIL;

	_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

		m_pMeshCom->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			pShader->Begin_Pass(iPass);

			m_pMeshCom->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	return NOERROR;
}

CBT_Composite_Node * CDeerKing::LeftHand_Attack(_float fWeight)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("왼팔 휘두르기");
	CBT_Play_Ani* Show_Ani31 = Node_Ani("왼팔 휘두르기", 31, fWeight);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.05, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.366, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.134, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 5.f, 0.216, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0", L"Bone_LeftHand", 0.4, 60, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Small_0", L"Bone_LeftHand", 0.4, 60, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Small_1", L"Bone_LeftHand", 0.4, 60, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani31);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 0.666, 1, 0.117, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::RightFoot_Attack(_float fWeight)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("오른발로 찍기");
	CBT_Play_Ani* Show_Ani34 = Node_Ani("오른발로 찍기", 34, fWeight);
	//CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.05, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.2);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.766, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.567, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 1.f, 0.217, 0);
	
	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0"	, L"ShieldPos"	, 0.9, 1, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("눈 위로 폭발1", L"DeerKing_SnowChunk_Up_Particle_1"	, L"ShieldPos"	, 0.9, 1, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("눈 위로 폭발2", L"DeerKing_SnowChunk_Up_Particle_2"	, L"ShieldPos"	, 0.9, 2, 0.01, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("길쭉한 눈"	, L"DeerKing_Snow_Up_LongLine_0"		, L"ShieldPos"	, 0.8, 2, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Particle_0"	, L"ShieldPos"	, 0.9, 2, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"					, L"ShieldPos"	, 0.9, 3, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"							, L"ShieldPos"	, 0.9, 5, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"							, L"ShieldPos"	, 0.9, 5, 0.01, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"							, L"ShieldPos"	, 1.05, 1, 0.01, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("눈 위로 폭발2", L"DeerKing_SnowChunk_Heavy_Particle_0", L"ShieldPos"	, 0.9, 1, 0.01, 0);

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

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani34);
	//MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.816, 1, 0.1, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Rush_RightFoot()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("돌진");
	CBT_Play_Ani* Show_Ani33 = Node_Ani("돌진", 33, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.066);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", -1.f, 0.3, 0);
	CBT_Wait* Wait1 = Node_Wait("대기", 0.117, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 13.f, 0.267, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 1.f, 0.333, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0", L"ShieldPos"	, 0.4, 20, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("눈 위로 폭발1", L"DeerKing_SnowChunk_Up_Particle_1", L"ShieldPos"	, 0.4, 20, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Particle_0", L"ShieldPos", 0.4, 10, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0", L"ShieldPos"				, 0.4, 25, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0", L"ShieldPos"						, 0.4, 5, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1", L"ShieldPos"						, 0.5, 5, 0.01, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2", L"ShieldPos"						, 0.6, 5, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani33);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.466, 1, 0.384, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::WhirlWind_RightFoot()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("휠윈드");
	CBT_Play_Ani* Show_Ani32 = Node_Ani("휠윈드", 32, 0.9f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	//CBT_Wait* Wait0 = Node_Wait("대기", 0.233, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.433);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 5.f, 1, 0);
	CBT_Wait* Wait1 = Node_Wait("대기", 0.417, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 2.f, 0.366, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0", L"ShieldPos"	, 0.6, 40, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0", L"ShieldPos"			, 0.6, 5, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1", L"ShieldPos"			, 0.6, 5, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2", L"ShieldPos"			, 0.7, 1, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2", L"ShieldPos"			, 1.4, 1, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani32);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	//SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.433, 1, 1, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Head_ColdBeam()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("머리 고드름");
	CBT_Play_Ani* Show_Ani37 = Node_Ani("머리 고드름", 37, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("대기", 0.6, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.4, 0);
	CBT_Wait* Wait1 = Node_Wait("대기", 0.7, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 1.f, 0.25, 0);
	CBT_Wait* Wait2 = Node_Wait("대기", 0.616, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 2.f, 0.234, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"					, L"Bone_LeftCorner", 0.9, 5, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Small_0"					, L"Bone_LeftCorner", 0.9, 5, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Small_1"					, L"Bone_LeftCorner", 0.9, 5, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("얼음", L"DeerKing_IceStone_Up_Small_Particle_0"		, L"Bone_LeftCorner", 0.9, 1, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("눈 덩어리", L"DeerKing_SnowChunk_Up_Small_Particle_0"	, L"Bone_LeftCorner", 0.9, 2, 0.01, 0);

	CBT_CreateEffect* Effect5 = Node_New_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0"	, L"Bone_LeftCorner", nullptr, L"Self_Dir", L"V3_Null", 1.55, 5, 0.1, 0);
	CBT_CreateEffect* Effect6 = Node_New_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_1"	, L"Bone_LeftCorner", nullptr, L"Self_Dir", L"V3_Null", 1.55, 5, 0.1, 0);
	CBT_CreateEffect* Effect7 = Node_New_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_2"	, L"Bone_LeftCorner", nullptr, L"Self_Dir", L"V3_Null", 1.55, 5, 0.1, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Particle_0"		, L"Bone_LeftCorner"	, 1.55, 2, 0.01, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"						, L"Bone_LeftCorner"	, 1.55, 10, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"							, L"Bone_LeftCorner"	, 1.55, 2, 0.01, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"							, L"Bone_LeftCorner"	, 1.55, 2, 0.01, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"							, L"Bone_LeftCorner"	, 1.55, 2, 0.01, 0);
	CBT_CreateEffect* Effect13 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_LongLine_1", L"Bone_LeftCorner", nullptr, L"Self_Dir", L"V3_Null", 1.55, 8, 0.1, 0);

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

	// 고드름 생성
	CBT_CreateBullet* Col0 = Node_CreateBullet("충돌체", L"Monster_DeerKingColdBeam", L"Bone_LeftCorner", L"Self_Dir"				, 0, 1, 1.766, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col1 = Node_CreateBullet("충돌체", L"Monster_DeerKingColdBeam", L"ColdBeam_Left_Pos", L"ColdBeam_Left_Dir"	, 0, 1, 1.766, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col2 = Node_CreateBullet("충돌체", L"Monster_DeerKingColdBeam", L"ColdBeam_Right_Pos", L"ColdBeam_Right_Dir"	, 0, 1, 1.766, 1, 0, 0, CBT_Service_Node::Finite);

	Root_Parallel->Add_Service(Col0);
	Root_Parallel->Add_Service(Col1);
	Root_Parallel->Add_Service(Col2);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani37);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Slide_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("머리 고드름");
	CBT_Play_Ani* Show_Ani28 = Node_Ani("슬라이드 시작", 28, 0.8f);
	CBT_Play_Ani* Show_Ani29 = Node_Ani("슬라이드 루프", 29, 0.95f);
	CBT_Play_Ani* Show_Ani30 = Node_Ani("슬라이드 끝", 30, 0.85f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", -2.f, 0.316, 0);
	CBT_Wait* Wait0 = Node_Wait("대기", 0.334, 0);
	// 애니28번 * 0.8 = 0.866
	// 애니29번 * 0.95 = 0.259
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 10.f, 0.116 + 0.259 + 0.066, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 6.f, 0.1, 0);
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 4.f, 0.1, 0);
	CBT_MoveDirectly* Move4 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.1, 0);
	CBT_MoveDirectly* Move5 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 1.f, 0.1, 0);

	CBT_CreateEffect* Effect0 = Node_New_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0"	, L"Self_Pos", nullptr, L"Self_Dir", L"V3_Null", 0.6, 20, 0.1, 0);
	CBT_CreateEffect* Effect1 = Node_New_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_1"	, L"Self_Pos", nullptr, L"Self_Dir", L"V3_Null", 0.6, 20, 0.1, 0);
	CBT_CreateEffect* Effect8 = Node_New_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_2"	, L"Self_Pos", nullptr, L"Self_Dir", L"V3_Null", 0.6, 20, 0.1, 0);
	CBT_CreateEffect* Effect11 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_LongLine_1"					, L"Self_Pos", nullptr, L"Self_Dir", L"V3_Null", 0.6, 20, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Small_Particle_0", L"Self_Pos"		, 0.7, 20, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"						, L"Self_Pos"		, 0.6, 20, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"								, L"Self_Pos_Front"	, 0.8, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"								, L"Self_Pos_Front"	, 0.8, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"								, L"Self_Pos_Front"	, 0.8, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Mid_0"							, L"Self_Pos_Front"	, 0.6, 25, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Mid_1"						, L"Self_Pos_Front"	, 0.6, 25, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("길쭉한 눈", L"DeerKing_Snow_Up_LongLine_0"				, L"Self_Pos"		, 0.6, 25, 0.01, 0);

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

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani28);
	MainSeq->Add_Child(Show_Ani29);
	MainSeq->Add_Child(Show_Ani30);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(PushColOn);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(Move3);
	SubSeq->Add_Child(Move4);
	SubSeq->Add_Child(Move5);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.75, 1, 0.441, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Jump_Attack(_float fWeight)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("점프해서 방패 찍기");
	CBT_Play_Ani* Show_Ani35 = Node_Ani("점프해서 방패 찍기", 35, fWeight);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.466, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 1.f, 0.2, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 10.f, 0.584, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.85, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 0.7f, 0.316, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0"	, L"Self_Pos", 0.5, 3, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("눈 위로 폭발1", L"DeerKing_SnowChunk_Up_Particle_1"	, L"Self_Pos", 0.5, 3, 0.01, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("길쭉한 눈", L"DeerKing_Snow_Up_LongLine_0"			, L"Self_Pos", 0.5, 2, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Particle_0"	, L"Self_Pos", 0.5, 2, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"					, L"Self_Pos", 0.5, 3, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"							, L"Self_Pos", 0.5, 5, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"							, L"Self_Pos", 0.5, 5, 0.01, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"							, L"Self_Pos", 0.5, 1, 0.01, 0);

	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0"		, L"Self_Pos", 1.4, 3, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("눈 위로 폭발1", L"DeerKing_SnowChunk_Up_Particle_1"		, L"Self_Pos", 1.4, 3, 0.01, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("눈 위로 폭발2", L"DeerKing_SnowChunk_Heavy_Particle_0"	, L"Self_Pos", 1.4, 1, 0.01, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("길쭉한 눈", L"DeerKing_Snow_Up_LongLine_0"				, L"Self_Pos", 1.4, 2, 0.01, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Particle_0"		, L"Self_Pos", 1.4, 2, 0.01, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"					, L"Self_Pos", 1.4, 3, 0.01, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"							, L"Self_Pos", 1.4, 5, 0.01, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"							, L"Self_Pos", 1.4, 5, 0.01, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"							, L"Self_Pos", 1.4, 1, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
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
	MainSeq->Add_Child(Show_Ani35);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.35, 1, 0.1, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Throwing()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	
	CBT_Sequence* MainSeq = Node_Sequence("투사체 던지기");
	CBT_Play_Ani* Show_Ani27 = Node_Ani("투사체 던지기", 27, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("대기0", 1.033, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 1.f, 0.383, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.417, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 1.f, 0.25, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani27);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("버프 전 손 오오라", L"DeerKing_IceBullet_Charge_Hand_Smoke_0"			, L"Bone_LeftHand", 0.1, 55, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("버프 전 손 오오라", L"DeerKing_IceBullet_Charge_Hand_Smoke_After_0"	, L"Bone_LeftHand", 1.35, 10, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("버프 전 손 오오라", L"DeerKing_IceBullet_Charge_Hand_Smoke_After_1"	, L"Bone_LeftHand", 1.35, 10, 0, 0);
	
	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);

	CBT_CreateBullet* Col0 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos0", L"", 20, 4, 1.516 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col1 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos1", L"", 20, 4, 1.566 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col2 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos2", L"", 20, 4, 1.616 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col3 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos3", L"", 20, 4, 1.666 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col4 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos4", L"", 20, 4, 1.716 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col5 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos5", L"", 20, 4, 1.766 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col6 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos6", L"", 20, 4, 1.816 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col7 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos7", L"", 20, 4, 1.866 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);

	Root_Parallel->Add_Service(Col0);
	Root_Parallel->Add_Service(Col1);
	Root_Parallel->Add_Service(Col2);
	Root_Parallel->Add_Service(Col3);
	Root_Parallel->Add_Service(Col4);
	Root_Parallel->Add_Service(Col5);
	Root_Parallel->Add_Service(Col6);
	Root_Parallel->Add_Service(Col7);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Jump_In_Place()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("날기");
	CBT_Play_Ani* Show_Ani43 = Node_Ani("날기 시작", 43, 0.85f);
	CBT_Play_Ani* Show_Ani44 = Node_Ani("떨어짐 시작", 44, 0.9f);
	CBT_Play_Ani* Show_Ani46 = Node_Ani("떨어짐 끝", 46, 0.9f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.3, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.5);
	// Show_Ani43 * 0.85 = 0.85
	// Show_Ani44 * 0.9 = 0.90

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0"		, L"Self_Pos", 0.5, 3, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("눈 위로 폭발1", L"DeerKing_SnowChunk_Up_Particle_1"		, L"Self_Pos", 0.5, 3, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Particle_0"		, L"Self_Pos", 0.5, 2, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"						, L"Self_Pos", 0.5, 10, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"								, L"Self_Pos", 0.5, 5, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"								, L"Self_Pos", 0.5, 5, 0.01, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"								, L"Self_Pos", 0.5, 1, 0.01, 0);

	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet"						, L"Bone_LeftJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet"						, L"Bone_RightJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_01"			, L"Bone_LeftJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect21 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_01"			, L"Bone_RightJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect22 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_02"			, L"Bone_LeftJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect23 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_03"			, L"Bone_RightJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect24 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_GunEff"				, L"Bone_LeftJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect25 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_GunEff"				, L"Bone_RightJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("손 냉기", L"DeerKing_IceSmoke_Mid_1"						, L"Bone_LeftHand"	, 0.5, 65, 0.01, 0);
	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("손 냉기", L"DeerKing_IceSmoke_Mid_1"						, L"Bone_RightHand"	, 0.5, 65, 0.01, 0);

	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0"		, L"Self_Pos", 1.5, 3, 0.01, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("눈 위로 폭발1", L"DeerKing_SnowChunk_Up_Particle_1"		, L"Self_Pos", 1.5, 3, 0.01, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("눈 위로 폭발2", L"DeerKing_SnowChunk_Heavy_Particle_0"	, L"Self_Pos", 1.5, 1, 0.01, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Particle_0"		, L"Self_Pos", 1.5, 2, 0.01, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"					, L"Self_Pos", 1.5, 10, 0.01, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"							, L"Self_Pos", 1.5, 5, 0.01, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"							, L"Self_Pos", 1.5, 5, 0.01, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"							, L"Self_Pos", 1.5, 1, 0.01, 0);
	
	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
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

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani43);
	MainSeq->Add_Child(Show_Ani44);
	MainSeq->Add_Child(Show_Ani46);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);

	CBT_CreateBullet* Col0 = Node_CreateBullet("점프 찍기 충돌체", L"Monster_DeerKingJumpInPlaceCol", L"SelfPos", L"", 0, 0.1, 1.023 + 0.239 + 0.2, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Rush_Body()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("돌진 몸통 박치기");
	CBT_Play_Ani* Show_Ani50 = Node_Ani("돌진 시작", 50, 0.85f);
	CBT_Play_Ani* Show_Ani51 = Node_Ani("돌진 중", 51, 0.95f);
	CBT_Play_Ani* Show_Ani53 = Node_Ani("돌진 끝", 53, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	//CBT_Wait* Wait0 = Node_Wait("대기0", 0.083, 0);
	CBT_ChaseDir* Chase0 = Node_ChaseDir("추적", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);

	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 1.f, 0.1, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 2.f, 0.3, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 5.f, 0.3, 0);
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("이동3", L"Monster_Speed", L"Monster_Dir", 25.f, 0.475, 0);
	CBT_MoveDirectly* Move4 = Node_MoveDirectly_Rush("이동4", L"Monster_Speed", L"Monster_Dir", 3.f, 0.433, 0);
	CBT_MoveDirectly* Move5 = Node_MoveDirectly_Rush("이동5", L"Monster_Speed", L"Monster_Dir", 2.f, 0.433, 0);
	CBT_MoveDirectly* Move6 = Node_MoveDirectly_Rush("이동6", L"Monster_Speed", L"Monster_Dir", 1.f, 0.433, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);
	//0.617 + 0.142 + 0.416 (돌진 속도) = 1.175

	// 밀림 처리 뺴줘야함.

	// Show_Ani50 * 0.85 = 0.906
	// Show_Ani51 * 0.95 = 0.142

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Mid"			, L"Bone_LeftJet"	, 0.5, 65, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Mid"			, L"Bone_RightJet"	, 0.5, 65, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_01"	, L"Bone_LeftJet"	, 0.5, 65, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_01"	, L"Bone_RightJet"	, 0.5, 65, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_02"	, L"Bone_LeftJet"	, 0.5, 65, 0.01, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_02"	, L"Bone_RightJet"	, 0.5, 65, 0.01, 0);

	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"				, L"Self_Pos"		, 0.6, 20, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"						, L"Self_Pos_Front"	, 0.8, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"						, L"Self_Pos_Front"	, 0.8, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"						, L"Self_Pos_Front"	, 0.8, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Mid_0"					, L"Self_Pos_Front"	, 0.6, 25, 0.01, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Mid_1"					, L"Self_Pos_Front"	, 0.6, 25, 0.01, 0);

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

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani50);
	MainSeq->Add_Child(Show_Ani51);
	MainSeq->Add_Child(Show_Ani53);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	//SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Chase0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(Move3);
	SubSeq->Add_Child(Move4);
	SubSeq->Add_Child(Move5);
	SubSeq->Add_Child(Move6);
	SubSeq->Add_Child(PushColOn);

	CBT_CreateBuff* Col0 = Node_CreateBuff("몸통 충돌체", L"Monster_DeerKingRushCol", 0.475, 0.9, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);


	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Jump_Fist()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("점프 내려치기");
	CBT_Play_Ani* Show_Ani47 = Node_Ani("점프 내려치기 시작", 47, 0.9f);
	CBT_Play_Ani* Show_Ani48 = Node_Ani("점프 내려치기 중", 48, 0.95f);
	CBT_Play_Ani* Show_Ani49 = Node_Ani("점프 내려치기 끝", 49, 0.9f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.6, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.15);

	// Show_Ani47 * 0.9 = 0.99
	// Show_Ani48 * 0.95 = 0.031

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet"				, L"Bone_LeftJet"	, 0.2, 50, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet"				, L"Bone_RightJet"	, 0.2, 50, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_01"	, L"Bone_LeftJet"	, 0.2, 60, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_01"	, L"Bone_RightJet"	, 0.2, 60, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_02"	, L"Bone_LeftJet"	, 0.2, 60, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_02"	, L"Bone_RightJet"	, 0.2, 60, 0.01, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("제트팩 불꽃", L"DeerKing_JumpPack_Fire"			, L"Bone_LeftJet"	, 0.2, 50, 0.01, 0.1);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("제트팩 불꽃", L"DeerKing_JumpPack_Fire"			, L"Bone_RightJet"	, 0.2, 50, 0.01, 0.1);

	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"				, L"Self_Pos"		, 1.25, 20, 0.01, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"						, L"Self_Pos_Front"	, 1.25, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"					, L"Self_Pos_Front"	, 1.25, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"					, L"Self_Pos_Front"	, 1.25, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("연기", L"DeerKing_SnowChunk_Up_Particle_0"		, L"Self_Pos"		, 1.25, 25, 0.01, 0);
	//CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("연기", L"DeerKing_SnowChunk_Up_Particle_1"		, L"Self_Pos"		, 1.25, 25, 0.01, 0);

	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"			, L"Self_Pos"		, 0.53, 20, 0.01, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"					, L"Self_Pos_Front"	, 0.53, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"					, L"Self_Pos_Front"	, 0.53, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"					, L"Self_Pos_Front"	, 0.53, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("연기", L"DeerKing_SnowChunk_Up_Particle_0"		, L"Self_Pos"		, 0.53, 25, 0.01, 0);
	//CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("연기", L"DeerKing_SnowChunk_Up_Particle_1"		, L"Self_Pos"		, 0.53, 25, 0.01, 0);
	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("왜곡", L"DeerKing_JumpPack_Distortion"			, L"Self_Pos"		, 0.53, 1, 0.01, 0);

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
	//Root_Parallel->Add_Service(Effect13);
	Root_Parallel->Add_Service(Effect14);
	Root_Parallel->Add_Service(Effect15);
	Root_Parallel->Add_Service(Effect16);
	Root_Parallel->Add_Service(Effect17);
	Root_Parallel->Add_Service(Effect18);
	//Root_Parallel->Add_Service(Effect19);
	Root_Parallel->Add_Service(Effect20);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani47);
	MainSeq->Add_Child(Show_Ani48);
	MainSeq->Add_Child(Show_Ani49);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Chase_Timer(0.554, 12.f));


	CBT_CreateBullet* Col0 = Node_CreateBullet("점프 찍기 충돌체", L"Monster_DeerKingJumpFistCol", L"JumpFist_Pos", L"", 0, 0.1, 0.99 + 0.031 + 0.35, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Blade_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("왼손 얼음칼 베기 ");
	CBT_Play_Ani* Show_Ani38 = Node_Ani("왼손 얼음칼 베기", 38, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.033, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.2);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", -1.f, 0.333, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 1.034, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 5.f, 0.316, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.567, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", -1.f, 0.383, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani38);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Chase_Timer(_double dRunTime, _float fSpeed)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", dRunTime, 0);

	CBT_Sequence* SubSeq = Node_Sequence("추적");
	CBT_MoveDirectly* Chase0 = Node_MoveDirectly_Chase("추적", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", fSpeed, 2);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Wait0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Chase0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::RightFoot_Attack__Rush_Or_WhirlWind()
{
	CBT_Sequence* Root_Seq = Node_Sequence("오른발로 찍고, 돌진이나 돌기");

	CBT_Selector* Random_Sel = Node_Selector_Random("돌진이나 돌기");

	Root_Seq->Add_Child(RightFoot_Attack(0.5f));
	Root_Seq->Add_Child(Random_Sel);
	Random_Sel->Add_Child(Rush_RightFoot());
	Random_Sel->Add_Child(WhirlWind_RightFoot());

	return Root_Seq;
}

CBT_Composite_Node * CDeerKing::Smart_Three_Attack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("똑똑한 3연타");

	CBT_Selector* Random_Sel = Node_Selector_Random("2타 Or 3타");

	Root_Seq->Add_Child(LeftHand_Attack(0.8f));

	Root_Seq->Add_Child(Random_Sel);
	Random_Sel->Add_Child(RightFoot_Attack());
	Random_Sel->Add_Child(RightFoot_Attack__Rush_Or_WhirlWind());

	return Root_Seq;
}

CBT_Composite_Node * CDeerKing::Smart_JumpAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("점프 어택 연계");

	CBT_Selector* Random_Sel0 = Node_Selector_Random("2타 Or No");
	CBT_Wait* Wait0 = Node_Wait("대기", 0, 0);

	CBT_Selector* Random_Sel1 = Node_Selector_Random("돌진이나 돌기");

	Root_Seq->Add_Child(Jump_Attack(0.6f));

	Root_Seq->Add_Child(Random_Sel0);

	Random_Sel0->Add_Child(Random_Sel1);

	Random_Sel1->Add_Child(Rush_RightFoot());
	Random_Sel1->Add_Child(WhirlWind_RightFoot());

	Random_Sel0->Add_Child(Wait0);

	return Root_Seq;
}

CBT_Composite_Node * CDeerKing::Start_Game()
{
	CBT_Selector* Root_Sel = Node_Selector("게임 시작");

	CBT_CompareValue* Check_Upper_HPRatio60 = Node_FLOAT_A_Smaller_Than_Value("체력 60퍼 미만", L"HPRatio", 60.f);

	Root_Sel->Add_Child(Check_Upper_HPRatio60);
	Check_Upper_HPRatio60->Set_Child(HP_Final());
	Root_Sel->Add_Child(More_Than_HP_60());

	return Root_Sel;
}

CBT_Composite_Node * CDeerKing::More_Than_HP_60()
{
	CBT_Selector* Root_Sel = Node_Selector("근거리 원거리 구분 공격");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_More_Than_HP60());

	Root_Sel->Add_Child(FarAttack_More_Than_HP60());

	return Root_Sel;
}

CBT_Composite_Node * CDeerKing::NearAttack_Dist5_More_Than_HP60()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	Root_Sel->Add_Child(LeftHand_Attack());
	Root_Sel->Add_Child(Head_ColdBeam());
	Root_Sel->Add_Child(Smart_Three_Attack());

	return Root_Sel;
}

CBT_Composite_Node * CDeerKing::FarAttack_More_Than_HP60()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Throwing());
	Root_Sel->Add_Child(Slide_Attack());
	Root_Sel->Add_Child(Smart_JumpAttack());

	return Root_Sel;
}

CBT_Composite_Node * CDeerKing::HP_Final()
{
	CBT_Selector* Root_Sel = Node_Selector("근거리 원거리 구분 공격");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_Final());

	Root_Sel->Add_Child(FarAttack_Fianl());

	return Root_Sel;
}

CBT_Composite_Node * CDeerKing::NearAttack_Dist5_Final()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	Root_Sel->Add_Child(LeftHand_Attack());
	Root_Sel->Add_Child(Jump_In_Place());
	Root_Sel->Add_Child(Rush_Body());

	return Root_Sel;
}

CBT_Composite_Node * CDeerKing::FarAttack_Fianl()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Jump_Fist());
	Root_Sel->Add_Child(Rush_Body());
	Root_Sel->Add_Child(Throwing());
	//Root_Sel->Add_Child(Blade_Attack());
	
	return Root_Sel;
}

void CDeerKing::Down()
{
	if (true == m_bDown_StartAni)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.410f))
			m_bThrow_Shield = true;

		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_bDown_Start = false;
			m_bAIController = true;

			++m_iDownCount;

			m_pMeshCom->SetUp_Animation(Ani_Idle);
		}
	}

	// 충돌처리, bCanHit를 무기가 false시켜줄것임.
	if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

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

void CDeerKing::Update_Shield()
{
	if (true == m_bFinish_Throw_Shield)
		return;

	CTransform* pShieldTrans = static_cast<CTransform*>(m_pShield->Get_Component(L"Com_Transform"));
	
	TRANS_INFO transInfo = pShieldTrans->Get_Info();

	_v3 vShield_Pos = *(_v3*)&transInfo.matWorld.m[3];

	if (0.8f >= transInfo.matWorld.m[3][1])
	{
		vShield_Pos.y = 0.8f;
		m_bFinish_Throw_Shield = true;

		// 방패 떨어진 자리에 이펙트 발생시키기
		CTransform* pShieldTrans = static_cast<CTransform*>(m_pShield->Get_Component(L"Com_Transform"));
		_mat matShield = pShieldTrans->Get_WorldMat();
		_v3 vShieldPos = _v3(matShield.m[3][0], matShield.m[3][1], matShield.m[3][2]);

		g_pManagement->Create_Effect(L"DeerKing_SnowChunk_Up_Particle_0"	, vShieldPos, nullptr);
		g_pManagement->Create_Effect(L"DeerKing_SnowChunk_Up_Particle_1"	, vShieldPos, nullptr);
		g_pManagement->Create_Effect(L"DeerKing_SnowChunk_Heavy_Particle_0"	, vShieldPos, nullptr);
		g_pManagement->Create_Effect(L"DeerKing_IceStone_Up_Particle_0"		, vShieldPos, nullptr);
		g_pManagement->Create_Effect(L"DeerKing_Snow_Up_Particle_0"			, vShieldPos, nullptr);
		g_pManagement->Create_Effect(L"DeerKing_IceSmoke_0"					, vShieldPos, nullptr);
		g_pManagement->Create_Effect(L"DeerKing_IceSmoke_2"					, vShieldPos, nullptr);

		g_pManagement->Create_Effect_Delay(L"DeerKing_SnowChunk_Up_Particle_0"		, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_SnowChunk_Up_Particle_1"		, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_SnowChunk_Heavy_Particle_0"	, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_IceStone_Up_Particle_0"		, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"IceGirl_Buff_Break_1"					, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"IceGirl_Buff_Break_2"					, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_Snow_Up_Particle_0"			, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_Snow_Up_Particle_0"			, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_Snow_Up_Particle_0"			, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_IceSmoke_0"					, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_IceSmoke_1"					, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_IceSmoke_2"					, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_Tornade_Mesh_0"				, 0.85f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_ParticleEffect_Delay(L"IceFloorAura_01", 2.0f			, 0.8f	, m_pTransformCom->Get_Pos());
		g_pManagement->Create_ParticleEffect_Delay(L"IceFloorAura_02", 2.0f			, 0.8f	, m_pTransformCom->Get_Pos());
		g_pManagement->Create_ParticleEffect_Delay(L"IceFloorAura_03", 2.0f			, 0.8f	, m_pTransformCom->Get_Pos());
	}
	else
	{
		vShield_Pos += m_vThrowing_Dir * _float(m_fThrowing_Speed * m_dTimeDelta);

		m_fThrowing_Speed -= 0.05f;

		m_vThrowing_Dir += _v3(0.f, -0.02f, 0.f);
	}

	transInfo.matWorld.m[3][0] = vShield_Pos.x;
	transInfo.matWorld.m[3][1] = vShield_Pos.y;
	transInfo.matWorld.m[3][2] = vShield_Pos.z;

	pShieldTrans->Set_Info(transInfo);
}

void CDeerKing::Update_Dir_Shield_Throwing()
{
	m_bOld_RightHandAttach_Pos = m_vRightHandAttach;

	D3DXFRAME_DERIVED* pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_vRightHandAttach = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_RightHandAttach", m_vRightHandAttach);

	m_bCur_RightHandAttach_Pos = m_vRightHandAttach;

	m_vThrowing_Dir = *D3DXVec3Normalize(&_v3(), &(m_bCur_RightHandAttach_Pos - m_bOld_RightHandAttach_Pos));

}

HRESULT CDeerKing::Update_Bone_Of_BlackBoard()
{
	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHand");
	m_vLeftHand = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftHand", m_vLeftHand);

	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHand");
	m_vRightHand = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_RightHand", m_vRightHand);

	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine3_LeftJet");
	m_vLeftJet = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftJet", m_vLeftJet);

	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine3_RightJet");
	m_vRightJet = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_RightJet", m_vRightJet);

	// 머리 박기 패턴 - 고드름 생성위치
	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head_LeftCorner");
	m_vHeadColdBeamPos = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_vHeadColdBeamPos += (m_pTransformCom->Get_Axis(AXIS_Z) * 0.5f);
	m_vHeadColdBeamPos += (m_pTransformCom->Get_Axis(AXIS_Y) * -1.3f);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftCorner", m_vHeadColdBeamPos);
	
	// RightHandAttach는  특별히 Update_Dir_Shield_Throwing에서 업데이트 시켜줌

	return S_OK;
}

HRESULT CDeerKing::Update_Value_Of_BB()
{
	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL);

	if (nullptr == pPlayer)
		return E_FAIL;

	CTransform* pPlayer_Trans = TARGET_TO_TRANS(pPlayer);

	_mat matSelf = m_pTransformCom->Get_WorldMat();
	_v3 vSelfRight = *D3DXVec3Normalize(&_v3(), (_v3*)&matSelf.m[0]);
	_v3 vSelfUp = *D3DXVec3Normalize(&_v3(), (_v3*)&matSelf.m[1]);
	_v3 vSelfLook = *D3DXVec3Normalize(&_v3(), (_v3*)&matSelf.m[2]);
	_v3 vSelfPos = *(_v3*)&matSelf.m[3];

	// 1. 플레이어 좌표 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Player_Pos", pPlayer_Trans->Get_Pos());
	// 2. 체력 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HP", m_tObjParam.fHp_Cur);
	// 3. 체력 비율 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HPRatio", _float(m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max) * 100);

	// 4. 본인 좌표
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Pos", m_pTransformCom->Get_Pos());
	// 4-1. 본인 좌표 조금 앞에
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Pos_Front", m_pTransformCom->Get_Pos() + m_pTransformCom->Get_Axis(AXIS_Z) * 1.6f);

	/////////////////
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"V3_Null", V3_NULL);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Dir", m_pTransformCom->Get_Axis(AXIS_Z));
	/////////////////

	// 1. 방패 찍기 좌표
	//CTransform* pShieldTrans = static_cast<CTransform*>(m_pShield->Get_Component(L"Com_Transform"));
	//_mat matShield = pShieldTrans->Get_WorldMat();
	//_v3 vShieldPos = _v3(matShield.m[3][0], matShield.m[3][1], matShield.m[3][2]);
	//m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ShieldPos", vShieldPos + _v3(0.f, -0.7f, 0.f));
	
	//
	_v3 vShieldPos = m_pTransformCom->Get_Pos() + m_pTransformCom->Get_Axis(AXIS_Z) * 0.5f + m_pTransformCom->Get_Axis(AXIS_X) * 0.5f;
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ShieldPos", vShieldPos);


	// 2. 투사체 던지기 좌표
	_float fLength = 1.5f;

	_v3 vBulletPos = m_pTransformCom->Get_Pos() + m_pTransformCom->Get_Axis(AXIS_Y) * 2.3f + m_pTransformCom->Get_Axis(AXIS_Z) * 1.8f + m_pTransformCom->Get_Axis(AXIS_X) * -0.5f;

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos0", vBulletPos + fLength * vSelfUp);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos1", vBulletPos + fLength * *D3DXVec3TransformNormal(&_v3(), &vSelfUp, D3DXMatrixRotationAxis(&_mat(), &vSelfLook, D3DXToRadian(45))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos2", vBulletPos + fLength * *D3DXVec3TransformNormal(&_v3(), &vSelfUp, D3DXMatrixRotationAxis(&_mat(), &vSelfLook, D3DXToRadian(90))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos3", vBulletPos + fLength * *D3DXVec3TransformNormal(&_v3(), &vSelfUp, D3DXMatrixRotationAxis(&_mat(), &vSelfLook, D3DXToRadian(130))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos4", vBulletPos + fLength * *D3DXVec3TransformNormal(&_v3(), &vSelfUp, D3DXMatrixRotationAxis(&_mat(), &vSelfLook, D3DXToRadian(180))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos5", vBulletPos + fLength * *D3DXVec3TransformNormal(&_v3(), &vSelfUp, D3DXMatrixRotationAxis(&_mat(), &vSelfLook, D3DXToRadian(220))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos6", vBulletPos + fLength * *D3DXVec3TransformNormal(&_v3(), &vSelfUp, D3DXMatrixRotationAxis(&_mat(), &vSelfLook, D3DXToRadian(270))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos7", vBulletPos + fLength * *D3DXVec3TransformNormal(&_v3(), &vSelfUp, D3DXMatrixRotationAxis(&_mat(), &vSelfLook, D3DXToRadian(310))));

	// 3. 몬스터 본인 좌표
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SelfPos", vSelfPos);

	// 4. 점프 주먹 찍기 좌표
	fLength = 1.f;
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"JumpFist_Pos", m_vLeftHand + fLength * vSelfRight);

	// 5. 고드름 좌표
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ColdBeam_Left_Dir"	, *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationAxis(&_mat(), &vSelfUp, D3DXToRadian(-40))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ColdBeam_Right_Dir"	, *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationAxis(&_mat(), &vSelfUp, D3DXToRadian(40))));
	
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ColdBeam_Left_Pos", m_vHeadColdBeamPos + m_pTransformCom->Get_Axis(AXIS_X) * -1.f);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ColdBeam_Right_Pos", m_vHeadColdBeamPos + m_pTransformCom->Get_Axis(AXIS_X) * 1.f);


	return S_OK;
}

HRESULT CDeerKing::Update_NF()
{
	// 플레이어 발견 못 했을 때
	if (false == m_bFindPlayer)
	{
		// 플레이어 좌표 구함.
		_v3 vPlayer_Pos = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL))->Get_Pos();

		// 플레이어와 몬스터의 거리
		_v3 vLengthTemp = vPlayer_Pos - m_pTransformCom->Get_Pos();
		vLengthTemp.y = 0.f;
		_float fLength = D3DXVec3Length(&vLengthTemp);

		//cout << "거리 : " << fLength << endl;

		// 플레이어가 최소거리안에 있는가?
		if (fLength < m_fMinLength)
		{
			// 플레이어 발견
			m_bFindPlayer = true;
		}
		// 플레이어가 최대거리 안에 있는가?
		else if (fLength < m_fMaxLength)
		{
			// 플레이어가 시야각 안에 있는가?
			if (Is_InFov(m_fFov, m_pTransformCom, vPlayer_Pos))
			{
				// 플레이어 발견
				m_bFindPlayer = true;
			}
			else
			{
				m_pMeshCom->SetUp_Animation(Ani_AppearanceLoop);
			}
		}
		// 플레이어가 최대거리 밖에 있는가?
		else
			m_pMeshCom->SetUp_Animation(Ani_AppearanceLoop);


		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_pMeshCom->Reset_OldIndx();
			m_pMeshCom->SetUp_Animation(Ani_AppearanceLoop);
		}

	}
	// 플레이어 발견
	else
	{
		m_pMeshCom->SetUp_Animation(Ani_Appearance);

		if (m_pMeshCom->Is_Finish_Animation(0.97f))
		{
			m_pMeshCom->SetUp_Animation(Ani_AppearanceLoop);
			m_bFight = true;
		}
	}

	return S_OK;
}

HRESULT CDeerKing::Update_Collider()
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
	m_vecAttackCol[0]->Update(m_vLeftHand);


	m_pCollider->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pCollider->Get_Radius().y, 0.f));
	return S_OK;
}

void CDeerKing::Check_PhyCollider()
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
		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		m_dHitTime = 0;	// 피격가능 타임 초기화

		m_bFight = true;		// 싸움 시작

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			// 체력 비율 70 이하되면 스턴
			if (0 == m_iDownCount)
			{
				if (0.7 >= (m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max))
				{
					m_bDown_Start = true;

					m_tObjParam.bDown = true;

					m_pMeshCom->SetUp_Animation(Ani_Throw_Shield);
					m_bDown_StartAni = true;	//down 함수 내부에서 쓸 것임.
					m_pAIControllerCom->Reset_BT();
					m_bAIController = false;

				}
			}


			//cout << "IceGirl - Check_PhyCollider : " << m_tObjParam.fHp_Cur << endl;

		}
		else
		{
			m_pMeshCom->SetUp_Animation(Ani_Death);	// 죽음처리 시작
			Start_Dissolve(0.7f, false, true, 6.9f);
			m_pShield->Start_Dissolve(0.7f, false, false, 6.9f);
			
			CParticleMgr::Get_Instance()->Create_BossDeadParticle_Effect(m_pTransformCom->Get_Pos() + _v3(0.f, 1.3f, 0.f), 5.2f, 1.f);
			//g_pManagement->Create_Effect_Delay(L"Boss_Dead_Particle"					, 5.2f, _v3(0.f, 1.3f, 0.f), m_pTransformCom);
			g_pManagement->Create_ParticleEffect_Delay(L"SpawnParticle_ForBoss"	, 2.5f	, 6.9f, m_pTransformCom->Get_Pos() + _v3(0.f, 0.5f, 0.f));

			g_pManagement->Create_Effect_Offset(L"DeerKing_IceSmoke_0", 0.1f, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Offset(L"DeerKing_IceSmoke_1", 0.1f, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Offset(L"DeerKing_IceSmoke_2", 0.1f, m_pTransformCom->Get_Pos(), nullptr);

			g_pManagement->Create_Effect_Delay(L"DeerKing_SnowChunk_Up_Particle_0"		, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_SnowChunk_Up_Particle_1"		, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_SnowChunk_Heavy_Particle_0"	, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_Snow_Up_Particle_0"			, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_Snow_Up_Particle_0"			, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_Snow_Up_Particle_0"			, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_IceSmoke_0"					, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_IceSmoke_1"					, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_IceSmoke_2"					, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_Snow_Up_LongLine_1"			, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
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

void CDeerKing::Push_Collider()
{
	list<CGameObject*> tmpList[3];

	tmpList[0] = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);
	tmpList[1] = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
	tmpList[2] = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);

	for (auto& ListObj : tmpList)
	{
		for (auto& iter : ListObj)
		{
			CCollider* pCollider = TARGET_TO_COL(iter);

			// 지금 속도값 임의로 넣었는데 구해서 넣어줘야함 - 완료
			if (m_pCollider->Check_Sphere(pCollider, m_pTransformCom->Get_Axis(AXIS_Z), m_pAIControllerCom->Get_FloatValue(L"Monster_Speed")))
			{
				CTransform* pTrans = TARGET_TO_TRANS(iter);
				CNavMesh*   pNav = TARGET_TO_NAV(iter);

				// 방향 구해주고
				_v3 vDir = m_pTransformCom->Get_Pos() - pTrans->Get_Pos();
				V3_NORMAL_SELF(&vDir);

				// y축 이동은 하지말자
				vDir.y = 0;

				// 네비 메쉬타게 끔 세팅
				pTrans->Set_Pos(pNav->Move_OnNaviMesh(NULL, &pTrans->Get_Pos(), &vDir, m_pCollider->Get_Length().x));
			}
		}
	}
}

void CDeerKing::OnCollisionEnter()
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
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));


	// =============================================================================================
}

void CDeerKing::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

HRESULT CDeerKing::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_DeerKing", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	// for.Com_AIController
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"AIController", L"Com_AIController", (CComponent**)&m_pAIControllerCom)))
		return E_FAIL;

	// for.Com_NavMesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	m_pCollider->Set_Radius(_v3{ 1.f, 1.f, 1.f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y , 0.f });


	return NOERROR;
}

HRESULT CDeerKing::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat		ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShaderCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CDeerKing::Ready_Weapon()
{
	// 오른손 방패
	m_pShield = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pShield->Change_WeaponData(CWeapon::WPN_DeerKingShield);

	D3DXFRAME_DERIVED* pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pShield->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pShield->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	return S_OK;
}

HRESULT CDeerKing::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

HRESULT CDeerKing::Ready_Collider()
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


	////////// 공격용 콜라이더
	m_vecAttackCol.reserve(1);

	// 왼쪽 주먹
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 1.f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(m_vLeftHand);
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CDeerKing::Ready_NF(void * pArg)
{
	INFO eTemp = *(INFO*)pArg;

	m_fFov = eTemp.fFov;
	m_fMaxLength = eTemp.fMaxLength;
	m_fMinLength = eTemp.fMinLength;

	return S_OK;
}

CDeerKing * CDeerKing::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDeerKing* pInstance = new CDeerKing(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CDeerKing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDeerKing::Clone_GameObject(void * pArg)
{
	CDeerKing* pInstance = new CDeerKing(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CDeerKing");
		Safe_Release(pInstance);
	}

	return pInstance;	return nullptr;
}

void CDeerKing::Free()
{
	Safe_Release(m_pShield);
	Safe_Release(m_pCollider);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
