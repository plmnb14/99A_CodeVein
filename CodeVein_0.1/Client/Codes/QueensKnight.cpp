#include "stdafx.h"
#include "..\Headers\QueensKnight.h"
#include "..\Headers\Weapon.h"


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

	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 150.f;
	m_tObjParam.fHp_Max = m_tObjParam.fHp_Cur;
	m_tObjParam.fDamage = 20.f;

	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));



	///////////////// 행동트리 init

	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create();	//인자에 true 주면 콘솔창에 디버깅정보 뜸, default = false

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", m_tObjParam.fHp_Cur);
	pBlackBoard->Set_Value(L"MAXHP", m_tObjParam.fHp_Max);
	pBlackBoard->Set_Value(L"HPRatio", 100);
	pBlackBoard->Set_Value(L"Show", true);
	pBlackBoard->Set_Value(L"Show_Near", true);

	pBlackBoard->Set_Value(L"PushCol", true);	// 충돌여부 제어변수
	pBlackBoard->Set_Value(L"PhyCol", true); // 피격판정 제어 변수

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

	Start_Sel->Add_Child(Start_Game());

	//CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.2);
	//Start_Sel->Add_Child(Rotation0);

	//CBT_Wait* Wait0 = Node_Wait("대기", 1, 0);
	//Start_Sel->Add_Child(Wait0);

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
		return DEAD_OBJ;

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

	if (false == m_bReadyDead && true == m_pAIControllerCom->Get_BoolValue(L"PhyCol"))
		Check_PhyCollider();

	OnCollisionEnter();

	return NOERROR;
}

_int CQueensKnight::Late_Update_GameObject(_double TimeDelta)
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

	m_pSword->Update_GameObject(m_dTimeDelta);
	m_pShield->Update_GameObject(m_dTimeDelta);
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CQueensKnight::Render_GameObject_SetPass(CShader * pShader, _int iPass)
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

