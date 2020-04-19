#include "stdafx.h"
#include "..\Headers\FireBoy.h"
#include "..\Headers\BossHP.h"

CFireBoy::CFireBoy(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CFireBoy::CFireBoy(const CFireBoy & rhs)
	: CMonster(rhs)
{
}

HRESULT CFireBoy::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CFireBoy::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_NF(pArg);
	Ready_Weapon();
	Ready_BoneMatrix();
	Ready_Collider();

	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 2000.f;
	m_tObjParam.fHp_Max = m_tObjParam.fHp_Cur;
	m_tObjParam.fDamage = 20.f;

	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));


	///////////////// 행동트리 init

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

	CBT_Selector* Start_Sel = Node_Selector("행동 시작");
	//CBT_Sequence* Start_Sel = Node_Sequence("행동 시작");	//테스트

	pBehaviorTree->Set_Child(Start_Sel);


	//////////// 아래에 주석해놓은 4줄이 본게임에서 쓸 것임, 차례대로 공격함.

	//CBT_CompareValue* Check_ShowValue = Node_BOOL_A_Equal_Value("시연회 변수 체크", L"Show", true);
	//Check_ShowValue->Set_Child(Start_Show());
	//Start_Sel->Add_Child(Check_ShowValue);
	//Start_Sel->Add_Child(Start_Game());

	////////////

	// 패턴 확인용,  각 패턴 함수를 아래에 넣으면 재생됨

	Start_Sel->Add_Child(Start_Game());

	//CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.2);
	//Start_Sel->Add_Child(Rotation0);

	//CBT_Wait* Wait0 = Node_Wait("대기", 1, 0);
	//Start_Sel->Add_Child(Wait0);


	/////////////
	// UI 추가(지원)
	m_pBossUI = static_cast<CBossHP*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BossHP", nullptr));
	m_pBossUI->Set_UI_Pos(WINCX * 0.5f, WINCY * 0.1f);
	if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBossUI, SCENE_STAGE, L"Layer_BossHP", nullptr)))
		return E_FAIL;
	return S_OK;
}

_int CFireBoy::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	Push_Collider();

	CGameObject::Update_GameObject(TimeDelta);

	// 죽었을 경우
	if (m_bIsDead)
		return DEAD_OBJ;
	
	// 죽음 애니메이션
	if (m_bReadyDead)
	{
		// 죽기전 UI 비활성화
		m_pBossUI->Set_Active(false);
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

		if (true == m_bAIController)
			m_pAIControllerCom->Update_AIController(TimeDelta);

		// 플레이어 발견 시, UI 활성화(지원)
		m_pBossUI->Set_Active(true);
			
		// 보스UI 업데이트
		// 체력이 0이 되었을때 밀림현상 방지.
		if(0 >= m_tObjParam.fHp_Cur)
			m_pBossUI->Set_BossHPInfo(0, 100);
		else
			m_pBossUI->Set_BossHPInfo(m_tObjParam.fHp_Cur, m_tObjParam.fHp_Max);
	}

	if (false == m_bReadyDead)
		Check_PhyCollider();

	OnCollisionEnter();

	m_pTransformCom->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	return NOERROR;
}

_int CFireBoy::Late_Update_GameObject(_double TimeDelta)
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

	return NOERROR;
}

HRESULT CFireBoy::Render_GameObject()
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

	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CFireBoy::Render_GameObject_SetPass(CShader * pShader, _int iPass)
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

