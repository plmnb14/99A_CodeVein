#include "stdafx.h"
#include "..\Headers\PoisonButterfly.h"
#include "..\Headers\PoisonBullet.h"

CPoisonButterfly::CPoisonButterfly(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPoisonButterfly::CPoisonButterfly(const CPoisonButterfly & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPoisonButterfly::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CPoisonButterfly::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(10.f, 0.5f, 1.f));
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));


	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create(false);	//인자에 true 주면 콘솔창에 디버깅정보 뜸, default = false

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	//체력 키보드로 깎고 0 이하면 사망

	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", 100);
	pBlackBoard->Set_Value(L"MAXHP", 100);
	pBlackBoard->Set_Value(L"HPRatio", 100);
	pBlackBoard->Set_Value(L"Show", true);

	
	CBT_Selector* Start_Sel = Node_Selector("행동 시작");
	CBT_UpdatePos* UpdatePlayerPosService = Node_UpdatePos("Update_Player_Pos", L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE)), 0, 0.01, 0, CBT_Service_Node::Infinite);
	CBT_UpdateGageRatio* UpdatePlayerHPservice = Node_UpdateGageRatio("Update_Player_Pos", L"HPRatio", L"MaxHP", L"HP", 0, 0.01, 0, CBT_Service_Node::Infinite);
	CBT_UpdateGageRatio* UpdateHPRatioService = Node_UpdateGageRatio("체력 비율", L"HPRatio", L"MAXHP", L"HP", 1, 0.01, 0, CBT_Service_Node::Infinite);

	pBehaviorTree->Set_Child(Start_Sel);

	Start_Sel->Add_Service(UpdatePlayerPosService);
	Start_Sel->Add_Service(UpdatePlayerHPservice);
	Start_Sel->Add_Service(UpdateHPRatioService);

	//////////// 아래에 주석해놓은 4줄이 본게임에서 쓸 것임, 차례대로 공격함.

	//CBT_CompareValue* Check_ShowValue = Node_BOOL_A_Equal_Value("시연회 변수 체크", L"Show", true);
	//Check_ShowValue->Set_Child(Start_Show());
	//Start_Sel->Add_Child(Check_ShowValue);
	//Start_Sel->Add_Child(Start_Game());

	////////////


	// 패턴 확인용,  각 패턴 함수를 아래에 넣으면 재생됨.
	Start_Sel->Add_Child(Rush());




	
	//나무위키

	/////////////////////////////////////////////////////////////////////////////일반 공격
	/*
	뒤부터 시작, 물고 한바퀴 돌아서 독 Y OR N , 다시 물고 원래대로
	22 -> 23
	
	24   왼쪽 냠
	25	오른쪽 냠

	냠 -> 빙그르르냠
	27 -> 26   0.5에서 넘어가야 돼

	*/
	/////////////////////////////////////////////////////////////////////////////특수 공격
	/*
	19  저격샷
	20   좁은 범위 한바퀴 독살포 
	21   전방에 살랑살랑 독 살포
	28  기모아서 독 소용돌이
	29  오발탄

	*/
	/////////////////////////////////////////////////////////////////////////////단독 공격
	/*
	17   돌진 5번
	18   트린다미어 , Y OR N
		
	*/

	/////////////////////////////////////////////////////////////////////////////일반 동작
	/*
	0번    오른쪽 걷기
	1번		왼쪽 걷기
	2		앞으로 걷기
	3		뒤로 걷기




	*/

	/////////////////////////////////////////////////////////////////////////////일반 동작
	/*
	4		오른쪽 90도
	5		왼쪽 90도
	6		기본
	
	7		그로기 시작
	8		그로기 루프
	9		그로기 끝


	11		반피 2페이즈 시작

	12		데쑤  페이퍼번 디졸브  중앙이늦게 사라짐

	//   누적치
	13		뒤에서  맞음
	14		앞에서  맞음

	// 항상 15번 상태하다가 16번이 되어야한다.  그리고 다음부터는 만날일이 없음
	15		조우 전 대기
	16		조우
	*/












	/////////////////////////////////////////////////////////////////////////////일반 공격
	

	/////////////////////////////////////////////////////////////////////////////특수 공격
	//// 20 -> 뒤돌아서 덥썩
	//CBT_Play_Ani* pAni20 = Node_Ani("뒤돌아서 덥썩", 20, 0.9f);
	//pSequence->Add_Child(pAni20);


	/////////////////////////////////////////////////////////////////////////////단독 공격

	//저격
	//// 19 -> 좌회전 한바퀴  덥썩
	//CBT_Play_Ani* pAni19 = Node_Ani("좌회전 한바퀴 덥썩", 19, 0.9f);
	//pSequence->Add_Child(pAni19);


	//독기 폭발
	// 28 -> 좁은 범위 독안개
	//CBT_Play_Ani* pAni28 = Node_Ani("좁은 범위 독안개", 28, 0.9f);
	//pSequence->Add_Child(pAni28);


	//돌진
	//// 17 -> 돌진하면서 5번 타격 후 텀블링
	//CBT_Play_Ani* pAni17 = Node_Ani("돌진하면서 5번 타격 후 텀블링", 17, 0.9f);
	//pSequence->Add_Child(pAni17);


	//// 29 -> 오발탄
	//CBT_Play_Ani* pAni29 = Node_Ani("5발쏘기", 29, 0.9f);
	//pSequence->Add_Child(pAni29);


	//// 18 -> 트린다미어
	//CBT_Play_Ani* pAni18 = Node_Ani("트린다미어", 18, 0.9f);
	//pSequence->Add_Child(pAni18);








	//// 21 -> 꼬리살상 독 살포
	//CBT_Play_Ani* pAni21 = Node_Ani("꼬리살상 독 살포", 21, 0.9f);
	//pSequence->Add_Child(pAni21);


	////최초 조우시
	//// 15 -> 혼자 있을 때 idle상태
	//CBT_Play_Ani* pAni15 = Node_Ani("혼자 있을 때 IDLE상태", 15, 0.9f);
	//pSequence->Add_Child(pAni15);

	//// 16 -> 보스존 진입 시
	//CBT_Play_Ani* pAni16 = Node_Ani("보스존 진입 시", 16, 0.9f);
	//pSequence->Add_Child(pAni16);

	//// 움찔
	////13 -> 뒤맞고 움찔
	//CBT_Play_Ani* pAni13 = Node_Ani("뒤맞고 움찔", 13, 0.9f);
	//pSequence->Add_Child(pAni13);

	////14 -> 앞맞고 움찔
	//CBT_Play_Ani* pAni14 = Node_Ani("앞맞고 움찔", 14, 0.9f);
	//pSequence->Add_Child(pAni14);


	////12 -> 페이퍼번 , 디졸브 로 사망
	//CBT_Play_Ani* pAni12 = Node_Ani("사망", 12, 0.9f);
	//pSequence->Add_Child(pAni12);

	////11 -> 반피되면  2페이지 시작 시
	//CBT_Play_Ani* pAni11 = Node_Ani("반피되면 2페이지", 11, 0.9f);
	//pSequence->Add_Child(pAni11);


	///////////////////////////아웃
	////10 -> 회피
	//CBT_Play_Ani* pAni10 = Node_Ani("회피", 10, 0.9f);
	//pSequence->Add_Child(pAni10);





	return NOERROR;
}

