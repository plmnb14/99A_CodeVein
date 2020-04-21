#include "stdafx.h"
#include "..\Headers\SwordShieldGenji.h"
#include "..\Headers\Weapon.h"

#include "MonsterUI.h"
//#include "DamegeNumUI.h"

CSwordShieldGenji::CSwordShieldGenji(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
	ZeroMemory(m_matBones, sizeof(_mat*) * Bone_End);
}

CSwordShieldGenji::CSwordShieldGenji(const CSwordShieldGenji & rhs)
	: CMonster(rhs)
{
	ZeroMemory(m_matBones, sizeof(_mat*) * Bone_End);
}

HRESULT CSwordShieldGenji::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CSwordShieldGenji::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	Ready_NF(pArg);
	Ready_Weapon();
	Ready_BoneMatrix();
	Ready_Collider();

	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 100.f;
	m_tObjParam.fHp_Max = m_tObjParam.fHp_Cur;

	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));


	//// MonsterHP UI
	m_pMonsterUI = static_cast<CMonsterUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MonsterHPUI", pArg));
	m_pMonsterUI->Set_Target(this);
	m_pMonsterUI->Set_Bonmatrix(m_matBones[Bone_Head]);
	m_pMonsterUI->Ready_GameObject(NULL);

	/*m_pDamegeNumUI = static_cast<CDamegeNumUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_DamegeNumUI", pArg));
	m_pDamegeNumUI->Set_Target(this);
	m_pDamegeNumUI->Ready_GameObject(pArg);*/

	/////////////// 행동트리 init

	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL);

	if (nullptr == pPlayer)
		return E_FAIL;

	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create();

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", m_tObjParam.fHp_Cur);
	pBlackBoard->Set_Value(L"MAXHP", m_tObjParam.fHp_Max);
	pBlackBoard->Set_Value(L"Show", true);

	//CBT_Selector* Start_Sel = Node_Selector("행동 시작"); // 찐
	CBT_Sequence* Start_Sel = Node_Sequence("행동 시작"); // 테스트
	CBT_UpdateGageRatio* UpdateHPRatioService = Node_UpdateGageRatio("체력 비율", L"HPRatio", L"MAXHP", L"HP", 1, 0.01, 0, CBT_Service_Node::Infinite);

	pBehaviorTree->Set_Child(Start_Sel);

	Start_Sel->Add_Service(UpdateHPRatioService);

	//CBT_CompareValue* Check_ShowValue = Node_BOOL_A_Equal_Value("시연회 변수 체크", L"Show", true);
	//Check_ShowValue->Set_Child(Start_Show());
	//Start_Sel->Add_Child(Check_ShowValue);
	Start_Sel->Add_Child(Start_Game());

	//Start_Sel->Add_Child(LGuard_MoveAround());

	//CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.2);
	//Start_Sel->Add_Child(Rotation0);

	// 보여주기용
	/*Start_Sel->Add_Child(ShortDelay_Sting());

	CBT_RotationDir* TurnDir0 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir0);

	Start_Sel->Add_Child(Upper_Slash());

	CBT_RotationDir* TurnDir1 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir1);

	Start_Sel->Add_Child(LongDelay_Sting());

	CBT_RotationDir* TurnDir2 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir2);

	Start_Sel->Add_Child(Shield_Attack());

	CBT_RotationDir* TurnDir3 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir3);

	Start_Sel->Add_Child(Turning_Cut());

	CBT_RotationDir* TurnDir4 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir4);

	Start_Sel->Add_Child(ShortDelay_Sting());

	CBT_RotationDir* TurnDir5 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir5);

	Start_Sel->Add_Child(Guard(3));*/

	return NOERROR;
}

_int CSwordShieldGenji::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL))
		return E_FAIL;

	Push_Collider();

	CGameObject::Update_GameObject(TimeDelta);

	// 죽었을 경우
	if (m_bIsDead)
		return DEAD_OBJ;

	// MonsterHP UI
	m_pMonsterUI->Update_GameObject(TimeDelta);
	//m_pDamegeNumUI->Update_GameObject(TimeDelta);

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

	}

	if(false == m_bReadyDead)
		Check_PhyCollider();

	m_pTransformCom->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	return NO_EVENT;
}