CBT_Composite_Node * CFireBoy::Arm_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("팔꿈치 치기");
	CBT_Play_Ani* Show_Ani32 = Node_Ani("팔꿈치 치기", 32, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.183, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.417f, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("바닥 먼지", L"FireBoy_ArmAttack_FloorDust", L"Bone_LeftForeArm"	, 0.76, 3, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("공중 먼지", L"FireBoy_ArmAttack_Dust", L"Bone_LeftForeArm"		, 0.8, 1, 0, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani32);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);




	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 0.65, 1, 0.1, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Gun_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("포신으로 치기");
	CBT_Play_Ani* Show_Ani39 = Node_Ani("포신으로 치기", 39, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.116, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.767, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.847, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 0.6f, 0.17, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani39);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);




	CBT_CreateBullet* Col0 = Node_CreateBullet("포신 때리기 Col", L"Monster_GunAttackCol", L"GunAttackPos", L"", 0, 0.07, 1.01, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_Tornado()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("불 토네이도");
	CBT_Play_Ani* Show_Ani30 = Node_Ani("불 토네이도", 30, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적", L"Player_Pos", 0.2);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.1, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.5f, 0.233, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 1.85, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 0.7f, 0.3, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("시전 준비 - 왼손에 불"	, L"FireBoy_Charge_Hand_Fire"		, L"Bone_LeftHand"	, 0.6, 50, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("시전 직전 - 바닥에 불"	, L"FireBoy_FireTornade_ReadyFire"	, L"SelfPos"		, 0.55, 1, 0, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("시전 직전 - 불기둥"		, L"FireBoy_FireTornade_ReadyFire_Up", L"SelfPos"		, 1.15, 3, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("토네이도"					, L"FireBoy_FireTornade_Mesh"		, L"SelfPos"		, 1.3, 1, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("토네이도"					, L"FireBoy_FireTornade_Mesh"		, L"SelfPos"		, 1.5, 1, 0, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("토네이도"				, L"FireBoy_FireTornade_Mesh"		, L"SelfPos"		, 1.7, 1, 0, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("토네이도"				, L"FireBoy_FireTornade_Mesh_2"		, L"SelfPos"		, 1.4, 1, 0, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("토네이도"				, L"FireBoy_FireTornade_Mesh_2"		, L"SelfPos"		, 1.6, 1, 0, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("토네이도"				, L"FireBoy_FireTornade_Mesh_2"		, L"SelfPos"		, 1.8, 1, 0, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("바닥에 용암같은거"		, L"FireBoy_FireTornade_Floor_01"	, L"SelfPos"		, 1.1, 30, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("바닥에 용암같은거"		, L"FireBoy_FireTornade_Floor_02"	, L"SelfPos"		, 1.1, 30, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("바닥에 용암같은거"		, L"FireBoy_FireTornade_Floor_03"	, L"SelfPos"		, 1.1, 30, 0, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("불똥"						, L"FireBoy_Fire_Particle_01"		, L"SelfPos"		, 0.95, 60, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("불똥"						, L"FireBoy_Fire_Particle_02"		, L"SelfPos"		, 0.95, 60, 0, 0);

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

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani30);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_CreateBullet* Col0 = Node_CreateBullet("토네이도 충돌체", L"Monster_FireTornadoCol", L"SelfPos", L"", 0, 2, 1.856, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Back_Dash()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("뒤로 대쉬");
	CBT_Play_Ani* Show_Ani14 = Node_Ani("뒤로 대쉬", 14, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", -5.f, 0.333, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -7.f, 0.433, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", -3.f, 0.517, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani14);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_Tracking()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("추적하면서 불 발사");
	CBT_Play_Ani* Show_Ani36 = Node_Ani("추적하면서 불 발사", 36, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.883, 0);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적", L"Player_Pos", 2.467, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("총기 입구 이펙트", L"FireBoy_FireBullet_GunEff", L"Bone_Muzzle", 1.0, 70, 0, 0);

	Root_Parallel->Add_Service(Effect0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani36);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(ChaseDir0);




	CBT_CreateBullet* Col0 = Node_CreateBullet("화염 발사", L"Monster_FireBullet", L"Bone_Muzzle", L"StraightFireDir", 8, 1, 0.833, 12, 0.2, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_Cone()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("부채꼴로 불 발사");
	CBT_Play_Ani* Show_Ani35 = Node_Ani("부채꼴로 불 발사", 35, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 1.166, 0);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적", L"Player_Pos", 4.05, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("총기 입구 이펙트", L"FireBoy_FireBullet_GunEff", L"Bone_Muzzle", 1.0, 70, 0, 0);

	Root_Parallel->Add_Service(Effect0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani35);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(ChaseDir0);



	CBT_CreateBullet* Col0 = Node_CreateBullet("화염 발사", L"Monster_FireBullet", L"Bone_Muzzle", L"FireDir", 8, 1, 1.166, 20, 0.2, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_BigSphere()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("추적 화염구 발사");
	CBT_Play_Ani* Show_Ani33 = Node_Ani("추적 화염구 발사", 33, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.283, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 1.f, 0.35, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적", L"Player_Pos", 0.183);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.884, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", -1.f, 0.35, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("총기 입구 이펙트", L"FireBoy_FireBullet_GunEff", L"Bone_Muzzle", 0.7, 10, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("총기 입구 샷 이펙트", L"FireBoy_FireSphere_ShotCircle", L"Bone_Muzzle", 0.8, 1, 0, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani33);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);


	CBT_CreateBullet* Col0 = Node_CreateBullet("화염구 발사", L"Monster_FireSphere", L"Bone_Muzzle", L"FireDir", 12, 1.5, 0.816, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_Ground()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("타겟 바닥에서 불");
	CBT_Play_Ani* Show_Ani31 = Node_Ani("타겟 바닥에서 불", 31, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적", L"Player_Pos", 0.2);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.733, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 4.f, 0.233, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.984, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -4.f, 0.233, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("왼손에 불", L"FireBoy_Charge_Hand_Fire", L"Bone_LeftHand", 0.6, 30, 0, 0);

	Root_Parallel->Add_Service(Effect0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani31);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);




	CBT_CreateBullet* Col0 = Node_CreateBullet("타겟 바닥에 불냄", L"Monster_FireGround", L"Player_Pos", L"", 0, 5, 1.266, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_Flame()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("폭죽 십자가 불");
	CBT_Play_Ani* Show_Ani28 = Node_Ani("폭죽 십자가 불", 28, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적", L"Player_Pos", 0.2);
	CBT_Wait* Wait0 = Node_Wait("대기0", 1.133, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 1.5f, 0.633, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.484, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -1.5f, 0.633, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani28);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);




	CBT_CreateBullet* Col0 = Node_CreateBullet("공중에서 불 나옴", L"Monster_FireBullet", L"FlamePos0", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col1 = Node_CreateBullet("공중에서 불 나옴", L"Monster_FireBullet", L"FlamePos1", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col2 = Node_CreateBullet("공중에서 불 나옴", L"Monster_FireBullet", L"FlamePos2", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col3 = Node_CreateBullet("공중에서 불 나옴", L"Monster_FireBullet", L"FlamePos3", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col4 = Node_CreateBullet("공중에서 불 나옴", L"Monster_FireBullet", L"FlamePos4", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col5 = Node_CreateBullet("공중에서 불 나옴", L"Monster_FireBullet", L"FlamePos5", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col6 = Node_CreateBullet("공중에서 불 나옴", L"Monster_FireBullet", L"FlamePos6", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col7 = Node_CreateBullet("공중에서 불 나옴", L"Monster_FireBullet", L"FlamePos7", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col8 = Node_CreateBullet("공중에서 불 나옴", L"Monster_FireBullet", L"FlamePos8", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);

	Root_Parallel->Add_Service(Col0);
	Root_Parallel->Add_Service(Col1);
	Root_Parallel->Add_Service(Col2);
	Root_Parallel->Add_Service(Col3);
	Root_Parallel->Add_Service(Col4);
	Root_Parallel->Add_Service(Col5);
	Root_Parallel->Add_Service(Col6);
	Root_Parallel->Add_Service(Col7);
	Root_Parallel->Add_Service(Col8);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Start_Game()
{
	CBT_Selector* Root_Sel = Node_Selector("게임 시작");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 4);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist4());

	Root_Sel->Add_Child(FarAttack());

	return Root_Sel;
}

CBT_Composite_Node * CFireBoy::NearAttack_Dist4()
{
	CBT_Sequence* Root_Seq = Node_Sequence("랜덤 Dist4 공격");

	Root_Seq->Add_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CFireBoy::NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	Root_Sel->Add_Child(Arm_Attack());
	Root_Sel->Add_Child(Gun_Attack());
	Root_Sel->Add_Child(Fire_Tornado());
	Root_Sel->Add_Child(Back_Dash());

	return Root_Sel;
}

CBT_Composite_Node * CFireBoy::FarAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Fire_Tracking());
	Root_Sel->Add_Child(Fire_Cone());
	Root_Sel->Add_Child(Fire_BigSphere());
	Root_Sel->Add_Child(Fire_Ground());
	Root_Sel->Add_Child(Fire_Flame());

	return Root_Sel;
}

CBT_Composite_Node * CFireBoy::Start_Show()
{
	CBT_Selector* Root_Sel = Node_Selector("시연회");

	CBT_CompareValue* Check_ShowValueN = Node_BOOL_A_Equal_Value("시연회 근거리 변수 체크", L"Show_Near", true);

	Root_Sel->Add_Child(Check_ShowValueN);
	Check_ShowValueN->Set_Child(Show_NearAttack());

	Root_Sel->Add_Child(Show_FarAttack());

	return Root_Sel;
}

CBT_Composite_Node * CFireBoy::Show_NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("순서대로 근거리 공격");

	CBT_Cooldown* Cool0 = Node_Cooldown("쿨0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("쿨1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("쿨2", 300);

	CBT_SetValue* Show_OffNearAttack = Node_BOOL_SetValue("시연회 OFF", L"Show_Near", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Arm_Attack());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Gun_Attack());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Fire_Tornado());

	Root_Sel->Add_Child(Show_OffNearAttack);

	return Root_Sel;
}

CBT_Composite_Node * CFireBoy::Show_FarAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("순서대로 원거리 공격");
	
	CBT_Cooldown* Cool0 = Node_Cooldown("쿨0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("쿨1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("쿨2", 300);
	CBT_Cooldown* Cool3 = Node_Cooldown("쿨3", 300);
	CBT_Cooldown* Cool4 = Node_Cooldown("쿨4", 300);

	CBT_SetValue* Show_ValueOff = Node_BOOL_SetValue("시연회 OFF", L"Show", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Fire_Tracking());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Fire_Cone());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Fire_BigSphere());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(Fire_Ground());
	Root_Sel->Add_Child(Cool4);
	Cool4->Set_Child(Fire_Flame());

	Root_Sel->Add_Child(Show_ValueOff);

	return Root_Sel;
}

void CFireBoy::Down()
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
			m_bDown_Finish = true;
			m_bAIController = true;

			m_pMeshCom->SetUp_Animation(Ani_Idle);
		}
	}


	// 충돌처리, bCanHit를 무기가 false시켜줄것임.
	if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		if (true == m_bDown_LoopAni)
			m_pMeshCom->Reset_OldIndx();	//루프 애니 초기화

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

HRESULT CFireBoy::Update_Bone_Of_BlackBoard()
{
	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftForeArm");
	m_vLeftForeArm = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftForeArm", m_vLeftForeArm);

	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHand");
	m_vLeftHand = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftHand", m_vLeftHand);

	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Muzzle");
	m_vMuzzle = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Muzzle", m_vMuzzle);

	return S_OK;
}

HRESULT CFireBoy::Update_Value_Of_BB()
{
	// 1. 플레이어 좌표 업데이트
	CTransform* pPlayerTransCom = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Player_Pos", pPlayerTransCom->Get_Pos());
	// 2. 체력 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HP", m_tObjParam.fHp_Cur);
	// 3. 체력 비율 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HPRatio", _float(m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max) * 100);



	// 1. 포신으로 때리기 Collider 좌표,  팔이 순식간에 지나가서 충돌이 잘 안되므로 콜라이더 반짝 등장으로 충돌할 것임.
	_mat matSelf = m_pTransformCom->Get_WorldMat();
	_v3 vSelfPos = *(_v3*)matSelf.m[3];
	_v3 vSelfLook = *D3DXVec3Normalize(&_v3(), (_v3*)matSelf.m[2]);
	_v3 vSelfRight = *D3DXVec3Normalize(&_v3(), (_v3*)matSelf.m[0]);

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"GunAttackPos", vSelfPos + vSelfLook * 1.5f + _v3(0.f, 1.f, 0.f));
	
	// 2. 포신에서 일자로 불 발사하는 방향

	_v3 vDirTemp0;

	_v3 vTempFrontDir = _v3(0.f, 0.f, 1.f);
	_float fRadian = D3DXVec3Dot(&vSelfLook, &vTempFrontDir);

	if (fRadian >= 0)
		D3DXVec3TransformNormal(&vDirTemp0, &vSelfLook, D3DXMatrixRotationX(&_mat(), D3DXToRadian(15)));
	else if (fRadian < 0)
		D3DXVec3TransformNormal(&vDirTemp0, &vSelfLook, D3DXMatrixRotationX(&_mat(), D3DXToRadian(-15)));

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"StraightFireDir", vDirTemp0);

	// 3. 포신에서 부채꼴, 화염구 발사하는 방향

	if (fRadian >= 0)
		D3DXVec3TransformNormal(&vDirTemp0, &vSelfLook, D3DXMatrixRotationX(&_mat(), D3DXToRadian(3)));
	else if (fRadian < 0)
		D3DXVec3TransformNormal(&vDirTemp0, &vSelfLook, D3DXMatrixRotationX(&_mat(), D3DXToRadian(-3)));

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FireDir", vDirTemp0);


	// 4. Flame, 플레이어 위에서 십자가로 불 떨어짐
	_v3 vReverseLook = -vSelfLook;
	_v3 vReverseRight = -vSelfRight;

	_v3 vPlayerPos = pPlayerTransCom->Get_Pos();
	_float fLength = 2.f;

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos0", _v3(0.f, 5.f, 0.f) + vPlayerPos);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos1", _v3(0.f, 5.f, 0.f) + vPlayerPos + vSelfLook * fLength);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos2", _v3(0.f, 5.f, 0.f) + vPlayerPos + vSelfRight * fLength);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos3", _v3(0.f, 5.f, 0.f) + vPlayerPos + vReverseLook * fLength);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos4", _v3(0.f, 5.f, 0.f) + vPlayerPos + vReverseRight * fLength);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos5", _v3(0.f, 5.f, 0.f) + vPlayerPos + vSelfLook * fLength * 2);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos6", _v3(0.f, 5.f, 0.f) + vPlayerPos + vSelfRight * fLength * 2);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos7", _v3(0.f, 5.f, 0.f) + vPlayerPos + vReverseLook * fLength * 2);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos8", _v3(0.f, 5.f, 0.f) + vPlayerPos + vReverseRight * fLength * 2);

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlameDir", _v3(0.f, -1.f, 0.f));


	// 5. 화염 토이네도
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SelfPos", vSelfPos);

	return S_OK;
}

