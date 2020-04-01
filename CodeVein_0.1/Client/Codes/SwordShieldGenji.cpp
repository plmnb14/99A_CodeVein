#include "stdafx.h"
#include "..\Headers\SwordShieldGenji.h"

CSwordShieldGenji::CSwordShieldGenji(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CSwordShieldGenji::CSwordShieldGenji(const CSwordShieldGenji & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSwordShieldGenji::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CSwordShieldGenji::Ready_GameObject(void * pArg)
{

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(1.f, 0.f, 1.f));
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));


	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create();

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


	Start_Sel->Add_Child(ShortDelay_Sting());

	CBT_RotationDir* TurnDir0 = Node_RotationDir("Look 회전", L"Player_Pos", 4);
	Start_Sel->Add_Child(TurnDir0);

	Start_Sel->Add_Child(Upper_Slash());

	CBT_RotationDir* TurnDir1 = Node_RotationDir("Look 회전", L"Player_Pos", 4);
	Start_Sel->Add_Child(TurnDir1);

	Start_Sel->Add_Child(LongDelay_Sting());

	CBT_RotationDir* TurnDir2 = Node_RotationDir("Look 회전", L"Player_Pos", 4);
	Start_Sel->Add_Child(TurnDir2);

	Start_Sel->Add_Child(Shield_Attack());

	CBT_RotationDir* TurnDir3 = Node_RotationDir("Look 회전", L"Player_Pos", 4);
	Start_Sel->Add_Child(TurnDir3);

	Start_Sel->Add_Child(Turning_Cut());

	CBT_RotationDir* TurnDir4 = Node_RotationDir("Look 회전", L"Player_Pos", 4);
	Start_Sel->Add_Child(TurnDir4);

	Start_Sel->Add_Child(ShortDelay_Sting());

	CBT_RotationDir* TurnDir5 = Node_RotationDir("Look 회전", L"Player_Pos", 4);
	Start_Sel->Add_Child(TurnDir5);

	Start_Sel->Add_Child(Guard(3));

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

_int CSwordShieldGenji::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	m_pAIControllerCom->Update_AIController(TimeDelta);

	return _int();
}

_int CSwordShieldGenji::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

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

CBT_Composite_Node * CSwordShieldGenji::Upper_Slash()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");	
	CBT_Sequence* MainSeq = Node_Sequence("어퍼슬래쉬");
	CBT_Play_Ani* Show_Ani25 = Node_Ani("어퍼슬래쉬", 25, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

	//2.5초 돌질 후 공격;
	CBT_Sequence* SubSeq = Node_Sequence("돌진");
	CBT_Wait* Wait0 = Node_Wait("대기", 1.1, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("돌진", 20, 0.2, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani25);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::LongDelay_Sting()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("긴 딜레이 찌르기");
	CBT_Sequence* MainSeq = Node_Sequence("긴 딜레이 찌르기");
	CBT_Play_Ani* Show_Ani24 = Node_Ani("길게 찌르기", 24, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.5, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 2, 0.5, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani24);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Shield_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("방패치기");
	CBT_Sequence* MainSeq = Node_Sequence("방패치기");
	CBT_Play_Ani* Show_Ani23 = Node_Ani("방패치기", 23, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.5, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 1, 0.5, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani23);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Turning_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("한바퀴 돌면서 베기");
	CBT_Sequence* MainSeq = Node_Sequence("한바퀴 돌면서 베기");
	CBT_Play_Ani* Show_Ani22 = Node_Ani("한바퀴 돌면서 베기", 22, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 1.0, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 4, 0.5, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani22);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::ShortDelay_Sting()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("짧은 딜레이 찌르기");
	CBT_Sequence* MainSeq = Node_Sequence("짧은 딜레이 찌르기");
	CBT_Play_Ani* Show_Ani21 = Node_Ani("찌르기", 21, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.5, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", 2, 0.3, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani21);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Hit_Weakly()
{
	CBT_Sequence* Root_Seq = Node_Sequence("방패들고 약하게 맞음");

	CBT_Play_Ani* Show_Ani2 = Node_Ani("방패들고 약하게 맞음", 2, 0.95f);

	Root_Seq->Add_Child(Show_Ani2);

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::Hit_Strongly()
{
	CBT_Sequence* Root_Seq = Node_Sequence("방패들고 강하게 맞음");

	CBT_Play_Ani* Show_Ani1 = Node_Ani("방패들고 강하게 맞음", 1, 0.95f);

	Root_Seq->Add_Child(Show_Ani1);

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::Guard(_double dGuardTime)
{
	//345
	CBT_Sequence* Root_Seq = Node_Sequence("방패들기");

	CBT_Play_Ani* Show_Ani3 = Node_Ani("시작", 3, 0.8f);
	CBT_Play_Ani* Show_Ani4 = Node_Ani("루프", 4, 0.95f);
	CBT_Wait* Wait0 = Node_Wait("루프 대기", dGuardTime, 0);
	CBT_Play_Ani* Show_Ani5 = Node_Ani("끝", 5, 0.8f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 1.f);
	
	Root_Seq->Add_Child(Show_Ani3);
	Root_Seq->Add_Child(Show_Ani4);
	Root_Seq->Add_Child(Wait0);
	Root_Seq->Add_Child(Show_Ani5);
	Root_Seq->Add_Child(Show_Ani42);

	return Root_Seq;
}

HRESULT CSwordShieldGenji::Update_Bone_Of_BlackBoard()
{
	return E_NOTIMPL;
}

HRESULT CSwordShieldGenji::Update_Value_Of_BB()
{
	return E_NOTIMPL;
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

	Safe_Release(pManagement);

	return NOERROR;
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
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