CBT_Composite_Node * CQueensKnight::Normal_HorizontalCut1()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("일반 가로베기1");
	CBT_Play_Ani* Show_Ani50 = Node_Ani("일반 가로베기1", 50, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.166, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.567, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 7.f, 0.333, 0);


	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani50);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.866, 1, 0.317, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Normal_VerticalCut1()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("일반 세로베기1");
	CBT_Play_Ani* Show_Ani49 = Node_Ani("일반 세로베기1", 49, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.333, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 6, 0.4, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 1.117, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -2, 0.216, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.15, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 1, 0.484, 0);


	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani49);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.75, 1, 0.15, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::TwoCombo_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("2연속 베기");
	CBT_Play_Ani* Show_Ani50 = Node_Ani("일반 가로베기1", 50, 0.5);
	CBT_Play_Ani* Show_Ani48 = Node_Ani("일반 세로베기", 48, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.166, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.567, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 7.f, 0.333, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.45, 0);
	//50번애니 * 0.5 = 1.616
	CBT_RotationDir* Rotation1 = Node_RotationDir("플레이어 바라보기", L"Player_Pos", 0.09);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("플레이어 바라보기", L"Player_Pos", 0.01, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.15, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 6.f, 0.366, 0);
	CBT_Wait* Wait3 = Node_Wait("대기3", 0.934, 0);
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("이동3", L"Monster_Speed", L"Monster_Dir", -1.f, 0.483, 0);
	CBT_MoveDirectly* Move4 = Node_MoveDirectly_Rush("이동4", L"Monster_Speed", L"Monster_Dir", 1.f, 0.5, 0);


	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani50);
	MainSeq->Add_Child(Show_Ani48);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Rotation1);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(Wait3);
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
	CBT_Play_Ani* Show_Ani50 = Node_Ani("일반 가로베기1", 50, 0.5);
	CBT_Play_Ani* Show_Ani48 = Node_Ani("일반 세로베기", 48, 0.4f);
	CBT_Play_Ani* Show_Ani45 = Node_Ani("방패치기", 45, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.166, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.567, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 7.f, 0.333, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.45, 0);
	//50번애니 * 0.5 = 1.616
	CBT_RotationDir* Rotation1 = Node_RotationDir("플레이어 바라보기1", L"Player_Pos", 0.09);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("플레이어 바라보기", L"Player_Pos", 0.01, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.15, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 6.f, 0.366, 0);
	CBT_Wait* Wait3 = Node_Wait("대기3", 0.857, 0);
	CBT_RotationDir* Rotation2 = Node_RotationDir("플레이어 바라보기2", L"Player_Pos", 0.09);
	CBT_ChaseDir* ChaseDir1 = Node_ChaseDir("플레이어 바라보기1", L"Player_Pos", 0.01, 0);
	//48번애니 * 0.4 = 1.473
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("이동3", L"Monster_Speed", L"Monster_Dir", 6.f, 0.45, 0);



	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani50);
	MainSeq->Add_Child(Show_Ani48);
	MainSeq->Add_Child(Show_Ani45);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Rotation1);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(Wait3);
	SubSeq->Add_Child(Rotation2);
	SubSeq->Add_Child(ChaseDir1);
	SubSeq->Add_Child(Move3);

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
	CBT_Play_Ani* Show_Ani55 = Node_Ani("백스텝 베기", 55, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.1f);
	
	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("대기", 0.866, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", -8, 0.484, 0);



	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani55);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.65, 1, 0.25, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Sting()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("찌르기");
	CBT_Play_Ani* Show_Ani42 = Node_Ani("찌르기", 42, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.366, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 6, 0.317, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.983, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -1, 0.317, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 1, 0.35, 0);


	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani42);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
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
	CBT_Play_Ani* Show_Ani43 = Node_Ani("돌면서 날개치기", 43, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.05, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", -6, 0.25, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.85, 0);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation1 = Node_RotationDir("방향 추적2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 8, 0.483, 0);


	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani43);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 1.533, 1, 0.45, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Rush()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("돌진 찌르기");
	CBT_Play_Ani* Show_Ani42 = Node_Ani("돌진 찌르기", 42, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.276, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적2", L"Player_Pos", 0.2);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 30, 0.307, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.983, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -1, 0.317, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 1, 0.35, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani42);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(PushColOn);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.456, 1, 0.36, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Flash()
{
	//CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("이동");

	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("이동", L"FlashPos", 0.2);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOff", L"PhyCol", true);

	MainSeq->Add_Child(PhyColOff);
	MainSeq->Add_Child(MoveTo0);
	MainSeq->Add_Child(PhyColOn);

	

	//CBT_StartDissolve* Dissolve0 = Node_StartDissolve("디졸브", 0.7f, false, 0.01);
	//MainSeq->Add_Service(Dissolve0);

	//CBT_StartDissolve* Dissolve1 = Node_StartDissolve("디졸브", 0.7f, true, 0.15);
	//MainSeq->Add_Service(Dissolve1);

	return MainSeq;
}

CBT_Composite_Node * CQueensKnight::Flash_Wing_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("돌면서 날개치기");
	CBT_Play_Ani* Show_Ani43 = Node_Ani("돌면서 날개치기", 43, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.15, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", -6, 0.25, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.75, 0);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("점멸 이동", L"FlashPos", 0.1);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOff", L"PhyCol", true);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 8, 0.483, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani43);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(PhyColOn);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 1.533, 1, 0.45, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Flash_Rush()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("돌진 찌르기");
	CBT_Play_Ani* Show_Ani42 = Node_Ani("돌진 찌르기", 42, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.176, 0);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("점멸 이동", L"FlashPos", 0.1);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOff", L"PhyCol", true);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 30, 0.307, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.983, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -1, 0.317, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 1, 0.35, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);



	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani42);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(PhyColOn);
	SubSeq->Add_Child(Rotation0);
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
	CBT_Play_Ani* Show_Ani38 = Node_Ani("점프 시작", 38, 0.7f);
	CBT_Play_Ani* Show_Ani40 = Node_Ani("내려 찍기", 40, 0.9f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.871, 0);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("점멸 이동", L"FlashPos", 0.05);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOff", L"PhyCol", true);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적2", L"Player_Pos", 0.1);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani38);
	MainSeq->Add_Child(Show_Ani40);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(MoveTo0);
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
	CBT_Play_Ani* Show_Ani37 = Node_Ani("점멸 베기", 37, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("기본", 15, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.733, 0);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("점멸 이동", L"FlashPos", 0.1);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOff", L"PhyCol", true);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("방향 추적2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 5.f, 0.267, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 1.55, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -0.3f, 0.667, 0);



	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani37);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(PhyColOn);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.283, 1, 0.133, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
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
	Dist0->Set_Child(NearAttack_Dist3_More_Than_HP70());

	Root_Sel->Add_Child(FarAttack_More_Than_HP70());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::NearAttack_Dist3_More_Than_HP70()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	Root_Sel->Add_Child(Normal_HorizontalCut1());
	Root_Sel->Add_Child(Normal_VerticalCut1());
	Root_Sel->Add_Child(TwoCombo_Cut());
	Root_Sel->Add_Child(ThreeCombo_Cut());
	Root_Sel->Add_Child(BackStep_Cut());
	Root_Sel->Add_Child(Sting());
	Root_Sel->Add_Child(Wing_Attack());

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

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 3);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_More_Than_HP40());

	Root_Sel->Add_Child(FarAttack_More_Than_HP40());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::NearAttack_Dist5_More_Than_HP40()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	Root_Sel->Add_Child(Normal_HorizontalCut1());
	Root_Sel->Add_Child(Normal_VerticalCut1());
	Root_Sel->Add_Child(TwoCombo_Cut());
	Root_Sel->Add_Child(ThreeCombo_Cut());
	Root_Sel->Add_Child(BackStep_Cut());
	Root_Sel->Add_Child(Sting());
	Root_Sel->Add_Child(Wing_Attack());
	Root_Sel->Add_Child(Flash());

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

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 3);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist3_Final());

	Root_Sel->Add_Child(FarAttack_Fianl());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::NearAttack_Dist3_Final()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	Root_Sel->Add_Child(Normal_HorizontalCut1());
	Root_Sel->Add_Child(Normal_VerticalCut1());
	Root_Sel->Add_Child(TwoCombo_Cut());
	Root_Sel->Add_Child(ThreeCombo_Cut());
	Root_Sel->Add_Child(BackStep_Cut());
	Root_Sel->Add_Child(Sting());
	Root_Sel->Add_Child(Wing_Attack());
	Root_Sel->Add_Child(Flash());
	Root_Sel->Add_Child(Flash());
	Root_Sel->Add_Child(Flash());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::FarAttack_Fianl()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Flash_Rush());
	Root_Sel->Add_Child(Flash_Jump_Attack());
	Root_Sel->Add_Child(Flash_Cut());
	Root_Sel->Add_Child(Flash());

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

	return nullptr;
}

