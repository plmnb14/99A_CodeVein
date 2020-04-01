#include "stdafx.h"
#include "..\Headers\SwordGenji.h"

CSwordGenji::CSwordGenji(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CSwordGenji::CSwordGenji(const CSwordGenji & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSwordGenji::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CSwordGenji::Ready_GameObject(void * pArg)
{

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

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
	//Start_Sel->Add_Child(Start_Game());

	//CBT_Wait* Wait1 = Node_Wait("RushWait1", 0.5, 0);
	//Start_Sel->Add_Child(Wait1);

	//Start_Sel->Add_Child(Throwing_BackStep());

	CBT_RotationDir* TurnDir0 = Node_RotationDir("Look 회전", L"Player_Pos", 0.15);
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

	Start_Sel->Add_Child(Throwing_BackStep());

	////방패 벗겨짐
	//CBT_Play_Ani* pAni0 = Node_Ani("방패 벗겨짐 ", 0, 0.9f);
	//pSequence->Add_Child(pAni0);

	////방패 풀림
	//CBT_Play_Ani* pAni1 = Node_Ani("방패 풀림 ", 1, 0.9f);
	//pSequence->Add_Child(pAni1);

	////방패 틱틱
	//CBT_Play_Ani* pAni2 = Node_Ani("방패 틱틱 ", 2, 0.9f);
	//pSequence->Add_Child(pAni2);

	////가드 시작
	//CBT_Play_Ani* pAni3 = Node_Ani("가드 시작 ", 3, 0.9f);
	//pSequence->Add_Child(pAni3);

	////가드 루프
	//CBT_Play_Ani* pAni4 = Node_Ani("가드 루프 ", 4, 0.9f);
	//pSequence->Add_Child(pAni4);

	////가드 끝
	//CBT_Play_Ani* pAni5 = Node_Ani("가드 끝 ", 5, 0.9f);
	//pSequence->Add_Child(pAni5);

	////가드 달리기
	//CBT_Play_Ani* pAni6 = Node_Ani("가드 달리기 ", 6, 0.9f);
	//pSequence->Add_Child(pAni6);

	////가드 오른쪽 걷기
	//CBT_Play_Ani* pAni7 = Node_Ani("가드 뒤로 걷기 ", 7, 0.9f);
	//pSequence->Add_Child(pAni7);

	////가드 왼쪽 걷기
	//CBT_Play_Ani* pAni8 = Node_Ani("가드 왼쪽 걷기 ", 8, 0.9f);
	//pSequence->Add_Child(pAni8);

	////가드 앞오른쪾 걷기
	//CBT_Play_Ani* pAni9 = Node_Ani("가드 앞오른쪾 걷기 ", 9, 0.9f);
	//pSequence->Add_Child(pAni9);

	////가드 앞왼쪽오른발 걷기
	//CBT_Play_Ani* pAni10 = Node_Ani("가드 앞왼쪽오른발 걷기 ", 10, 0.9f);
	//pSequence->Add_Child(pAni10);

	////가드 앞왼쪽왼발 걷기
	//CBT_Play_Ani* pAni11 = Node_Ani("가드 앞왼쪽왼발 걷기 ", 11, 0.9f);
	//pSequence->Add_Child(pAni11);

	////가드 앞으로 걷기
	//CBT_Play_Ani* pAni12 = Node_Ani("가드 앞으로 걷기 ", 12, 0.9f);
	//pSequence->Add_Child(pAni12);

	////가드 뒤오른쪽오른발 걷기
	//CBT_Play_Ani* pAni13 = Node_Ani("가드 뒤오른쪽오른발 걷기 ", 13, 0.9f);
	//pSequence->Add_Child(pAni13);

	////가드 뒤 오른쪽왼발 걷기
	//CBT_Play_Ani* pAni14 = Node_Ani("가드 뒤 오른쪽왼발 걷기 ", 14, 0.9f);
	//pSequence->Add_Child(pAni14);

	////가드 뒤 왼쪽 걷기
	//CBT_Play_Ani* pAni15 = Node_Ani("가드 뒤 왼쪽 걷기 ", 15, 0.9f);
	//pSequence->Add_Child(pAni15);

	////가드 뒤 걷기
	//CBT_Play_Ani* pAni16 = Node_Ani("가드 뒤 걷기 ", 16, 0.9f);
	//pSequence->Add_Child(pAni16);

	////가드 오른쪽180도
	//CBT_Play_Ani* pAni17 = Node_Ani("가드 오른쪽180도 ", 17, 0.9f);
	//pSequence->Add_Child(pAni17);

	////가드 오른쪽 90도
	//CBT_Play_Ani* pAni18 = Node_Ani("가드 오른쪽 90도 ", 18, 0.9f);
	//pSequence->Add_Child(pAni18);

	////가드 왼쪽 180도
	//CBT_Play_Ani* pAni19 = Node_Ani("가드 왼쪽 180도 ", 19, 0.9f);
	//pSequence->Add_Child(pAni19);

	////가드 왼쪽 90도
	//CBT_Play_Ani* pAni20 = Node_Ani("가드 왼쪽 90도 ", 20, 0.9f);
	//pSequence->Add_Child(pAni20);

	////막고 지르기
	//CBT_Play_Ani* pAni21 = Node_Ani("막고 지르기 ", 21, 0.9f);
	//pSequence->Add_Child(pAni21);

	////방패들고 휘돌아치기
	//CBT_Play_Ani* pAni22 = Node_Ani("방패들고 휘돌아치기 ", 22, 0.9f);
	//pSequence->Add_Child(pAni22);

	////방패로 후려치기
	//CBT_Play_Ani* pAni23 = Node_Ani("방패로 후려치기 ", 23, 0.9f);
	//pSequence->Add_Child(pAni23);

	////방패들고 지르기
	//CBT_Play_Ani* pAni24 = Node_Ani("방패들고 지르기 ", 24, 0.9f);
	//pSequence->Add_Child(pAni24);

	////방패로 치고 어퍼치기
	//CBT_Play_Ani* pAni25 = Node_Ani("방패로 치고 어퍼치기 ", 25, 0.9f);
	//pSequence->Add_Child(pAni25);

	////주머니에서 표창
	//CBT_Play_Ani* pAni26 = Node_Ani("주머니에서 표창 ", 26, 0.9f);
	//pSequence->Add_Child(pAni26);

	////등에서 표창내던지기
	//CBT_Play_Ani* pAni27 = Node_Ani("등에서 표창 내던지기 ", 27, 0.9f);
	//pSequence->Add_Child(pAni27);

	////사선후려베기
	//CBT_Play_Ani* pAni28 = Node_Ani("사선 후려베기 ", 28, 0.9f);
	//pSequence->Add_Child(pAni28);

	////크게 옆구리베기
	//CBT_Play_Ani* pAni29 = Node_Ani("크게 옆구리 베기", 29, 0.9f);
	//pSequence->Add_Child(pAni29);

	////테니스 토스 두번베기
	//CBT_Play_Ani* pAni30 = Node_Ani("테니스 토스 두번베기 ", 30, 0.9f);
	//pSequence->Add_Child(pAni30);

	////힘껏 내려배는척하면서 찌르기
	//CBT_Play_Ani* pAni31 = Node_Ani("힘껏 내려배는척 하면서 찌르기 ", 31, 0.9f);
	//pSequence->Add_Child(pAni31);

	////앉아서 왼쪽옆구리 베기
	//CBT_Play_Ani* pAni32 = Node_Ani("앉아서 왼쪽옆구리베기 ", 32, 0.9f);
	//pSequence->Add_Child(pAni32);

	////백스텝하며 표창
	//CBT_Play_Ani* pAni33 = Node_Ani("백스텝하며 표창 ", 33, 0.9f);
	//pSequence->Add_Child(pAni33);

	////달려가 지르기 시작
	//CBT_Play_Ani* pAni34 = Node_Ani("달려가 지르기 시작 ", 34, 0.9f);
	//pSequence->Add_Child(pAni34);

	////달려가 지르기 루프
	//CBT_Play_Ani* pAni35 = Node_Ani("달려가 지르기 루프 ", 35, 0.9f);
	//pSequence->Add_Child(pAni35);

	////달려가 지르기 끝
	//CBT_Play_Ani* pAni36 = Node_Ani("달려가 지르기 끝 ", 36, 0.9f);
	//pSequence->Add_Child(pAni36);

	////테니스 토스 세번베기
	//CBT_Play_Ani* pAni37 = Node_Ani("테니스 토스 세번베기 ", 37, 0.9f);
	//pSequence->Add_Child(pAni37);

	////휘청거리게 크게 베기
	//CBT_Play_Ani* pAni38 = Node_Ani("휘청거리게 크게 베기 ", 38, 0.9f);
	//pSequence->Add_Child(pAni38);

	////손날치듯이
	//CBT_Play_Ani* pAni39 = Node_Ani("손날치듯이 ", 39, 0.9f);
	//pSequence->Add_Child(pAni39);

	////손등으로 뺨치듯이
	//CBT_Play_Ani* pAni40 = Node_Ani("손등으로 뺨치듯이 ", 40, 0.9f);
	//pSequence->Add_Child(pAni40);

	////미운놈 꿀밤패듯이
	//CBT_Play_Ani* pAni41 = Node_Ani("미운놈 꿀밤패듯이", 41, 0.9f);
	//pSequence->Add_Child(pAni41);

	////총 기본
	//CBT_Play_Ani* pAni42 = Node_Ani("총 기본", 42, 0.9f);
	//pSequence->Add_Child(pAni42);

	////개머리판 치기
	//CBT_Play_Ani* pAni43 = Node_Ani("개머리판 치기 ", 43, 0.9f);
	//pSequence->Add_Child(pAni43);

	////정조준
	//CBT_Play_Ani* pAni44 = Node_Ani("정조준 ", 44, 0.9f);
	//pSequence->Add_Child(pAni44);

	////팔꿈치 치기
	//CBT_Play_Ani* pAni45 = Node_Ani("팔꿈치 치기 ", 45, 0.9f);
	//pSequence->Add_Child(pAni45);

	////한손 총 찌르기
	//CBT_Play_Ani* pAni46 = Node_Ani("한손 총 찌르기 ", 46, 0.9f);
	//pSequence->Add_Child(pAni46);

	////총 왼쪽부터 베기
	//CBT_Play_Ani* pAni47 = Node_Ani("총 왼쪽부터 베기 ", 47, 0.9f);
	//pSequence->Add_Child(pAni47);

	////백덤블링중 2회 쏘기
	//CBT_Play_Ani* pAni48 = Node_Ani("백덤블링중 2회 쏘기 ", 48, 0.9f);
	//pSequence->Add_Child(pAni48);

	////총 쏘기
	//CBT_Play_Ani* pAni49 = Node_Ani("총 쏘기 ", 49, 0.9f);
	//pSequence->Add_Child(pAni49);



	return NOERROR;
}

_int CSwordGenji::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	m_pAIControllerCom->Update_AIController(TimeDelta);

	return _int();
}

_int CSwordGenji::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

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

	return NOERROR;
}

