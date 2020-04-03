#include "stdafx.h"
#include "..\Headers\SwordGenji.h"
#include "..\Headers\Weapon.h"

CSwordGenji::CSwordGenji(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	ZeroMemory(m_matBones, sizeof(_mat*) * Bone_End);
}

CSwordGenji::CSwordGenji(const CSwordGenji & rhs)
	: CGameObject(rhs)
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

	m_pNavMesh->Ready_NaviMesh(m_pGraphic_Dev, L"Navmesh_StageBase.dat");
	m_pNavMesh->Set_SubsetIndex(0);

	Ready_Weapon();
	Ready_BoneMatrix();
	Ready_Collider();
	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 3.f;

	m_pTransformCom->Set_Pos(_v3(1.f, 0.f, 1.f));
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));





	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create(true);

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", 100);
	pBlackBoard->Set_Value(L"MAXHP", 100);
	pBlackBoard->Set_Value(L"HPRatio", 100);
	pBlackBoard->Set_Value(L"Show", true);

	//CBT_Selector* Start_Sel = Node_Selector("행동 시작"); // 찐
	CBT_Sequence* Start_Sel = Node_Sequence("행동 시작"); // 테스트
	CBT_UpdatePos* UpdatePlayerPosService = Node_UpdatePos("Update_Player_Pos", L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE)), 0, 0.01, 0, CBT_Service_Node::Infinite);
	CBT_UpdateGageRatio* UpdatePlayerHPservice = Node_UpdateGageRatio("Update_Player_Pos", L"HPRatio", L"MaxHP", L"HP", 0, 0.01, 0, CBT_Service_Node::Infinite);
	CBT_UpdateGageRatio* UpdateHPRatioService = Node_UpdateGageRatio("체력 비율", L"HPRatio", L"MAXHP", L"HP", 1, 0.01, 0, CBT_Service_Node::Infinite);

	pBehaviorTree->Set_Child(Start_Sel);

	Start_Sel->Add_Service(UpdatePlayerPosService);
	Start_Sel->Add_Service(UpdatePlayerHPservice);
	Start_Sel->Add_Service(UpdateHPRatioService);

	//CBT_CompareValue* Check_ShowValue = Node_BOOL_A_Equal_Value("시연회 변수 체크", L"Show", true);
	//Check_ShowValue->Set_Child(Start_Show());
	//Start_Sel->Add_Child(Check_ShowValue);
	Start_Sel->Add_Child(Start_Game());
	//Start_Sel->Add_Child(Normal_Cut1());


	//CBT_Wait* Wait1 = Node_Wait("RushWait1", 0.5, 0);
	//Start_Sel->Add_Child(Wait1);

	//Start_Sel->Add_Child(Throwing_BackStep());


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

	m_pMeshCom->SetUp_Animation(Ani_Idle);

	return NOERROR;
}

_int CSwordGenji::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	// 죽었을 경우
	if (m_bIsDead)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			return DEAD_OBJ;
		}
	}
	else
	{
		if (true == m_bAIController)
			m_pAIControllerCom->Update_AIController(TimeDelta);

		Check_Collider();
	}

	// 네비메쉬 태우기
	if (-1 == m_pNavMesh->Get_CellIndex())
	{
		// 안탔을 경우
	}
	else
		m_pTransformCom->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	return _int();
}

_int CSwordGenji::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

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
			m_pShaderCom->Begin_Pass(0);

			if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE))))
				return E_FAIL;

			m_pShaderCom->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			m_pShaderCom->End_Pass();
		}
	}

	m_pShaderCom->End_Shader();

	m_pSword->Update_GameObject(m_dTimeDelta);
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

