#include "stdafx.h"
#include "..\Headers\DeerKing.h"
#include "..\Headers\Weapon.h"
#include "..\Headers\BossHP.h"
#include "ClothManager.h"

#include "MassageUI.h"

using namespace physx;

CDeerKing::CDeerKing(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CDeerKing::CDeerKing(const CDeerKing & rhs)
	: CMonster(rhs), m_pCloth(rhs.m_pCloth)
{
}

HRESULT CDeerKing::Ready_GameObject_Prototype()
{

	return S_OK;
}

HRESULT CDeerKing::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_NF(pArg);
	Ready_Weapon();
	Ready_BoneMatrix();
	Ready_Collider();
	Ready_Cloth();
	Ready_Sound();

	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 10000.f * pow(1.5f, g_eStageIdx_Cur - 1);
	m_tObjParam.fHp_Max = m_tObjParam.fHp_Cur;
	m_tObjParam.fDamage = 500.f * pow(1.5f, g_eStageIdx_Cur - 1);
	m_tObjParam.fArmor_Cur = 100.f * pow(1.5f, g_eStageIdx_Cur - 1);

	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));


	///////////////// 행동트리 init

	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL);

	if (nullptr == pPlayer)
		return E_FAIL;

	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create();	//인자에 true 주면 콘솔창에 디버깅정보 뜸, default = false

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", m_tObjParam.fHp_Cur);
	pBlackBoard->Set_Value(L"MAXHP", m_tObjParam.fHp_Max);
	pBlackBoard->Set_Value(L"Show", true);
	pBlackBoard->Set_Value(L"Show_Near", true);

	pBlackBoard->Set_Value(L"PushCol", true);	// 충돌여부 제어변수
	//pBlackBoard->Set_Value(L"PhyCol", true); // 피격판정 제어 변수

	//소리 제어 변수
	pBlackBoard->Set_Value(L"SFX_01_Tag", 0);
	pBlackBoard->Set_Value(L"SFX_01_Play", false);
	pBlackBoard->Set_Value(L"SFX_01_Stop", false);
	pBlackBoard->Set_Value(L"SFX_02_Tag", 0);
	pBlackBoard->Set_Value(L"SFX_02_Play", false);
	pBlackBoard->Set_Value(L"SFX_02_Stop", false);
	pBlackBoard->Set_Value(L"Voice_Tag", 0);
	pBlackBoard->Set_Value(L"Voice_Play", false);
	pBlackBoard->Set_Value(L"Voice_Stop", false);
	pBlackBoard->Set_Value(L"Step_Play", false);
	pBlackBoard->Set_Value(L"Step_Tag", 20);

	pBlackBoard->Set_Value(L"CamShake1", false);
	pBlackBoard->Set_Value(L"CamShake2", false);
	pBlackBoard->Set_Value(L"CamShake4", false);


	CBT_Selector* Start_Sel = Node_Selector("행동 시작");
	//CBT_Sequence* Start_Sel = Node_Sequence("행동 시작");	//테스트

	pBehaviorTree->Set_Child(Start_Sel);

	//////////// 아래에 주석해놓은 4줄이 본게임에서 쓸 것임, 차례대로 공격함.

	CBT_CompareValue* Check_ShowValue = Node_BOOL_A_Equal_Value("시연회 변수 체크", L"Show", false);
	Check_ShowValue->Set_Child(Start_Game());
	Start_Sel->Add_Child(Check_ShowValue);
	Start_Sel->Add_Child(Start_Show());

	////////////

	// 패턴 확인용,  각 패턴 함수를 아래에 넣으면 재생됨

	//Start_Sel->Add_Child(MoveAround());
	
	//CBT_RotationDir* Rotation0 = Node_RotationDir("돌기", L"Player_Pos", 0.2);
	//Start_Sel->Add_Child(Rotation0);

	//CBT_Wait* Wait0 = Node_Wait("대기", 1, 0);
	//Start_Sel->Add_Child(Wait0);

	/////////////
	// UI 추가(지원)
	m_pBossUI = static_cast<CBossHP*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BossHP", nullptr));
	m_pBossUI->Set_Target(this);
	m_pBossUI->Set_UI_Pos(WINCX * 0.5f, WINCY * 0.1f);
	m_pBossUI->Set_BossName(CBossNameUI::Index_DeerKing);
	if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBossUI, SCENE_STAGE, L"Layer_BossHP", nullptr)))
		return E_FAIL;

	return S_OK;
}

_int CDeerKing::Update_GameObject(_double TimeDelta)
{
	m_pCloth->putToSleep();

	if (false == m_bEnable)
		return NO_EVENT;

	if (true == m_pAIControllerCom->Get_BoolValue(L"PushCol"))
		Push_Collider();

	CGameObject::Update_GameObject(TimeDelta);

	Update_Effect();

	// 죽었을 경우
	if (m_bIsDead)
	{
		m_bEnable = false;
		g_pPhysx->Get_Scene()->removeActor(*m_pCloth);

		Check_DropItem(MONSTER_NAMETYPE::M_DeerKing);
	}

	// 죽음 애니메이션
	if (m_bReadyDead)
	{
		// 죽기전 UI 비활성화
		m_pBossUI->Set_Active(false);

		if (false == m_bFinishCamShake && m_pMeshCom->Is_Finish_Animation(0.7f))
		{
			m_bFinishCamShake = true;
			SHAKE_CAM_lv3;

			g_pSoundManager->Stop_Sound(CSoundManager::Effect_SFX_01);
			g_pSoundManager->Play_Sound(L"Boss_DeadEff.ogg", CSoundManager::Effect_SFX_01, CSoundManager::Effect_Sound);
		}

		m_fBGMFade -= _float(TimeDelta) * 0.25f;
		if (m_fBGMFade <= 0.f)
			m_fBGMFade = 0.f;
		g_pSoundManager->Set_Volume(CSoundManager::BGM_Volume, m_fBGMFade);

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
		//// 어그로 관리
		Set_Target_Auto(true);

		// 뼈 위치 업데이트
		Update_Bone_Of_BlackBoard();
		// BB 직접 업데이트
		Update_Value_Of_BB();

		if (false == m_bThrow_Shield)
		{	// 2페이지 돌입직전에 방패가 던져질 방향 업데이트
			Update_Dir_Shield_Throwing();
		}

		if (true == m_bAIController)
			m_pAIControllerCom->Update_AIController(TimeDelta);

		// 보스UI 업데이트
		// 체력이 0이 되었을때 밀림현상 방지.
		/*if (0 >= m_tObjParam.fHp_Cur)
			m_pBossUI->Set_BossHPInfo(0, 100);
		else
			m_pBossUI->Set_BossHPInfo(m_tObjParam.fHp_Cur, m_tObjParam.fHp_Max);*/
	}

	if (false == m_bReadyDead)
		Check_PhyCollider();

	OnCollisionEnter();

	m_pTransformCom->Set_Pos(m_pNavMeshCom->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	//====================================================================================================
	// 컬링
	//====================================================================================================
	m_bInFrustum = m_pOptimizationCom->Check_InFrustumforObject(&m_pTransformCom->Get_Pos(), 2.f);
	//====================================================================================================

	if (nullptr != m_pCloth)
		Update_Cloth();

	return NOERROR;
}

_int CDeerKing::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == m_pRendererCom)
		return E_FAIL;

	//=============================================================================================
	// 그림자랑 모션블러는 프리스텀 안에 없으면 안그림
	//=============================================================================================
	if (!m_bDissolve)
	{
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
			return E_FAIL;
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_SHADOWTARGET, this)))
			return E_FAIL;
	}

	else
	{ 
		if (FAILED(m_pRendererCom->Add_RenderList(RENDER_ALPHA, this)))
			return E_FAIL;
	}

	if (m_bInFrustum)
	{
		if (false == m_bDissolve)
		{
			if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
				return E_FAIL;
		}
	}
	//=============================================================================================


	m_dTimeDelta = TimeDelta;

	m_pShield->Late_Update_GameObject(TimeDelta);

	if (false == m_bThrow_Shield)
		m_pShield->Update_GameObject(m_dTimeDelta);
	else
		Update_Shield();

	return NOERROR;
}

HRESULT CDeerKing::Render_GameObject()
{
	//if (nullptr == m_pShaderCom ||
	//	nullptr == m_pMeshCom)
	//	return E_FAIL;

	//m_pMeshCom->Play_Animation(_float(m_dTimeDelta)); // * alpha

	//if (m_bInFrustum)
	//{
	//	if (FAILED(SetUp_ConstantTable(m_pShaderCom)))
	//		return E_FAIL;

	//	m_pShaderCom->Begin_Shader();

	//	_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

	//	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	//	{
	//		_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

	//		m_pMeshCom->Update_SkinnedMesh(i);

	//		for (_uint j = 0; j < iNumSubSet; ++j)
	//		{
	//			m_iPass = m_pMeshCom->Get_MaterialPass(i, j);

	//			if (m_bDissolve)
	//				m_iPass = 3;

	//			m_pShaderCom->Begin_Pass(m_iPass);

	//			m_pShaderCom->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

	//			m_pShaderCom->Commit_Changes();

	//			m_pMeshCom->Render_Mesh(i, j);

	//			m_pShaderCom->End_Pass();
	//		}
	//	}

	//	m_pShaderCom->End_Shader();
	//}

	//Update_Collider();
	//Draw_Collider();

	return NOERROR;
}

HRESULT CDeerKing::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	if (nullptr != m_pCloth)
		Change_Vertex();

	m_pMeshCom->Play_Animation(DELTA_60 * m_dAniPlayMul);

	if (m_bInFrustum)
	{
		if (FAILED(SetUp_ConstantTable(pShader)))
			return E_FAIL;

		_uint iNumMeshContainer = m_pMeshCom->Get_NumMeshContainer();

		for (_uint i = 0; i < iNumMeshContainer; ++i)
		{
			_uint iNumSubSet = m_pMeshCom->Get_NumMaterials(i);

			m_pMeshCom->Update_SkinnedMesh(i);

			for (_uint j = 0; j < iNumSubSet; ++j)
			{
				m_iPass = !m_bDissolve ? m_pMeshCom->Get_MaterialPass(i, j) : 3;

				pShader->Begin_Pass(m_iPass);

				pShader->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

				pShader->Commit_Changes();

				m_pMeshCom->Render_Mesh(i, j);

				pShader->End_Pass();
			}
		}

	}

	Update_Collider();
	Draw_Collider();

	return S_OK;
}

HRESULT CDeerKing::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	//============================================================================================
	// 공통 변수
	//============================================================================================
	_mat	WorldMatrix = m_pTransformCom->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &WorldMatrix, sizeof(_mat))))
		return E_FAIL;

	//============================================================================================
	// 모션 블러 상수
	//============================================================================================
	if (_bIsForMotionBlur)
	{
		_mat	ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
		_mat	ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);

		if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
			return E_FAIL;

		m_matLastWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		_float fBloomPower = 0.5f;
		if (FAILED(pShader->Set_Value("g_fBloomPower", &fBloomPower, sizeof(_float))))
			return E_FAIL;
	}

	//============================================================================================
	// 쉐이더 실행
	//============================================================================================
	_uint iNumMeshContainer = m_pMeshCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		_uint iNumSubSet = m_pMeshCom->Get_NumMaterials(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			pShader->Begin_Pass(iPass);

			pShader->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}
	//============================================================================================

	return S_OK;
}