_int CSwordShieldGenji::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;
	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
		return E_FAIL;
	//if (FAILED(m_pRendererCom->Add_RenderList(RENDER_SHADOWTARGET, this)))
	//	return E_FAIL;

	m_dTimeDelta = TimeDelta;

	m_pSword->Late_Update_GameObject(TimeDelta);
	m_pShield->Late_Update_GameObject(TimeDelta);

	return _int();
}

HRESULT CSwordShieldGenji::Render_GameObject()
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

			//if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE_MAP))))
			//	return E_FAIL;

			m_pShaderCom->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			m_pShaderCom->End_Pass();
		}
	}

	m_pShaderCom->End_Shader();

	m_pSword->Update_GameObject(m_dTimeDelta);
	m_pShield->Update_GameObject(m_dTimeDelta);
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CSwordShieldGenji::Render_GameObject_SetPass(CShader * pShader, _int iPass)
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

CBT_Composite_Node * CSwordShieldGenji::Upper_Slash()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("어퍼슬래쉬");
	CBT_Play_Ani* Show_Ani25 = Node_Ani("어퍼슬래쉬", 25, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.25, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.317, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.683, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 1.f, 0.267, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 1, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", -0.6f, 0.433, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani25);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.417, 1, 0.116, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::LongDelay_Sting()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("긴 딜레이 찌르기");
	CBT_Sequence* MainSeq = Node_Sequence("긴 딜레이 찌르기");
	CBT_Play_Ani* Show_Ani24 = Node_Ani("길게 찌르기", 24, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.25, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 1.f, 0.817, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.916, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 0.5f, 1.267, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani24);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.133, 1, 0.117, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Shield_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("방패치기");
	CBT_Sequence* MainSeq = Node_Sequence("방패치기");
	CBT_Play_Ani* Show_Ani23 = Node_Ani("방패치기", 23, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.233, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 1, 0.717, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani23);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.933, 1, 0.134, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Turning_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("한바퀴 돌면서 베기");
	CBT_Sequence* MainSeq = Node_Sequence("한바퀴 돌면서 베기");
	CBT_Play_Ani* Show_Ani22 = Node_Ani("한바퀴 돌면서 베기", 22, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.433, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2, 1.317, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani22);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.183, 1, 0.134, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::ShortDelay_Sting()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("짧은 딜레이 찌르기");
	CBT_Sequence* MainSeq = Node_Sequence("짧은 딜레이 찌르기");
	CBT_Play_Ani* Show_Ani21 = Node_Ani("찌르기", 21, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.2, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 1, 0.65, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 2, 0.3, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani21);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.05, 1, 0.133, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Anomaly_LongSting()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("긴 딜레이 찌르기");
	CBT_Sequence* MainSeq = Node_Sequence("긴 딜레이 찌르기");
	CBT_Play_Ani* Show_Ani24 = Node_Ani("길게 찌르기", 24, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.25, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 1.f, 0.817, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.916, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("짧은 이동1", L"Monster_Speed", L"Monster_Dir", 0.5f, 1.267, 0);

	//CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("긴 이동")

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani24);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.133, 1, 0.117, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Guard(_double dGuardTime)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("방패들기");
	CBT_Sequence* MainSeq = Node_Sequence("방패들기");
	CBT_Play_Ani* Show_Ani3 = Node_Ani("시작", 3, 0.8f);
	CBT_Play_Ani* Show_Ani4 = Node_Ani("루프", 4, 0.95f);
	CBT_Wait* Wait0 = Node_Wait("루프 대기", dGuardTime, 0);
	CBT_Play_Ani* Show_Ani5 = Node_Ani("끝", 5, 0.8f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_ChaseDir* RotationDir0 = Node_ChaseDir("이동", L"Player_Pos", dGuardTime + 1, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani3);
	MainSeq->Add_Child(Show_Ani4);
	MainSeq->Add_Child(Wait0);
	MainSeq->Add_Child(Show_Ani5);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(RotationDir0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::LGuard_MoveAround()
{
	CBT_Sequence* Root_Seq = Node_Sequence("왼쪽 가드 서성임");

	CBT_SetValue* BlockOn = Node_BOOL_SetValue("Block On", L"Block", true);
	CBT_Play_Ani* Show_Ani8 = Node_Ani("왼쪽으로 가드이동", 8, 0.1f);
	CBT_SetValue* BlockOff = Node_BOOL_SetValue("Block Off", L"Block", false);

	Root_Seq->Add_Child(BlockOn);
	Root_Seq->Add_Child(Show_Ani8);
	Root_Seq->Add_Child(LeftMoveAround());
	Root_Seq->Add_Child(BlockOff);

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::RGuard_MoveAround()
{
	CBT_Sequence* Root_Seq = Node_Sequence("오른쪽 가드 서성임");

	CBT_SetValue* BlockOn = Node_BOOL_SetValue("Block On", L"Block", true);
	CBT_Play_Ani* Show_Ani7 = Node_Ani("오른쪽으로 가드이동", 7, 0.1f);
	CBT_SetValue* BlockOff = Node_BOOL_SetValue("Block Off", L"Block", false);

	Root_Seq->Add_Child(BlockOn);
	Root_Seq->Add_Child(Show_Ani7);
	Root_Seq->Add_Child(RightMoveAround());
	Root_Seq->Add_Child(BlockOff);

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::Start_Game()
{
	CBT_Sequence* Root_Seq = Node_Sequence("게임 시작");

	Root_Seq->Add_Child(MoveAround());
	Root_Seq->Add_Child(Dist_Attack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::Dist_Attack()
{
	CBT_Selector* Root_Sel = Node_Selector("거리 판단");

	CBT_DistCheck* DistCheck0 = Node_DistCheck("거리 체크4", L"Player_Pos", 3);
	CBT_DistCheck* DistCheck1 = Node_DistCheck("거리 체크4", L"Player_Pos", 4);

	Root_Sel->Add_Child(DistCheck0);
	DistCheck0->Set_Child(RotationAndNearAttack());

	Root_Sel->Add_Child(DistCheck1);
	DistCheck1->Set_Child(RotationAndNearAttack_NotShield());

	Root_Sel->Add_Child(Chase());

	return Root_Sel;
}

CBT_Composite_Node * CSwordShieldGenji::LeftMoveAround()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("왼쪽 이동 서성임");

	CBT_MoveAround*	MoveAround0 = Node_MoveAround("왼쪽으로 서성 이동", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 0.7f, 2, 2);

	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("플레이어 추적", L"Player_Pos", 5, 0);

	Root_Parallel->Set_Main_Child(MoveAround0);

	Root_Parallel->Set_Sub_Child(ChaseDir0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::RightMoveAround()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("오른쪽 이동 서성임");

	CBT_MoveAround*	MoveAround0 = Node_MoveAround("오른쪽으로 서성 이동", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", -0.7f, 2, 2);

	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("플레이어 추적", L"Player_Pos", 5, 0);

	Root_Parallel->Set_Main_Child(MoveAround0);

	Root_Parallel->Set_Sub_Child(ChaseDir0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::RotationAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("돌고 랜덤 근접 공격");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);

	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::RotationAndNearAttack_NotShield()
{
	CBT_Sequence* Root_Seq = Node_Sequence("돌고 랜덤 근접 공격, 방패공격 제외");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);

	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(NearAttack_Except_Shield_Attack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::Chase_Guard_NearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("추적_가드_근접공격");

	Root_Seq->Add_Child(Chase());
	Root_Seq->Add_Child(Guard(2));
	Root_Seq->Add_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::MoveAround()
{
	CBT_Selector* Movek_Sel = Node_Selector_Random("서성이기");

	Movek_Sel->Add_Child(LGuard_MoveAround());
	Movek_Sel->Add_Child(RGuard_MoveAround());

	return Movek_Sel;
}

CBT_Composite_Node * CSwordShieldGenji::Chase()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_MoveDirectly* pChase = Node_MoveDirectly_Chase("추적", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 3.f, 3.5f);

	CBT_Play_Ani* Show_Ani139 = Node_Ani("추적", 139, 1.f);

	Root_Parallel->Set_Main_Child(pChase);

	Root_Parallel->Set_Sub_Child(Show_Ani139);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::NearAttack_Except_Shield_Attack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근거리 공격");

	Root_Sel->Add_Child(Upper_Slash());
	Root_Sel->Add_Child(LongDelay_Sting());
	Root_Sel->Add_Child(Shield_Attack());
	Root_Sel->Add_Child(Turning_Cut());
	Root_Sel->Add_Child(ShortDelay_Sting());

	return Root_Sel;
}

CBT_Composite_Node * CSwordShieldGenji::NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근거리 공격, 방패치기 제외");

	Root_Sel->Add_Child(Upper_Slash());
	Root_Sel->Add_Child(LongDelay_Sting());
	//Root_Sel->Add_Child(Shield_Attack());	방패치기는 따로 관리
	Root_Sel->Add_Child(Turning_Cut());
	Root_Sel->Add_Child(ShortDelay_Sting());

	return Root_Sel;
}

HRESULT CSwordShieldGenji::Update_Bone_Of_BlackBoard()
{
	return E_NOTIMPL;
}

HRESULT CSwordShieldGenji::Update_Value_Of_BB()
{
	// 1. 플레이어 좌표 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL))->Get_Pos());
	// 2. 체력 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HP", m_tObjParam.fHp_Cur);

	//// 1. 몬스터 뒤쪽 방향 저장
	//_v3 vBackDir = -(*(_v3*)&m_pTransformCom->Get_WorldMat().m[2]);
	//m_pAIControllerCom->Set_Value_Of_BlackBoard(L"BackDir", vBackDir);


	return E_NOTIMPL;
}

HRESULT CSwordShieldGenji::Update_NF()
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
		case Client::CSwordShieldGenji::Sit1:
			m_pMeshCom->SetUp_Animation(Ani_StandUp1);
			break;
		case Client::CSwordShieldGenji::Sit2:
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

HRESULT CSwordShieldGenji::Update_Collider()
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

	m_pCollider->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pCollider->Get_Radius().y, 0.f));

	return S_OK;
}

void CSwordShieldGenji::Skill_Movement(_float _fspeed, _v3 _vDir)
{
	_v3 tmpLook;
	_float fSpeed = _fspeed;

	tmpLook = _vDir;
	D3DXVec3Normalize(&tmpLook, &tmpLook);

	// 네비게이션 적용하면 
	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &tmpLook, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

}

void CSwordShieldGenji::Decre_Skill_Movement(_float _fMutiply)
{
	m_fSkillMoveSpeed_Cur -= (0.3f - m_fSkillMoveAccel_Cur * m_fSkillMoveAccel_Cur * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60")) * _fMutiply;
	m_fSkillMoveAccel_Cur += g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

	if (m_fSkillMoveSpeed_Cur < 0.f)
	{
		m_fSkillMoveAccel_Cur = 0.5f;
		m_fSkillMoveSpeed_Cur = 0.f;
	}
}

void CSwordShieldGenji::Check_PhyCollider()
{
	if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		m_pMeshCom->Reset_OldIndx();	//애니 인덱스 초기화

		m_bAIController = false;

		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		m_bFight = true;	// 싸움 시작

		m_fSkillMoveSpeed_Cur = 4.f;
		m_fSkillMoveAccel_Cur = 0.f;
		m_fSkillMoveMultiply = 0.5f;

		// 맞을때 플레이어의 룩을 받아와서 그 방향으로 밈.

		_mat matPlayer = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL))->Get_WorldMat();
		m_vPushDir_forHitting = *(_v3*)&matPlayer.m[2];

		m_pAIControllerCom->Reset_BT();
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Block", false);

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			if (true == m_pAIControllerCom->Get_BoolValue(L"Block") && Is_InFov(150, m_pTransformCom, *(_v3*)&matPlayer.m[3]))
			{
				m_pMeshCom->SetUp_Animation(Ani_GuardHit_Weak);

				m_fSkillMoveSpeed_Cur = 2.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 0.5f;
			}
			else
			{
				_float fAngle = D3DXToDegree(m_pTransformCom->Chase_Target_Angle(&TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL))->Get_Pos()));

				if (0.f <= fAngle && fAngle < 90.f)
					m_pMeshCom->SetUp_Animation(Ani_Dmg01_FR);
				else if (90.f <= fAngle && fAngle < 180.f)
					m_pMeshCom->SetUp_Animation(Ani_Dmg01_BR);
				else if (-90.f <= fAngle && fAngle < 0)
					m_pMeshCom->SetUp_Animation(Ani_Dmg01_FL);
				else
					m_pMeshCom->SetUp_Animation(Ani_Dmg01_BL);
			}
		}
		else
		{
			m_pMeshCom->SetUp_Animation(Ani_Death);	// 죽음처리 시작
			Start_Dissolve(0.5f, false, true);
			m_pShield->Start_Dissolve();
			m_pSword->Start_Dissolve();
			//g_pManagement->Create_Spawn_Effect(m_pTransformCom->Get_Pos());
		}
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_bAIController = true;
			m_tObjParam.bIsHit = false;

			//m_pMeshCom->SetUp_Animation(Ani_Idle);
		}

		else if (m_pMeshCom->Is_Finish_Animation(0.5f))	// 이때부터 재충돌 가능
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

void CSwordShieldGenji::Push_Collider()
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
			if (m_pCollider->Check_Sphere(pCollider, m_pAIControllerCom->Get_V3Value(L"Monster_Dir"), m_pAIControllerCom->Get_FloatValue(L"Monster_Speed")))
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

HRESULT CSwordShieldGenji::Add_Component(void* pArg)
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_NormalGenji", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
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

	m_pCollider->Set_Radius(_v3{ 0.5f, 0.5f, 0.5f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y , 0.f });

	return NOERROR;
}

