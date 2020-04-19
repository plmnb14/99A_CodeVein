#include "stdafx.h"
#include "..\Headers\IceGirl.h"
#include "..\Headers\Weapon.h"
#include "..\Headers\BossHP.h"

CIceGirl::CIceGirl(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CIceGirl::CIceGirl(const CIceGirl & rhs)
	: CMonster(rhs)
{
}

HRESULT CIceGirl::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CIceGirl::Ready_GameObject(void * pArg)
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

	pBlackBoard->Set_Value(L"PushCol", true);	// 충돌여부 제어변수
	pBlackBoard->Set_Value(L"PhyCol", true); // 피격판정 제어 변수
	pBlackBoard->Set_Value(L"Ice_Barrier_On", false);	// 얼음 보호막 변수

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

	CBT_Wait* Wait0 = Node_Wait("대기", 1, 0);
	Start_Sel->Add_Child(Wait0);

	/////////////
	// UI 추가(지원)
	m_pBossUI = static_cast<CBossHP*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BossHP", nullptr));
	m_pBossUI->Set_UI_Pos(WINCX * 0.5f, WINCY * 0.2f);
	if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBossUI, SCENE_STAGE, L"Layer_BossHP", nullptr)))
		return E_FAIL;
	return S_OK;
}

_int CIceGirl::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (true == m_pAIControllerCom->Get_BoolValue(L"PushCol"))
		Push_Collider();

	CGameObject::Update_GameObject(TimeDelta);

	// 죽었을 경우
	if (m_bIsDead)
		return DEAD_OBJ;
	
		

	// 죽음 애니메이션
	if (m_bReadyDead)
	{
		// 죽으면서 UI 비활성화
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
		if (0 >= m_tObjParam.fHp_Cur)
			m_pBossUI->Set_BossHPInfo(0, 100);
		else
			m_pBossUI->Set_BossHPInfo(m_tObjParam.fHp_Cur, m_tObjParam.fHp_Max);
	}

	if (false == m_pAIControllerCom->Get_BoolValue(L"Ice_Barrier_On"))
		m_tObjParam.bIsDodge = false;

	if (false == m_bReadyDead && true == m_pAIControllerCom->Get_BoolValue(L"PhyCol"))
		Check_PhyCollider();

	OnCollisionEnter();

	return NOERROR;
}

_int CIceGirl::Late_Update_GameObject(_double TimeDelta)
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

	return NOERROR;
}

HRESULT CIceGirl::Render_GameObject()
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

	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CIceGirl::Render_GameObject_SetPass(CShader * pShader, _int iPass)
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

