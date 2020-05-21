#include "stdafx.h"
#include "..\Headers\PoisonButterfly.h"
#include "..\Headers\BossHP.h"
#include "MassageUI.h"

CPoisonButterfly::CPoisonButterfly(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
	ZeroMemory(m_matBones, sizeof(_mat*) * Bone_End);
}

CPoisonButterfly::CPoisonButterfly(const CPoisonButterfly & rhs)
	: CMonster(rhs)
{
	ZeroMemory(m_matBones, sizeof(_mat*) * Bone_End);
}

HRESULT CPoisonButterfly::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CPoisonButterfly::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_NF(pArg);
	Ready_BoneMatrix();
	Ready_Collider();
	Ready_Sound();

	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 10000.f;
	m_tObjParam.fHp_Max = m_tObjParam.fHp_Cur;
	m_tObjParam.fDamage = 20.f;

	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));



	///////////////// �ൿƮ�� init

	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL);

	if (nullptr == pPlayer)
		return E_FAIL;

	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create();	//���ڿ� true �ָ� �ܼ�â�� ��������� ��, default = false

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", m_tObjParam.fHp_Cur);
	pBlackBoard->Set_Value(L"MAXHP", m_tObjParam.fHp_Max);
	pBlackBoard->Set_Value(L"Show", true);
	pBlackBoard->Set_Value(L"Show_Near", true);

	pBlackBoard->Set_Value(L"PushCol", true);	// �浹���� �����

	//�Ҹ� ���� ����
	pBlackBoard->Set_Value(L"SFX_01_Tag", 0);
	pBlackBoard->Set_Value(L"SFX_01_Play", false);
	pBlackBoard->Set_Value(L"SFX_01_Stop", false);
	pBlackBoard->Set_Value(L"SFX_02_Tag", 0);
	pBlackBoard->Set_Value(L"SFX_02_Play", false);
	pBlackBoard->Set_Value(L"SFX_02_Stop", false);

	pBlackBoard->Set_Value(L"Voice_Tag", 0);
	pBlackBoard->Set_Value(L"Voice_Play", false);
	pBlackBoard->Set_Value(L"Voice_Stop", false);

	CBT_Selector* Start_Sel = Node_Selector("�ൿ ����");
	//CBT_Sequence* Start_Sel = Node_Sequence("�ൿ ����");	//�׽�Ʈ

	pBehaviorTree->Set_Child(Start_Sel);

	//////////// �Ʒ��� �ּ��س��� 4���� �����ӿ��� �� ����, ���ʴ�� ������.

	CBT_CompareValue* Check_ShowValue = Node_BOOL_A_Equal_Value("�ÿ�ȸ ���� üũ", L"Show", false);
	Check_ShowValue->Set_Child(Start_Game());
	Start_Sel->Add_Child(Check_ShowValue);
	Start_Sel->Add_Child(Start_Show());

	////////////

	// ���� Ȯ�ο�,  �� ���� �Լ��� �Ʒ��� ������ �����

	//Start_Sel->Add_Child(OneTurn_Poison());

	//CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.2);
	//Start_Sel->Add_Child(Rotation0);

	/////////////
	// UI �߰�(����)
	m_pBossUI = static_cast<CBossHP*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BossHP", nullptr));
	m_pBossUI->Set_UI_Pos(WINCX * 0.5f, WINCY * 0.1f);
	m_pBossUI->Set_BossName(CBossNameUI::Index_PoisonButterfly);
	if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBossUI, SCENE_STAGE, L"Layer_BossHP", nullptr)))
		return E_FAIL;

	return NOERROR;
}

_int CPoisonButterfly::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (true == m_pAIControllerCom->Get_BoolValue(L"PushCol"))
		Push_Collider();

	CGameObject::Update_GameObject(TimeDelta);

	// �׾��� ���
	if (m_bIsDead)
		m_bEnable = false;

	// ���� �ִϸ��̼�
	if (m_bReadyDead)
	{
		// �����鼭 UI ��Ȱ��ȭ
		m_pBossUI->Set_Active(false);

		if ( false == m_bFinishCamShake && m_pMeshCom->Is_Finish_Animation(0.5f))
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


	// �÷��̾� �̹߰�
	if (false == m_bFight)
	{
		Update_NF();
	}
	// �÷��̾� �߰�
	else
	{
		//// ��׷� ����
		Set_Target_Auto(true);

		// �� ��ġ ������Ʈ
		Update_Bone_Of_BlackBoard();
		// BB ���� ������Ʈ
		Update_Value_Of_BB();

		if (true == m_bAIController)
			m_pAIControllerCom->Update_AIController(TimeDelta);

		// ����UI ������Ʈ
		// ü���� 0�� �Ǿ����� �и����� ����.
		if (0 >= m_tObjParam.fHp_Cur)
			m_pBossUI->Set_BossHPInfo(0, 100);
		else
			m_pBossUI->Set_BossHPInfo(m_tObjParam.fHp_Cur, m_tObjParam.fHp_Max);
	}

	if (false == m_bReadyDead)
		Check_PhyCollider();

	OnCollisionEnter();

	m_pTransformCom->Set_Pos(m_pNavMeshCom->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	//====================================================================================================
	// �ø�
	//====================================================================================================
	m_bInFrustum = m_pOptimizationCom->Check_InFrustumforObject(&m_pTransformCom->Get_Pos(), 2.f);
	//====================================================================================================


	return NOERROR;
}

_int CPoisonButterfly::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	//=============================================================================================
	// �׸��ڶ� ��Ǻ��� �������� �ȿ� ������ �ȱ׸�
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

	return NO_EVENT;
}