CBT_Composite_Node * CSwordGenji::Normal_Cut1()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("일반 베기1");
	CBT_Play_Ani* Show_Ani41 = Node_Ani("일반 베기1", 41, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 1.1, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 10, 0.1, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani41);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateCollider* pHitCol = Node_UpdateCollider("무기 히트 On", m_pSword->Get_pTarget_Param(), 1.2, 1, 0.2, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Normal_Cut2()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("일반 베기2");
	CBT_Play_Ani* Show_Ani40 = Node_Ani("일반 베기2", 40, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 1.1, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 10, 0.1, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani40);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateCollider* pHitCol = Node_UpdateCollider("무기 히트 On", m_pSword->Get_pTarget_Param(), 1, 1, 0.2, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Normal_Cut3()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("일반 베기3");
	CBT_Play_Ani* Show_Ani39 = Node_Ani("일반 베기3", 39, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.4, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 2.5, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani39);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateCollider* pHitCol = Node_UpdateCollider("무기 히트 On", m_pSword->Get_pTarget_Param(), 0.7, 1, 0.3, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Strong_RightCut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("강하게 오른쪽으로 베기");
	CBT_Play_Ani* Show_Ani28 = Node_Ani("강하게 오른쪽으로 베기", 28, 0.9f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 2.5, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani28);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(Move0);

	CBT_UpdateCollider* pHitCol = Node_UpdateCollider("무기 히트 On", m_pSword->Get_pTarget_Param(), 0.45, 1, 0.2, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Strong_LeftCut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("강하게 왼쪽으로 베기");
	CBT_Play_Ani* Show_Ani29 = Node_Ani("강하게 왼쪽으로 베기", 29, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 2.5, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani29);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(Move0);

	CBT_UpdateCollider* pHitCol = Node_UpdateCollider("무기 히트 On", m_pSword->Get_pTarget_Param(), 0.55, 1, 0.35, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::ThreeCombo_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("3연속 베기");
	CBT_Play_Ani* Show_Ani37 = Node_Ani("3연속 베기", 37, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.2, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 2.5, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani37);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateCollider* pHitCol0 = Node_UpdateCollider("무기 히트 On", m_pSword->Get_pTarget_Param(), 0.7, 1, 0.2, 0);
	CBT_UpdateCollider* pHitCol1 = Node_UpdateCollider("무기 히트 On", m_pSword->Get_pTarget_Param(), 1.1, 1, 0.2, 0);
	CBT_UpdateCollider* pHitCol2 = Node_UpdateCollider("무기 히트 On", m_pSword->Get_pTarget_Param(), 1.5, 1, 0.2, 0);
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
	CBT_Play_Ani* Show_Ani34 = Node_Ani("달리기 시작", 34, 0.7f);
	CBT_Play_Ani* Show_Ani35 = Node_Ani("달리기", 35, 0.95f);
	CBT_Play_Ani* Show_Ani36 = Node_Ani("베기", 36, 0.8f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	//서브 병렬
	CBT_Sequence* MoveSeq = Node_Sequence("돌진");
	CBT_Wait* RunWaitF = Node_Wait("RunWait0", 0.2, 0);
	CBT_MoveDirectly* Move = Node_MoveDirectly_Rush("돌진", 10, 0.7, 0);
	CBT_Wait* RunWaitB = Node_Wait("RunWait1", 0.6, 0);

	Root_Parallel->Set_Main_Child(Run_Cut_Seq);
	Run_Cut_Seq->Add_Child(Show_Ani34);
	Run_Cut_Seq->Add_Child(Show_Ani35);
	Run_Cut_Seq->Add_Child(Show_Ani36);
	Run_Cut_Seq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(MoveSeq);
	MoveSeq->Add_Child(RunWaitF);
	MoveSeq->Add_Child(Move);
	MoveSeq->Add_Child(RunWaitB);

	CBT_UpdateCollider* pHitCol = Node_UpdateCollider("무기 히트 On", m_pSword->Get_pTarget_Param(), 0.95, 1, 0.7, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Start_Game()
{
	CBT_Selector* Root_Seq = Node_Selector("게임 시작");

	CBT_DistCheck* DistCheck0 = Node_DistCheck("거리 체크", L"Player_Pos", 2);
	CBT_DistCheck* DistCheck1 = Node_DistCheck("거리 체크", L"Player_Pos", 5);
	CBT_DistCheck* DistCheck2 = Node_DistCheck("거리 체크", L"Player_Pos", 7);

	Root_Seq->Add_Child(DistCheck0);
	DistCheck0->Set_Child(RotationAndNearAttack());

	Root_Seq->Add_Child(DistCheck1);
	DistCheck1->Set_Child(ChaseAndNearAttack());

	Root_Seq->Add_Child(DistCheck2);
	DistCheck2->Set_Child(LookPlayer_FarAttack());

	Root_Seq->Add_Child(Chase());

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

	Root_Seq->Add_Child(Chase());
	Root_Seq->Add_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordGenji::Chase()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_MoveDirectly* pChase = Node_MoveDirectly_Chase("추적", L"Player_Pos", 3.f, 2.f);

	CBT_Play_Ani* Show_Ani139 = Node_Ani("추적", 139, 1.f);

	Root_Parallel->Set_Main_Child(pChase);

	Root_Parallel->Set_Sub_Child(Show_Ani139);

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
	Root_Sel->Add_Child(Horizon_Cut());
	Root_Sel->Add_Child(Sting_Attack());
	Root_Sel->Add_Child(TwoCombo_Cut());

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

CBT_Composite_Node * CSwordGenji::Horizon_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("수평베기");
	CBT_Play_Ani* Show_Ani32 = Node_Ani("수평베기", 32, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.3, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 2.5, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani32);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateCollider* pHitCol = Node_UpdateCollider("무기 히트 On", m_pSword->Get_pTarget_Param(), 0.8, 1, 0.3, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Sting_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("찌르기");
	CBT_Play_Ani* Show_Ani31 = Node_Ani("찌르기", 31, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 1.0, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 2.5, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani31);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateCollider* pHitCol = Node_UpdateCollider("무기 히트 On", m_pSword->Get_pTarget_Param(), 1.5, 1, 0.2, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::TwoCombo_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("2연속 베기");
	CBT_Play_Ani* Show_Ani30 = Node_Ani("2연속 베기", 30, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.2, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 2.5, 0.4, 0);
	CBT_Wait* Wait1 = Node_Wait("대기", 0.25, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동", 2.5, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani30);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateCollider* pHitCol0 = Node_UpdateCollider("무기 히트 On", m_pSword->Get_pTarget_Param(), 0.7, 1, 0.25, 0);
	CBT_UpdateCollider* pHitCol1 = Node_UpdateCollider("무기 히트 On", m_pSword->Get_pTarget_Param(), 1.25, 1, 0.2, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Throwing1()
{
	CBT_Sequence* Root_Seq = Node_Sequence("던지기 1");

	CBT_Play_Ani* Show_Ani26 = Node_Ani("던지기 1", 26, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	Root_Seq->Add_Child(Show_Ani26);
	Root_Seq->Add_Child(Show_Ani42);

	return Root_Seq;
}

CBT_Composite_Node * CSwordGenji::Throwing2()
{
	CBT_Sequence* Root_Seq = Node_Sequence("던지기 2");

	CBT_Play_Ani* Show_Ani26 = Node_Ani("던지기 2", 27, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	Root_Seq->Add_Child(Show_Ani26);
	Root_Seq->Add_Child(Show_Ani42);

	return Root_Seq;
}

CBT_Composite_Node * CSwordGenji::Throwing_BackStep()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("백스탭 던지기");
	CBT_Play_Ani* Show_Ani33 = Node_Ani("백스탭 던지기", 33, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 1.0, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", -2.5, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani33);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Show_ChaseAndNeqrAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("공격 또는 추적");
	CBT_MoveDirectly* Chase = Node_MoveDirectly_Chase("추적", L"Player_Pos", 3.f, 2.f);

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
	Cool7->Set_Child(Horizon_Cut());
	Root_Sel->Add_Child(Cool8);
	Cool8->Set_Child(Sting_Attack());
	Root_Sel->Add_Child(Cool9);
	Cool9->Set_Child(TwoCombo_Cut());

	return Root_Sel;
}

HRESULT CSwordGenji::Update_Bone_Of_BlackBoard()
{
	return E_NOTIMPL;
}

HRESULT CSwordGenji::Update_Value_Of_BB()
{
	return E_NOTIMPL;
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

	return S_OK;
}


void CSwordGenji::Check_Collider()
{
	// 충돌처리, bCanHit를 무기가 false시켜줄것임.
	if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		m_pMeshCom->Reset_OldIndx();	//애니 인덱스 초기화

		m_tObjParam.fHp_Cur -= 0.99f;	// 체력 임의로 닳게 만듦.

		m_bAIController = false;
		cout << "나도 부딪힘 ^^" << endl;
		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		m_pAIControllerCom->Reset_BT();

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			m_pMeshCom->SetUp_Animation(Ani_Dmg01_FL);	//방향에 따른 모션 해줘야함.

			// 디졸브 시작
		}
		else
		{
			m_pMeshCom->SetUp_Animation(Ani_Death);	// 죽음처리 시작
			m_bIsDead = true;
		}
	}
	else
	{
		// 맞았을 아래 태울것임
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_bAIController = true;
			m_tObjParam.bIsHit = false;

			//m_pMeshCom->SetUp_Animation(Ani_Idle);
		}

		else if (m_pMeshCom->Is_Finish_Animation(0.7f))	// 이때부터 재충돌 가능
		{
			m_tObjParam.bIsHit = false;
		}
	}

}

HRESULT CSwordGenji::Draw_Collider()
{
	for (auto& iter : m_vecPhysicCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	return S_OK;
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

	_tchar name[256] = { 0, };
	INFO eTemp = *(INFO*)pArg;

	if (nullptr == pArg)
		lstrcpy(name, L"Mesh_NormalGenji");
	else
	{
		switch (eTemp.eType)
		{
		case CSwordGenji::Jungle:
			lstrcpy(name, L"Mesh_JungleGenji");
			break;
		case CSwordGenji::Normal:
			lstrcpy(name, L"Mesh_NormalGenji");
			break;
		case CSwordGenji::White:
			lstrcpy(name, L"Mesh_WhiteGenji");
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;


	return NOERROR;
}

HRESULT CSwordGenji::SetUp_ConstantTable()
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

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CSwordGenji::Ready_Weapon()
{
	m_pSword = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pSword->Change_WeaponData(CWeapon::WPN_SSword_Normal);

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

HRESULT CSwordGenji::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;

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
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pSword);
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