CBT_Composite_Node * CSwordGenji::Normal_Cut1()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("일반 베기1");
	CBT_Play_Ani* Show_Ani41 = Node_Ani("일반 베기1", 41, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 1.1, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 10, 0.1, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani41);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Normal_Cut2()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("일반 베기2");
	CBT_Play_Ani* Show_Ani40 = Node_Ani("일반 베기2", 40, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 1.1, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 10, 0.1, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani40);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Normal_Cut3()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("일반 베기3");
	CBT_Play_Ani* Show_Ani39 = Node_Ani("일반 베기3", 39, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.4, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 2.5, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani39);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Strong_RightCut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("강하게 오른쪽으로 베기");
	CBT_Play_Ani* Show_Ani28 = Node_Ani("강하게 오른쪽으로 베기", 28, 0.9f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 2.5, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani28);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Strong_LeftCut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("강하게 왼쪽으로 베기");
	CBT_Play_Ani* Show_Ani29 = Node_Ani("강하게 왼쪽으로 베기", 29, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 2.5, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani29);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::ThreeCombo_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("3연속 베기");
	CBT_Play_Ani* Show_Ani37 = Node_Ani("3연속 베기", 37, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.2, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 2.5, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani37);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

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
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

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

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Horizon_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("수평베기");
	CBT_Play_Ani* Show_Ani32 = Node_Ani("수평베기", 32, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.3, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 2.5, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani32);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Sting_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("찌르기");
	CBT_Play_Ani* Show_Ani31 = Node_Ani("찌르기", 31, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 1.0, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 2.5, 0.4, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani31);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::TwoCombo_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("2연속 베기");
	CBT_Play_Ani* Show_Ani30 = Node_Ani("2연속 베기", 30, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

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

	return Root_Parallel;
}