CBT_Composite_Node * CDeerKing::LeftHand_Attack(_float fWeight)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("왼팔 휘두르기");
	CBT_Play_Ani* Show_Ani25 = Node_Ani("왼팔 휘두르기", 25, fWeight);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.05, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.366, 0);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("목소리 재생", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("목소리 재생", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("목소리 이름 설정", L"Voice_Tag", 10);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.134, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 5.f, 0.216, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0", L"Bone_LeftHand", 0.4, 60, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Mid_0", L"Bone_LeftHand", 0.4, 60, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Mid_1", L"Bone_LeftHand", 0.4, 60, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani25);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", &m_tObjParam, CBT_UpdateParam::Collider, 0.666, 1, 0.117, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::RightFoot_Attack(_float fWeight)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("오른발로 찍기");
	CBT_Play_Ani* Show_Ani28 = Node_Ani("오른발로 찍기", 28, fWeight);
	//CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.05, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.2);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.766, 0);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("카메라 흔들기", L"CamShake2", true);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("목소리 재생", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("목소리 재생", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("목소리 이름 설정", L"Voice_Tag", 10);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 1);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.567, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 1.f, 0.217, 0);
	
	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0"	, L"ShieldPos"	, 0.9, 1, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("눈 위로 폭발1", L"DeerKing_SnowChunk_Up_Particle_1"	, L"ShieldPos"	, 0.9, 1, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("눈 위로 폭발2", L"DeerKing_SnowChunk_Up_Particle_2"	, L"ShieldPos"	, 0.9, 2, 0.01, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("길쭉한 눈"	, L"DeerKing_Snow_Up_LongLine_0"		, L"ShieldPos"	, 0.8, 2, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Particle_0"	, L"ShieldPos"	, 0.9, 2, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"					, L"ShieldPos"	, 0.9, 20, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"							, L"ShieldPos"	, 0.9, 5, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"							, L"ShieldPos"	, 0.9, 5, 0.01, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"							, L"ShieldPos"	, 1.05, 1, 0.01, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("눈 위로 폭발2", L"DeerKing_SnowChunk_Heavy_Particle_0", L"ShieldPos"	, 0.9, 1, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_LongLine_1"				, L"ShieldPos", nullptr, L"Self_Dir", L"V3_Null", 0.9, 5, 0.1, 0);
	CBT_CreateEffect* Effect11 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_Particle_0"				, L"ShieldPos", nullptr, L"Self_Dir", L"V3_Null", 0.9, 10, 0.1, 0);

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

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani28);
	//MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.816, 1, 0.1, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Rush_RightFoot()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("돌진");
	CBT_Play_Ani* Show_Ani27 = Node_Ani("돌진", 27, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("방향 추적", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.066);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", -1.f, 0.3, 0);
	CBT_Wait* Wait1 = Node_Wait("대기", 0.117, 0);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("카메라 흔들기", L"CamShake1", true);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("목소리 재생", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("목소리 재생", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("목소리 이름 설정", L"Voice_Tag", 10);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 1);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 13.f, 0.267, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 1.f, 0.333, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0", L"ShieldPos"	, 0.4, 20, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("눈 위로 폭발1", L"DeerKing_SnowChunk_Up_Particle_1", L"ShieldPos"	, 0.4, 20, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Particle_0", L"ShieldPos", 0.4, 20, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0", L"ShieldPos"				, 0.4, 25, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0", L"ShieldPos"						, 0.4, 5, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1", L"ShieldPos"						, 0.5, 5, 0.01, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2", L"ShieldPos"						, 0.6, 5, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_LongLine_1", L"ShieldPos", nullptr, L"Self_Dir", L"V3_Null", 0.4, 20, 0.1, 0);
	CBT_CreateEffect* Effect8 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_Particle_0", L"Self_Pos_Front", nullptr, L"Self_Dir", L"V3_Null", 0.4, 20, 0.1, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);
	Root_Parallel->Add_Service(Effect8);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani27);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.466, 1, 0.384, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::WhirlWind_RightFoot()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("휠윈드");
	CBT_Play_Ani* Show_Ani26 = Node_Ani("휠윈드", 26, 0.9f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	//CBT_Wait* Wait0 = Node_Wait("대기", 0.233, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.433);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("목소리 재생", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("목소리 재생", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("목소리 이름 설정", L"Voice_Tag", 10);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 5.f, 1, 0);
	CBT_Wait* Wait1 = Node_Wait("대기", 0.417, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 2.f, 0.366, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0", L"ShieldPos"	, 0.6, 40, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0", L"ShieldPos"			, 0.6, 5, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1", L"ShieldPos"			, 0.6, 5, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2", L"ShieldPos"			, 0.7, 1, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2", L"ShieldPos"			, 1.4, 1, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_LongLine_1", L"ShieldPos", nullptr, L"Self_Dir", L"V3_Null", 0.6, 40, 0.1, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani26);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	//SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.433, 1, 1, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Head_ColdBeam()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("머리 고드름");
	CBT_Play_Ani* Show_Ani31 = Node_Ani("머리 고드름", 31, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("대기", 0.6, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.4, 0);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("카메라 흔들기", L"CamShake2", true);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("목소리 재생", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("목소리 재생", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("목소리 이름 설정", L"Voice_Tag", 10);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 1);
	CBT_Wait* Wait1 = Node_Wait("대기", 0.7, 0);
	CBT_SetValue* CamShake1 = Node_BOOL_SetValue("카메라 흔들기", L"CamShake2", true);
	CBT_SetValue* Sound2Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound2Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound2Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 2);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 1.f, 0.25, 0);
	CBT_Wait* Wait2 = Node_Wait("대기", 0.616, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 2.f, 0.234, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"					, L"Bone_LeftCorner", 0.9, 5, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Small_0"					, L"Bone_LeftCorner", 0.9, 5, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Small_1"					, L"Bone_LeftCorner", 0.9, 5, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("얼음", L"DeerKing_IceStone_Up_Small_Particle_0"		, L"Bone_LeftCorner", 0.9, 1, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("눈 덩어리", L"DeerKing_SnowChunk_Up_Small_Particle_0"	, L"Bone_LeftCorner", 0.9, 2, 0.01, 0);

	CBT_CreateEffect* Effect5 = Node_New_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0"	, L"Bone_LeftCorner", nullptr, L"Self_Dir", L"V3_Null", 1.55, 5, 0.1, 0);
	CBT_CreateEffect* Effect6 = Node_New_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_1"	, L"Bone_LeftCorner", nullptr, L"Self_Dir", L"V3_Null", 1.55, 5, 0.1, 0);
	CBT_CreateEffect* Effect7 = Node_New_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_2"	, L"Bone_LeftCorner", nullptr, L"Self_Dir", L"V3_Null", 1.55, 5, 0.1, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Particle_0"		, L"Bone_LeftCorner"	, 1.55, 2, 0.01, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"						, L"Bone_LeftCorner"	, 1.55, 15, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"							, L"Bone_LeftCorner"	, 1.55, 2, 0.01, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"							, L"Bone_LeftCorner"	, 1.55, 2, 0.01, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"							, L"Bone_LeftCorner"	, 1.55, 2, 0.01, 0);
	CBT_CreateEffect* Effect13 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_LongLine_1"					, L"Bone_LeftCorner", nullptr, L"Self_Dir", L"V3_Null", 1.55, 3, 0.1, 0);
	CBT_CreateEffect* Effect14 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_Particle_0"					, L"Bone_LeftCorner", nullptr, L"Self_Dir", L"V3_Null", 1.54, 5, 0.1, 0);

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

	// 고드름 생성
	CBT_CreateBullet* Col0 = Node_CreateBullet("충돌체", L"Monster_DeerKingColdBeam", L"ColdBeam_Left_Pos", L"ColdBeam_Left_Dir"	, 0, 1, 1.766, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col1 = Node_CreateBullet("충돌체", L"Monster_DeerKingColdBeam", L"ColdBeam_Mid_Pos", L"ColdBeam_Mid_Dir"		, 0, 1, 1.766, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col2 = Node_CreateBullet("충돌체", L"Monster_DeerKingColdBeam", L"ColdBeam_Right_Pos", L"ColdBeam_Right_Dir"	, 0, 1, 1.766, 1, 0, 0, CBT_Service_Node::Finite);

	Root_Parallel->Add_Service(Col0);
	Root_Parallel->Add_Service(Col1);
	Root_Parallel->Add_Service(Col2);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani31);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(CamShake1);
	SubSeq->Add_Child(Sound2Stop);
	SubSeq->Add_Child(Sound2Play);
	SubSeq->Add_Child(Sound2Tag);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Slide_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("머리 고드름");
	CBT_Play_Ani* Show_Ani22 = Node_Ani("슬라이드 시작", 22, 0.8f);
	CBT_Play_Ani* Show_Ani23 = Node_Ani("슬라이드 루프", 23, 0.95f);
	CBT_Play_Ani* Show_Ani24 = Node_Ani("슬라이드 끝", 24, 0.85f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", -2.f, 0.316, 0);
	//CBT_Wait* Wait0 = Node_Wait("대기", 0.334, 0);
	CBT_Wait* Wait0 = Node_Wait("대기", 0.45, 0);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("목소리 재생", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("목소리 재생", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("목소리 이름 설정", L"Voice_Tag", 10);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 3);
	// 애니28번 * 0.8 = 0.866
	// 애니29번 * 0.95 = 0.259
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	//CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 20.f, 0.116 + 0.259 + 0.066, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 20.f, 0.259 + 0.066, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 10.f, 0.1, 0);
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 5.f, 0.1, 0);
	CBT_MoveDirectly* Move4 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.1, 0);
	CBT_MoveDirectly* Move5 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 1.f, 0.1, 0);

	CBT_CreateEffect* Effect0 = Node_New_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0"	, L"Self_Pos", nullptr, L"Self_Dir", L"V3_Null", 0.6, 20, 0.1, 0);
	CBT_CreateEffect* Effect1 = Node_New_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_1"	, L"Self_Pos", nullptr, L"Self_Dir", L"V3_Null", 0.6, 20, 0.1, 0);
	CBT_CreateEffect* Effect8 = Node_New_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_2"	, L"Self_Pos", nullptr, L"Self_Dir", L"V3_Null", 0.6, 20, 0.1, 0);
	CBT_CreateEffect* Effect11 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_LongLine_1"					, L"Self_Pos", nullptr, L"Self_Dir", L"V3_Null", 0.6, 20, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Small_Particle_0", L"Self_Pos"		, 0.7, 20, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"						, L"Self_Pos"		, 0.6, 20, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"								, L"Self_Pos_Front"	, 0.8, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"								, L"Self_Pos_Front"	, 0.8, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"								, L"Self_Pos_Front"	, 0.8, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Mid_0"							, L"Self_Pos_Front"	, 0.6, 25, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Mid_1"						, L"Self_Pos_Front"	, 0.6, 25, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("길쭉한 눈", L"DeerKing_Snow_Up_LongLine_0"				, L"Self_Pos"		, 0.6, 25, 0.01, 0);
	CBT_CreateEffect* Effect12 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_LongLine_1", L"Self_Pos", nullptr, L"Self_Dir", L"V3_Null", 0.6, 20, 0.1, 0);
	CBT_CreateEffect* Effect13 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_Particle_0", L"Self_Pos_Front", nullptr, L"Self_Dir", L"V3_Null", 0.6, 25, 0.1, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("", L"DeerKing_Distortion_Circle", L"Bone_RightHand", 0.95, 1, 0.01, 0);

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

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani22);
	MainSeq->Add_Child(Show_Ani23);
	MainSeq->Add_Child(Show_Ani24);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(PushColOn);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(Move3);
	SubSeq->Add_Child(Move4);
	SubSeq->Add_Child(Move5);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.75, 1, 0.441, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Jump_Attack(_float fWeight)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("점프해서 방패 찍기");
	CBT_Play_Ani* Show_Ani29 = Node_Ani("점프해서 방패 찍기", 29, fWeight);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.466, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 1.f, 0.2, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 10.f, 0.584, 0);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("카메라 흔들기", L"CamShake4", true);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("목소리 재생", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("목소리 재생", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("목소리 이름 설정", L"Voice_Tag", 10);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 1);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.85, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 0.7f, 0.316, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0"	, L"Self_Pos", 0.5, 3, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("눈 위로 폭발1", L"DeerKing_SnowChunk_Up_Particle_1"	, L"Self_Pos", 0.5, 3, 0.01, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("길쭉한 눈", L"DeerKing_Snow_Up_LongLine_0"			, L"Self_Pos", 0.5, 2, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Particle_0"	, L"Self_Pos", 0.5, 2, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"					, L"Self_Pos", 0.5, 10, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"							, L"Self_Pos", 0.5, 5, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"							, L"Self_Pos", 0.5, 5, 0.01, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"							, L"Self_Pos", 0.5, 1, 0.01, 0);

	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0"		, L"Self_Pos", 1.4, 3, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("눈 위로 폭발1", L"DeerKing_SnowChunk_Up_Particle_1"		, L"Self_Pos", 1.4, 3, 0.01, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("눈 위로 폭발2", L"DeerKing_SnowChunk_Heavy_Particle_0"	, L"Self_Pos", 1.4, 1, 0.01, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("길쭉한 눈", L"DeerKing_Snow_Up_LongLine_0"				, L"Self_Pos", 1.4, 2, 0.01, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Particle_0"		, L"Self_Pos", 1.4, 2, 0.01, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"					, L"Self_Pos", 1.4, 10, 0.01, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"							, L"Self_Pos", 1.4, 5, 0.01, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"							, L"Self_Pos", 1.4, 5, 0.01, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"							, L"Self_Pos", 1.4, 1, 0.01, 0);
	CBT_CreateEffect* Effect18 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_LongLine_1"					, L"Self_Pos", nullptr, L"Self_Dir", L"V3_Null", 1.4, 5, 0.1, 0);
	CBT_CreateEffect* Effect19 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_Particle_0"					, L"Self_Pos", nullptr, L"Self_Dir", L"V3_Null", 1.4, 10, 0.1, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
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

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani29);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("무기 히트 On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.35, 1, 0.1, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Throwing()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");
	
	CBT_Sequence* MainSeq = Node_Sequence("투사체 던지기");
	CBT_Play_Ani* Show_Ani21 = Node_Ani("투사체 던지기", 21, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("대기0", 1.033, 0);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("목소리 재생", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("목소리 재생", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("목소리 이름 설정", L"Voice_Tag", 10);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 1.f, 0.383, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.417, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 1.f, 0.25, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani21);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("버프 전 손 오오라", L"DeerKing_IceBullet_Charge_Hand_Smoke_0"			, L"Bone_LeftHand", 0.1, 55, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("버프 전 손 오오라", L"DeerKing_IceBullet_Charge_Hand_Smoke_After_0"	, L"Bone_LeftHand", 1.35, 10, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("버프 전 손 오오라", L"DeerKing_IceBullet_Charge_Hand_Smoke_After_1"	, L"Bone_LeftHand", 1.35, 10, 0, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("왜곡"				, L"DeerKing_Distortion_Circle"						, L"Bone_LeftHand", 1.55, 1, 0, 0);
	
	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);

	CBT_CreateBullet* Col0 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos0", L"Self_Dir", 20, 4, 1.516, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col1 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos1", L"Self_Dir", 20, 4, 1.666, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col2 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos2", L"Self_Dir", 20, 4, 1.716, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col3 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos3", L"Self_Dir", 20, 4, 1.866, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col4 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos4", L"Self_Dir", 20, 4, 1.916, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col5 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos5", L"Self_Dir", 20, 4, 2.066, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col6 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos6", L"Self_Dir", 20, 4, 2.116, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col7 = Node_CreateBullet("토네이도 충돌체", L"Monster_DeerKingBullet", L"Throwing_Pos7", L"Self_Dir", 20, 4, 2.266, 1, 0, 0, CBT_Service_Node::Finite);

	Root_Parallel->Add_Service(Col0);
	Root_Parallel->Add_Service(Col1);
	Root_Parallel->Add_Service(Col2);
	Root_Parallel->Add_Service(Col3);
	Root_Parallel->Add_Service(Col4);
	Root_Parallel->Add_Service(Col5);
	Root_Parallel->Add_Service(Col6);
	Root_Parallel->Add_Service(Col7);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::MoveAround()
{
	CBT_Selector* Move_Sel = Node_Selector_Random("서성이기");

	Move_Sel->Add_Child(LeftMoveAround());
	Move_Sel->Add_Child(RightMoveAround());

	return Move_Sel;
}

CBT_Composite_Node * CDeerKing::LeftMoveAround()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("왼쪽 이동 서성임");

	CBT_Sequence* MainSeq = Node_Sequence("왼쪽 이동 서성임");
	CBT_Play_Ani* Show_Ani6 = Node_Ani("왼쪽 이동", 6, 0.f);
	CBT_SetValue* StepPlay = Node_BOOL_SetValue("발소리 재생", L"Step_Play", true);
	CBT_MoveAround*	MoveAround0 = Node_MoveAround("왼쪽으로 서성 이동", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 2.f, 1.5, 0.7);
	CBT_SetValue* StepStop = Node_BOOL_SetValue("발소리 재생", L"Step_Play", false);

	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("플레이어 추적", L"Player_Pos", 5, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani6);
	MainSeq->Add_Child(StepPlay);
	MainSeq->Add_Child(MoveAround0);
	MainSeq->Add_Child(StepStop);

	Root_Parallel->Set_Sub_Child(ChaseDir0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::RightMoveAround()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("오른쪽 이동 서성임");

	CBT_Sequence* MainSeq = Node_Sequence("오른쪽 이동 서성임");
	CBT_Play_Ani* Show_Ani5 = Node_Ani("오른쪽 이동", 5, 0.f);
	CBT_SetValue* StepPlay = Node_BOOL_SetValue("발소리 재생", L"Step_Play", true);
	CBT_MoveAround*	MoveAround0 = Node_MoveAround("왼쪽으로 서성 이동", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", -2.f, 1.5, 0.7);
	CBT_SetValue* StepStop = Node_BOOL_SetValue("발소리 재생", L"Step_Play", false);

	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("플레이어 추적", L"Player_Pos", 5, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani5);
	MainSeq->Add_Child(StepPlay);
	MainSeq->Add_Child(MoveAround0);
	MainSeq->Add_Child(StepStop);

	Root_Parallel->Set_Sub_Child(ChaseDir0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Jump_In_Place()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("날기");
	CBT_Play_Ani* Show_Ani37 = Node_Ani("날기 시작", 37, 0.85f);
	CBT_Play_Ani* Show_Ani38 = Node_Ani("떨어짐 시작", 38, 0.9f);
	CBT_Play_Ani* Show_Ani40 = Node_Ani("떨어짐 끝", 40, 0.9f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", 0.3, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.5);
	// Show_Ani43 * 0.85 = 0.85
	// Show_Ani44 * 0.9 = 0.90
	CBT_Wait* Wait1 = Node_Wait("대기0", 0.662, 0);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("카메라 흔들기", L"CamShake4", true);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("목소리 재생", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("목소리 재생", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("목소리 이름 설정", L"Voice_Tag", 12);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 1);
	CBT_SetValue* Sound2Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Stop", true);
	CBT_SetValue* Sound2Play = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Play", true);
	CBT_SetValue* Sound2Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_02_Tag", 5);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0"		, L"Self_Pos", 0.5, 3, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("눈 위로 폭발1", L"DeerKing_SnowChunk_Up_Particle_1"		, L"Self_Pos", 0.5, 3, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Particle_0"		, L"Self_Pos", 0.5, 2, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"						, L"Self_Pos", 0.5, 10, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"								, L"Self_Pos", 0.5, 5, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"								, L"Self_Pos", 0.5, 5, 0.01, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"								, L"Self_Pos", 0.5, 1, 0.01, 0);

	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet"						, L"Bone_LeftJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet"						, L"Bone_RightJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_01"			, L"Bone_LeftJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect21 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_01"			, L"Bone_RightJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect22 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_02"			, L"Bone_LeftJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect23 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_03"			, L"Bone_RightJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect24 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_GunEff"				, L"Bone_LeftJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect25 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_GunEff"				, L"Bone_RightJet"	, 0.5, 35, 0.01, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("손 냉기", L"DeerKing_IceSmoke_Mid_1"						, L"Bone_LeftHand"	, 0.5, 65, 0.01, 0);
	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("손 냉기", L"DeerKing_IceSmoke_Mid_1"						, L"Bone_RightHand"	, 0.5, 65, 0.01, 0);

	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("눈 위로 폭발0", L"DeerKing_SnowChunk_Up_Particle_0"		, L"Self_Pos", 1.5, 3, 0.01, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("눈 위로 폭발1", L"DeerKing_SnowChunk_Up_Particle_1"		, L"Self_Pos", 1.5, 3, 0.01, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("눈 위로 폭발2", L"DeerKing_SnowChunk_Heavy_Particle_0"	, L"Self_Pos", 1.5, 1, 0.01, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Particle_0"		, L"Self_Pos", 1.5, 10, 0.01, 0);
	CBT_CreateEffect* Effect27 = Node_CreateEffect_Finite("얼음 위로 폭발0", L"DeerKing_IceStone_Up_Particle_0"		, L"Self_Pos", 1.5, 10, 0.01, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"					, L"Self_Pos", 1.5, 10, 0.01, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"							, L"Self_Pos", 1.5, 5, 0.01, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"							, L"Self_Pos", 1.5, 5, 0.01, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"							, L"Self_Pos", 1.5, 1, 0.01, 0);
	CBT_CreateEffect* Effect26 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_LongLine_1"					, L"Self_Pos", nullptr, L"Self_Dir", L"V3_Null", 1.5, 5, 0.1, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
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
	Root_Parallel->Add_Service(Effect23);
	Root_Parallel->Add_Service(Effect24);
	Root_Parallel->Add_Service(Effect25);
	Root_Parallel->Add_Service(Effect26);
	Root_Parallel->Add_Service(Effect27);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani37);
	MainSeq->Add_Child(Show_Ani38);
	MainSeq->Add_Child(Show_Ani40);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Sound2Stop);
	SubSeq->Add_Child(Sound2Play);
	SubSeq->Add_Child(Sound2Tag);

	CBT_CreateBullet* Col0 = Node_CreateBullet("점프 찍기 충돌체", L"Monster_DeerKingJumpInPlaceCol", L"SelfPos", L"", 0, 0.1, 1.023 + 0.239 + 0.2, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Rush_Body()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("돌진 몸통 박치기");
	CBT_Play_Ani* Show_Ani45 = Node_Ani("돌진 시작", 45, 0.85f);
	CBT_Play_Ani* Show_Ani46 = Node_Ani("돌진 중", 46, 0.95f);
	CBT_Play_Ani* Show_Ani48 = Node_Ani("돌진 끝", 48, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	//CBT_Wait* Wait0 = Node_Wait("대기0", 0.083, 0);
	CBT_ChaseDir* Chase0 = Node_ChaseDir("추적", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 1.f, 0.1, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 2.f, 0.3, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", 5.f, 0.3, 0);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("목소리 재생", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("목소리 재생", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("목소리 이름 설정", L"Voice_Tag", 10);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_SetValue* Sound2Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Stop", true);
	CBT_SetValue* Sound2Play = Node_BOOL_SetValue("소리1 재생", L"SFX_02_Play", true);
	CBT_SetValue* Sound2Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_02_Tag", 4);
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("이동3", L"Monster_Speed", L"Monster_Dir", 25.f, 0.475, 0);
	CBT_MoveDirectly* Move4 = Node_MoveDirectly_Rush("이동4", L"Monster_Speed", L"Monster_Dir", 3.f, 0.433, 0);
	CBT_MoveDirectly* Move5 = Node_MoveDirectly_Rush("이동5", L"Monster_Speed", L"Monster_Dir", 2.f, 0.433, 0);
	CBT_MoveDirectly* Move6 = Node_MoveDirectly_Rush("이동6", L"Monster_Speed", L"Monster_Dir", 1.f, 0.433, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);
	//0.617 + 0.142 + 0.416 (돌진 속도) = 1.175

	// 밀림 처리 뺴줘야함.

	// Show_Ani50 * 0.85 = 0.906
	// Show_Ani51 * 0.95 = 0.142

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Mid"			, L"Bone_LeftJet"	, 0.5, 65, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Mid"			, L"Bone_RightJet"	, 0.5, 65, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_01"	, L"Bone_LeftJet"	, 0.5, 65, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_01"	, L"Bone_RightJet"	, 0.5, 65, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_02"	, L"Bone_LeftJet"	, 0.5, 65, 0.01, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_02"	, L"Bone_RightJet"	, 0.5, 65, 0.01, 0);

	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"				, L"Self_Pos"		, 0.6, 20, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"						, L"Self_Pos_Front"	, 0.8, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"						, L"Self_Pos_Front"	, 0.8, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"						, L"Self_Pos_Front"	, 0.8, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Mid_0"					, L"Self_Pos_Front"	, 0.6, 25, 0.01, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Mid_1"					, L"Self_Pos_Front"	, 0.6, 25, 0.01, 0);
	CBT_CreateEffect* Effect12 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_LongLine_1"			, L"Self_Pos_Front", nullptr, L"Self_Dir", L"V3_Null", 0.7, 10, 0.1, 0);
	CBT_CreateEffect* Effect13 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_Particle_0"			, L"Self_Pos_Front", nullptr, L"Self_Dir", L"V3_Null", 0.6, 25, 0.1, 0);

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
	MainSeq->Add_Child(Show_Ani45);
	MainSeq->Add_Child(Show_Ani46);
	MainSeq->Add_Child(Show_Ani48);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	//SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Chase0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Sound2Stop);
	SubSeq->Add_Child(Sound2Play);
	SubSeq->Add_Child(Sound2Tag);
	SubSeq->Add_Child(Move3);
	SubSeq->Add_Child(Move4);
	SubSeq->Add_Child(Move5);
	SubSeq->Add_Child(Move6);
	SubSeq->Add_Child(PushColOn);

	CBT_CreateBuff* Col0 = Node_CreateBuff("몸통 충돌체", L"Monster_DeerKingRushCol", 0.475, 0.9, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);


	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Jump_Fist()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("점프 내려치기");
	CBT_Play_Ani* Show_Ani42 = Node_Ani("점프 내려치기 시작", 42, 0.9f);
	CBT_Play_Ani* Show_Ani43 = Node_Ani("점프 내려치기 중", 43, 0.95f);
	CBT_Play_Ani* Show_Ani44 = Node_Ani("점프 내려치기 끝", 44, 0.9f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");

	CBT_Wait* Wait0 = Node_Wait("대기", 0.6, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.15);
	// Show_Ani47 * 0.9 = 0.99
	// Show_Ani48 * 0.95 = 0.031
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("목소리 재생", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("목소리 재생", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("목소리 이름 설정", L"Voice_Tag", 10);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 1);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("카메라 흔들기", L"CamShake4", true);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet"				, L"Bone_LeftJet"	, 0.2, 50, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet"				, L"Bone_RightJet"	, 0.2, 50, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_01"	, L"Bone_LeftJet"	, 0.2, 60, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_01"	, L"Bone_RightJet"	, 0.2, 60, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_02"	, L"Bone_LeftJet"	, 0.2, 60, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("제트팩 불꽃", L"FireBoy_FireBullet_Particle_02"	, L"Bone_RightJet"	, 0.2, 60, 0.01, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("제트팩 불꽃", L"DeerKing_JumpPack_Fire"			, L"Bone_LeftJet"	, 0.2, 50, 0.01, 0.1);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("제트팩 불꽃", L"DeerKing_JumpPack_Fire"			, L"Bone_RightJet"	, 0.2, 50, 0.01, 0.1);

	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"				, L"Self_Pos"		, 1.25, 10, 0.01, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"						, L"Self_Pos_Front"	, 1.25, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"					, L"Self_Pos_Front"	, 1.25, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"					, L"Self_Pos_Front"	, 1.25, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("연기", L"DeerKing_SnowChunk_Up_Particle_0"		, L"Self_Pos"		, 1.25, 25, 0.01, 0);
	//CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("연기", L"DeerKing_SnowChunk_Up_Particle_1"		, L"Self_Pos"		, 1.25, 25, 0.01, 0);
	CBT_CreateEffect* Effect21 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_LongLine_1"			, L"Self_Pos", nullptr, L"Self_Dir", L"V3_Null", 1.25, 10, 0.1, 0);
	CBT_CreateEffect* Effect22 = Node_New_CreateEffect_Finite("", L"DeerKing_Snow_Up_Particle_0"			, L"Self_Pos", nullptr, L"Self_Dir", L"V3_Null", 1.24, 20, 0.1, 0);

	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0"			, L"Self_Pos"		, 0.53, 20, 0.01, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_0"					, L"Self_Pos_Front"	, 0.53, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_1"					, L"Self_Pos_Front"	, 0.53, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_2"					, L"Self_Pos_Front"	, 0.53, 2, 0.01, 0.2);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("연기", L"DeerKing_SnowChunk_Up_Particle_0"		, L"Self_Pos"		, 0.53, 25, 0.01, 0);
	//CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("연기", L"DeerKing_SnowChunk_Up_Particle_1"		, L"Self_Pos"		, 0.53, 25, 0.01, 0);
	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("왜곡", L"DeerKing_JumpPack_Distortion"			, L"Self_Pos"		, 0.53, 1, 0.01, 0);

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
	//Root_Parallel->Add_Service(Effect13);
	Root_Parallel->Add_Service(Effect14);
	Root_Parallel->Add_Service(Effect15);
	Root_Parallel->Add_Service(Effect16);
	Root_Parallel->Add_Service(Effect17);
	Root_Parallel->Add_Service(Effect18);
	//Root_Parallel->Add_Service(Effect19);
	Root_Parallel->Add_Service(Effect20);
	Root_Parallel->Add_Service(Effect21);
	Root_Parallel->Add_Service(Effect22);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani42);
	MainSeq->Add_Child(Show_Ani43);
	MainSeq->Add_Child(Show_Ani44);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Chase_Timer(0.554, 12.f));
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay); 
	SubSeq->Add_Child(VoiceTag);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);

	CBT_CreateBullet* Col0 = Node_CreateBullet("점프 찍기 충돌체", L"Monster_DeerKingJumpFistCol", L"JumpFist_Pos", L"", 0, 0.1, 0.99 + 0.031 + 0.35, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Blade_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("왼손 얼음칼 베기 ");
	CBT_Play_Ani* Show_Ani32 = Node_Ani("왼손 얼음칼 베기", 32, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.033, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.2);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", -1.f, 0.333, 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 1.034, 0);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("목소리 재생", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("목소리 재생", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("목소리 이름 설정", L"Voice_Tag", 10);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", 5.f, 0.316, 0);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("카메라 흔들기", L"CamShake2", true);
	CBT_Wait* Wait2 = Node_Wait("대기2", 0.567, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("이동2", L"Monster_Speed", L"Monster_Dir", -1.f, 0.383, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani32);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);

	CBT_CreateBuff* Col0 = Node_CreateBuff("얼음검 소환", L"Monster_DeerKingIceSword", 0.684 + 0.2, 1.366 - 0.2, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::RightHand_Attack(_float fWeight)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("오른팔 휘두르기");
	CBT_Play_Ani* Show_Ani41 = Node_Ani("오른팔 휘두르기", 41, fWeight);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("기본", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기", 0.2, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("돌기0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("이동0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.666, 0);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("목소리 재생", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("목소리 재생", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("목소리 이름 설정", L"Voice_Tag", 10);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("소리1 재생", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("소리1 이름 설정", L"SFX_01_Tag", 0);
	CBT_Wait* Wait1 = Node_Wait("대기1", 0.35, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("이동1", L"Monster_Speed", L"Monster_Dir", -1.f, 0.117, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("잔눈", L"DeerKing_Snow_Up_Particle_0", L"Bone_RightHand", 0.4, 40, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Mid_0", L"Bone_RightHand", 0.4, 40, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("연기", L"DeerKing_IceSmoke_Mid_1", L"Bone_RightHand", 0.4, 40, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("왜곡", L"DeerKing_Distortion_Circle", L"Bone_RightHand", 0.9, 1, 0, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani41);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_CreateBullet* Col0 = Node_CreateBullet("오른손 휘두르기 충돌체", L"Monster_DeerKingRightHandCol", L"RightHandCol_Pos", L"", 0, 0.11, 0.85, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::Chase_Timer(_double dRunTime, _float fSpeed)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("병렬");

	CBT_Sequence* MainSeq = Node_Sequence("이동");
	CBT_Wait* Wait0 = Node_Wait("대기0", dRunTime, 0);

	CBT_Sequence* SubSeq = Node_Sequence("추적");
	CBT_MoveDirectly* Chase0 = Node_MoveDirectly_Chase("추적", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", fSpeed, 2);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Wait0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Chase0);

	return Root_Parallel;
}

CBT_Composite_Node * CDeerKing::RightFoot_Attack__Rush_Or_WhirlWind()
{
	CBT_Sequence* Root_Seq = Node_Sequence("오른발로 찍고, 돌진이나 돌기");

	CBT_Selector* Random_Sel = Node_Selector_Random("돌진이나 돌기");

	Root_Seq->Add_Child(RightFoot_Attack(0.5f));
	Root_Seq->Add_Child(Random_Sel);
	Random_Sel->Add_Child(Rush_RightFoot());
	Random_Sel->Add_Child(WhirlWind_RightFoot());

	return Root_Seq;
}

CBT_Composite_Node * CDeerKing::Smart_Three_Attack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("똑똑한 3연타");

	CBT_Selector* Random_Sel = Node_Selector_Random("2타 Or 3타");

	Root_Seq->Add_Child(LeftHand_Attack(0.8f));

	Root_Seq->Add_Child(Random_Sel);
	Random_Sel->Add_Child(RightFoot_Attack());
	Random_Sel->Add_Child(RightFoot_Attack__Rush_Or_WhirlWind());

	return Root_Seq;
}

CBT_Composite_Node * CDeerKing::Smart_JumpAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("점프 어택 연계");

	CBT_Selector* Random_Sel0 = Node_Selector_Random("2타 Or No");
	CBT_Wait* Wait0 = Node_Wait("대기", 0, 0);

	CBT_Selector* Random_Sel1 = Node_Selector_Random("돌진이나 돌기");

	Root_Seq->Add_Child(Jump_Attack(0.6f));

	Root_Seq->Add_Child(Random_Sel0);

	Random_Sel0->Add_Child(Random_Sel1);

	Random_Sel1->Add_Child(Rush_RightFoot());
	Random_Sel1->Add_Child(WhirlWind_RightFoot());

	Random_Sel0->Add_Child(Wait0);

	return Root_Seq;
}

CBT_Composite_Node * CDeerKing::Four_Combo_Punch()
{
	CBT_Sequence* Root_Seq = Node_Sequence("4콤보 주먹");

	Root_Seq->Add_Child(LeftHand_Attack(0.95f));
	Root_Seq->Add_Child(RightHand_Attack(0.95f));
	Root_Seq->Add_Child(LeftHand_Attack(0.95f));
	Root_Seq->Add_Child(RightHand_Attack(0.95f));

	return Root_Seq;
}

CBT_Composite_Node * CDeerKing::Start_Game()
{
	CBT_Selector* Root_Sel = Node_Selector("게임 시작");

	CBT_CompareValue* Check_Upper_HPRatio60 = Node_FLOAT_A_Smaller_Than_Value("체력 60퍼 미만", L"HPRatio", 60.f);

	Root_Sel->Add_Child(Check_Upper_HPRatio60);
	Check_Upper_HPRatio60->Set_Child(HP_Final());
	Root_Sel->Add_Child(More_Than_HP_60());

	return Root_Sel;
}

CBT_Composite_Node * CDeerKing::More_Than_HP_60()
{
	CBT_Selector* Root_Sel = Node_Selector("근거리 원거리 구분 공격");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_More_Than_HP60());

	Root_Sel->Add_Child(FarAttack_More_Than_HP60());

	return Root_Sel;
}

CBT_Composite_Node * CDeerKing::NearAttack_Dist5_More_Than_HP60()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	Root_Sel->Add_Child(LeftHand_Attack());
	Root_Sel->Add_Child(Head_ColdBeam());
	Root_Sel->Add_Child(Smart_Three_Attack());

	return Root_Sel;
}

CBT_Composite_Node * CDeerKing::FarAttack_More_Than_HP60()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Throwing());
	Root_Sel->Add_Child(Slide_Attack());
	Root_Sel->Add_Child(Smart_JumpAttack());
	Root_Sel->Add_Child(MoveAround());
	
	return Root_Sel;
}

CBT_Composite_Node * CDeerKing::HP_Final()
{
	CBT_Selector* Root_Sel = Node_Selector("근거리 원거리 구분 공격");

	CBT_DistCheck* Dist0 = Node_DistCheck("거리 체크", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_Final());

	Root_Sel->Add_Child(FarAttack_Fianl());

	return Root_Sel;
}

CBT_Composite_Node * CDeerKing::NearAttack_Dist5_Final()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 근접 공격");

	Root_Sel->Add_Child(LeftHand_Attack());
	Root_Sel->Add_Child(Jump_In_Place());
	Root_Sel->Add_Child(Rush_Body());
	Root_Sel->Add_Child(Four_Combo_Punch());
	
	return Root_Sel;
}

CBT_Composite_Node * CDeerKing::FarAttack_Fianl()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("랜덤 원거리 공격");

	Root_Sel->Add_Child(Jump_Fist());
	Root_Sel->Add_Child(Rush_Body());
	Root_Sel->Add_Child(Jump_In_Place());
	Root_Sel->Add_Child(Throwing());
	Root_Sel->Add_Child(Blade_Attack());
	Root_Sel->Add_Child(MoveAround());

	return Root_Sel;
}

CBT_Composite_Node * CDeerKing::Start_Show()
{
	CBT_Selector* Root_Sel = Node_Selector("시연회");

	CBT_CompareValue* Check_ShowValueN = Node_BOOL_A_Equal_Value("시연회 근거리 변수 체크", L"Show_Near", true);

	Root_Sel->Add_Child(Check_ShowValueN);
	Check_ShowValueN->Set_Child(Show_RotationAndNearAttack());

	Root_Sel->Add_Child(Show_FarAttack());

	return Root_Sel;
}

CBT_Composite_Node * CDeerKing::Show_RotationAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("추적 후 순서대로 공격");
	CBT_RotationDir* Rotation0 = Node_RotationDir("플레이어 바라보기", L"Player_Pos", 0.2);

	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(Show_NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CDeerKing::Show_NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("순서대로 근거리 공격");

	CBT_Cooldown* Cool0 = Node_Cooldown("쿨0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("쿨1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("쿨2", 300);
	CBT_Cooldown* Cool3 = Node_Cooldown("쿨3", 300);
	CBT_Cooldown* Cool4 = Node_Cooldown("쿨4", 300);
	CBT_Cooldown* Cool5 = Node_Cooldown("쿨5", 300);
	CBT_Cooldown* Cool6 = Node_Cooldown("쿨6", 300);


	CBT_SetValue* Show_OffNearAttack = Node_BOOL_SetValue("시연회 OFF", L"Show_Near", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(LeftHand_Attack());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(RightFoot_Attack());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Smart_Three_Attack());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(Head_ColdBeam());

	Root_Sel->Add_Child(Show_OffNearAttack);

	return Root_Sel;
}

CBT_Composite_Node * CDeerKing::Show_FarAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("순서대로 원거리 공격");

	CBT_Cooldown* Cool0 = Node_Cooldown("쿨0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("쿨1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("쿨2", 300);
	CBT_Cooldown* Cool3 = Node_Cooldown("쿨3", 300);

	CBT_Play_Ani* Show_Ani3 = Node_Ani("기본", Ani_Appearance_End, 0.95f);

	CBT_SetValue* Show_ValueOff = Node_BOOL_SetValue("시연회 OFF", L"Show", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Slide_Attack());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Smart_JumpAttack());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Throwing());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(Show_Ani3);

	Root_Sel->Add_Child(Show_ValueOff);
	
	return Root_Sel;;
}

void CDeerKing::Down()
{
	if (true == m_bDown_StartAni)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.410f))
			m_bThrow_Shield = true;

		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_bDown_Start = false;
			m_bAIController = true;

			++m_iDownCount;

			m_pMeshCom->SetUp_Animation(Ani_Idle);
		}
	}

	// 충돌처리, bCanHit를 무기가 false시켜줄것임.
	if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		m_dHitTime = 0;	// 피격가능 타임 초기화

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
}

void CDeerKing::Update_Shield()
{
	if (true == m_bFinish_Throw_Shield)
		return;

	CTransform* pShieldTrans = static_cast<CTransform*>(m_pShield->Get_Component(L"Com_Transform"));
	
	TRANS_INFO transInfo = pShieldTrans->Get_Info();

	_v3 vShield_Pos = *(_v3*)&transInfo.matWorld.m[3];

	if (-0.5f >= transInfo.matWorld.m[3][1])
	{		
		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::DearKing_SFX_02);
		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"SE_BOSSGUY_MATERIAL_011.ogg"), CSoundManager::CHANNELID::DearKing_SFX_02, CSoundManager::SOUND::Effect_Sound);

		SHAKE_CAM_lv2;

		vShield_Pos.y = -0.5f;
		m_bFinish_Throw_Shield = true;

		// 방패 떨어진 자리에 이펙트 발생시키기
		CTransform* pShieldTrans = static_cast<CTransform*>(m_pShield->Get_Component(L"Com_Transform"));
		_mat matShield = pShieldTrans->Get_WorldMat();
		_v3 vShieldPos = _v3(matShield.m[3][0], matShield.m[3][1], matShield.m[3][2]);

		g_pManagement->Create_Effect(L"DeerKing_SnowChunk_Up_Particle_0"	, vShieldPos, nullptr);
		g_pManagement->Create_Effect(L"DeerKing_SnowChunk_Up_Particle_1"	, vShieldPos, nullptr);
		g_pManagement->Create_Effect(L"DeerKing_SnowChunk_Heavy_Particle_0"	, vShieldPos, nullptr);
		g_pManagement->Create_Effect(L"DeerKing_IceStone_Up_Particle_0"		, vShieldPos, nullptr);
		g_pManagement->Create_Effect(L"DeerKing_Snow_Up_Particle_0"			, vShieldPos, nullptr);
		g_pManagement->Create_Effect(L"DeerKing_IceSmoke_0"					, vShieldPos, nullptr);
		g_pManagement->Create_Effect(L"DeerKing_IceSmoke_2"					, vShieldPos, nullptr);

		g_pManagement->Create_Effect_Delay(L"DeerKing_SnowChunk_Up_Particle_0"		, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_SnowChunk_Up_Particle_1"		, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_SnowChunk_Heavy_Particle_0"	, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_IceStone_Up_Particle_0"		, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"IceGirl_Buff_Break_1"					, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"IceGirl_Buff_Break_2"					, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_Snow_Up_Particle_0"			, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_Snow_Up_Particle_0"			, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_Snow_Up_Particle_0"			, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_IceSmoke_0"					, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_IceSmoke_1"					, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_IceSmoke_2"					, 0.8f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_Effect_Delay(L"DeerKing_Tornade_Mesh_0"				, 0.85f	, m_pTransformCom->Get_Pos(), nullptr);
		g_pManagement->Create_ParticleEffect_Delay(L"IceFloorAura_01", 2.0f			, 0.8f	, m_pTransformCom->Get_Pos());
		g_pManagement->Create_ParticleEffect_Delay(L"IceFloorAura_02", 2.0f			, 0.8f	, m_pTransformCom->Get_Pos());
		g_pManagement->Create_ParticleEffect_Delay(L"IceFloorAura_03", 2.0f			, 0.8f	, m_pTransformCom->Get_Pos());
	}
	else
	{
		vShield_Pos += m_vThrowing_Dir * _float(m_fThrowing_Speed * m_dTimeDelta);

		m_fThrowing_Speed -= 0.05f;

		m_vThrowing_Dir += _v3(0.f, -0.02f, 0.f);
	}

	transInfo.matWorld.m[3][0] = vShield_Pos.x;
	transInfo.matWorld.m[3][1] = vShield_Pos.y;
	transInfo.matWorld.m[3][2] = vShield_Pos.z;

	pShieldTrans->Set_Info(transInfo);
}

