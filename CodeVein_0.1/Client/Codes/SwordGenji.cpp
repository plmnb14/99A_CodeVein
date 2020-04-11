#include "stdafx.h"
#include "..\Headers\SwordGenji.h"
#include "..\Headers\Weapon.h"

#include "MonsterUI.h"
#include "DamegeNumUI.h"
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

	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 100.f;
	m_tObjParam.fHp_Max = m_tObjParam.fHp_Cur;

	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	// MonsterHP UI
 	pMonsterHpUI = static_cast<CMonsterUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MonsterHPUI", pArg));
 	pMonsterHpUI->Set_Target(this);
	pMonsterHpUI->Set_Bonmatrix(m_matBones[Bone_Head]);
 	pMonsterHpUI->Ready_GameObject(pArg);

	/*m_pDamegeNumUI = static_cast<CDamegeNumUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_DamegeNumUI", pArg));
	m_pDamegeNumUI->Set_Target(this);
	m_pDamegeNumUI->Ready_GameObject(NULL);*/

	//m_pGet_Item = static_cast<CGet_ItemUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Get_ItemUI", pArg));
	//m_pGet_Item->Set_ItemTarget(this);
	//m_pGet_Item->Ready_GameObject(pArg);


	//////////////////// 행동트리 init

	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create();

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", m_tObjParam.fHp_Cur);
	pBlackBoard->Set_Value(L"MAXHP", m_tObjParam.fHp_Max);
	pBlackBoard->Set_Value(L"HPRatio", 100);
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
	//Start_Sel->Add_Child(Left_Cut());

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

	// MonsterHP UI
	pMonsterHpUI->Update_GameObject(TimeDelta);
	//m_pDamegeNumUI->Update_GameObject(TimeDelta);
	//m_pGet_Item->Update_GameObject(TimeDelta);

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

	if (false == m_bReadyDead)
		Check_PhyCollider();

	// 네비매쉬로 y올려줘야함

	return NOERROR;
}

_int CSwordGenji::Late_Update_GameObject(_double TimeDelta)
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
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CSwordGenji::Render_GameObject_SetPass(CShader* pShader, _int iPass)
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

CBT_Composite_Node * CSwordGenji::Normal_Cut1()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("일반 베기1");
	CBT_Play_Ani* Show_Ani41 = Node_Ani("일반 베기1", 41, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.366, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 1.f, 0.584, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 2.f, 0.353, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 1.084, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 0.6f, 0.633, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani41);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.167, 1, 0.233, 0);
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
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 0.5f, 1.033, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2.f, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani40);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.05, 1, 0.283, 0);
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
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.087, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.583, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.617, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -0.6f, 0.533, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.417, 0);
	//CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 0.6f, 0.6, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 0.6f, 0.65, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani39);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
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
	CBT_Play_Ani* Show_Ani28 = Node_Ani("강하게 오른쪽으로 베기", 28, 0.9f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.167, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani28);
	MainSeq->Add_Child(Show_Ani42);


	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);


	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.483, 1, 0.167, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Strong_LeftCut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("강하게 왼쪽으로 베기");
	CBT_Play_Ani* Show_Ani29 = Node_Ani("강하게 왼쪽으로 베기", 29, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.15, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.483, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.584, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -0.6f, 0.35, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.416, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 0.6f, 0.704, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani29);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
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
	CBT_Play_Ani* Show_Ani37 = Node_Ani("3연속 베기", 37, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.333, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2, 0.417, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani37);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

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
	CBT_Play_Ani* Show_Ani34 = Node_Ani("달리기 시작", 34, 0.7f);
	CBT_Play_Ani* Show_Ani35 = Node_Ani("달리기", 35, 0.95f);
	CBT_Play_Ani* Show_Ani36 = Node_Ani("베기", 36, 0.8f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	//서브 병렬
	CBT_Sequence* MoveSeq = Node_Sequence("돌진");
	CBT_Wait* RunWaitF = Node_Wait("RunWait0", 0.367, 0);
	CBT_MoveDirectly* Move = Node_MoveDirectly_Rush("돌진", L"Monster_Speed", L"Monster_Dir", 6.4f, 0.7, 0);

	Root_Parallel->Set_Main_Child(Run_Cut_Seq);
	Run_Cut_Seq->Add_Child(Show_Ani34);
	Run_Cut_Seq->Add_Child(Show_Ani35);
	Run_Cut_Seq->Add_Child(Show_Ani36);
	Run_Cut_Seq->Add_Child(Show_Ani42);

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

CBT_Composite_Node * CSwordGenji::Horizon_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("수평베기");
	CBT_Play_Ani* Show_Ani32 = Node_Ani("수평베기", 32, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.417, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2, 0.416, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani32);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.9, 1, 0.317, 0);
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
	CBT_Wait* Wait0 = Node_Wait("대기", 0.917, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2, 0.716, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani31);
	MainSeq->Add_Child(Show_Ani42);

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
	CBT_Play_Ani* Show_Ani30 = Node_Ani("2연속 베기", 30, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.3, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2.5f, 0.433, 0);
	CBT_Wait* Wait1 = Node_Wait("대기", 0.3, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2.5f, 0.317, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani30);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

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
	CBT_Play_Ani* Show_Ani38 = Node_Ani("왼쪽으로 베기", 38, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.983, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2.f, 0.284, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 2.4f, 0.466, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 0.8f, 0.45, 0);
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 0.6f, 1.467, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani38);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(Move3);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.35, 1, 0.2, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Throwing1()
{
	CBT_Sequence* Root_Seq = Node_Sequence("옆에서 던지기");

	CBT_Play_Ani* Show_Ani26 = Node_Ani("옆에서 던지기", 26, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_CreateBullet* Bullet0 = Node_CreateBullet("수리검", L"Monster_SwordGenjiBullet", L"Bone_LeftHandAttach", L"ShotDir", 30, 5, 1.7, 1, 1, 0, CBT_Service_Node::Finite);

	Root_Seq->Add_Service(Bullet0);

	Root_Seq->Add_Child(Show_Ani26);
	Root_Seq->Add_Child(Show_Ani42);

	return Root_Seq;
}

CBT_Composite_Node * CSwordGenji::Throwing2()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("앞으로 던지기");
	CBT_Play_Ani* Show_Ani27 = Node_Ani("앞으로 던지기", 27, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 1.45, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.267, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 1.133, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -1.f, 0.333, 0);


	CBT_CreateBullet* Bullet0 = Node_CreateBullet("수리검", L"Monster_SwordGenjiBullet", L"Bone_LeftHandAttach", L"ShotDir", 30, 5, 1.733, 1, 0, 0, CBT_Service_Node::Finite);

	Root_Parallel->Add_Service(Bullet0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani27);
	MainSeq->Add_Child(Show_Ani42);

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
	CBT_Play_Ani* Show_Ani33 = Node_Ani("백스탭 던지기", 33, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.933, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", -2.5, 0.6, 0);

	CBT_CreateBullet* Bullet0 = Node_CreateBullet("수리검", L"Monster_SwordGenjiBullet", L"Bone_LeftHandAttach", L"ShotDir", 30, 5, 1, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Bullet0);

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
	Cool7->Set_Child(Horizon_Cut());
	Root_Sel->Add_Child(Cool8);
	Cool8->Set_Child(Sting_Attack());
	Root_Sel->Add_Child(Cool9);
	Cool9->Set_Child(TwoCombo_Cut());

	return Root_Sel;
}

HRESULT CSwordGenji::Update_Bone_Of_BlackBoard()
{
	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHandAttach");
	m_vLeftHand = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Bone_LeftHandAttach", m_vLeftHand);

	return S_OK;
}

HRESULT CSwordGenji::Update_Value_Of_BB()
{
	// 1. 플레이어 좌표 업데이트
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos());
	// 2. 체력 업데이트
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"HP", m_tObjParam.fHp_Cur);


	// 1. 평상시 총 발사 방향
	_mat matGunDir = m_pTransformCom->Get_WorldMat();
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"ShotDir", _v3(matGunDir.m[2][0], matGunDir.m[2][1], matGunDir.m[2][2]));


	return S_OK;
}