CBT_Composite_Node * CQueensKnight::Show_ChaseAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("추적 후 순서대로 공격");
	CBT_Play_Ani* Show_Ani14 = Node_Ani("추적모션", 14, 0.05f);
	CBT_MoveDirectly* Chase = Node_MoveDirectly_Chase("추적", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 5.f, 3.f);
	CBT_RotationDir* Rotation0 = Node_RotationDir("플레이어 바라보기", L"Player_Pos", 0.2);

	Root_Seq->Add_Child(Show_Ani14);
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
	CBT_Cooldown* Cool7 = Node_Cooldown("쿨7", 300);

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

HRESULT CQueensKnight::Update_Bone_Of_BlackBoard()
{
	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine3_WingB4");
	m_vWing = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Bone_Wing", m_vWing);

	
	return S_OK;
}

HRESULT CQueensKnight::Update_Value_Of_BB()
{
	// 1. 플레이어 좌표 업데이트
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos());
	// 2. 체력 업데이트
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"HP", m_tObjParam.fHp_Cur);
	// 3. 체력 비율 업데이트
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"HPRatio", _float(m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max) * 100);


	// 1. 점멸을 위한 플레이어 4방 위치.
	_mat matPlayer = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_WorldMat();
	_v3 vPlayerPos = *(_v3*)&matPlayer.m[3];
	_v3 vPlayerLook = *D3DXVec3Normalize(&_v3(), (_v3*)&matPlayer.m[2]);
	_v3 vPlayerRight = *D3DXVec3Normalize(&_v3(), (_v3*)&matPlayer.m[0]);

	_float fLength = 3.f;

	switch (CALC::Random_Num(0, 3))
	{
	case 0:
		m_pAIControllerCom->Set_Value_Of_BloackBoard(L"FlashPos", vPlayerPos + fLength * vPlayerLook);
		break;
	case 1:
		m_pAIControllerCom->Set_Value_Of_BloackBoard(L"FlashPos", vPlayerPos - fLength * vPlayerLook);
		break;
	case 2:
		m_pAIControllerCom->Set_Value_Of_BloackBoard(L"FlashPos", vPlayerPos + fLength * vPlayerRight);
		break;
	case 3:
		m_pAIControllerCom->Set_Value_Of_BloackBoard(L"FlashPos", vPlayerPos - fLength * vPlayerRight);
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CQueensKnight::Update_NF()
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

	m_pCollider->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pCollider->Get_Radius().y, 0.f));
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
			Start_Dissolve(0.7f, false, true);
			//g_pManagement->Create_Spawn_Effect(m_pTransformCom->Get_Pos());
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
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE));


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

					if (false == iter->Get_Target_Dodge())
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

HRESULT CQueensKnight::SetUp_ConstantTable()
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

HRESULT CQueensKnight::Ready_Weapon()
{
	// 오른손 무기
	m_pSword = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pSword->Change_WeaponData(CWeapon::WPN_QueenLance);

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pSword->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pSword->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	// 왼손 방패
	m_pShield = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pShield->Change_WeaponData(CWeapon::WPN_QueenShield);

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
	Safe_Release(m_pCollider);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}