_int CPoisonButterfly::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	m_pAIControllerCom->Update_AIController(TimeDelta);

	// 뼈 위치 업데이트
	Update_Bone_Of_BlackBoard();
	// BB 직접 업데이트
	Update_Value_Of_BB();

	return NOERROR;
}

_int CPoisonButterfly::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	return NOERROR;
}

HRESULT CPoisonButterfly::Render_GameObject()
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

CBT_Composite_Node * CPoisonButterfly::Left_Eat()
{
	CBT_Sequence* Root_Seq = Node_Sequence("왼쪽 냠");

	CBT_Play_Ani* Show_Ani24 = Node_Ani("왼쪽 냠", 24, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 6, 0.3f);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("왜곡", L"ButterFly_Distortion_Circle", L"Bone_Tail6", 1.0, 1, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("보라 파티클", L"ButterFly_PointParticle", L"Bone_Tail6", 0.5, 60, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("옅은 독안개", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0.5, 60, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("반짝 보라 모래", L"ButterFly_GlitterSand", L"Bone_Tail6", 0.5, 60, 0.01, 0);

	Root_Seq->Add_Service(Effect0);
	Root_Seq->Add_Service(Effect1);
	Root_Seq->Add_Service(Effect2);
	Root_Seq->Add_Service(Effect3);

	Root_Seq->Add_Child(Show_Ani24);
	Root_Seq->Add_Child(Show_Ani6);

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Right_Eat()
{
	CBT_Sequence* Root_Seq = Node_Sequence("오른쪽 냠");

	CBT_Play_Ani* Show_Ani25 = Node_Ani("오른쪽 냠", 25, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 6, 0.3f);

	Root_Seq->Add_Child(Show_Ani25);
	Root_Seq->Add_Child(Show_Ani6);

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Eat_Turn_Eat()
{
	CBT_Sequence* Root_Seq = Node_Sequence("냠 회전 냠");

	CBT_Play_Ani* Show_Ani22 = Node_Ani("좌회전 한바퀴 덥썩", 22, 0.95f);
	CBT_Play_Ani* Show_Ani23 = Node_Ani("뒤돌아서 덥썩", 23, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 6, 0.3f);

	Root_Seq->Add_Child(Show_Ani22);
	Root_Seq->Add_Child(Show_Ani23);
	Root_Seq->Add_Child(Show_Ani6);

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Poison_Tornado_After_Charging()
{
	CBT_Sequence* Root_Seq = Node_Sequence("기모아서 독 소용돌이");

	CBT_Play_Ani* Show_Ani28 = Node_Ani("기모아서 독 소용돌이", 28, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 6, 0.3f);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("기 모을 때 처음 스모크", L"ButterFly_SoftSmoke_Ready", L"Self_Pos", 0.3, 5, 0.2, 0);
	//CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("기 모을 때 메쉬이펙트", L"ButterFly_Distortion_Circle", L"Self_Pos", 0.3, 1, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("기 모을 때 스모크 01", L"ButterFly_SoftSmoke", L"Self_Pos", 0.3, 1, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("기 모을 때 스모크 02", L"ButterFly_SoftSmoke_Bottom", L"Self_Pos", 0.3, 1, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("폭발할 때 처음 빨간 스모크", L"ButterFly_Smoke_Red_Once", L"Self_Pos", 0.3, 1, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("폭발하는 빨간 스모크", L"ButterFly_Smoke_Red_Particle", L"Self_Pos", 0.3, 1, 0.01, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("바닥에 깔리는 빨간 스모크", L"ButterFly_SoftSmoke_Floor", L"Self_Pos", 0.3, 1, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("폭발할 때 자두색 동그라미 파티클", L"ButterFly_PointParticle_Plum", L"Self_Pos", 1.0, 1, 0.01, 0);
	//CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("폴발할 때 회오리 메쉬이펙트", L"ButterFly_RingLine", L"Self_Pos", 0.3, 1, 0.01, 0);
	//CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("회오리 메쉬 왜곡", L"ButterFly_RingLine_Distortion", L"Self_Pos", 0.3, 1, 0.01, 0);

	Root_Seq->Add_Service(Effect0);
	//Root_Seq->Add_Service(Effect1);
	Root_Seq->Add_Service(Effect2);
	Root_Seq->Add_Service(Effect3);
	Root_Seq->Add_Service(Effect4);
	Root_Seq->Add_Service(Effect5);
	Root_Seq->Add_Service(Effect6);
	Root_Seq->Add_Service(Effect7);
	//Root_Seq->Add_Service(Effect8);
	//Root_Seq->Add_Service(Effect9);



	Root_Seq->Add_Child(Show_Ani28);
	Root_Seq->Add_Child(Show_Ani6);

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::OneTurn_Poison()
{
	CBT_Sequence* Root_Seq = Node_Sequence("좁은 범위 한바퀴 독 발사");

	CBT_Play_Ani* Show_Ani20 = Node_Ani("좁은 범위 한바퀴 독 발사", 20, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 6, 0.3f);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("전체적으로 보라 동그라미 파티클", L"ButterFly_SoftSmoke_Ready", L"Self_Pos", 0.3, 5, 0.2, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("전체적으로 쓰이는 옅은 독안개", L"ButterFly_SoftSmoke_Ready", L"Self_Pos", 0.3, 5, 0.2, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("반짝이는 보라색 모래", L"ButterFly_SoftSmoke_Ready", L"Self_Pos", 0.3, 5, 0.2, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("안개와 같이 나오는 왜곡", L"ButterFly_SoftSmoke_Ready", L"Self_Pos", 0.3, 5, 0.2, 0);

	Root_Seq->Add_Service(Effect0);
	Root_Seq->Add_Service(Effect1);
	Root_Seq->Add_Service(Effect2);
	Root_Seq->Add_Service(Effect3);

	Root_Seq->Add_Child(Show_Ani20);
	Root_Seq->Add_Child(Show_Ani6);

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Rush()
{
	CBT_Simple_Parallel* RushParallel = Node_Parallel_Immediate("돌진 병렬");
	CBT_Sequence* RushAniSeq = Node_Sequence("RushSequence");
	CBT_Play_Ani* Show_Ani17 = Node_Ani("5회 타격후 덤블링", 17, 0.95f);
	CBT_Play_Ani* Show_Ani6_9 = Node_Ani("기본", 6, 0.3f);

	CBT_Sequence* MoveSequence = Node_Sequence("돌진 이동");
	CBT_Wait* RushWaitF = Node_Wait("RushWait0", 1.7, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("돌진", 10, 0.7, 0);
	CBT_Wait* RushWaitB = Node_Wait("RushWait1", 0.2, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("전체적으로 보라 동그라미 파티클", L"ButterFly_PointParticle", L"Self_Pos", 1, 5, 0.2, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("전체적으로 옅은 독안개", L"ButterFly_SoftSmoke_Mist", L"Self_Pos", 0.3, 5, 0.2, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("보라색 투명한 물방울", L"ButterFly_WaterSplash", L"Self_Pos", 0.3, 5, 0.2, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("반짝이는 보라색 모래", L"ButterFly_GlitterSand", L"Self_Pos", 0.3, 5, 0.2, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("안개와 같이 나오는 왜곡", L"ButterFly_Distortion_Smoke", L"Self_Pos", 0.3, 5, 0.2, 0);

	RushParallel->Add_Service(Effect0);
	RushParallel->Add_Service(Effect1);
	RushParallel->Add_Service(Effect2);
	RushParallel->Add_Service(Effect3);
	RushParallel->Add_Service(Effect4);


	RushParallel->Set_Sub_Child(MoveSequence);
	MoveSequence->Add_Child(RushWaitF);
	MoveSequence->Add_Child(Move0);
	MoveSequence->Add_Child(RushWaitB);

	RushParallel->Set_Main_Child(RushAniSeq);

	RushAniSeq->Add_Child(Show_Ani17);
	RushAniSeq->Add_Child(Show_Ani6_9);

	return RushParallel;
}

CBT_Composite_Node * CPoisonButterfly::Fire_5Bullet()
{
	CBT_Sequence* Root_Seq = Node_Sequence("5발탄");

	CBT_Play_Ani* Show_Ani29 = Node_Ani("5발탄", 29, 0.95f);
	CBT_Play_Ani* Show_Ani6_0 = Node_Ani("기본", 6, 0.3f);

	
	//CBT_CreateBullet* PoisonBullet0 = Node_CreateBullet("독 총알", L"Monster_PoisonBullet", L"Self_Pos", L"Self_PoisonDir0", 5, 5, 1, 1, 0, CBT_Service_Node::Finite);
	//CBT_CreateBullet* PoisonBullet1 = Node_CreateBullet("독 총알", L"Monster_PoisonBullet", L"Self_Pos", L"Self_PoisonDir1", 5, 5, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet2 = Node_CreateBullet("독 총알", L"Monster_PoisonBullet", L"Self_Pos", L"Self_PoisonDir2", 5, 5, 1, 1, 0, CBT_Service_Node::Finite);
	//CBT_CreateBullet* PoisonBullet3 = Node_CreateBullet("독 총알", L"Monster_PoisonBullet", L"Self_Pos", L"Self_PoisonDir3", 5, 5, 1, 1, 0, CBT_Service_Node::Finite);
	//CBT_CreateBullet* PoisonBullet4 = Node_CreateBullet("독 총알", L"Monster_PoisonBullet", L"Self_Pos", L"Self_PoisonDir4", 5, 5, 1, 1, 0, CBT_Service_Node::Finite);
	//Root_Seq->Add_Service(PoisonBullet0);
	//Root_Seq->Add_Service(PoisonBullet1);
	Root_Seq->Add_Service(PoisonBullet2);
	//Root_Seq->Add_Service(PoisonBullet3);
	//Root_Seq->Add_Service(PoisonBullet4);

	Root_Seq->Add_Child(Show_Ani29);
	Root_Seq->Add_Child(Show_Ani6_0);


	CBT_Wait* Wait0 = Node_Wait("대기", 2, 0);
	Root_Seq->Add_Child(Wait0);

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Fire_ChaseBullet()
{
	CBT_Sequence* Root_Seq = Node_Sequence("반바퀴 돌아서 독샷");

	CBT_Play_Ani* Show_Ani19 = Node_Ani("반바퀴 돌아서 독샷", 19, 0.95f);
	Root_Seq->Add_Child(Show_Ani19);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 6, 0.3f);
	Root_Seq->Add_Child(Show_Ani6);

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::WhirlWind()
{
	CBT_Sequence* Root_Seq = Node_Sequence("빙그르르");

	CBT_Play_Ani* Show_Ani18 = Node_Ani("빙그르르", 18, 0.95f);
	Root_Seq->Add_Child(Show_Ani18);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("기본", 6, 0.3f);
	Root_Seq->Add_Child(Show_Ani6);

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근거리 공격");

	Root_Sel->Add_Child(Left_Eat());
	Root_Sel->Add_Child(Right_Eat());
	Root_Sel->Add_Child(Eat_Turn_Eat());
	Root_Sel->Add_Child(Poison_Tornado_After_Charging());
	Root_Sel->Add_Child(OneTurn_Poison());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::FarAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Rush());
	Root_Sel->Add_Child(Fire_5Bullet());
	Root_Sel->Add_Child(Fire_ChaseBullet());
	Root_Sel->Add_Child(WhirlWind());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::Start_Game()
{
	CBT_Sequence* Root_Seq = Node_Sequence("게임 시작");

	Root_Seq->Add_Child(ChaseAndNearAttack());
	Root_Seq->Add_Child(TurnAndFarAttack());

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::ChaseAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("랜덤 공격 또는 추적");
	CBT_MoveDirectly* Chase = Node_MoveDirectly_Chace("추적", L"Player_Pos", 3.f, 2.f);

	//CBT_CreateEffect* Poison = Node_CreateEffect("포이즌", L"ButterFly_SoftSmoke", L"Bone_Tail6", 1, 0.5f, 0.f, CBT_Service_Node::Infinite);
	//Check_Length_Seq->Add_Service(Poison);

	Root_Seq->Add_Child(Chase);
	Root_Seq->Add_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::TurnAndFarAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("랜덤 원거리 공격");
	CBT_RotationDir* TurnDir = Node_RotationDir("Look 회전", L"Player_Pos", 4);

	Root_Seq->Add_Child(TurnDir);
	Root_Seq->Add_Child(FarAttack());

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Start_Show()
{
	CBT_Sequence* Root_Seq = Node_Sequence("시연회");
	//CBT_Selector* Root_Seq = Node_Selector("시연회");

	Root_Seq->Add_Child(Show_ChaseAndNearAttack());
	Root_Seq->Add_Child(Show_TurnAndFarAttack());

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Show_ChaseAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("공격 또는 추적");
	CBT_MoveDirectly* Chase = Node_MoveDirectly_Chace("추적", L"Player_Pos", 3.f, 2.f);

	//CBT_CreateEffect* Poison = Node_CreateEffect("포이즌", L"ButterFly_SoftSmoke", L"Bone_Tail6", 1, 0.5f, 0.f, CBT_Service_Node::Infinite);
	//Check_Length_Seq->Add_Service(Poison);

	Root_Seq->Add_Child(Chase);
	Root_Seq->Add_Child(Show_NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Show_TurnAndFarAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("순서대로 원거리 공격");
	CBT_RotationDir* TurnDir = Node_RotationDir("Look 회전", L"Player_Pos", 4);

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

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Left_Eat());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Right_Eat());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Eat_Turn_Eat());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(Poison_Tornado_After_Charging());
	Root_Sel->Add_Child(Cool4);
	Cool4->Set_Child(OneTurn_Poison());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::Show_FarAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("순서대로 원거리 공격");

	CBT_Cooldown* Cool0 = Node_Cooldown("쿨0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("쿨1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("쿨2", 300);
	CBT_Cooldown* Cool3 = Node_Cooldown("쿨3", 300);

	CBT_SetValue* Show_ValueOff = Node_BOOL_SetValue("시연회 OFF", L"Show", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Rush());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Fire_5Bullet());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Fire_ChaseBullet());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(WhirlWind());

	Root_Sel->Add_Child(Show_ValueOff);

	return Root_Sel;
}

HRESULT CPoisonButterfly::Update_Bone_Of_BlackBoard()
{
	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Tail6");
	m_vTail = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);

	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Bone_Tail6", m_vTail);

	return S_OK;
}

HRESULT CPoisonButterfly::Update_Value_Of_BB()
{
	_v3 vSelfDir = *(_v3*)&m_pTransformCom->Get_WorldMat().m[2];

	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_PoisonDir0", *D3DXVec3TransformNormal(&_v3(), &vSelfDir, D3DXMatrixRotationY(&_mat(), D3DXToRadian(25))));
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_PoisonDir1", *D3DXVec3TransformNormal(&_v3(), &vSelfDir, D3DXMatrixRotationY(&_mat(), D3DXToRadian(12.5f))));
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_PoisonDir2", vSelfDir);
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_PoisonDir3", *D3DXVec3TransformNormal(&_v3(), &vSelfDir, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-12.5f))));
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_PoisonDir4", *D3DXVec3TransformNormal(&_v3(), &vSelfDir, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-25))));

	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_Pos", m_pTransformCom->Get_Pos());


	return S_OK;
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_PoisonButterFly", L"Com_DynamicMesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	// for.Com_AIController
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"AIController", L"Com_AIController", (CComponent**)&m_pAIControllerCom)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	//m_pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	m_pCollider->Set_Radius(_v3(1.f, 1.f, 1.f));


	return NOERROR;
}

HRESULT CPoisonButterfly::SetUp_ConstantTable()
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
	Safe_Release(m_pCollider);
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