CBT_Composite_Node * CIceGirl::Turn_Cut(_float fWeight)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("회전 베기");
	CBT_Play_Ani* Show_Ani34 = Node_Ani("돌면서 베기", 34, fWeight);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.116, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 3.f, 0.534, 0);

	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("바닥 얼음 오오라", L"IceFloorAura_01", L"Self_Foot", 0.2, 25, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("바닥 얼음 오오라", L"IceFloorAura_02", L"Self_Foot", 0.2, 25, 0, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("바닥 얼음 오오라", L"IceFloorAura_03", L"Self_Foot", 0.2, 25, 0, 0);

	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("칼 냉기 스모크1", L"IceSmoke_01", L"Sword_MidPos", 0.2, 150, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("칼 냉기 스모크2", L"IceSmoke_02", L"Sword_MidPos", 0.2, 150, 0, 0);

	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("파랑 원 파티클", L"IceGirl_PointParticle_Blue", L"Sword_MidPos"		, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("초록 원 파티클", L"IceGirl_PointParticle_Green", L"Sword_MidPos"		, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("파랑 반짝이 파티클", L"IceGirl_FlashParticle_Blue", L"Sword_MidPos"	, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("초록 반짝이 파티클", L"IceGirl_FlashParticle_Green", L"Sword_MidPos"	, 0.38, 80, 0, 0);

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
	MainSeq->Add_Child(Show_Ani34);;
	MainSeq->Add_Child(Show_Ani0);;

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.833, 1, 0.217, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::ThreeCombo_Cut1()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("3연속 베기");
	CBT_Play_Ani* Show_Ani38 = Node_Ani("베기1", 38, 0.4f);
	CBT_Play_Ani* Show_Ani37 = Node_Ani("베기2", 37, 0.4f);
	CBT_Play_Ani* Show_Ani35 = Node_Ani("베기3", 35, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.083, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.45, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.333 + 0.016, 0);
	// 애니38 * 0.4f = 0.966
	CBT_RotationDir* Rotation1 = Node_RotationDir("돌기1", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 3.f, 0.35, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.507 + 0.016, 0);
	// 애니37 * 0.4f = 0.973
	CBT_RotationDir* Rotation2 = Node_RotationDir("돌기2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 3.f, 0.5, 0);
	CBT_Wait* Wait3 = Node_Wait("대기3", 0.767, 0);
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("이동3", L"Monster_Speed", L"Monster_Dir", 1.f, 0.317, 0);
	CBT_Wait* Wait4 = Node_Wait("대기4", 0.25, 0);
	CBT_MoveDirectly* Move4 = Node_MoveDirectly_Rush("이동4", L"Monster_Speed", L"Monster_Dir", -0.6f, 0.716, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("바닥 얼음 오오라 1타", L"IceFloorAura_01", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("바닥 얼음 오오라 1타", L"IceFloorAura_02", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("바닥 얼음 오오라 1타", L"IceFloorAura_03", L"Self_Foot", 0.3, 20, 0, 0);

	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("바닥 얼음 오오라 2타", L"IceFloorAura_01", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("바닥 얼음 오오라 2타", L"IceFloorAura_02", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("바닥 얼음 오오라 2타", L"IceFloorAura_03", L"Self_Foot", 1, 20, 0, 0);

	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("바닥 얼음 오오라 3타", L"IceFloorAura_01", L"Self_Foot", 1.8, 25, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("바닥 얼음 오오라 3타", L"IceFloorAura_02", L"Self_Foot", 1.8, 25, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("바닥 얼음 오오라 3타", L"IceFloorAura_03", L"Self_Foot", 1.8, 25, 0, 0);

	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("칼 냉기 스모크1", L"IceSmoke_01"	, L"Sword_MidPos"	, 0.2, 150, 0, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("칼 냉기 스모크2", L"IceSmoke_02"	, L"Sword_MidPos"	, 0.2, 150, 0, 0);
	
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("파랑 원 파티클", L"IceGirl_PointParticle_Blue"			, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("초록 원 파티클", L"IceGirl_PointParticle_Green"		, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("파랑 반짝이 파티클", L"IceGirl_FlashParticle_Blue"		, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("초록 반짝이 파티클", L"IceGirl_FlashParticle_Green"	, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("파랑 원 파티클", L"IceGirl_PointParticle_Blue"			, L"Sword_MidPos"	, 1, 80, 0, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("초록 원 파티클", L"IceGirl_PointParticle_Green"		, L"Sword_MidPos"	, 1, 80, 0, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("파랑 반짝이 파티클", L"IceGirl_FlashParticle_Blue"		, L"Sword_MidPos"	, 1, 80, 0, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("초록 반짝이 파티클", L"IceGirl_FlashParticle_Green"	, L"Sword_MidPos"	, 1, 80, 0, 0);
	
	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("파랑 원 파티클", L"IceGirl_PointParticle_Blue"			, L"Sword_MidPos"	, 1.6, 80, 0, 0);
	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("초록 원 파티클", L"IceGirl_PointParticle_Green"		, L"Sword_MidPos"	, 1.6, 80, 0, 0);
	CBT_CreateEffect* Effect21 = Node_CreateEffect_Finite("파랑 반짝이 파티클", L"IceGirl_FlashParticle_Blue"		, L"Sword_MidPos"	, 1.6, 80, 0, 0);
	CBT_CreateEffect* Effect22 = Node_CreateEffect_Finite("초록 반짝이 파티클", L"IceGirl_FlashParticle_Green"	, L"Sword_MidPos"	, 1.6, 80, 0, 0);

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
	Root_Parallel->Add_Service(Effect14);
	Root_Parallel->Add_Service(Effect15);
	Root_Parallel->Add_Service(Effect16);
	Root_Parallel->Add_Service(Effect17);
	Root_Parallel->Add_Service(Effect18);
	Root_Parallel->Add_Service(Effect19);
	Root_Parallel->Add_Service(Effect20);
	Root_Parallel->Add_Service(Effect21);
	Root_Parallel->Add_Service(Effect22);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani38);
	MainSeq->Add_Child(Show_Ani37);
	MainSeq->Add_Child(Show_Ani35);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Rotation1);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Rotation2);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(Wait3);
	SubSeq->Add_Child(Move3);
	SubSeq->Add_Child(Wait4);
	SubSeq->Add_Child(Move4);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.616, 1, 0.084, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.45, 1, 0.166, 0);
	CBT_UpdateParam* pHitCol2 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.973 + 0.55, 1, 0.166, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);
	Root_Parallel->Add_Service(pHitCol2);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::ThreeCombo_Cut2()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("3연속 베기");
	CBT_Play_Ani* Show_Ani38 = Node_Ani("베기1", 38, 0.4f);
	CBT_Play_Ani* Show_Ani37 = Node_Ani("베기2", 37, 0.4f);
	CBT_Play_Ani* Show_Ani34 = Node_Ani("돌면서 베기", 34, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);


	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.083, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.45, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.333 + 0.016, 0);
	// 애니38 * 0.4f = 0.966
	CBT_RotationDir* Rotation1 = Node_RotationDir("돌기1", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 3.f, 0.35, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.507 + 0.116, 0);
	// 애니37 * 0.4f = 0.973
	CBT_RotationDir* Rotation2 = Node_RotationDir("돌기2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 3.f, 0.534, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("바닥 얼음 오오라 1타", L"IceFloorAura_01", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("바닥 얼음 오오라 1타", L"IceFloorAura_02", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("바닥 얼음 오오라 1타", L"IceFloorAura_03", L"Self_Foot", 0.3, 20, 0, 0);

	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("바닥 얼음 오오라 2타", L"IceFloorAura_01", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("바닥 얼음 오오라 2타", L"IceFloorAura_02", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("바닥 얼음 오오라 2타", L"IceFloorAura_03", L"Self_Foot", 1, 20, 0, 0);

	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("바닥 얼음 오오라 3타", L"IceFloorAura_01", L"Self_Foot", 2.0, 25, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("바닥 얼음 오오라 3타", L"IceFloorAura_02", L"Self_Foot", 2.0, 25, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("바닥 얼음 오오라 3타", L"IceFloorAura_03", L"Self_Foot", 2.0, 25, 0, 0);

	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("칼 냉기 스모크1", L"IceSmoke_01"	, L"Sword_MidPos"	, 0.2, 180, 0, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("칼 냉기 스모크2", L"IceSmoke_02"	, L"Sword_MidPos"	, 0.2, 180, 0, 0);
	
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("파랑 원모양 파티클", L"IceGirl_PointParticle_Blue"	, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("초록 원모양 파티클", L"IceGirl_PointParticle_Green"	, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("파랑 반짝이 파티클", L"IceGirl_FlashParticle_Blue"	, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("초록 반짝이 파티클", L"IceGirl_FlashParticle_Green"	, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("파랑 원모양 파티클", L"IceGirl_PointParticle_Blue"	, L"Sword_MidPos"	, 1, 80, 0, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("초록 원모양 파티클", L"IceGirl_PointParticle_Green"	, L"Sword_MidPos"	, 1, 80, 0, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("파랑 반짝이 파티클", L"IceGirl_FlashParticle_Blue"	, L"Sword_MidPos"	, 1, 80, 0, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("초록 반짝이 파티클", L"IceGirl_FlashParticle_Green"	, L"Sword_MidPos"	, 1, 80, 0, 0);
	
	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("파랑 원모양 파티클", L"IceGirl_PointParticle_Blue"	, L"Sword_MidPos"	, 1.95, 80, 0, 0);
	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("초록 원모양 파티클", L"IceGirl_PointParticle_Green"	, L"Sword_MidPos"	, 1.95, 80, 0, 0);
	CBT_CreateEffect* Effect21 = Node_CreateEffect_Finite("파랑 반짝이 파티클", L"IceGirl_FlashParticle_Blue"	, L"Sword_MidPos"	, 1.95, 80, 0, 0);
	CBT_CreateEffect* Effect22 = Node_CreateEffect_Finite("초록 반짝이 파티클", L"IceGirl_FlashParticle_Green"	, L"Sword_MidPos"	, 1.95, 80, 0, 0);

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
	Root_Parallel->Add_Service(Effect14);
	Root_Parallel->Add_Service(Effect15);
	Root_Parallel->Add_Service(Effect16);
	Root_Parallel->Add_Service(Effect17);
	Root_Parallel->Add_Service(Effect18);
	Root_Parallel->Add_Service(Effect19);
	Root_Parallel->Add_Service(Effect20);
	Root_Parallel->Add_Service(Effect21);
	Root_Parallel->Add_Service(Effect22);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani38);
	MainSeq->Add_Child(Show_Ani37);
	MainSeq->Add_Child(Show_Ani34);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Rotation1);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Rotation2);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.616, 1, 0.084, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.45, 1, 0.166, 0);
	CBT_UpdateParam* pHitCol2 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.973 + 0.833, 1, 0.217, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);
	Root_Parallel->Add_Service(pHitCol2);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Jump_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("점프 공격");
	CBT_Play_Ani* Show_Ani39 = Node_Ani("점프 공격", 39, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 1.0, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 10.f, 0.183, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("차징 오른손 파티클" , L"IceGirl_Charge_Hand_Particle", L"CreateSwordBulletPos", 0.1, 60, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("차징 오른손 스모크1", L"IceGirl_Charge_Hand_Smoke", L"CreateSwordBulletPos", 0.1, 60, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("차징 오른손 스모크2", L"IceGirl_Charge_Hand_Smoke_2", L"CreateSwordBulletPos", 0.1, 60, 0, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("칼 냉기 스모크1", L"IceSmoke_01", L"Sword_MidPos", 0.2, 60, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("칼 냉기 스모크2", L"IceSmoke_02", L"Sword_MidPos", 0.2, 60, 0, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani39);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.316, 1, 0.117, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Ice_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("3연속 베기");
	CBT_Play_Ani* Show_Ani38 = Node_Ani("베기1", 38, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);
	
	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.083, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.45, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.417, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -0.6f, 0.483, 0);

	// 얼음 장판을 칼 휘두르면서 차례대로 소환하고


	// 얼음 기둥은 칼 다 휘두르고 일괄적으로 소환(ColdBeam 클래스에서 소환)
	CBT_CreateBullet* Ice0 = Node_CreateBullet("얼음 소환", L"Monster_ColdBeam", L"IceCut_Pos0", L"IceCut_Dir0", 0, 1.0, 0.73, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Ice1 = Node_CreateBullet("얼음 소환", L"Monster_ColdBeam", L"IceCut_Pos1", L"IceCut_Dir1", 0, 1.0, 0.76, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Ice2 = Node_CreateBullet("얼음 소환", L"Monster_ColdBeam", L"IceCut_Pos2", L"IceCut_Dir2", 0, 1.0, 0.79, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Ice3 = Node_CreateBullet("얼음 소환", L"Monster_ColdBeam", L"IceCut_Pos3", L"IceCut_Dir3", 0, 1.0, 0.82, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Ice4 = Node_CreateBullet("얼음 소환", L"Monster_ColdBeam", L"IceCut_Pos4", L"IceCut_Dir4", 0, 1.0, 0.85, 1, 1, 0, CBT_Service_Node::Finite);

	Root_Parallel->Add_Service(Ice0);
	Root_Parallel->Add_Service(Ice1);
	Root_Parallel->Add_Service(Ice2);
	Root_Parallel->Add_Service(Ice3);
	Root_Parallel->Add_Service(Ice4);
	

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani38);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.616, 1, 0.084, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Ice_Barrier()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("얼음 보호막 시전");
	CBT_Play_Ani* Show_Ani29 = Node_Ani("얼음 보호막", 29, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("보호막 변수 On");
	CBT_SetValue* BarrierOn = Node_BOOL_SetValue("얼음 보호막 변수 On", L"Ice_Barrier_On", true);

	CBT_CreateBuff* Barrier0 = Node_CreateBuff("얼음 보호막 생성", L"Monster_IceBarrier", 10, 1.5, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Barrier0);
	CBT_UpdateParam* BarrierValue0 = Node_UpdateParam("닷지 On", &m_tObjParam, CBT_UpdateParam::Dodge, 1.5, 1, 0, 0);
	Root_Parallel->Add_Service(BarrierValue0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("버프 전 오른손 오오라", L"IceGirl_Buff_Charge_Smoke_01", L"Bone_LeftHand", 0.1, 45, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("스모크1", L"IceGirl_Buff_Bubble_BreakSmoke", L"Self_Foot", 0.8, 30, 0, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani29);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(BarrierOn);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Charge_Rush()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("기모아서 돌진");
	CBT_Play_Ani* Show_Ani32 = Node_Ani("기모아서 돌진", 32, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);
	
	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("플레이어 보기", L"Player_Pos", 2, 0);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.133);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 30.f, 0.183, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);


	CBT_CreateBullet* Bullet0 = Node_CreateBullet("검기 발사",L"Monster_SwordBullet", L"CreateSwordBulletPos", L"IceCut_Dir2"/*뱡향 같음*/, 23, 0.9, 2.3, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Bullet0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("차징 오른손 파티클", L"IceGirl_Charge_Hand_Particle", L"CreateSwordBulletPos", 0.1, 60, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("차징 오른손 스모크1", L"IceGirl_Charge_Hand_Smoke", L"CreateSwordBulletPos", 0.1, 60, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("차징 오른손 스모크2", L"IceGirl_Charge_Hand_Smoke_2", L"CreateSwordBulletPos", 0.1, 60, 0, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("바닥 얼음 오오라", L"IceFloorAura_01", L"Self_Foot", 1.9, 25, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("바닥 얼음 오오라", L"IceFloorAura_02", L"Self_Foot", 1.9, 25, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("바닥 얼음 오오라", L"IceFloorAura_03", L"Self_Foot", 1.9, 25, 0, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("칼 냉기 스모크1", L"IceSmoke_01", L"Sword_MidPos", 0.1, 160, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("칼 냉기 스모크2", L"IceSmoke_02", L"Sword_MidPos", 0.1, 160, 0, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani32);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(PushColOn);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 2.133, 1, 0.183, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Dodge_Left()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	
	CBT_Sequence* MainSeq = Node_Sequence("좌 회피");
	CBT_Play_Ani* Show_Ani15 = Node_Ani("좌 회피", 15, 0.5f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("이동", L"DodgePosL", 0.4);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 5.f, 0.3, 0);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOn", L"PhyCol", true);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani15);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(PhyColOn);
	SubSeq->Add_Child(PushColOn);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Dodge_Right()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("좌 회피");
	CBT_Play_Ani* Show_Ani14 = Node_Ani("우 회피", 14, 0.5f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("이동", L"DodgePosR", 0.4);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 5.f, 0.3, 0);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOn", L"PhyCol", true);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani14);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(PhyColOn);
	SubSeq->Add_Child(PushColOn);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Dodge_Front()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("좌 회피");
	CBT_Play_Ani* Show_Ani16 = Node_Ani("앞 회피", 16, 0.5f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("이동", L"DodgePosF", 0.4);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 5.f, 0.3, 0);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOn", L"PhyCol", true);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani16);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(PhyColOn);
	SubSeq->Add_Child(PushColOn);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Dodge_Back()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("좌 회피");
	CBT_Play_Ani* Show_Ani17 = Node_Ani("뒤 회피", 17, 0.5f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("이동", L"DodgePosB", 0.4);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 5.f, 0.3, 0);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOn", L"PhyCol", true);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani17);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(PhyColOn);
	SubSeq->Add_Child(PushColOn);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Chase_ThreeCombo_Cut1()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("3연속 베기");
	CBT_Play_Ani* Show_Ani38 = Node_Ani("베기1", 38, 0.4f);
	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 6);
	CBT_Play_Ani* Show_Ani37 = Node_Ani("베기2", 37, 0.4f);
	CBT_DistCheck* Dist1 = Node_DistCheck("거리 체크", L"Player_Pos", 6);
	CBT_Play_Ani* Show_Ani35 = Node_Ani("베기3", 35, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.083, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.45, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.333 + 0.016, 0);
	// 애니38 * 0.4f = 0.966
	CBT_RotationDir* Rotation1 = Node_RotationDir("돌기1", L"Player_Pos", 0.1);
	//CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 3.f, 0.35, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.507 + 0.016, 0);
	// 애니37 * 0.4f = 0.973
	CBT_RotationDir* Rotation2 = Node_RotationDir("돌기2", L"Player_Pos", 0.1);
	//CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 3.f, 0.5, 0);
	CBT_Wait* Wait3 = Node_Wait("대기3", 0.767, 0);
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("이동3", L"Monster_Speed", L"Monster_Dir", 1.f, 0.317, 0);
	CBT_Wait* Wait4 = Node_Wait("대기4", 0.25, 0);
	CBT_MoveDirectly* Move4 = Node_MoveDirectly_Rush("이동4", L"Monster_Speed", L"Monster_Dir", -0.6f, 0.716, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("바닥 얼음 오오라 1타", L"IceFloorAura_01", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("바닥 얼음 오오라 1타", L"IceFloorAura_02", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("바닥 얼음 오오라 1타", L"IceFloorAura_03", L"Self_Foot", 0.3, 20, 0, 0);

	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("바닥 얼음 오오라 2타", L"IceFloorAura_01", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("바닥 얼음 오오라 2타", L"IceFloorAura_02", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("바닥 얼음 오오라 2타", L"IceFloorAura_03", L"Self_Foot", 1, 20, 0, 0);

	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("바닥 얼음 오오라 3타", L"IceFloorAura_01", L"Self_Foot", 1.8, 25, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("바닥 얼음 오오라 3타", L"IceFloorAura_02", L"Self_Foot", 1.8, 25, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("바닥 얼음 오오라 3타", L"IceFloorAura_03", L"Self_Foot", 1.8, 25, 0, 0);

	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("칼 냉기 스모크1", L"IceSmoke_01", L"Sword_MidPos", 0.2, 150, 0, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("칼 냉기 스모크2", L"IceSmoke_02", L"Sword_MidPos", 0.2, 150, 0, 0);

	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("파랑 원 파티클", L"IceGirl_PointParticle_Blue", L"Sword_MidPos", 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("초록 원 파티클", L"IceGirl_PointParticle_Green", L"Sword_MidPos", 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("파랑 반짝이 파티클", L"IceGirl_FlashParticle_Blue", L"Sword_MidPos", 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("초록 반짝이 파티클", L"IceGirl_FlashParticle_Green", L"Sword_MidPos", 0.38, 80, 0, 0);

	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("파랑 원 파티클", L"IceGirl_PointParticle_Blue", L"Sword_MidPos", 1, 80, 0, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("초록 원 파티클", L"IceGirl_PointParticle_Green", L"Sword_MidPos", 1, 80, 0, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("파랑 반짝이 파티클", L"IceGirl_FlashParticle_Blue", L"Sword_MidPos", 1, 80, 0, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("초록 반짝이 파티클", L"IceGirl_FlashParticle_Green", L"Sword_MidPos", 1, 80, 0, 0);

	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("파랑 원 파티클", L"IceGirl_PointParticle_Blue", L"Sword_MidPos", 1.6, 80, 0, 0);
	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("초록 원 파티클", L"IceGirl_PointParticle_Green", L"Sword_MidPos", 1.6, 80, 0, 0);
	CBT_CreateEffect* Effect21 = Node_CreateEffect_Finite("파랑 반짝이 파티클", L"IceGirl_FlashParticle_Blue", L"Sword_MidPos", 1.6, 80, 0, 0);
	CBT_CreateEffect* Effect22 = Node_CreateEffect_Finite("초록 반짝이 파티클", L"IceGirl_FlashParticle_Green", L"Sword_MidPos", 1.6, 80, 0, 0);

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
	Root_Parallel->Add_Service(Effect14);
	Root_Parallel->Add_Service(Effect15);
	Root_Parallel->Add_Service(Effect16);
	Root_Parallel->Add_Service(Effect17);
	Root_Parallel->Add_Service(Effect18);
	Root_Parallel->Add_Service(Effect19);
	Root_Parallel->Add_Service(Effect20);
	Root_Parallel->Add_Service(Effect21);
	Root_Parallel->Add_Service(Effect22);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani38);
	MainSeq->Add_Child(Dist0);
	Dist0->Set_Child(Show_Ani37);
	MainSeq->Add_Child(Dist1);
	Dist1->Set_Child(Show_Ani35);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Rotation1);
	//SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Chase_Timer(0.35, 10.f));
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Rotation2);
	//SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(Chase_Timer(0.5, 12.f));
	SubSeq->Add_Child(Wait3);
	SubSeq->Add_Child(Move3);
	SubSeq->Add_Child(Wait4);
	SubSeq->Add_Child(Move4);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.616, 1, 0.084, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.45, 1, 0.166, 0);
	CBT_UpdateParam* pHitCol2 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.973 + 0.55, 1, 0.166, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);
	Root_Parallel->Add_Service(pHitCol2);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Chase_ThreeCombo_Cut2()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("3연속 베기");
	CBT_Play_Ani* Show_Ani38 = Node_Ani("베기1", 38, 0.4f);
	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 6);
	CBT_Play_Ani* Show_Ani37 = Node_Ani("베기2", 37, 0.4f);
	CBT_DistCheck* Dist1 = Node_DistCheck("거리 체크", L"Player_Pos", 6);
	CBT_Play_Ani* Show_Ani34 = Node_Ani("돌면서 베기", 34, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);


	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.083, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.45, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.333 + 0.016, 0);
	// 애니38 * 0.4f = 0.966
	CBT_RotationDir* Rotation1 = Node_RotationDir("돌기1", L"Player_Pos", 0.1);
	//CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 3.f, 0.35, 0);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.507 + 0.116, 0);
	// 애니37 * 0.4f = 0.973
	CBT_RotationDir* Rotation2 = Node_RotationDir("돌기2", L"Player_Pos", 0.1);
	//CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 3.f, 0.534, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("바닥 얼음 오오라 1타", L"IceFloorAura_01", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("바닥 얼음 오오라 1타", L"IceFloorAura_02", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("바닥 얼음 오오라 1타", L"IceFloorAura_03", L"Self_Foot", 0.3, 20, 0, 0);

	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("바닥 얼음 오오라 2타", L"IceFloorAura_01", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("바닥 얼음 오오라 2타", L"IceFloorAura_02", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("바닥 얼음 오오라 2타", L"IceFloorAura_03", L"Self_Foot", 1, 20, 0, 0);

	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("바닥 얼음 오오라 3타", L"IceFloorAura_01", L"Self_Foot", 2.0, 25, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("바닥 얼음 오오라 3타", L"IceFloorAura_02", L"Self_Foot", 2.0, 25, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("바닥 얼음 오오라 3타", L"IceFloorAura_03", L"Self_Foot", 2.0, 25, 0, 0);

	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("칼 냉기 스모크1", L"IceSmoke_01", L"Sword_MidPos", 0.2, 180, 0, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("칼 냉기 스모크2", L"IceSmoke_02", L"Sword_MidPos", 0.2, 180, 0, 0);

	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("파랑 원모양 파티클", L"IceGirl_PointParticle_Blue", L"Sword_MidPos", 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("초록 원모양 파티클", L"IceGirl_PointParticle_Green", L"Sword_MidPos", 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("파랑 반짝이 파티클", L"IceGirl_FlashParticle_Blue", L"Sword_MidPos", 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("초록 반짝이 파티클", L"IceGirl_FlashParticle_Green", L"Sword_MidPos", 0.38, 80, 0, 0);

	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("파랑 원모양 파티클", L"IceGirl_PointParticle_Blue", L"Sword_MidPos", 1, 80, 0, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("초록 원모양 파티클", L"IceGirl_PointParticle_Green", L"Sword_MidPos", 1, 80, 0, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("파랑 반짝이 파티클", L"IceGirl_FlashParticle_Blue", L"Sword_MidPos", 1, 80, 0, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("초록 반짝이 파티클", L"IceGirl_FlashParticle_Green", L"Sword_MidPos", 1, 80, 0, 0);

	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("파랑 원모양 파티클", L"IceGirl_PointParticle_Blue", L"Sword_MidPos", 1.95, 80, 0, 0);
	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("초록 원모양 파티클", L"IceGirl_PointParticle_Green", L"Sword_MidPos", 1.95, 80, 0, 0);
	CBT_CreateEffect* Effect21 = Node_CreateEffect_Finite("파랑 반짝이 파티클", L"IceGirl_FlashParticle_Blue", L"Sword_MidPos", 1.95, 80, 0, 0);
	CBT_CreateEffect* Effect22 = Node_CreateEffect_Finite("초록 반짝이 파티클", L"IceGirl_FlashParticle_Green", L"Sword_MidPos", 1.95, 80, 0, 0);

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
	Root_Parallel->Add_Service(Effect14);
	Root_Parallel->Add_Service(Effect15);
	Root_Parallel->Add_Service(Effect16);
	Root_Parallel->Add_Service(Effect17);
	Root_Parallel->Add_Service(Effect18);
	Root_Parallel->Add_Service(Effect19);
	Root_Parallel->Add_Service(Effect20);
	Root_Parallel->Add_Service(Effect21);
	Root_Parallel->Add_Service(Effect22);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani38);
	MainSeq->Add_Child(Dist0);
	Dist0->Set_Child(Show_Ani37);
	MainSeq->Add_Child(Dist1);
	Dist1->Set_Child(Show_Ani34);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Rotation1);
	SubSeq->Add_Child(Chase_Timer(0.35, 10.f));
	//SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Rotation2);
	//SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(Chase_Timer(0.534, 12.f));

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.616, 1, 0.084, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.45, 1, 0.166, 0);
	CBT_UpdateParam* pHitCol2 = Node_UpdateParam("무기 히트 On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.973 + 0.833, 1, 0.217, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);
	Root_Parallel->Add_Service(pHitCol2);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Chase_Timer(_double dRunTime, _float fSpeed)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", dRunTime, 0);

	CBT_Sequence* SubSeq = Node_Sequence("추적");
	CBT_MoveDirectly* Chase0 = Node_MoveDirectly_Chase("추적", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", fSpeed, 3);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Wait0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Chase0);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Start_Game()
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

CBT_Composite_Node * CIceGirl::More_Than_HP_70()
{
	CBT_Selector* Root_Sel = Node_Selector("근거리 원거리 구분 공격");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_More_Than_HP70());

	Root_Sel->Add_Child(FarAttack_More_Than_HP70());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::NearAttack_Dist5_More_Than_HP70()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	Root_Sel->Add_Child(Turn_Cut(0.95f));
	Root_Sel->Add_Child(ThreeCombo_Cut1());
	Root_Sel->Add_Child(ThreeCombo_Cut2());
	Root_Sel->Add_Child(Jump_Attack());
	Root_Sel->Add_Child(Random_Dodge());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::FarAttack_More_Than_HP70()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Charge_Rush());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::More_Than_HP_40()
{
	CBT_Selector* Root_Sel = Node_Selector("근거리 원거리 구분 공격");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_More_Than_HP40());

	Root_Sel->Add_Child(FarAttack_More_Than_HP40());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::NearAttack_Dist5_More_Than_HP40()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	Root_Sel->Add_Child(Chase_ThreeCombo_Cut1());
	Root_Sel->Add_Child(Chase_ThreeCombo_Cut2());
	Root_Sel->Add_Child(Ice_Cut());
	Root_Sel->Add_Child(Turn_Cut(0.95f));
	Root_Sel->Add_Child(Random_Dodge());
	
	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::FarAttack_More_Than_HP40()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Charge_Rush());
	Root_Sel->Add_Child(Dash_To_Target());
	Root_Sel->Add_Child(Create_IceBarrier_Or_Not());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::HP_Final()
{
	CBT_Selector* Root_Sel = Node_Selector("근거리 원거리 구분 공격");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_Final());

	Root_Sel->Add_Child(FarAttack_Fianl());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::NearAttack_Dist5_Final()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	Root_Sel->Add_Child(Chase_ThreeCombo_Cut1());
	//Root_Sel->Add_Child(Chase_ThreeCombo_Cut2());
	Root_Sel->Add_Child(Ice_Cut());
	Root_Sel->Add_Child(Create_IceBarrier_Or_Not());
	Root_Sel->Add_Child(Cut_BackDodge());
	Root_Sel->Add_Child(Random_Dodge());
	Root_Sel->Add_Child(Create_IceBarrier_Or_Not());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::FarAttack_Fianl()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Charge_Rush());
	Root_Sel->Add_Child(Create_IceBarrier_Or_Not());
	Root_Sel->Add_Child(Dash_To_Target());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::Create_IceBarrier_Or_Not()
{
	/*
	아이스 베리어 사용중임? -> ㅇㅇ -> 기본 모션
	-> ㄴㄴ -> 얼음 보호막 시전
	*/

	CBT_Selector* Root_Sel = Node_Selector("얼음 보호막 시전");

	CBT_CompareValue* Check_Ice_Barrier = Node_BOOL_A_Equal_Value("배리어 사용중임?", L"Ice_Barrier_On", true);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본1", 0, 0.f);

	Root_Sel->Add_Child(Check_Ice_Barrier);
	Check_Ice_Barrier->Set_Child(Show_Ani0);

	Root_Sel->Add_Child(Ice_Barrier());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::Cut_BackDodge()
{
	CBT_Sequence* Root_Seq = Node_Sequence("베고 뒤로 회피");
	
	Root_Seq->Add_Child(Turn_Cut(0.4f));
	Root_Seq->Add_Child(Dodge_Back());

	return Root_Seq;
}

CBT_Composite_Node * CIceGirl::Dash_To_Target()
{
	CBT_Sequence* Root_Seq = Node_Sequence("타겟에게 대쉬");

	CBT_RotationDir* Rotation0 = Node_RotationDir("타겟에게 대쉬", L"Player_Pos", 0.1);
	
	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(Dodge_Front());

	return Root_Seq;
}

CBT_Composite_Node * CIceGirl::Random_Dodge()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 회피");

	Root_Sel->Add_Child(Dodge_Left());
	Root_Sel->Add_Child(Dodge_Right());
	Root_Sel->Add_Child(Dodge_Front());
	Root_Sel->Add_Child(Dodge_Back());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::Start_Show()
{
	CBT_Selector* Root_Sel = Node_Selector("시연회");

	CBT_CompareValue* Check_ShowValueN = Node_BOOL_A_Equal_Value("시연회 근거리 변수 체크", L"Show_Near", true);

	Root_Sel->Add_Child(Check_ShowValueN);
	Check_ShowValueN->Set_Child(Show_RotationAndNearAttack());

	Root_Sel->Add_Child(Show_FarAttack());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::Show_RotationAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("추적 후 순서대로 공격");
	CBT_RotationDir* Rotation0 = Node_RotationDir("플레이어 바라보기", L"Player_Pos", 0.2);

	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(Show_NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CIceGirl::Show_NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("순서대로 근거리 공격");

	CBT_Cooldown* Cool0 = Node_Cooldown("쿨0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("쿨1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("쿨2", 300);
	CBT_Cooldown* Cool3 = Node_Cooldown("쿨3", 300);
	CBT_Cooldown* Cool4 = Node_Cooldown("쿨4", 300);

	CBT_SetValue* Show_OffNearAttack = Node_BOOL_SetValue("시연회 OFF", L"Show_Near", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(ThreeCombo_Cut1());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(ThreeCombo_Cut2());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Jump_Attack());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(Ice_Cut());
	Root_Sel->Add_Child(Cool4);
	Cool4->Set_Child(Ice_Barrier());

	Root_Sel->Add_Child(Show_OffNearAttack);

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::Show_FarAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("순서대로 원거리 공격");

	CBT_Cooldown* Cool0 = Node_Cooldown("쿨0", 300);

	CBT_SetValue* Show_ValueOff = Node_BOOL_SetValue("시연회 OFF", L"Show", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Charge_Rush());

	Root_Sel->Add_Child(Show_ValueOff);

	return Root_Sel;
}

void CIceGirl::Down()
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
			m_bAIController = true;

			++m_iDownCount;

			// Down 안에서 쓰는 bool 리셋
			m_bDown_StartAni = true;
			m_dDownTime = 0;

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

	if (true == m_bDown_LoopAni)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_pMeshCom->Reset_OldIndx();
			m_pMeshCom->SetUp_Animation(Ani_Down_Loop);
		}
	}

}

HRESULT CIceGirl::Update_Bone_Of_BlackBoard()
{
	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHand");
	m_vLeftHand = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftHand", m_vLeftHand);

	return S_OK;
}

HRESULT CIceGirl::Update_Value_Of_BB()
{
	// 1. 플레이어 좌표 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_Pos());
	// 2. 체력 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HP", m_tObjParam.fHp_Cur);
	// 3. 체력 비율 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HPRatio", _float(m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max) * 100);


	// 1. 베기 -> 얼음 소환 좌표
	_v3 vSelfLook = *D3DXVec3Normalize(&_v3(), (_v3*)&m_pTransformCom->Get_WorldMat().m[2]);
	_v3 vSelfPos = m_pTransformCom->Get_Pos();

	// 얼음 소환 방향
	_v3 vDir0 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(50)));
	_v3 vDir1 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(25)));
	_v3 vDir3 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-25)));
	_v3 vDir4 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-50)));
	
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Dir0", vDir0);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Dir1", vDir1);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Dir2", vSelfLook);	// 중앙
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Dir3", vDir3);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Dir4", vDir4);

	// 얼음 소환 좌표 = Position + Look * Alpha
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Pos0", vSelfPos + vDir0 * 3.4f);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Pos1", vSelfPos + vDir1 * 3.2f);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Pos2", vSelfPos + vSelfLook * 3.f);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Pos3", vSelfPos + vDir3 * 3.2f);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Pos4", vSelfPos + vDir4 * 3.4f);

	

	// 2. 플레이어 발에 얼음생기는 좌표
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Foot", vSelfPos);


	// 3. 찌르기 검기 생성 위치
	_mat matSwordBullet = static_cast<CTransform*>(m_pSword->Get_Component(L"Com_Transform"))->Get_WorldMat();
	// 칼 끝 좌표
	_v3 vSwordBulletPos = _v3(matSwordBullet.m[3][0], matSwordBullet.m[3][1], matSwordBullet.m[3][2]) + (_v3(0.f, 0.3f, 0.f));

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"CreateSwordBulletPos", vSwordBulletPos);
	
	// 칼 가운데 좌표
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Sword_MidPos", vSwordBulletPos +_v3(matSwordBullet.m[2][0], matSwordBullet.m[2][1], matSwordBullet.m[2][2]) * 2.35f);

	// 4. 회피 방향 4개, // 회피방향도 추가해야함.
	_float fLength = 4.f;
	_v3 vSelfRight = *D3DXVec3Normalize(&_v3(), (_v3*)&m_pTransformCom->Get_WorldMat().m[0]);

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DodgePosF", vSelfPos + fLength * vSelfLook);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DodgePosB", vSelfPos - fLength * vSelfLook);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DodgePosR", vSelfPos + fLength * vSelfRight);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DodgePosL", vSelfPos - fLength * vSelfRight);

	return S_OK;
}

HRESULT CIceGirl::Update_NF()
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

HRESULT CIceGirl::Update_Collider()
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

void CIceGirl::Check_PhyCollider()
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

					m_pMeshCom->SetUp_Animation(Ani_Down_Start);
					m_bDown_StartAni = true;	//down 함수 내부에서 쓸 것임.
					m_pAIControllerCom->Reset_BT();
					m_bAIController = false;

				}
			}

			if (1 == m_iDownCount)
			{
				if (0.4 >= (m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max))
				{
					m_bDown_Start = true;

					m_tObjParam.bDown = true;

					m_pMeshCom->SetUp_Animation(Ani_Down_Start);
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
			Start_Dissolve(0.4f, false, true, 4.2f);
			m_pSword->Start_Dissolve(0.4f, false, false, 4.2f);
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

void CIceGirl::Push_Collider()
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

void CIceGirl::OnCollisionEnter()
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

void CIceGirl::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

HRESULT CIceGirl::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_IceGirl", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
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

HRESULT CIceGirl::SetUp_ConstantTable()
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

HRESULT CIceGirl::Ready_Weapon()
{
	// 오른손 무기
	m_pSword = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pSword->Change_WeaponData(CWeapon::WPN_QueenLance);

	// Trail
	m_pSword->Set_Enable_Trail(true);
	m_pSword->Set_TrailIndex(8);
	m_pSword->Set_TrailUseMask(0);

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pSword->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pSword->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	return S_OK;
}

HRESULT CIceGirl::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

HRESULT CIceGirl::Ready_Collider()
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

	return S_OK;
}

HRESULT CIceGirl::Ready_NF(void * pArg)
{
	INFO eTemp = *(INFO*)pArg;

	m_fFov = eTemp.fFov;
	m_fMaxLength = eTemp.fMaxLength;
	m_fMinLength = eTemp.fMinLength;

	return S_OK;
}

CIceGirl * CIceGirl::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CIceGirl* pInstance = new CIceGirl(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CIceGirl");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CIceGirl::Clone_GameObject(void * pArg)
{
	CIceGirl* pInstance = new CIceGirl(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CIceGirl");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIceGirl::Free()
{
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