void CDeerKing::Update_Dir_Shield_Throwing()
{
	m_bOld_RightHandAttach_Pos = m_vRightHandAttach;

	m_vRightHandAttach = *(_v3*)(&(m_pRightHandAttachFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_RightHandAttach", m_vRightHandAttach);

	m_bCur_RightHandAttach_Pos = m_vRightHandAttach;

	m_vThrowing_Dir = *D3DXVec3Normalize(&_v3(), &(m_bCur_RightHandAttach_Pos - m_bOld_RightHandAttach_Pos));

}

HRESULT CDeerKing::Update_Bone_Of_BlackBoard()
{
	m_pLeftHandFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHand");
	m_vLeftHand = *(_v3*)(&(m_pLeftHandFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftHand", m_vLeftHand);

	m_pLeftHandAttachFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHandAttach");
	m_vLeftHandAttach = *(_v3*)(&(m_pLeftHandAttachFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftHandAttach", m_vLeftHandAttach);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Mat_LeftHandAttach", m_pLeftHandAttachFrame->CombinedTransformationMatrix);

	m_pRightHandFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHand");
	m_vRightHand = *(_v3*)(&(m_pRightHandFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_RightHand", m_vRightHand);

	m_pSpine3_LeftJetFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine3_LeftJet");
	m_vLeftJet = *(_v3*)(&(m_pSpine3_LeftJetFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftJet", m_vLeftJet);

	m_pSpine3_RightJetFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine3_RightJet");
	m_vRightJet = *(_v3*)(&(m_pSpine3_RightJetFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_RightJet", m_vRightJet);

	// 머리 박기 패턴 - 고드름 생성위치
	m_pHead_LeftCornerFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head_LeftCorner");
	m_vHeadColdBeamPos = *(_v3*)(&(m_pHead_LeftCornerFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_vHeadColdBeamPos += (m_pTransformCom->Get_Axis(AXIS_Z) * 0.5f);
	m_vHeadColdBeamPos += (m_pTransformCom->Get_Axis(AXIS_Y) * -1.3f);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftCorner", m_vHeadColdBeamPos);
	
	// RightHandAttach는  특별히 Update_Dir_Shield_Throwing에서 업데이트 시켜줌

	return S_OK;
}

HRESULT CDeerKing::Update_Value_Of_BB()
{
	CGameObject* pPlayer = CMonster::Get_pTargetObject();

	if (nullptr == pPlayer)
		return E_FAIL;

	CTransform* pPlayer_Trans = TARGET_TO_TRANS(pPlayer);

	_mat matSelf = m_pTransformCom->Get_WorldMat();
	_v3 vSelfRight = *D3DXVec3Normalize(&_v3(), (_v3*)&matSelf.m[0]);
	_v3 vSelfUp = *D3DXVec3Normalize(&_v3(), (_v3*)&matSelf.m[1]);
	_v3 vSelfLook = *D3DXVec3Normalize(&_v3(), (_v3*)&matSelf.m[2]);
	_v3 vSelfPos = *(_v3*)&matSelf.m[3];

	// 1. 플레이어 좌표 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Player_Pos", pPlayer_Trans->Get_Pos());
	// 2. 체력 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HP", m_tObjParam.fHp_Cur);
	// 3. 체력 비율 업데이트
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HPRatio", _float(m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max) * 100);

	// 4. 본인 좌표
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Pos", m_pTransformCom->Get_Pos());
	// 4-1. 본인 좌표 조금 앞에
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Pos_Front", m_pTransformCom->Get_Pos() + m_pTransformCom->Get_Axis(AXIS_Z) * 1.6f);

	/////////////////
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"V3_Null", V3_NULL);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Dir", m_pTransformCom->Get_Axis(AXIS_Z));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Mat", m_pTransformCom->Get_WorldMat());
	/////////////////


	// 사운드
	// SFX_01
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_01_Stop"))	// 멈춤
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_01_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::DearKing_SFX_01);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_01_Play"))	// 재생
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_01_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_01_Tag")]), CSoundManager::CHANNELID::DearKing_SFX_01, CSoundManager::SOUND::Effect_Sound);
	}

	// SFX_02
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_02_Stop"))	// 멈춤
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_02_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::DearKing_SFX_02);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_02_Play"))	// 재생
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_02_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_02_Tag")]), CSoundManager::CHANNELID::DearKing_SFX_02, CSoundManager::SOUND::Effect_Sound);
	}

	// Voice
	if (true == m_pAIControllerCom->Get_BoolValue(L"Voice_Stop"))	// 멈춤
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Voice_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::DearKing_Voice);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"Voice_Play"))	// 재생
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Voice_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"Voice_Tag")]), CSoundManager::CHANNELID::DearKing_Voice, CSoundManager::SOUND::Effect_Sound);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"Step_Play"))	// 재생
	{
		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"Step_Tag")]), CSoundManager::CHANNELID::DearKing_Step, CSoundManager::SOUND::Effect_Sound);
	}

	// 카메라 흔들기
	if (true == m_pAIControllerCom->Get_BoolValue(L"CamShake1"))
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"CamShake1", false);

		SHAKE_CAM_lv1;
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"CamShake2"))
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"CamShake2", false);

		SHAKE_CAM_lv2;
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"CamShake4"))
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"CamShake4", false);

		SHAKE_CAM_lv4;
	}



	// 1. 방패 찍기 좌표
	//CTransform* pShieldTrans = static_cast<CTransform*>(m_pShield->Get_Component(L"Com_Transform"));
	//_mat matShield = pShieldTrans->Get_WorldMat();
	//_v3 vShieldPos = _v3(matShield.m[3][0], matShield.m[3][1], matShield.m[3][2]);
	//m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ShieldPos", vShieldPos + _v3(0.f, -0.7f, 0.f));
	
	//
	_v3 vShieldPos = m_pTransformCom->Get_Pos() + m_pTransformCom->Get_Axis(AXIS_Z) * 0.5f + m_pTransformCom->Get_Axis(AXIS_X) * 0.5f;
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ShieldPos", vShieldPos);


	// 2. 투사체 던지기 좌표
	_float fLength = 1.5f;

	_v3 vBulletPos = m_pTransformCom->Get_Pos() + m_pTransformCom->Get_Axis(AXIS_Y) * 2.3f + m_pTransformCom->Get_Axis(AXIS_Z) * 1.8f + m_pTransformCom->Get_Axis(AXIS_X) * -0.5f;

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos0", vBulletPos + fLength * vSelfUp);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos1", vBulletPos + fLength * *D3DXVec3TransformNormal(&_v3(), &vSelfUp, D3DXMatrixRotationAxis(&_mat(), &vSelfLook, D3DXToRadian(45))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos2", vBulletPos + fLength * *D3DXVec3TransformNormal(&_v3(), &vSelfUp, D3DXMatrixRotationAxis(&_mat(), &vSelfLook, D3DXToRadian(90))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos3", vBulletPos + fLength * *D3DXVec3TransformNormal(&_v3(), &vSelfUp, D3DXMatrixRotationAxis(&_mat(), &vSelfLook, D3DXToRadian(135))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos4", vBulletPos + fLength * *D3DXVec3TransformNormal(&_v3(), &vSelfUp, D3DXMatrixRotationAxis(&_mat(), &vSelfLook, D3DXToRadian(180))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos5", vBulletPos + fLength * *D3DXVec3TransformNormal(&_v3(), &vSelfUp, D3DXMatrixRotationAxis(&_mat(), &vSelfLook, D3DXToRadian(225))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos6", vBulletPos + fLength * *D3DXVec3TransformNormal(&_v3(), &vSelfUp, D3DXMatrixRotationAxis(&_mat(), &vSelfLook, D3DXToRadian(270))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Throwing_Pos7", vBulletPos + fLength * *D3DXVec3TransformNormal(&_v3(), &vSelfUp, D3DXMatrixRotationAxis(&_mat(), &vSelfLook, D3DXToRadian(315))));

	// 3. 몬스터 본인 좌표
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SelfPos", vSelfPos);

	// 4. 점프 주먹 찍기 좌표
	fLength = 1.f;
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"JumpFist_Pos", m_vLeftHand + fLength * vSelfRight);

	// 5. 고드름 좌표
	//m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ColdBeam_Left_Dir"	, *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationAxis(&_mat(), &vSelfUp, D3DXToRadian(-40))));
	//m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ColdBeam_Right_Dir"	, *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationAxis(&_mat(), &vSelfUp, D3DXToRadian(40))));
	//
	//m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ColdBeam_Left_Pos", m_vHeadColdBeamPos + m_pTransformCom->Get_Axis(AXIS_X) * -0.5f);
	//m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ColdBeam_Right_Pos", m_vHeadColdBeamPos + m_pTransformCom->Get_Axis(AXIS_X) * 0.5f);

	_v3 vDir0 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-30)));
	_v3 vDir1 = vSelfLook;
	_v3 vDir2 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(30)));

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ColdBeam_Left_Dir", vDir0);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ColdBeam_Mid_Dir", vDir1);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ColdBeam_Right_Dir", vDir2);

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ColdBeam_Left_Pos", vSelfPos + vDir0);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ColdBeam_Mid_Pos", vSelfPos + vDir1);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ColdBeam_Right_Pos", vSelfPos + vDir2);

	// 6. 얼음칼 생성 후 베기
	// 방향
	_v3 IceSwordDir = *D3DXVec3Normalize(&_v3(), &(m_vLeftHandAttach - m_vLeftHand));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceSword_Dir", IceSwordDir);
	// 생성 위치
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceSword_Pos", m_vLeftHand);

	// 7. 오른손 휘두르기 충돌체 좌표
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"RightHandCol_Pos", m_vRightHand);


	return S_OK;
}