CBT_Composite_Node * CSwordGenji::Throwing1()
{
	CBT_Sequence* Root_Seq = Node_Sequence("던지기 1");

	CBT_Play_Ani* Show_Ani26 = Node_Ani("던지기 1", 26, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

	Root_Seq->Add_Child(Show_Ani26);
	Root_Seq->Add_Child(Show_Ani42);

	return Root_Seq;
}

CBT_Composite_Node * CSwordGenji::Throwing2()
{
	CBT_Sequence* Root_Seq = Node_Sequence("던지기 2");

	CBT_Play_Ani* Show_Ani26 = Node_Ani("던지기 2", 27, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

	Root_Seq->Add_Child(Show_Ani26);
	Root_Seq->Add_Child(Show_Ani42);

	return Root_Seq;
}

CBT_Composite_Node * CSwordGenji::Throwing_BackStep()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("백스탭 던지기");
	CBT_Play_Ani* Show_Ani33 = Node_Ani("백스탭 던지기", 33, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

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

HRESULT CSwordGenji::Update_Bone_Of_BlackBoard()
{
	return E_NOTIMPL;
}

HRESULT CSwordGenji::Update_Value_Of_BB()
{
	return E_NOTIMPL;
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

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_NormalGenji", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	// for.Com_AIController
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"AIController", L"Com_AIController", (CComponent**)&m_pAIControllerCom)))
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
	// 검, 총, 방패
	// 

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
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