HRESULT CPoisonButterfly::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	m_pMeshCom->Play_Animation(_float(m_dTimeDelta)); // * alpha

	if (m_bInFrustum)
	{
		m_pShaderCom->Begin_Shader();

		_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

		for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
		{
			if (FAILED(SetUp_ConstantTable(m_pShaderCom)))
				return E_FAIL;

			_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

			m_pMeshCom->Update_SkinnedMesh(i);

			for (_uint j = 0; j < iNumSubSet; ++j)
			{
				m_iPass = m_pMeshCom->Get_MaterialPass(i, j);

				if (m_bDissolve)
					m_iPass = 3;

				m_pShaderCom->Begin_Pass(m_iPass);

				m_pShaderCom->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

				m_pShaderCom->Commit_Changes();

				m_pMeshCom->Render_Mesh(i, j);

				m_pShaderCom->End_Pass();
			}
		}

		m_pShaderCom->End_Shader();

	}

	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CPoisonButterfly::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	if (nullptr == pShader ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	m_pMeshCom->Play_Animation(DELTA_60 * m_dAniPlayMul);

	if (m_bInFrustum)
	{
		if (FAILED(SetUp_ConstantTable(pShader)))
			return E_FAIL;

		_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

		for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
		{
			_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

			m_pMeshCom->Update_SkinnedMesh(i);

			for (_uint j = 0; j < iNumSubSet; ++j)
			{
				m_iPass = m_pMeshCom->Get_MaterialPass(i, j);

				if (m_bDissolve)
					m_iPass = 3;

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

HRESULT CPoisonButterfly::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	if (false == m_bEnable)
		return S_OK;

	if (nullptr == pShader ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	//============================================================================================
	// ���� ����
	//============================================================================================

	_mat	ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat	ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);
	_mat	WorldMatrix = m_pTransformCom->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &WorldMatrix, sizeof(_mat))))
		return E_FAIL;

	//============================================================================================
	// ��� �� ���
	//============================================================================================
	if (_bIsForMotionBlur)
	{
		if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
			return E_FAIL;

		m_matLastWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		_bool bMotionBlur = true;
		if (FAILED(pShader->Set_Bool("g_bMotionBlur", bMotionBlur)))
			return E_FAIL;
		_bool bDecalTarget = false;
		if (FAILED(pShader->Set_Bool("g_bDecalTarget", bDecalTarget)))
			return E_FAIL;
		_float fBloomPower = 0.5f;
		if (FAILED(pShader->Set_Value("g_fBloomPower", &fBloomPower, sizeof(_float))))
			return E_FAIL;
	}

	//============================================================================================
	// ��Ÿ ���
	//============================================================================================
	else
	{
		_mat matWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		if (FAILED(pShader->Set_Value("g_matWVP", &matWVP, sizeof(_mat))))
			return E_FAIL;
	}

	//============================================================================================
	// ���̴� ����
	//============================================================================================
	_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			_int tmpPass = m_pMeshCom->Get_MaterialPass(i, j);

			pShader->Begin_Pass(iPass);
			pShader->Commit_Changes();

			pShader->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	//============================================================================================

	return NOERROR;
}