HRESULT CDeerKing::Update_NF()
{
	// 플레이어 발견 못 했을 때
	if (false == m_bFindPlayer)
	{
		// 플레이어 좌표 구함.
		_v3 vPlayer_Pos = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_Pos();
		
		// 동료 좌표 구함
		_v3 vColleague_Pos = _v3(10000.f, 10000.f, 10000.f);
		
		CGameObject* pColleague = g_pManagement->Get_GameObjectBack(L"Layer_Colleague", SCENE_STAGE);
		
		// 동료가 있으면 좌표 갱신
		if (nullptr != pColleague)
			vColleague_Pos = TARGET_TO_TRANS(pColleague)->Get_Pos();

		// 플레이어와 몬스터의 거리
		_v3 vP_LengthTemp = vPlayer_Pos - m_pTransformCom->Get_Pos();
		vP_LengthTemp.y = 0.f;
		_float fP_Length = D3DXVec3Length(&vP_LengthTemp);

		// 동료와 몬스터의 거리
		_v3 vC_LengthTemp = vColleague_Pos - m_pTransformCom->Get_Pos();
		vC_LengthTemp.y = 0.f;
		_float fC_Length = D3DXVec3Length(&vC_LengthTemp);



		// 플레이어가 최소거리안에 있는가?
		if (fP_Length < m_fMinLength || fC_Length < m_fMinLength)
		{
			// 플레이어 발견
			m_bFindPlayer = true;
		}
		// 플레이어가 최대거리 안에 있는가?
		else if (fP_Length < m_fMaxLength || fC_Length < m_fMaxLength)
		{
			// 플레이어가 시야각 안에 있는가?
			if (Is_InFov(m_fFov, m_pTransformCom, vPlayer_Pos) || Is_InFov(m_fFov, m_pTransformCom, vColleague_Pos))
			{
				// 플레이어 발견
				m_bFindPlayer = true;
			}
			else
			{
				m_pMeshCom->SetUp_Animation(Ani_Appearance);
			}
		}
		// 플레이어가 최대거리 밖에 있는가?
		else
			m_pMeshCom->SetUp_Animation(Ani_Appearance);

	}
	// 플레이어 발견
	else
	{
		m_pMeshCom->SetUp_Animation(Ani_Appearance_End);

		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_pMeshCom->SetUp_Animation(Ani_Idle);
			m_bFight = true;

			// 가까운 녀석 어그로 끌림.
			Set_Target_Auto();
		}

		if (!m_bUITrigger)
		{
			m_bUITrigger = true;

			// 플레이어 발견 시, UI 활성화(지원)
			m_pBossUI->Set_Active(true);

			CMassageUI* pMassageUI = static_cast<CMassageUI*>(g_pManagement->Get_GameObjectBack(L"Layer_BossMassageUI", SCENE_STAGE));
			pMassageUI->Set_BossName(BOSS_NAME::Deer_King);
			pMassageUI->Set_Check_Play_BossnameUI(true);

			g_pSoundManager->Stop_Sound(CSoundManager::Background_Loop);
			g_pSoundManager->Play_BGM(L"Boss_DeerKing_BGM.ogg");
		}
	}

	return S_OK;
}