HRESULT CFireBoy::Update_NF()
{
	// 플레이어 발견 못 했을 때
	if (false == m_bFindPlayer)
	{
		// 플레이어 좌표 구함.
		_v3 vPlayer_Pos = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_Pos();

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
				m_pMeshCom->SetUp_Animation(Ani_Idle);
			}
		}
		// 플레이어가 최대거리 밖에 있는가?
		else
			m_pMeshCom->SetUp_Animation(Ani_Idle);
	}
	// 플레이어 발견
	else
	{
		m_pMeshCom->SetUp_Animation(Ani_Appearance);

		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_pMeshCom->SetUp_Animation(Ani_Idle);
			m_bFight = true;
		}
	}

	return S_OK;
}

HRESULT CFireBoy::Update_Collider()
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

	///////// 공격용 콜라이더
	m_vecAttackCol[0]->Update(m_vLeftForeArm);

	m_pCollider->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pCollider->Get_Radius().y, 0.f));
	return S_OK;
}

void CFireBoy::Check_PhyCollider()
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

		m_bFight = true;		// 싸움 시작

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			// 체력 비율 70 이하되면 스턴
			if (false == m_bDown_Finish)
			{
				if (0.7 >= (m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max))
				{
					m_bDown_Start = true;

					m_tObjParam.bDown = true;

					m_pMeshCom->SetUp_Animation(Ani_Down_Start);
					m_bDown_StartAni = true;	//down 함수 내부에서 쓸 것임.
					m_pAIControllerCom->Reset_BT();
					m_bAIController = false;

				}
			}

		}
		else
		{
			m_pMeshCom->SetUp_Animation(Ani_Death);	// 죽음처리 시작
			Start_Dissolve(0.4f, false, true, 4.2f);
			g_pManagement->Create_Effect_Delay(L"Boss_Dead_Particle", 3.9f, _v3(0.f, 1.3f, 0.f), m_pTransformCom);
			g_pManagement->Create_ParticleEffect_Delay(L"SpawnParticle_ForBoss", 2.5f, 4.2f, m_pTransformCom->Get_Pos() + _v3(0.f, 0.5f, 0.f));
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

void CFireBoy::Push_Collider()
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

void CFireBoy::OnCollisionEnter()
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

void CFireBoy::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

HRESULT CFireBoy::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_FireBoy", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
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

	m_pCollider->Set_Radius(_v3{ 1.5f, 1.5f, 1.5f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y , 0.f });


	return NOERROR;
}

HRESULT CFireBoy::SetUp_ConstantTable()
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

HRESULT CFireBoy::Ready_Weapon()
{
	return E_NOTIMPL;
}

HRESULT CFireBoy::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

HRESULT CFireBoy::Ready_Collider()
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


	//////////////// 공격용 콜라이더
	m_vecAttackCol.reserve(5);

	// 팔꿈치
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 1.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(m_vLeftForeArm);
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CFireBoy::Ready_NF(void * pArg)
{
	INFO eTemp = *(INFO*)pArg;

	m_fFov = eTemp.fFov;
	m_fMaxLength = eTemp.fMaxLength;
	m_fMinLength = eTemp.fMinLength;

	return S_OK;
}

CFireBoy * CFireBoy::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFireBoy* pInstance = new CFireBoy(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CFireBoy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFireBoy::Clone_GameObject(void * pArg)
{
	CFireBoy* pInstance = new CFireBoy(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CFireBoy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFireBoy::Free()
{
	Safe_Release(m_pCollider);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}