CBT_Composite_Node * CPoisonButterfly::Left_Eat(_float fWeight)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("���� ��");
	CBT_Play_Ani* Show_Ani25 = Node_Ani("���� ��", 25, fWeight);//27
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����", L"Player_Pos", 1.4, 0);
	CBT_RotationDir* RotationDir0 = Node_RotationDir("���� ȸ��", L"Player_Pos", 0.156);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ְ�", L"ButterFly_Distortion_Circle", L"Bone_Tail6", 1.75f, 1, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("��¦ ���� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�ְ�Ȱ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0.f, 60, 0.7, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani25);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(RotationDir0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.566, 1, 0.234, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Right_Eat(_float fWeight)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("������ ��");
	CBT_Play_Ani* Show_Ani20 = Node_Ani("������ ��", 20, fWeight);//22
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����", L"Player_Pos", 1.2, 0);
	CBT_RotationDir* RotationDir0 = Node_RotationDir("���� ȸ��", L"Player_Pos", 0.183);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ְ�", L"ButterFly_Distortion_Circle", L"Bone_Tail6", 1.65f, 1, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("��¦ ���� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�ְ�Ȱ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0.f, 60, 0.7, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani20);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(RotationDir0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.383, 1, 0.2, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Eat_TurnEat()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("�� �ڵ��Ƽ� ����");

	CBT_Sequence* MainSeq = Node_Sequence("�� �ڵ��Ƽ� ��");
	CBT_Play_Ani* Show_Ani20 = Node_Ani("��ȸ�� �ѹ��� ����", 20, 0.6f);//22
	CBT_Play_Ani* Show_Ani18 = Node_Ani("�ڵ��Ƽ� ����", 18, 0.95f);//20
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����0", L"Player_Pos", 1.2, 0);
	CBT_RotationDir* RotationDir0 = Node_RotationDir("���� ȸ��0", L"Player_Pos", 0.183);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 0);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.417, 0);
	// 22�� �ִ� * 0.6 = 1.8
	CBT_Wait* Wait1 = Node_Wait("���1", 0.6, 0);
	CBT_RotationDir* RotationDir1 = Node_RotationDir("���� ȸ��1", L"Player_Pos", 0.216);
	CBT_SetValue* Sound1Stop1 = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play1 = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag1 = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 10, 0.2, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ְ�", L"ButterFly_Distortion_Circle", L"Bone_Tail6_Tongue2", 1.5, 1, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("��¦ ���� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�ְ�Ȱ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0.f, 60, 0.7, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("�ְ�", L"ButterFly_Distortion_Circle", L"Bone_Tail6_Tongue2", 3.0, 1, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani20);
	MainSeq->Add_Child(Show_Ani18);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(RotationDir0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(RotationDir1);
	SubSeq->Add_Child(Sound1Stop1);
	SubSeq->Add_Child(Sound1Play1);
	SubSeq->Add_Child(Sound1Tag1);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.383, 1, 0.2, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.8 + 0.816, 1, 0.45, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);


	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Poison_Tornado_After_Charging()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���Ƽ� �� �ҿ뵹��");

	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("�Ҹ�1 ���", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("�Ҹ�1 ���", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"Voice_Tag", 10);
	CBT_Play_Ani* Show_Ani26 = Node_Ani("���Ƽ� �� �ҿ뵹��", 26, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 0, 0.f);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("������ �� ó�� ����ũ 01", L"ButterFly_SoftSmoke_Ready_1", L"Self_Pos", 3.2, 1, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("������ �� ó�� ����ũ 02", L"ButterFly_SoftSmoke_Ready_2", L"Self_Pos", 3.2, 1, 0.01, 0);
	//CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("�� ���� �� �޽�����Ʈ", L"ButterFly_Distortion_Circle", L"Self_Pos", 0.3, 1, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("�� ���� �� ����ũ 01", L"ButterFly_SoftSmoke", L"Self_Pos", 0.3, 40, 1.2, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�� ���� �� ����ũ 02", L"ButterFly_SoftSmoke_Bottom", L"Self_Pos", 0.8, 40, 1.5, 0);
	//CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("������ �� ó�� ���� ����ũ", L"ButterFly_Smoke_Red_Once", L"Self_Pos"	, 0, 1, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("�����ϴ� ���� ����ũ", L"ButterFly_Smoke_Red_Particle", L"Self_Pos", 3, 15, 0.01, 0);
	//CBT_CreateEffect* Effect6 = Node_CreateParticle_Finite("�ٴڿ� �򸮴� ���� ����ũ", L"ButterFly_SoftSmoke_Floor", L"Self_Pos", 10, 0.8, 150, 0, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("�ٴڿ� �򸮴� ���� ����ũ", L"ButterFly_SoftSmoke_Floor", L"Self_Pos", 0.8, 60, 1.5, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("������ �� �ڵλ� ���׶�� ��ƼŬ", L"ButterFly_PointParticle_Plum", L"Self_Pos", 3.2, 20, 0.7, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("������ �� ȸ���� �޽�����Ʈ 01", L"ButterFly_RingLine", L"Self_Pos", 3, 1, 0.01, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("������ �� ȸ���� �޽�����Ʈ 02", L"ButterFly_RingLine", L"Self_Pos", 3.45, 1, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("ȸ���� �޽� �ְ�", L"ButterFly_RingLine_Distortion", L"Self_Pos", 3, 1, 0.01, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("�� ���� �� ����ũ ���", L"ButterFly_SoftSmoke_Chunk", L"Self_Pos", 0.8, 20, 1.5, 0);


	Root_Seq->Add_Service(Effect0);
	Root_Seq->Add_Service(Effect1);
	Root_Seq->Add_Service(Effect2);
	Root_Seq->Add_Service(Effect3);
	//Root_Seq->Add_Service(Effect4);
	Root_Seq->Add_Service(Effect5);
	Root_Seq->Add_Service(Effect6);
	Root_Seq->Add_Service(Effect7);
	Root_Seq->Add_Service(Effect8);
	Root_Seq->Add_Service(Effect9);
	Root_Seq->Add_Service(Effect10);
	Root_Seq->Add_Service(Effect11);

	CBT_CreateBullet* PoisonBullet0 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonTornado", L"Self_Pos", L"", 0, 2, 3.36, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Seq->Add_Service(PoisonBullet0);

	Root_Seq->Add_Child(VoiceStop);
	Root_Seq->Add_Child(VoicePlay);
	Root_Seq->Add_Child(VoiceTag);
	Root_Seq->Add_Child(Show_Ani26);
	Root_Seq->Add_Child(Show_Ani6);

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::OneTurn_Poison()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("���� ���� �� �߻�");
	CBT_Play_Ani* Show_Ani22 = Node_Ani("���� ���� �ѹ��� �� �߻�", 22, 0.95f);//24
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����0", L"Player_Pos", 1.2, 0);
	CBT_RotationDir* RotationDir0 = Node_RotationDir("���� ȸ��0", L"Player_Pos", 0.2);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("��ü������ ���� ���׶�� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("��ü������ ���̴� ���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("��¦�̴� ����� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�Ȱ��� ���� ������ �ְ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 150, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani22);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(RotationDir0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.25, 1, 1.816, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Eat_Turn()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("�� ����");
	CBT_Sequence* MainSeq = Node_Sequence("�� ����");
	CBT_Play_Ani* Show_Ani25 = Node_Ani("��", 25, 0.5f);//27, 0.5
	CBT_Play_Ani* Show_Ani24 = Node_Ani("����", 24, 0.95f);//26
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����0", L"Player_Pos", 1.4, 0);
	CBT_RotationDir* RotationDir0 = Node_RotationDir("���� ȸ��0", L"Player_Pos", 0.156);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 0);
	CBT_Wait* Wait0 = Node_Wait("���", 0.319, 0);
	// 27�� �ִ� * 0.5 = 1.875
	CBT_ChaseDir* ChaseDir1 = Node_ChaseDir("���� ����1", L"Player_Pos", 1, 0);
	CBT_RotationDir* RotationDir1 = Node_RotationDir("���� ȸ��1", L"Player_Pos", 0.166);
	CBT_SetValue* Sound1Stop1 = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play1 = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag1 = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 10, 0.367, 0);
	CBT_SetValue* Sound2Stop1 = Node_BOOL_SetValue("�Ҹ�2 ���", L"SFX_02_Stop", true);
	CBT_SetValue* Sound2Play1 = Node_BOOL_SetValue("�Ҹ�2 ���", L"SFX_02_Play", true);
	CBT_SetValue* Sound2Tag1 = Node_INT_SetValue("�Ҹ�2 �̸� ����", L"SFX_02_Tag", 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 10, 0.367, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("��ü������ ���� ���׶�� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 300, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("��ü������ ���̴� ���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 300, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("��¦�̴� ����� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 300, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�Ȱ��� ���� ������ �ְ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 300, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�ְ�", L"ButterFly_Distortion_Circle", L"Bone_Tail6", 1.8, 1, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("�ְ�", L"ButterFly_Distortion_Circle", L"Bone_Tail6", 3.7, 1, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani25);
	MainSeq->Add_Child(Show_Ani24);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(RotationDir0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(ChaseDir1);
	SubSeq->Add_Child(RotationDir1);
	SubSeq->Add_Child(Sound1Stop1);
	SubSeq->Add_Child(Sound1Play1);
	SubSeq->Add_Child(Sound1Tag1);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Sound2Stop1);
	SubSeq->Add_Child(Sound2Play1);
	SubSeq->Add_Child(Sound2Tag1);
	SubSeq->Add_Child(Move1);


	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.566, 1, 0.234, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.875 + 1.166, 1, 0.734, 0);

	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Rush()
{
	CBT_Simple_Parallel* RushParallel = Node_Parallel_Immediate("���� ����");
	CBT_Sequence* RushAniSeq = Node_Sequence("RushSequence");
	CBT_Play_Ani* Show_Ani15 = Node_Ani("����", 15, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* MoveSequence = Node_Sequence("���� �̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����0", L"Player_Pos", 0.2);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�ڷ� �̵�", L"Monster_Speed", L"Monster_Dir", -3.f, 1.083, 0);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����1", L"Player_Pos", 0.3, 0);
	CBT_RotationDir* Rotation1 = Node_RotationDir("���� ����1", L"Player_Pos", 0.267);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_SetValue* Sound1Stop1 = Node_BOOL_SetValue("�Ҹ�1 ���", L"Voice_Stop", true);
	CBT_SetValue* Sound1Play1 = Node_BOOL_SetValue("�Ҹ�1 ���", L"Voice_Play", true);
	CBT_SetValue* Sound1Tag1 = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"Voice_Tag", 11);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("����", L"Monster_Speed", L"Monster_Dir", 13, 1.1, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("��ü������ ���� ���׶�� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 150, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("��ü������ ���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 200, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("����� ������ �����", L"ButterFly_WaterSplash", L"Bone_Tail6", 0, 150, 0, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("��¦�̴� ����� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 150, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�Ȱ��� ���� ������ �ְ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 150, 0, 0);

	RushParallel->Add_Service(Effect0);
	RushParallel->Add_Service(Effect1);
	RushParallel->Add_Service(Effect2);
	RushParallel->Add_Service(Effect3);
	RushParallel->Add_Service(Effect4);

	RushParallel->Set_Main_Child(RushAniSeq);
	RushAniSeq->Add_Child(Show_Ani15);
	RushAniSeq->Add_Child(Show_Ani6);

	RushParallel->Set_Sub_Child(MoveSequence);
	MoveSequence->Add_Child(Rotation0);
	MoveSequence->Add_Child(Sound1Stop);
	MoveSequence->Add_Child(Sound1Play);
	MoveSequence->Add_Child(Sound1Tag);
	MoveSequence->Add_Child(Move0);
	MoveSequence->Add_Child(ChaseDir0);
	MoveSequence->Add_Child(Rotation1);
	MoveSequence->Add_Child(PushColOff);
	MoveSequence->Add_Child(Sound1Stop1);
	MoveSequence->Add_Child(Sound1Play1);
	MoveSequence->Add_Child(Sound1Tag1);
	MoveSequence->Add_Child(Move1);
	MoveSequence->Add_Child(PushColOn);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.85, 1, 1.1, 0);
	RushParallel->Add_Service(pHitCol);

	return RushParallel;
}

CBT_Composite_Node * CPoisonButterfly::Fire_5Bullet()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("5��ź");
	CBT_Play_Ani* Show_Ani27 = Node_Ani("5��ź", 27, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�߻��غ�");
	CBT_ChaseDir* Chase0 = Node_ChaseDir("���� ����", L"Player_Pos", 1.25, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ȸ��", L"Player_Pos", 0.2);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 3);

	CBT_CreateBullet* PoisonBullet0 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonBullet", L"Bone_Head", L"Self_PoisonDir0", 5, 5, 1.45, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet1 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonBullet", L"Bone_Head", L"Self_PoisonDir1", 5, 5, 1.45, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet2 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonBullet", L"Bone_Head", L"Self_PoisonDir2", 5, 5, 1.45, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet3 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonBullet", L"Bone_Head", L"Self_PoisonDir3", 5, 5, 1.45, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet4 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonBullet", L"Bone_Head", L"Self_PoisonDir4", 5, 5, 1.45, 1, 1, 0, CBT_Service_Node::Finite);

	Root_Parallel->Add_Service(PoisonBullet0);
	Root_Parallel->Add_Service(PoisonBullet1);
	Root_Parallel->Add_Service(PoisonBullet2);
	Root_Parallel->Add_Service(PoisonBullet3);
	Root_Parallel->Add_Service(PoisonBullet4);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani27);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Chase0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Fire_ChaseBullet()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("�ݹ��� ���Ƽ� ����");

	CBT_Sequence* MainSeq = Node_Sequence("�ݹ��� ���Ƽ� ����");
	CBT_Play_Ani* Show_Ani21 = Node_Ani("�ݹ��� ���Ƽ� ����", 21, 0.95f);//23
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	//CBT_Wait* Wait0 = Node_Wait("���", )
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("�Ҹ�1 ���", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("�Ҹ�1 ���", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"Voice_Tag", 10);


	CBT_CreateBullet* PoisonBullet0 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonChaseBullet", L"Bone_Tail6", L"Self_PoisonDir2", 5, 5, 2.5, 1, 1, 0, CBT_Service_Node::Finite);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("��ü������ ���� ���׶�� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("��ü������ ���̴� ���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("��¦�̴� ����� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�Ȱ��� ���� ������ �ְ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 150, 0.01, 0);

	Root_Parallel->Add_Service(PoisonBullet0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);


	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani21);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Turn_4PoisonShot()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("���鼭 ������� �� ������");
	
	CBT_Sequence* MainSeq = Node_Sequence("���鼭 ������� �� ������");
	CBT_Play_Ani* Show_Ani22 = Node_Ani("���鼭 ������� �� ������", 22, 0.95f);//24
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait*	Wait0 = Node_Wait("���", 1.5, 0);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 0);

	CBT_CreateBullet* PoisonBullet0 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonRotationBullet", L"Bone_Tail6", L"Self_Front", 1.5, 3, 1.53, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet1 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonRotationBullet", L"Bone_Tail6", L"Self_Right", 1.5, 3, 1.70, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet2 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonRotationBullet", L"Bone_Tail6", L"Self_Back", 1.5, 3, 1.87, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet3 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonRotationBullet", L"Bone_Tail6", L"Self_Left", 1.5, 3, 2.04, 1, 1, 0, CBT_Service_Node::Finite);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("��ü������ ���� ���׶�� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("��ü������ ���̴� ���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("��¦�̴� ����� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�Ȱ��� ���� ������ �ְ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 150, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);

	Root_Parallel->Add_Service(PoisonBullet0);
	Root_Parallel->Add_Service(PoisonBullet1);
	Root_Parallel->Add_Service(PoisonBullet2);
	Root_Parallel->Add_Service(PoisonBullet3);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani22);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Smart_Eat()
{
	CBT_Sequence* Root_Seq = Node_Sequence("�ȶ��� ���� ��, ������ ��");

	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 5);

	Root_Seq->Add_Child(Right_Eat(0.85f));
	Root_Seq->Add_Child(Dist0);
	Dist0->Set_Child(Left_Eat(0.85f));

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Rush_And_ChaseBullet()
{
	CBT_Simple_Parallel* RushParallel = Node_Parallel_Immediate("���� ����");
	CBT_Sequence* RushAniSeq = Node_Sequence("RushSequence");
	CBT_Play_Ani* Show_Ani15 = Node_Ani("����", 15, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* MoveSequence = Node_Sequence("���� �̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����0", L"Player_Pos", 0.2);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�ڷ� �̵�", L"Monster_Speed", L"Monster_Dir", -3.f, 1.083, 0);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����1", L"Player_Pos", 0.3, 0);
	CBT_RotationDir* Rotation1 = Node_RotationDir("���� ����1", L"Player_Pos", 0.267);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_SetValue* Sound1Stop1 = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play1 = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag1 = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("����", L"Monster_Speed", L"Monster_Dir", 13, 1.1, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);

	CBT_CreateBullet* PoisonBullet0 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonChaseBullet", L"Bone_Tail6", L"Self_PoisonDir2", 5, 5, 1.85, 1, 1, 0, CBT_Service_Node::Finite);
	RushParallel->Add_Service(PoisonBullet0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("��ü������ ���� ���׶�� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 150, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("��ü������ ���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 200, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("����� ������ �����", L"ButterFly_WaterSplash", L"Bone_Tail6", 0, 150, 0, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("��¦�̴� ����� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 150, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�Ȱ��� ���� ������ �ְ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 150, 0, 0);

	RushParallel->Add_Service(Effect0);
	RushParallel->Add_Service(Effect1);
	RushParallel->Add_Service(Effect2);
	RushParallel->Add_Service(Effect3);
	RushParallel->Add_Service(Effect4);

	RushParallel->Set_Main_Child(RushAniSeq);
	RushAniSeq->Add_Child(Show_Ani15);
	RushAniSeq->Add_Child(Show_Ani6);

	RushParallel->Set_Sub_Child(MoveSequence);
	MoveSequence->Add_Child(Rotation0);
	MoveSequence->Add_Child(Sound1Stop);
	MoveSequence->Add_Child(Sound1Play);
	MoveSequence->Add_Child(Sound1Tag);
	MoveSequence->Add_Child(Move0);
	MoveSequence->Add_Child(ChaseDir0);
	MoveSequence->Add_Child(Rotation1);
	MoveSequence->Add_Child(PushColOff);
	MoveSequence->Add_Child(Sound1Stop1);
	MoveSequence->Add_Child(Sound1Play1);
	MoveSequence->Add_Child(Sound1Tag1);
	MoveSequence->Add_Child(Move1);
	MoveSequence->Add_Child(PushColOn);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.85, 1, 1.1, 0);
	RushParallel->Add_Service(pHitCol);

	return RushParallel;
}

CBT_Composite_Node * CPoisonButterfly::WhirlWind()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("������");

	CBT_Sequence* MainSeq = Node_Sequence("������");
	CBT_Play_Ani* Show_Ani16 = Node_Ani("���׸���", 16, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 0, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����", L"Player_Pos", 1.2, 0);
	CBT_SetValue* Sound1Stop1 = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play1 = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag1 = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 3, 0.5, 0);
	CBT_SetValue* Sound2Stop1 = Node_BOOL_SetValue("�Ҹ�2 ���", L"SFX_02_Stop", true);
	CBT_SetValue* Sound2Play1 = Node_BOOL_SetValue("�Ҹ�2 ���", L"SFX_02_Play", true);
	CBT_SetValue* Sound2Tag1 = Node_INT_SetValue("�Ҹ�2 �̸� ����", L"SFX_02_Tag", 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 3, 0.5, 0);


	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("��ü������ ���� ���׶�� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("��ü������ ���̴� ���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("��¦�̴� ����� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�Ȱ��� ���� ������ �ְ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 150, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	// �׽�Ʈ ���� ����
	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani16);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Sound1Stop1);
	SubSeq->Add_Child(Sound1Play1);
	SubSeq->Add_Child(Sound1Tag1);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Sound2Stop1);
	SubSeq->Add_Child(Sound2Play1);
	SubSeq->Add_Child(Sound2Tag1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1, 1, 1.7, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.6, 1, 1.1, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Start_Game()
{
	CBT_Selector* Root_Sel = Node_Selector("���� ����");

	CBT_CompareValue* Check_Upper_HPRatio40 = Node_FLOAT_A_Smaller_Than_Value("ü�� 40�� �̸�", L"HPRatio", 40.f);
	CBT_CompareValue* Check_Upper_HPRatio70 = Node_FLOAT_A_Smaller_Than_Value("ü�� 70�� �̸�", L"HPRatio", 70.f);

	Root_Sel->Add_Child(Check_Upper_HPRatio40);
	Check_Upper_HPRatio40->Set_Child(HP_Final());
	Root_Sel->Add_Child(Check_Upper_HPRatio70);
	Check_Upper_HPRatio70->Set_Child(More_Than_HP_40());
	Root_Sel->Add_Child(More_Than_HP_70());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::More_Than_HP_70()
{
	CBT_Selector* Root_Sel = Node_Selector("�ٰŸ� ���Ÿ� ���� ����");

	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_More_Than_HP70());

	Root_Sel->Add_Child(FarAttack_More_Than_HP70());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::NearAttack_Dist5_More_Than_HP70()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���� ����");

	Root_Sel->Add_Child(Poison_Tornado_After_Charging());
	Root_Sel->Add_Child(OneTurn_Poison());
	Root_Sel->Add_Child(Eat_Turn());
	Root_Sel->Add_Child(Left_Eat());
	Root_Sel->Add_Child(Right_Eat());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::FarAttack_More_Than_HP70()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���Ÿ� ����");

	Root_Sel->Add_Child(Rush());
	Root_Sel->Add_Child(Fire_5Bullet());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::More_Than_HP_40()
{
	CBT_Selector* Root_Sel = Node_Selector("�ٰŸ� ���Ÿ� ���� ����");

	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_More_Than_HP40());

	Root_Sel->Add_Child(FarAttack_More_Than_HP40());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::NearAttack_Dist5_More_Than_HP40()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���� ����");

	Root_Sel->Add_Child(Poison_Tornado_After_Charging());
	Root_Sel->Add_Child(Eat_TurnEat());
	Root_Sel->Add_Child(WhirlWind());
	Root_Sel->Add_Child(Turn_4PoisonShot());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::FarAttack_More_Than_HP40()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���Ÿ� ����");

	Root_Sel->Add_Child(Rush());
	Root_Sel->Add_Child(Fire_5Bullet());
	Root_Sel->Add_Child(Fire_ChaseBullet());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::HP_Final()
{
	CBT_Selector* Root_Sel = Node_Selector("�ٰŸ� ���Ÿ� ���� ����");

	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_Final());

	Root_Sel->Add_Child(FarAttack_Fianl());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::NearAttack_Dist5_Final()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���� ����");

	Root_Sel->Add_Child(Poison_Tornado_After_Charging());
	Root_Sel->Add_Child(Eat_TurnEat());
	Root_Sel->Add_Child(WhirlWind());
	Root_Sel->Add_Child(Smart_Eat());
	Root_Sel->Add_Child(Turn_4PoisonShot());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::FarAttack_Fianl()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���Ÿ� ����");

	Root_Sel->Add_Child(Fire_5Bullet());
	Root_Sel->Add_Child(Rush_And_ChaseBullet());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::Start_Show()
{
	CBT_Selector* Root_Sel = Node_Selector("�ÿ�ȸ");

	CBT_CompareValue* Check_ShowValueN = Node_BOOL_A_Equal_Value("�ÿ�ȸ �ٰŸ� ���� üũ", L"Show_Near", true);

	Root_Sel->Add_Child(Check_ShowValueN);
	Check_ShowValueN->Set_Child(Show_ChaseAndNearAttack());

	Root_Sel->Add_Child(Show_TurnAndFarAttack());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::Show_ChaseAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� �� ������� ����");
	CBT_MoveDirectly* Chase = Node_MoveDirectly_Chase("����", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 5.f, 3.f);
	CBT_RotationDir* Rotation0 = Node_RotationDir("�÷��̾� �ٶ󺸱�", L"Player_Pos", 0.2);

	Root_Seq->Add_Child(Chase);
	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(Show_NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Show_TurnAndFarAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("������� ���Ÿ� ����");
	CBT_RotationDir* TurnDir = Node_RotationDir("Look ȸ��", L"Player_Pos", 0.2);

	Root_Seq->Add_Child(TurnDir);
	Root_Seq->Add_Child(Show_FarAttack());

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Show_NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("������� �ٰŸ� ����");

	CBT_Cooldown* Cool0 = Node_Cooldown("��0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("��1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("��2", 300);
	CBT_Cooldown* Cool3 = Node_Cooldown("��3", 300);
	CBT_Cooldown* Cool4 = Node_Cooldown("��4", 300);
	CBT_Cooldown* Cool5 = Node_Cooldown("��5", 300);
	CBT_Cooldown* Cool6 = Node_Cooldown("��6", 300);

	CBT_SetValue* Show_OffNearAttack = Node_BOOL_SetValue("�ÿ�ȸ OFF", L"Show_Near", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Left_Eat());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Right_Eat());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Eat_TurnEat());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(Poison_Tornado_After_Charging());
	Root_Sel->Add_Child(Cool4);
	Cool4->Set_Child(OneTurn_Poison());
	Root_Sel->Add_Child(Cool5);
	Cool5->Set_Child(Eat_Turn());
	Root_Sel->Add_Child(Cool6);
	Cool6->Set_Child(WhirlWind());

	Root_Sel->Add_Child(Show_OffNearAttack);

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::Show_FarAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("������� ���Ÿ� ����");

	CBT_Cooldown* Cool0 = Node_Cooldown("��0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("��1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("��2", 300);
	CBT_Cooldown* Cool3 = Node_Cooldown("��3", 300);
	CBT_Cooldown* Cool4 = Node_Cooldown("��4", 300);

	CBT_Play_Ani* Show_Ani3 = Node_Ani("�⺻", Ani_Appearance_End, 0.95f);

	CBT_SetValue* Show_ValueOff = Node_BOOL_SetValue("�ÿ�ȸ OFF", L"Show", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Rush());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Fire_5Bullet());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Fire_ChaseBullet());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(Turn_4PoisonShot());
	Root_Sel->Add_Child(Cool4);
	Cool4->Set_Child(Show_Ani3);

	Root_Sel->Add_Child(Show_ValueOff);

	return Root_Sel;
}

void CPoisonButterfly::Down()
{
	m_dDownTime += DELTA_60;

	// down ������.
	if (true == m_bDown_StartAni)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			// �ٿ� ������ ���� ����
			m_pMeshCom->SetUp_Animation(Ani_Down_Loop);
			m_bDown_StartAni = false;
			m_bDown_LoopAni = true;
		}
	}

	// down ������ ����,  ���� �������ε� ���� �� ����
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

			// Down �ȿ��� ���� bool ����
			m_bDown_StartAni = true;
			m_dDownTime = 0;

			m_pMeshCom->SetUp_Animation(Ani_Idle);
		}
	}


	// �浹ó��, bCanHit�� ���Ⱑ false�����ٰ���.
	if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		m_dHitTime = 0;	// �ǰݰ��� Ÿ�� �ʱ�ȭ

		if (true == m_bDown_LoopAni)
		{
			m_pMeshCom->Reset_OldIndx();	//���� �ִ� �ʱ�ȭ
			m_pMeshCom->SetUp_Animation(Ani_Down_Loop);
		}

		m_pAIControllerCom->Reset_BT();
	}
	else
	{
		m_dHitTime += DELTA_60;

		if (m_dHitTime > 0.5)
		{
			m_tObjParam.bIsHit = false;		// ���浹 ����
		}
	}

	//if (true == m_bDown_LoopAni)
	//{
	//	if (m_pMeshCom->Is_Finish_Animation(0.95f))
	//	{
	//		m_pMeshCom->Reset_OldIndx();
	//		m_pMeshCom->SetUp_Animation(Ani_Down_Loop);
	//	}
	//}
}

HRESULT CPoisonButterfly::Update_Bone_Of_BlackBoard()
{
	m_vTail6 = *(_v3*)(&(m_pTail6Frame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Tail6", m_vTail6);

	m_vTail4 = *(_v3*)(&(m_pTail4Frame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Tail4", m_vTail4);

	m_vTail2 = *(_v3*)(&(m_pTail2Frame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Tail2", m_vTail2);

	m_vHead = *(_v3*)(&(m_pHeadFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Head", m_vHead);

	m_vTail6_Tongue2 = *(_v3*)(&(m_pTail6_Tongue2Frame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Tail6_Tongue2", m_vTail6_Tongue2);

	return S_OK;
}

HRESULT CPoisonButterfly::Update_Value_Of_BB()
{
	CGameObject* pPlayer = CMonster::Get_pTargetObject();

	if (nullptr == pPlayer)
		return E_FAIL;

	CTransform* pPlayer_Trans = TARGET_TO_TRANS(pPlayer);

	// 1. �÷��̾� ��ǥ ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Player_Pos", pPlayer_Trans->Get_Pos());
	// 2. ü�� ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HP", m_tObjParam.fHp_Cur);
	// 3. ü�� ���� ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HPRatio", _float(m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max) * 100);

	// ����
	// SFX_01
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_01_Stop"))	// ����
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_01_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::Butterfly_SFX_01);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_01_Play"))	// ���
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_01_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_01_Tag")]), CSoundManager::CHANNELID::Butterfly_SFX_01, CSoundManager::SOUND::Effect_Sound);
	}

	// SFX_02
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_02_Stop"))	// ����
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_02_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::Butterfly_SFX_02);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_02_Play"))	// ���
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_02_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_02_Tag")]), CSoundManager::CHANNELID::Butterfly_SFX_02, CSoundManager::SOUND::Effect_Sound);
	}

	// SFX_03
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_03_Stop"))	// ����
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_03_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::Butterfly_SFX_02);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_03_Play"))	// ���
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_03_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_03_Tag")]), CSoundManager::CHANNELID::Butterfly_SFX_02, CSoundManager::SOUND::Effect_Sound);
	}

	// SFX_04
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_04_Stop"))	// ����
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_04_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::Butterfly_SFX_01);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_04_Play"))	// ���
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_04_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_04_Tag")]), CSoundManager::CHANNELID::Butterfly_SFX_01, CSoundManager::SOUND::Effect_Sound);
	}

	// SFX_05
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_05_Stop"))	// ����
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_05_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::Butterfly_SFX_02);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_05_Play"))	// ���
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_05_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_05_Tag")]), CSoundManager::CHANNELID::Butterfly_SFX_02, CSoundManager::SOUND::Effect_Sound);
	}

	// SFX_06
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_06_Stop"))	// ����
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_06_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::Butterfly_SFX_01);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_06_Play"))	// ���
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_06_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_06_Tag")]), CSoundManager::CHANNELID::Butterfly_SFX_01, CSoundManager::SOUND::Effect_Sound);
	}

	// Voice
	if (true == m_pAIControllerCom->Get_BoolValue(L"Voice_Stop"))	// ����
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Voice_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::ButterFly_Voice);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"Voice_Play"))	// ���
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Voice_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"Voice_Tag")]), CSoundManager::CHANNELID::ButterFly_Voice, CSoundManager::SOUND::Effect_Sound);
	}

	// 1. 5�� ����
	_v3 pPlayerPos = pPlayer_Trans->Get_Pos();
	_v3 vDirTemp0 = *D3DXVec3Normalize(&_v3(), &_v3(pPlayerPos - m_vHead));

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_PoisonDir0", *D3DXVec3TransformNormal(&_v3(), &vDirTemp0, D3DXMatrixRotationY(&_mat(), D3DXToRadian(25))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_PoisonDir1", *D3DXVec3TransformNormal(&_v3(), &vDirTemp0, D3DXMatrixRotationY(&_mat(), D3DXToRadian(12.5f))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_PoisonDir2", vDirTemp0);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_PoisonDir3", *D3DXVec3TransformNormal(&_v3(), &vDirTemp0, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-12.5f))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_PoisonDir4", *D3DXVec3TransformNormal(&_v3(), &vDirTemp0, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-25))));

	// 2. ���� ��ǥ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Pos", m_pTransformCom->Get_Pos());

	// 3. �����̼� �Ѿ� 4����
	_v3 vFrontDir = *D3DXVec3Normalize(&_v3(), (_v3*)m_pTransformCom->Get_WorldMat().m[2]);
	_v3 vRightDir = *D3DXVec3Normalize(&_v3(), (_v3*)m_pTransformCom->Get_WorldMat().m[0]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Front", vFrontDir);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Back", -vFrontDir);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Right", vRightDir);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Left", -vRightDir);

	return S_OK;
}