HRESULT CDeerKing::Update_Collider()
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
	m_vecAttackCol[0]->Update(m_vLeftHand);


	m_pColliderCom->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pColliderCom->Get_Radius().y, 0.f));
	return S_OK;
}

HRESULT CDeerKing::Update_Effect()
{
	g_pManagement->Create_Effect_Offset(L"DeerKing_Body_PointParticle"	, 0.01f, m_pTransformCom->Get_Pos(), nullptr);
	g_pManagement->Create_Effect_Offset(L"DeerKing_Body_Smoke"			, 0.1f, m_pTransformCom->Get_Pos(), nullptr);
	g_pManagement->Create_Effect_Offset(L"DeerKing_Body_Smoke_2"		, 0.25f, m_pTransformCom->Get_Pos(), nullptr);

	return S_OK;
}

void CDeerKing::Check_PhyCollider()
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

		//m_bFight = true;		// 싸움 시작
		m_bFindPlayer = true;

		Give_Mana_To_Player(2);

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			// 체력 비율 70 이하되면 스턴
			if (0 == m_iDownCount)
			{
				if (0.7 >= (m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max))
				{
					m_bDown_Start = true;

					m_tObjParam.bDown = true;

					m_pMeshCom->SetUp_Animation(Ani_Throw_Shield);
					m_bDown_StartAni = true;	//down 함수 내부에서 쓸 것임.
					m_pAIControllerCom->Reset_BT();
					m_bAIController = false;

					m_pAIControllerCom->Set_Value_Of_BlackBoard(L"PushCol", true);
				}
			}


			//cout << "IceGirl - Check_PhyCollider : " << m_tObjParam.fHp_Cur << endl;

		}
		else
		{
			m_pMeshCom->SetUp_Animation(Ani_Death);	// 죽음처리 시작
			Start_Dissolve(0.55f, false, true, 7.2f);
			m_pShield->Start_Dissolve(0.55f, false, false, 7.2f);

			for (_int i = 0; i < 20; i++)
				CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(_float(CCalculater::Random_Num(100, 300)), m_pTransformCom->Get_Pos(), 7.5f + (i * 0.08f)));

			CParticleMgr::Get_Instance()->Create_BossDeadParticle_Effect(m_pTransformCom->Get_Pos() + _v3(0.f, 1.3f, 0.f), 6.2f, 0.5f);
			//g_pManagement->Create_Effect_Delay(L"Boss_Dead_Particle"					, 5.2f, _v3(0.f, 1.3f, 0.f), m_pTransformCom);
			g_pManagement->Create_ParticleEffect_Delay(L"SpawnParticle_ForBoss"	, 2.5f	, 7.2f, m_pTransformCom->Get_Pos() + _v3(0.f, 0.5f, 0.f));

			g_pManagement->Create_Effect_Offset(L"DeerKing_IceSmoke_0", 0.1f, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Offset(L"DeerKing_IceSmoke_1", 0.1f, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Offset(L"DeerKing_IceSmoke_2", 0.1f, m_pTransformCom->Get_Pos(), nullptr);

			g_pManagement->Create_Effect_Delay(L"DeerKing_SnowChunk_Up_Particle_0"		, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_SnowChunk_Up_Particle_1"		, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_SnowChunk_Heavy_Particle_0"	, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_Snow_Up_Particle_0"			, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_Snow_Up_Particle_0"			, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_Snow_Up_Particle_0"			, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_IceSmoke_0"					, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_IceSmoke_1"					, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_IceSmoke_2"					, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
			g_pManagement->Create_Effect_Delay(L"DeerKing_Snow_Up_LongLine_1"			, 7.79f	, m_pTransformCom->Get_Pos(), nullptr);
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

void CDeerKing::Push_Collider()
{
	list<CGameObject*> tmpList[4];

	tmpList[0] = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);
	tmpList[1] = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
	tmpList[2] = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);
	tmpList[3] = g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE);

	for (auto& ListObj : tmpList)
	{
		if (ListObj.empty())
			continue;

		for (auto& iter : ListObj)
		{
			CCollider* pCollider = TARGET_TO_COL(iter);

			// 지금 속도값 임의로 넣었는데 구해서 넣어줘야함 - 완료
			if (m_pColliderCom->Check_Sphere(pCollider, m_pTransformCom->Get_Axis(AXIS_Z), m_pAIControllerCom->Get_FloatValue(L"Monster_Speed")))
			{
				CTransform* pTrans = TARGET_TO_TRANS(iter);
				CNavMesh*   pNav = TARGET_TO_NAV(iter);

				// 방향 구해주고
				_v3 vDir = m_pTransformCom->Get_Pos() - pTrans->Get_Pos();
				V3_NORMAL_SELF(&vDir);

				// y축 이동은 하지말자
				vDir.y = 0;

				// 네비 메쉬타게 끔 세팅
				pTrans->Set_Pos(pNav->Move_OnNaviMesh(NULL, &pTrans->Get_Pos(), &vDir, m_pColliderCom->Get_Length().x));
			}
		}
	}
}