HRESULT CSwordGenji::Update_NF()
{
	// 플레이어 발견 못 했을 때
	if (false == m_bFindPlayer)
	{
		// 플레이어 좌표 구함.
		_v3 vPlayer_Pos = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos();

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

	m_pCollider->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pCollider->Get_Radius().y, 0.f));

	return S_OK;
}

void CSwordGenji::Skill_Movement(_float _fspeed, _v3 _vDir)
{
	_v3 tmpLook;
	_float fSpeed = _fspeed;

	tmpLook = _vDir;
	D3DXVec3Normalize(&tmpLook, &tmpLook);

	// 네비게이션 적용하면 
	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &tmpLook, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));
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
		m_pMeshCom->Reset_OldIndx();	//애니 인덱스 초기화

		m_bAIController = false;

		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		m_bFight = true;	// 싸움 시작

		m_fSkillMoveSpeed_Cur = 4.f;
		m_fSkillMoveAccel_Cur = 0.f;
		m_fSkillMoveMultiply = 0.5f;

		// 맞을때 플레이어의 룩을 받아와서 그 방향으로 밈.
		m_vPushDir_forHitting = (*(_v3*)&TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_WorldMat().m[2]);

		m_pAIControllerCom->Reset_BT();


		if (m_tObjParam.fHp_Cur > 0.f)
		{
			m_pMeshCom->SetUp_Animation(Ani_Dmg01_FL);	//방향에 따른 모션 해줘야함.

			// 디졸브 시작
		}
		else
		{
			m_pMeshCom->SetUp_Animation(Ani_Death);	// 죽음처리 시작

			Start_Dissolve(0.7f, false, true);
			m_pSword->Start_Dissolve();
			//g_pManagement->Create_Spawn_Effect(m_pTransformCom->Get_Pos());
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
	list<CGameObject*> tmpList[3];

	tmpList[0] = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE);
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
		switch (eTemp.eColor)
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
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShaderCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
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

HRESULT CSwordGenji::Ready_NF(void * pArg)
{
	INFO eTemp = *(INFO*)pArg;

	m_eNF_Ani = eTemp.eNF_Ani;
	m_fFov = eTemp.fFov;
	m_fMaxLength = eTemp.fMaxLength;
	m_fMinLength = eTemp.fMinLength;

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
	Safe_Release(pMonsterHpUI);
	//Safe_Release(m_pDamegeNumUI);
	//Safe_Release(m_pGet_Item);

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