HRESULT CPoisonButterfly::Update_NF()
{
	// �÷��̾� �߰� �� ���� ��
	if (false == m_bFindPlayer)
	{
		// �÷��̾� ��ǥ ����.
		_v3 vPlayer_Pos = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_Pos();

		// ���� ��ǥ ����
		_v3 vColleague_Pos = _v3(10000.f, 10000.f, 10000.f);

		CGameObject* pColleague = g_pManagement->Get_GameObjectBack(L"Layer_Colleague", SCENE_STAGE);

		// ���ᰡ ������ ��ǥ ����
		if (nullptr != pColleague)
			vColleague_Pos = TARGET_TO_TRANS(pColleague)->Get_Pos();

		// �÷��̾�� ������ �Ÿ�
		_v3 vP_LengthTemp = vPlayer_Pos - m_pTransformCom->Get_Pos();
		vP_LengthTemp.y = 0.f;
		_float fP_Length = D3DXVec3Length(&vP_LengthTemp);

		// ����� ������ �Ÿ�
		_v3 vC_LengthTemp = vColleague_Pos - m_pTransformCom->Get_Pos();
		vC_LengthTemp.y = 0.f;
		_float fC_Length = D3DXVec3Length(&vC_LengthTemp);



		// �÷��̾ �ּҰŸ��ȿ� �ִ°�?
		if (fP_Length < m_fMinLength || fC_Length < m_fMinLength)
		{
			// �÷��̾� �߰�
			m_bFindPlayer = true;
		}
		// �÷��̾ �ִ�Ÿ� �ȿ� �ִ°�?
		else if (fP_Length < m_fMaxLength || fC_Length < m_fMaxLength)
		{
			// �÷��̾ �þ߰� �ȿ� �ִ°�?
			if (Is_InFov(m_fFov, m_pTransformCom, vPlayer_Pos) || Is_InFov(m_fFov, m_pTransformCom, vColleague_Pos))
			{
				// �÷��̾� �߰�
				m_bFindPlayer = true;
			}
			else
			{
				m_pMeshCom->SetUp_Animation(Ani_Appearance);
			}
		}
		// �÷��̾ �ִ�Ÿ� �ۿ� �ִ°�?
		else
			m_pMeshCom->SetUp_Animation(Ani_Appearance);
	}
	// �÷��̾� �߰�
	else
	{
		m_pMeshCom->SetUp_Animation(Ani_Appearance_End);


		if (!m_bAppearanceEffect && m_pMeshCom->Is_Finish_Animation(0.472f))
		{
			g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"SE_GODDESS_BARK_004.ogg"), CSoundManager::CHANNELID::ButterFly_Voice, CSoundManager::SOUND::Effect_Sound);

			// ����Ʈ ���� �� ��ġ ������Ʈ
			Update_Bone_Of_BlackBoard();

			m_bAppearanceEffect = true;
			for (_int i = 0; i < 6; i++)
				g_pManagement->Create_Effect_Delay(L"ButterFly_Crying_Distortion", 0.02f * i, m_vTail6);
		}

		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::ButterFly_Voice);

			m_pMeshCom->SetUp_Animation(Ani_Idle);
			m_bFight = true;

			// ����� �༮ ��׷� ����.
			Set_Target_Auto();
		}

		if (!m_bUITrigger)
		{
			m_bUITrigger = true;

			// �÷��̾� �߰� ��, UI Ȱ��ȭ(����)
			m_pBossUI->Set_Active(true);

			CMassageUI* pMassageUI = static_cast<CMassageUI*>(g_pManagement->Get_GameObjectBack(L"Layer_BossMassageUI", SCENE_STAGE));
			pMassageUI->Set_BossName(BOSS_NAME::Poison_Butterfly);
			pMassageUI->Set_Check_Play_BossnameUI(true);

			g_pSoundManager->Stop_Sound(CSoundManager::Background_Loop);
			g_pSoundManager->Play_BGM(L"Boss_Butterfly_BGM.ogg");
		}
	}

	return S_OK;
}