void CDeerKing::OnCollisionEnter()
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
	{
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE));
	}

	// =============================================================================================
}

void CDeerKing::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

						iter->Hit_Target(m_tObjParam.fDamage);
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

HRESULT CDeerKing::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_DeerKing", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	// for.Com_AIController
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"AIController", L"Com_AIController", (CComponent**)&m_pAIControllerCom)))
		return E_FAIL;

	// for.Com_NavMesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMeshCom)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	//=================================================================================
	// for.Com_Optimaization
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Optimization", L"Com_Optimization", (CComponent**)&m_pOptimizationCom)))
		return E_FAIL;

	// for.Com_BattleAgent
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"BattleAgent", L"Com_BattleAgent", (CComponent**)&m_pBattleAgentCom)))
		return E_FAIL;
	//=================================================================================

	m_pColliderCom->Set_Radius(_v3{ 1.8f, 1.8f, 1.8f });
	m_pColliderCom->Set_Dynamic(true);
	m_pColliderCom->Set_Type(COL_SPHERE);
	m_pColliderCom->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pColliderCom->Get_Radius().y , 0.f });


	return NOERROR;
}

HRESULT CDeerKing::SetUp_ConstantTable(CShader* pShader)
{
	if (nullptr == pShader)
		return E_FAIL;

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat		ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", pShader)))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 쉐이더 재질정보 수치 입력
	//=============================================================================================
	_float	fEmissivePower = 10.f;	// 이미시브 : 높을 수록, 자체 발광이 강해짐.
	_float	fSpecularPower = 0.75f;	// 메탈니스 : 높을 수록, 정반사가 강해짐.
	_float	fRoughnessPower = 0.2f;	// 러프니스 : 높을 수록, 빛 산란이 적어짐(빛이 응집됨).
	_float	fRimLightPower = 0.f;	// 림		: 높을 수록 빛이 퍼짐(림라이트의 범위가 넓어지고 , 밀집도가 낮아짐).

	if (FAILED(pShader->Set_Value("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fSpecularPower", &fSpecularPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fRoughnessPower", &fRoughnessPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fRimAlpha", &fRimLightPower, sizeof(_float))))
		return E_FAIL;
	//=============================================================================================

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CDeerKing::Ready_Weapon()
{
	// 오른손 방패
	m_pShield = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pShield->Change_WeaponData(WPN_DeerKingShield);
	m_pShield->Set_WeaponDamage(WEAPON_DATA::WPN_DeerKingShield, 500.f * pow(1.5f, g_eStageIdx_Cur - 1));

	D3DXFRAME_DERIVED* pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pShield->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pShield->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	return S_OK;
}

HRESULT CDeerKing::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	// 뼈 주소
	m_pLeftHandFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHand");
	m_pLeftHandAttachFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHandAttach");
	m_pRightHandFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHand");
	m_pRightHandAttachFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pSpine3_LeftJetFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine3_LeftJet");
	m_pSpine3_RightJetFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine3_RightJet");
	m_pHead_LeftCornerFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head_LeftCorner");

	return S_OK;
}