HRESULT CSwordShieldGenji::SetUp_ConstantTable()
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

HRESULT CSwordShieldGenji::Ready_Weapon()
{
	// 오른손 무기
	m_pSword = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pSword->Change_WeaponData(CWeapon::WPN_Halverd_Normal);

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pSword->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pSword->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	// 왼손 방패
	m_pShield = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pShield->Change_WeaponData(CWeapon::WPN_Shield_Normal);

	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHandAttach");
	m_pShield->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pShield->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());


	return S_OK;
}

HRESULT CSwordShieldGenji::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

HRESULT CSwordShieldGenji::Ready_Collider()
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

HRESULT CSwordShieldGenji::Ready_NF(void * pArg)
{
	INFO eTemp = *(INFO*)pArg;

	m_eNF_Ani = eTemp.eNF_Ani;
	m_fFov = eTemp.fFov;
	m_fMaxLength = eTemp.fMaxLength;
	m_fMinLength = eTemp.fMinLength;

	return S_OK;
}

CSwordShieldGenji * CSwordShieldGenji::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSwordShieldGenji* pInstance = new CSwordShieldGenji(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CSwordShieldGenji");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSwordShieldGenji::Clone_GameObject(void * pArg)
{
	// 검, 총, 방패
	// 

	CSwordShieldGenji* pInstance = new CSwordShieldGenji(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CSwordShieldGenji");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwordShieldGenji::Free()
{
	Safe_Release(m_pMonsterUI);
	//Safe_Release(m_pDamegeNumUI);

	Safe_Release(m_pShield);
	Safe_Release(m_pSword);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCollider);

	CGameObject::Free();
}
