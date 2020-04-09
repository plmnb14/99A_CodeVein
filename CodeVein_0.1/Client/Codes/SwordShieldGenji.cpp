#include "stdafx.h"
#include "..\Headers\SwordShieldGenji.h"
#include "..\Headers\Weapon.h"

#include "MonsterUI.h"
#include "DamegeNumUI.h"

CSwordShieldGenji::CSwordShieldGenji(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	ZeroMemory(m_matBones, sizeof(_mat*) * Bone_End);
}

CSwordShieldGenji::CSwordShieldGenji(const CSwordShieldGenji & rhs)
	: CGameObject(rhs)
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
	m_tObjParam.fHp_Max = 100.f;

	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));


	//// MonsterHP UI
	m_pMonsterUI = static_cast<CMonsterUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MonsterHPUI", pArg));
	m_pMonsterUI->Set_Target(this);
	m_pMonsterUI->Set_Bonmatrix(m_matBones[Bone_Head]);
	m_pMonsterUI->Ready_GameObject(NULL);

	m_pDamegeNumUI = static_cast<CDamegeNumUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_DamegeNumUI", pArg));
	m_pDamegeNumUI->Set_Target(this);
	m_pDamegeNumUI->Ready_GameObject(pArg);

	/////////////// 행동트리 init

	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create(false);

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", m_tObjParam.fHp_Cur);
	pBlackBoard->Set_Value(L"MAXHP", m_tObjParam.fHp_Max);
	pBlackBoard->Set_Value(L"HPRatio", 100);
	pBlackBoard->Set_Value(L"Show", true);
	// Monster_Speed -> delta 곱해서 저장해줬음, Monster_Dir

	//CBT_Selector* Start_Sel = Node_Selector("행동 시작"); // 찐
	CBT_Sequence* Start_Sel = Node_Sequence("행동 시작"); // 테스트
	CBT_UpdateGageRatio* UpdateHPRatioService = Node_UpdateGageRatio("체력 비율", L"HPRatio", L"MAXHP", L"HP", 1, 0.01, 0, CBT_Service_Node::Infinite);

	pBehaviorTree->Set_Child(Start_Sel);

	Start_Sel->Add_Service(UpdateHPRatioService);

	//CBT_CompareValue* Check_ShowValue = Node_BOOL_A_Equal_Value("시연회 변수 체크", L"Show", true);
	//Check_ShowValue->Set_Child(Start_Show());
	//Start_Sel->Add_Child(Check_ShowValue);
	Start_Sel->Add_Child(Start_Game());

	//Start_Sel->Add_Child(ShortDelay_Sting());

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
	if (false == m_bEnable)
		return NO_EVENT;

	Push_Collider();

	CGameObject::Update_GameObject(TimeDelta);

	// 죽었을 경우
	if (m_bIsDead)
		return DEAD_OBJ;

	// MonsterHP UI
	m_pMonsterUI->Update_GameObject(TimeDelta);
	m_pDamegeNumUI->Update_GameObject(TimeDelta);

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

	Check_PhyCollider();

	return _int();
}

_int CSwordShieldGenji::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

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
			m_pShaderCom->Begin_Pass(m_iPass);

			if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE_MAP))))
				return E_FAIL;

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