HRESULT CDeerKing::Ready_Collider()
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


	////////// 공격용 콜라이더
	m_vecAttackCol.reserve(1);

	// 왼쪽 주먹
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 1.f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(m_vLeftHand);
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CDeerKing::Ready_NF(void * pArg)
{
	INFO eTemp = *(INFO*)pArg;

	m_fFov = eTemp.fFov;
	m_fMaxLength = eTemp.fMaxLength;
	m_fMinLength = eTemp.fMinLength;

	//===================================================================
	// 림라이트 벨류 설정	
	//===================================================================
	m_pBattleAgentCom->Set_RimAlpha(0.5f);
	m_pBattleAgentCom->Set_RimValue(8.f);
	m_pBattleAgentCom->Set_OriginRimAlpha(0.5f);
	m_pBattleAgentCom->Set_OriginRimValue(8.f);
	//===================================================================

	//===================================================================
	// 트리거 소환용 설정
	//===================================================================
	// 트리거 소환되는 녀석이면
	if (eTemp.bSpawnOnTrigger)
	{
		// 스테이지 번호로 네비 불러옴
		_tchar szNavData[STR_128] = L"";

		lstrcpy(szNavData, (
			eTemp.eStageIdx == 0 ? L"Navmesh_Training.dat" :
			eTemp.eStageIdx == 1 ? L"Navmesh_Stage_01.dat" :
			eTemp.eStageIdx == 2 ? L"Navmesh_Stage_02.dat" :
			eTemp.eStageIdx == 3 ? L"Navmesh_Stage_03.dat" : L"Navmesh_Stage_04.dat"));

		m_pNavMeshCom->Set_Index(-1);
		m_pNavMeshCom->Ready_NaviMesh(m_pGraphic_Dev, szNavData);
		m_pNavMeshCom->Check_OnNavMesh(eTemp.vPos);
		m_pTransformCom->Set_Pos(eTemp.vPos);
		m_pTransformCom->Set_Angle(eTemp.vAngle);

		//m_pNavMeshCom->Set_SubsetIndex(eTemp.sSubSetIdx);
		//m_pNavMeshCom->Set_Index(eTemp.sCellIdx);
	}
	//===================================================================


	return S_OK;
}