HRESULT CPoisonButterfly::Update_Collider()
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

	/////// ���ݿ� �ݶ��̴�
	m_vecAttackCol[0]->Update(m_vTail6);
	m_vecAttackCol[1]->Update(m_vTail4);
	m_vecAttackCol[2]->Update(m_vTail2);

	m_pColliderCom->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pColliderCom->Get_Radius().y, 0.f));
	return S_OK;
}

void CPoisonButterfly::Check_PhyCollider()
{
	// �ٿ������ ���� ���Լ��� ź��.
	if (true == m_bDown_Start)
	{
		// �� �ȿ��� �浹ó���� �� ����.
		Down();
	}


	// �Ϲ� �浹ó��, bCanHit�� ���Ⱑ false�����ٰ���.
	else if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		//m_pMeshCom->Reset_OldIndx();	//�ִ� �ε��� �ʱ�ȭ

		//m_bAIController = false;

		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;		//�����ð� �Ŀ� true

		m_dHitTime = 0;	// �ǰݰ��� Ÿ�� �ʱ�ȭ

		//m_bFight = true;		// �ο� ����

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			// ü�� ���� 70 ���ϵǸ� ����
			if (0 == m_iDownCount)
			{
				if (0.7 >= (m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max))
				{
					g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::ButterFly_Voice);
					g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"Los_BossDown_02_gate_f.ogg"), CSoundManager::CHANNELID::ButterFly_Voice, CSoundManager::SOUND::Effect_Sound);

					m_bDown_Start = true;

					m_tObjParam.bDown = true;

					m_pMeshCom->SetUp_Animation(Ani_Down_Start);
					m_bDown_StartAni = true;	//down �Լ� ���ο��� �� ����.
					m_pAIControllerCom->Reset_BT();
					m_bAIController = false;

					m_pAIControllerCom->Set_Value_Of_BlackBoard(L"PushCol", true);
				}
			}

			if (1 == m_iDownCount)
			{
				if (0.4 >= (m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max))
				{
					g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::ButterFly_Voice);
					g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"Los_BossDown_02_gate_f.ogg"), CSoundManager::CHANNELID::ButterFly_Voice, CSoundManager::SOUND::Effect_Sound);

					m_bDown_Start = true;

					m_tObjParam.bDown = true;

					m_pMeshCom->SetUp_Animation(Ani_Down_Start);
					m_bDown_StartAni = true;	//down �Լ� ���ο��� �� ����.
					m_pAIControllerCom->Reset_BT();
					m_bAIController = false;

					m_pAIControllerCom->Set_Value_Of_BlackBoard(L"PushCol", true);
				}
			}




		}
		else
		{
			g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::ButterFly_Voice);
			g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"Los_BossDead_01_gate_f.ogg"), CSoundManager::CHANNELID::ButterFly_Voice, CSoundManager::SOUND::Effect_Sound);
		
			m_pMeshCom->SetUp_Animation(Ani_Death);	// ����ó�� ����
			Start_Dissolve(0.7f, false, true, 0.6f);
			for (_int i = 0; i < 20; i++)
				CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(100.f, m_pTransformCom->Get_Pos(), 0.3f + (i * 0.08f)));
			CParticleMgr::Get_Instance()->Create_BossDeadParticle_Effect(m_pTransformCom->Get_Pos() + _v3(0.f, 1.3f, 0.f), 0.6f, 0.5f);
			g_pManagement->Create_ParticleEffect_Delay(L"SpawnParticle_ForBoss", 1.f, 0.6f, m_pTransformCom->Get_Pos() + _v3(0.f, 1.3f, 0.f));
			 
			//CCameraMgr::Get_Instance()->MainCamera_Oscillatation_SetUp(2.f, 20.f, 0.5f, 0.9f, CCamera::CAM_OSC_TYPE::POS_OSC);
		}
	}
	else
	{
		m_dHitTime += DELTA_60;

		if (m_dHitTime > 0.5)
		{
			m_tObjParam.bIsHit = false;		// ���浹 ����
			//m_tObjParam.bCanHit = true;
		}
	}
}