CBT_Composite_Node * CSwordShieldGenji::Upper_Slash()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	CBT_Sequence* MainSeq = Node_Sequence("어퍼슬래쉬");
	CBT_Play_Ani* Show_Ani25 = Node_Ani("어퍼슬래쉬", 25, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("기본", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.25, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 3.f, 0.167, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani25);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

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
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 0.7f, 0.817, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani24);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

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
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동", L"Monster_Speed", L"Monster_Dir", 1, 0.663, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani21);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.05, 1, 0.133, 0);
	Root_Parallel->Add_Service(pHitCol);

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
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("방패들기");
	CBT_Sequence* MainSeq = Node_Sequence("방패들기");
	CBT_Play_Ani* Show_Ani3 = Node_Ani("시작", 3, 0.8f);
	CBT_Play_Ani* Show_Ani4 = Node_Ani("루프", 4, 0.95f);
	CBT_Wait* Wait0 = Node_Wait("루프 대기", dGuardTime, 0);
	CBT_Play_Ani* Show_Ani5 = Node_Ani("끝", 5, 0.8f);

	CBT_ChaseDir* RotationDir0 = Node_ChaseDir("이동", L"Player_Pos", dGuardTime + 1, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani3);
	MainSeq->Add_Child(Show_Ani4);
	MainSeq->Add_Child(Wait0);
	MainSeq->Add_Child(Show_Ani5);

	Root_Parallel->Set_Sub_Child(RotationDir0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Start_Game()
{
	CBT_Sequence* Root_Seq = Node_Sequence("게임 시작");

	Root_Seq->Add_Child(Dist_Attack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::Dist_Attack()
{
	CBT_Selector* Root_Sel = Node_Selector("거리 판단");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 3);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(MoveAround_NearAttack());

	Root_Sel->Add_Child(Chase());

	return Root_Sel;
}

CBT_Composite_Node * CSwordShieldGenji::Guard_LeftMoveAround()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("왼쪽 가드 병렬");

	CBT_MoveAround*	MoveAround0 = Node_MoveAround("왼쪽으로 이동", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 0.7f, 2, 0.4);

	CBT_Play_Ani* Show_Ani = Node_Ani("왼쪽으로 이동", 8, 0.95f);

	Root_Parallel->Set_Main_Child(MoveAround0);
	Root_Parallel->Set_Sub_Child(Show_Ani);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Guard_RightMoveAround()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("오른쪽 가드 병렬");

	CBT_MoveAround*	MoveAround0 = Node_MoveAround("오른쪽으로 이동", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", -0.7f, 2, 0.4);

	CBT_Play_Ani* Show_Ani = Node_Ani("오른쪽으로 이동", 7, 0.95f);

	Root_Parallel->Set_Main_Child(MoveAround0);
	Root_Parallel->Set_Sub_Child(Show_Ani);

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

CBT_Composite_Node * CSwordShieldGenji::Chase_Guard_NearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("추적_가드_근접공격");

	Root_Seq->Add_Child(Chase());
	Root_Seq->Add_Child(Guard(2));
	Root_Seq->Add_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::MoveAround_NearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("서성이고 공격");
	CBT_RotationDir* RotationDir0 = Node_RotationDir("플레이어 바라보기", L"Player_Pos", 0.2);
	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 3.f);

	Root_Seq->Add_Child(Guard_MoveAround());
	Root_Seq->Add_Child(RotationDir0);
	Root_Seq->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::Guard_MoveAround()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("서성이기");

	CBT_Sequence* MainSeq = Node_Sequence("가드");
	MainSeq->Add_Child(Guard_LeftMoveAround());
	MainSeq->Add_Child(Guard_RightMoveAround());

	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("플레이어 방향추적", L"Player_Pos", 5, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	Root_Parallel->Set_Sub_Child(ChaseDir0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Chase()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_MoveDirectly* pChase = Node_MoveDirectly_Chase("추적", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 3.f, 3.f);

	CBT_Play_Ani* Show_Ani139 = Node_Ani("추적", 139, 1.f);

	Root_Parallel->Set_Main_Child(pChase);

	Root_Parallel->Set_Sub_Child(Show_Ani139);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근거리 공격");

	Root_Sel->Add_Child(Upper_Slash());
	Root_Sel->Add_Child(LongDelay_Sting());
	Root_Sel->Add_Child(Shield_Attack());
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
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos());
	// 2. 체력 업데이트
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"HP", m_tObjParam.fHp_Cur);

	//// 1. 몬스터 뒤쪽 방향 저장
	//_v3 vBackDir = -(*(_v3*)&m_pTransformCom->Get_WorldMat().m[2]);
	//m_pAIControllerCom->Set_Value_Of_BloackBoard(L"BackDir", vBackDir);


	return E_NOTIMPL;
}

HRESULT CSwordShieldGenji::Update_NF()
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
			if (Is_InFov(m_fFov, vPlayer_Pos))
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

_bool CSwordShieldGenji::Is_InFov(_float fDegreeOfFov, _v3 vTargetPos)
{
	_v3 vThisLook = *(_v3*)(&m_pTransformCom->Get_WorldMat().m[2]);
	vThisLook.y = 0.f;
	D3DXVec3Normalize(&vThisLook, &vThisLook);

	_v3 FromThisToTarget = vTargetPos - m_pTransformCom->Get_Pos();
	FromThisToTarget.y = 0.f;
	D3DXVec3Normalize(&FromThisToTarget, &FromThisToTarget);


	_float fDot_Temp = D3DXVec3Dot(&vThisLook, &FromThisToTarget);
	_float fRadian = acosf(fDot_Temp);

	//cout << "시야각 : " << D3DXToDegree(fRadian) << endl;

	if (D3DXToDegree(fRadian) < fDegreeOfFov * 0.5f)
		return true;

	return false;
}

void CSwordShieldGenji::Check_PhyCollider()
{
	if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		m_pMeshCom->Reset_OldIndx();	//애니 인덱스 초기화

		//m_tObjParam.fHp_Cur -= 2.99f;	// 체력 임의로 닳게 만듦.

		m_bAIController = false;
		cout << "나도 부딪힘 ^^" << endl;
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

		else if (m_pMeshCom->Is_Finish_Animation(0.7f))	// 이때부터 재충돌 가능
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
	list<CGameObject*> tmpList = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE);

	for (auto& iter : tmpList)
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

HRESULT CSwordShieldGenji::Draw_Collider()
{
	for (auto& iter : m_vecPhysicCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	return S_OK;
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
	m_pSword->Change_WeaponData(CWeapon::WPN_SSword_Normal);

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pSword->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pSword->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	// 왼손 무기
	m_pShield = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pShield->Change_WeaponData(CWeapon::WPN_Hammer_Normal);

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
	Safe_Release(m_pDamegeNumUI);

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