HRESULT CDeerKing::Ready_Sound()
{
	m_mapSound.emplace(0, L"SE_BOSSGUY_SWING_000.ogg");
	m_mapSound.emplace(1, L"SE_BOSSGUY_MATERIAL_011.ogg");
	m_mapSound.emplace(2, L"SE_BOSSGUY_SWING_001.ogg");
	m_mapSound.emplace(3, L"SE_BOSSGUY_SHIELD_ATTACK_000.ogg");
	m_mapSound.emplace(4, L"SE_BOSSGUY_KETSUGI_BOOST_000.ogg");
	m_mapSound.emplace(5, L"SE_BOSSGUY_BOOST_002.ogg");

	m_mapSound.emplace(10, L"SE_BOSSGUY_BARK_ATTACK_002.ogg");
	
	m_mapSound.emplace(20, L"SE_FOOT_STEP_SNOW_DEEP_001.ogg");
	

	return S_OK;
}

HRESULT CDeerKing::Ready_Cloth()
{
	PxScene& scene = *g_pPhysx->Get_Scene();
	PxPhysics& physics = *g_pPhysx->Get_Physics();

	scene.fetchResults(true);

	PxSceneWriteLock scopedLock(scene);

	// 컴포넌트 잠깐 가져올 것임.
	CMesh_Dynamic* pMesh_Dynamic = static_cast<CMesh_Dynamic*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Mesh_DeerKing"));

	if (nullptr == pMesh_Dynamic)
	{
		MSG_BOX("CDeerKing Ready_Cloth : Failed To Get DeerKingMesh");
		return E_FAIL;
	}

	vector<PxVec4> vertices;
	vector<PxU16> primitives;
	PxClothMeshDesc meshDesc = CreateMesh(pMesh_Dynamic->Get_MeshContainer()[0]->pOriginalMesh, 1.f, PxQuat(PxIdentity), PxVec3(0, 0, 0), vertices, primitives);

	if (!meshDesc.isValid())
		MSG_BOX(" PxCloth : Failed to Craete Mesh");

	for (PxU32 i = 32979; i < 32979 + 829; i++)
	{
		if (vertices[i].y < 2.9f)
			vertices[i].w = 0.2f;
	}
	for (PxU32 i = 33892; i < 33892 + 829; i++)
	{
		if (vertices[i].y < 2.9f)
			vertices[i].w = 0.2f;
	}

	m_pCloth = g_pClothManager->CreateQuadifier(meshDesc);

	scene.addActor(*m_pCloth);

	// set solver settings
	m_pCloth->setSolverFrequency(120);

	// damp global particle velocity to 90% every 0.1 seconds
	m_pCloth->setDampingCoefficient(PxVec3(0.4f)); // damp local particle velocity
	m_pCloth->setLinearDragCoefficient(PxVec3(0.4f)); // transfer frame velocity
	m_pCloth->setAngularDragCoefficient(PxVec3(0.4f)); // transfer frame rotation

													 // reduce impact of frame acceleration
													 // x, z: cloth swings out less when walking in a circle
													 // y: cloth responds less to jump acceleration
	m_pCloth->setLinearInertiaScale(PxVec3(0.4f, 0.4f, 0.4f));

	// leave impact of frame torque at default
	//m_pCloth->setAngularInertiaScale(PxVec3(1.0f));
	m_pCloth->setAngularInertiaScale(PxVec3(0.2f));

	// reduce centrifugal force of rotating frame
	m_pCloth->setCentrifugalInertiaScale(PxVec3(0.3f));

	m_pCloth->setInertiaScale(0.5f);

	const bool useCustomConfig = true;

	// custom fiber configuration
	if (useCustomConfig)
	{
		PxClothStretchConfig stretchConfig;
		stretchConfig.stiffness = 1.0f;

		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eVERTICAL, PxClothStretchConfig(0.8f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eHORIZONTAL, PxClothStretchConfig(0.6f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eSHEARING, PxClothStretchConfig(0.5f));
		m_pCloth->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(0.5f));
		m_pCloth->setTetherConfig(PxClothTetherConfig(1.0f));
	}

	m_pCloth->putToSleep();

	return S_OK;
}

physx::PxClothMeshDesc CDeerKing::CreateMesh(LPD3DXMESH pMesh, physx::PxReal scale, physx::PxQuat rot, physx::PxVec3 offset, vector<physx::PxVec4>& vertices, vector<physx::PxU16>& indices)
{
	if (nullptr == pMesh)
	{
		MSG_BOX("CCloth : n_pMesh is Nullptr");
		return PxClothMeshDesc();
	}

	/*
	Attribld	1
	FaceStart	51146
	FaceCount	1344
	VertexStart	32979
	VertexCount	829

	Attribld	3
	FaceStart	52570
	FaceCount	1344
	VertexStart	33892
	VertexCount	829

	흔들릴 버텍스 총 수 : 34721 - 32979 = 1742  
	흔들릴 삼각형 총 수 : 53914 - 51146 = 2768
	*/

	_ulong numVertices = pMesh->GetNumVertices();
	_ulong numTriangles = pMesh->GetNumFaces();

	vertices.resize(numVertices);
	indices.resize(numTriangles * 3);

	vector<PxVec3> verticesTemp;
	verticesTemp.resize(numVertices);

	////////////////////////////////////////////////////
	// 버텍스 포지션 정보를 vec3 에서 vec4로 바꿔서 저장
	//IDirect3DVertexBuffer9* pVB = nullptr;
	_byte*		pVertices = nullptr;

	//m_pMesh->GetVertexBuffer(&pVB);
	//pVB->Lock(0, 0, (void**)&pVertices, 0);
	pMesh->LockVertexBuffer(0, (void**)&pVertices);

	_ulong dwStride = pMesh->GetNumBytesPerVertex();

	for (_ulong i = 0; i < numVertices; ++i)
	{
		PxVec3 vTemp = *(PxVec3*)(pVertices + (i * dwStride));
		verticesTemp[i] = vTemp;
	}

	PxVec3 *vSrc = (PxVec3*)&(*verticesTemp.begin());
	PxVec4 *vDest = &(*vertices.begin());
	for (_ulong i = 0; i < numVertices; i++, vDest++, vSrc++)
	{
		*vDest = PxVec4(scale * rot.rotate(*vSrc) + offset, 0.f);
	}

	//pVB->Unlock();
	pMesh->UnlockVertexBuffer();

	/////////////////////////////////////////////////////
	// 인덱스 정보 저장
	_byte *	pIndex = nullptr;

	pMesh->LockIndexBuffer(0, (void**)&pIndex);
	//pMesh->GetIndexBuffer(&m_pIB);
	//m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	for (_ulong i = 0; i < numTriangles * 3; ++i)
	{
		indices[i] = *((PxU16*)pIndex + i);
	}

	pMesh->UnlockIndexBuffer();


	PxClothMeshDesc meshDesc;

	// convert vertex array to PxBoundedData (desc.points)
	meshDesc.points.data = &(*vertices.begin());
	meshDesc.points.count = static_cast<PxU32>(numVertices);
	meshDesc.points.stride = sizeof(PxVec4);

	meshDesc.invMasses.data = &vertices.begin()->w;
	meshDesc.invMasses.count = static_cast<PxU32>(numVertices);
	meshDesc.invMasses.stride = sizeof(PxVec4);

	// convert face index array to PxBoundedData (desc.triangles)
	meshDesc.triangles.data = &(*indices.begin());
	meshDesc.triangles.count = static_cast<PxU32>(numTriangles);
	meshDesc.triangles.stride = sizeof(PxU16) * 3; // <- stride per triangle
												   //meshDesc.triangles.stride = sizeof(PxU32) * 3; // <- stride per triangle

	meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;

	return meshDesc;
}

void CDeerKing::Change_Vertex()
{
	physx::PxSceneWriteLock scopedLock(*g_pPhysx->Get_Scene());

	physx::PxClothFabric* pFabric = m_pCloth->getFabric();
	physx::PxClothParticleData* pData = m_pCloth->lockParticleData();

	LPD3DXMESH	pMesh = m_pMeshCom->Get_MeshContainer()[0]->pOriginalMesh;
	//LPD3DXMESH	pMesh = m_pDynamicMesh->Get_MeshContainer()[0]->MeshData.pMesh;
	_ulong dwStride = pMesh->GetNumBytesPerVertex();

	_byte* pVertices = nullptr;

	pMesh->LockVertexBuffer(0, (void**)&pVertices);


	_ulong adsfas = _ulong(pFabric->getNbParticles());

	for (_ulong i = 0; i < _ulong(pFabric->getNbParticles()); ++i)
	{
		*(_v3*)(pVertices + (i * dwStride)) = *(_v3*)(pData->particles + i);
	}

	pMesh->UnlockVertexBuffer();
}

void CDeerKing::Update_Cloth()
{
	PxSceneWriteLock scopedLock(*g_pPhysx->Get_Scene());

	D3DXQUATERNION quater;
	D3DXVECTOR3 vMonsterPos;
	D3DXMatrixDecompose(&D3DXVECTOR3(), &quater, &vMonsterPos, &m_pTransformCom->Get_WorldMat());

	PxTransform rootPose = PxTransform(PxVec3(vMonsterPos.x, vMonsterPos.y, vMonsterPos.z), *(PxQuat*)&quater);

	m_pCloth->setTargetPose(rootPose);

	PxReal strength = 20.0f;
	PxVec3 offset(PxReal(CALC::Random_Num_Double(-1, 1)), PxReal(CALC::Random_Num_Double(-1, 1)), PxReal(CALC::Random_Num_Double(-1, 1)));
	PxVec3 windAcceleration = strength * offset;
	m_pCloth->setExternalAcceleration(windAcceleration);
}

CDeerKing * CDeerKing::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDeerKing* pInstance = new CDeerKing(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CDeerKing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDeerKing::Clone_GameObject(void * pArg)
{
	CDeerKing* pInstance = new CDeerKing(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CDeerKing");
		Safe_Release(pInstance);
	}

	return pInstance;	return nullptr;
}

void CDeerKing::Free()
{
	Safe_Release(m_pShield);

	CMonster::Free();
}