void CPoisonButterfly::Push_Collider()
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

			// ���� �ӵ��� ���Ƿ� �־��µ� ���ؼ� �־������ - �Ϸ�
			if (m_pColliderCom->Check_Sphere(pCollider, m_pTransformCom->Get_Axis(AXIS_Z), m_pAIControllerCom->Get_FloatValue(L"Monster_Speed")))
			{
				CTransform* pTrans = TARGET_TO_TRANS(iter);
				CNavMesh*   pNav = TARGET_TO_NAV(iter);

				// ���� �����ְ�
				_v3 vDir = m_pTransformCom->Get_Pos() - pTrans->Get_Pos();
				V3_NORMAL_SELF(&vDir);

				// y�� �̵��� ��������
				vDir.y = 0;

				// �׺� �޽�Ÿ�� �� ����
				pTrans->Set_Pos(pNav->Move_OnNaviMesh(NULL, &pTrans->Get_Pos(), &vDir, m_pColliderCom->Get_Length().x));
			}
		}
	}
}

void CPoisonButterfly::OnCollisionEnter()
{
	// =============================================================================================
	// �浹
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

void CPoisonButterfly::OnCollisionEvent(list<CGameObject*> plistGameObject)
{
	// ���� �Ұ����̸� üũ ����
	if (false == m_tObjParam.bCanAttack)
		return;

	_bool bFirst = true;
	//���� ������Ʈ�� �޾ƿͼ�
	for (auto& iter : plistGameObject)
	{
		// ���� �� ���ٸ� ����
		if (false == iter->Get_Target_CanHit())
			continue;

		// ���� ���� Vec �ݶ��̴��� ���Ѵ�.
		for (auto& vecIter : m_vecAttackCol)
		{
			bFirst = true;

			// �����ݶ��̴��� ��
			for (auto& vecCol : iter->Get_PhysicColVector())
			{
				// ��ü ��ü�� ��ǥ�� �ݶ��̴�.
				if (vecIter->Check_Sphere(vecCol))
				{
					// ù��°�� ��豸 �ݶ��̴��ϱ� ���� �ݶ��̴��� �浹ó�� �Ѵ�.
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_PoisonButterFly", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
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

HRESULT CPoisonButterfly::SetUp_ConstantTable(CShader* pShader)
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
	// ���̴� �������� ��ġ �Է�
	//=============================================================================================
	_float	fEmissivePower = 3.f;	// �̹̽ú� : ���� ����, ��ü �߱��� ������.
	_float	fSpecularPower = 10.f;	// ��Ż�Ͻ� : ���� ����, ���ݻ簡 ������.
	_float	fRoughnessPower = 0.50f;	// �����Ͻ� : ���� ����, �� ����� ������(���� ������).
	_float	fRimLightPower = 0.f;	// ��		: ���� ���� ���� ����(������Ʈ�� ������ �о����� , �������� ������).
	_float	fMinSpecular = 1.f;	// �ּ� ��	: ���� ���� ���� ����(������Ʈ�� ������ �о����� , �������� ������).

	if (FAILED(pShader->Set_Value("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fSpecularPower", &fSpecularPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fRoughnessPower", &fRoughnessPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fRimAlpha", &fRimLightPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fMinSpecular", &fMinSpecular, sizeof(_float))))
		return E_FAIL;
	//=============================================================================================

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CPoisonButterfly::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	// �� �ּ� ����
	m_pTail6Frame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Tail6");
	m_pTail4Frame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Tail4");
	m_pTail2Frame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Tail2");
	m_pHeadFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head");
	m_pTail6_Tongue2Frame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Tail6_Tongue2");

	return S_OK;
}

HRESULT CPoisonButterfly::Ready_Collider()
{
	m_vecPhysicCol.reserve(10);

	//���üũ��
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Range]->_41, m_matBones[Bone_Range]->_42, m_matBones[Bone_Range]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//��
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 1.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Body]->_41, m_matBones[Bone_Body]->_42, m_matBones[Bone_Body]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//�Ӹ�
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 0.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Head]->_41, m_matBones[Bone_Head]->_42, m_matBones[Bone_Head]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);


	//////////// ���ݿ� �ݶ��̴�
	m_vecAttackCol.reserve(5);

	// ���� ��
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 1.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(m_vTail6);
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	// ���� �߰�
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 1.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(m_vTail4);
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	// ���� ó��
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 1.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(m_vTail2);
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CPoisonButterfly::Ready_NF(void * pArg)
{
	INFO eTemp = *(INFO*)pArg;

	m_fFov = eTemp.fFov;
	m_fMaxLength = eTemp.fMaxLength;
	m_fMinLength = eTemp.fMinLength;

	//===================================================================
	// ������Ʈ ���� ����	
	//===================================================================
	m_pBattleAgentCom->Set_RimAlpha(0.5f);
	m_pBattleAgentCom->Set_RimValue(8.f);
	m_pBattleAgentCom->Set_OriginRimAlpha(0.5f);
	m_pBattleAgentCom->Set_OriginRimValue(8.f);
	//===================================================================

	//===================================================================
	// Ʈ���� ��ȯ�� ����
	//===================================================================
	// Ʈ���� ��ȯ�Ǵ� �༮�̸�
	if (eTemp.bSpawnOnTrigger)
	{
		// �������� ��ȣ�� �׺� �ҷ���
		_tchar szNavData[STR_128] = L"";

		lstrcpy(szNavData, (
			eTemp.sStageIdx == 0 ? L"Navmesh_Training.dat" :
			eTemp.sStageIdx == 1 ? L"Navmesh_Stage_01.dat" :
			eTemp.sStageIdx == 2 ? L"Navmesh_Stage_02.dat" :
			eTemp.sStageIdx == 3 ? L"Navmesh_Stage_03.dat" : L"Navmesh_Stage_04.dat"));

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

HRESULT CPoisonButterfly::Ready_Sound()
{
	m_mapSound.emplace(0, L"SE_GODDESS_ATTACK_SWING_000.ogg");	// ���� ���� �ٶ��Ҹ�
	m_mapSound.emplace(1, L"SE_GODDESS_KETSUGI_MOVE_000.ogg");	// ����
	m_mapSound.emplace(2, L"skybridge3.ogg");	// ȸ����
	m_mapSound.emplace(3, L"SE_GODDESS_KETSUGI_001.ogg");	// �Ѿ� �߻�
	m_mapSound.emplace(10, L"SE_GODDESS_BARK_004.ogg");	// �����ϱ� �� �Ҹ�������
	m_mapSound.emplace(11, L"SE_GODDESS_BARK_007.ogg");	// ���� �Ҹ�������
	
	return S_OK;
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

	CMonster::Free();
}
