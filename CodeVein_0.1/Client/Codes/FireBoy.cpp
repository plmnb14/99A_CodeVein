#include "stdafx.h"
#include "..\Headers\FireBoy.h"
#include "..\Headers\BossHP.h"

CFireBoy::CFireBoy(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CFireBoy::CFireBoy(const CFireBoy & rhs)
	: CMonster(rhs)
{
}

HRESULT CFireBoy::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CFireBoy::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_NF(pArg);
	Ready_Weapon();
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

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", m_tObjParam.fHp_Cur);
	pBlackBoard->Set_Value(L"MAXHP", m_tObjParam.fHp_Max);

	pBlackBoard->Set_Value(L"Show", true);
	pBlackBoard->Set_Value(L"Show_Near", true);

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

	pBlackBoard->Set_Value(L"CamShake1", false);


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

	//Start_Sel->Add_Child(Start_Game());

	//CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.2);
	//Start_Sel->Add_Child(Rotation0);

	//CBT_Wait* Wait0 = Node_Wait("���", 1, 0);
	//Start_Sel->Add_Child(Wait0);


	/////////////
	// UI �߰�(����)
	m_pBossUI = static_cast<CBossHP*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BossHP", nullptr));
	m_pBossUI->Set_UI_Pos(WINCX * 0.5f, WINCY * 0.1f);
	m_pBossUI->Set_BossName(CBossNameUI::Index_FireBoy);
	if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBossUI, SCENE_STAGE, L"Layer_BossHP", nullptr)))
		return E_FAIL;
	return S_OK;
}

_int CFireBoy::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	Push_Collider();

	CGameObject::Update_GameObject(TimeDelta);

	// �׾��� ���
	if (m_bIsDead)
		return DEAD_OBJ;
	
	// ���� �ִϸ��̼�
	if (m_bReadyDead)
	{
		// �ױ��� UI ��Ȱ��ȭ
		m_pBossUI->Set_Active(false);

		if (false == m_bFinishCamShake && m_pMeshCom->Is_Finish_Animation(0.5f))
		{
			m_bFinishCamShake = true;
			SHAKE_CAM_lv3;

			g_pSoundManager->Stop_Sound(CSoundManager::Effect_SFX_01);
			g_pSoundManager->Play_Sound(L"Boss_DeadEff.ogg", CSoundManager::Effect_SFX_01, CSoundManager::Effect_Sound);
		}

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
		if(0 >= m_tObjParam.fHp_Cur)
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

_int CFireBoy::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == m_pRendererCom)
		return E_FAIL;

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

	return NOERROR;
}

HRESULT CFireBoy::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	m_pMeshCom->Play_Animation(_float(m_dTimeDelta)); // * alpha

	if (m_bInFrustum)
	{
		if (FAILED(SetUp_ConstantTable(m_pShaderCom)))
			return E_FAIL;

		m_pShaderCom->Begin_Shader();

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

HRESULT CFireBoy::Render_GameObject_Instancing_SetPass(CShader * pShader)
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

HRESULT CFireBoy::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
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
			pShader->Begin_Pass(iPass);
			pShader->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	//============================================================================================


	return NOERROR;
}

CBT_Composite_Node * CFireBoy::Arm_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�Ȳ�ġ ġ��");
	CBT_Play_Ani* Show_Ani23 = Node_Ani("�Ȳ�ġ ġ��", 23, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.0f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.183, 0);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("��Ҹ� ���", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("��Ҹ� ���", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("��Ҹ� �̸� ����", L"Voice_Tag", 11);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.367f, 0);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("��Ҹ� ���", L"CamShake1", true);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 6);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.05f, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ٴ� ����", L"FireBoy_ArmAttack_FloorDust", L"Bone_LeftForeArm"	, 0.76, 3, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ����", L"FireBoy_ArmAttack_Dust", L"Bone_LeftForeArm"		, 0.8, 1, 0, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani23);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Move1);


	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 0.65, 1, 0.1, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Gun_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�������� ġ��");
	CBT_Play_Ani* Show_Ani30 = Node_Ani("�������� ġ��", 30, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.0f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.116, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.767, 0);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("��Ҹ� ���", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("��Ҹ� ���", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("��Ҹ� �̸� ����", L"Voice_Tag", 11);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 7);
	CBT_SetValue* CamShake = Node_BOOL_SetValue("��Ҹ� ���", L"CamShake1", true);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.847, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 0.6f, 0.17, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani30);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(VoiceStop);
	SubSeq->Add_Child(VoicePlay);
	SubSeq->Add_Child(VoiceTag);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(CamShake);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);




	CBT_CreateBullet* Col0 = Node_CreateBullet("���� ������ Col", L"Monster_GunAttackCol", L"GunAttackPos", L"", 0, 0.07, 1.01, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_Tornado()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�� ����̵�");
	CBT_Play_Ani* Show_Ani21 = Node_Ani("�� ����̵�", 21, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.0f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����", L"Player_Pos", 0.2);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.1, 0);
	CBT_SetValue* VoiceStop = Node_BOOL_SetValue("��Ҹ� ���", L"Voice_Stop", true);
	CBT_SetValue* VoicePlay = Node_BOOL_SetValue("��Ҹ� ���", L"Voice_Play", true);
	CBT_SetValue* VoiceTag = Node_INT_SetValue("��Ҹ� �̸� ����", L"Voice_Tag", 12);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 2);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 2.5f, 0.233, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 1.85, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 0.7f, 0.3, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� �غ� - �޼տ� ��"	, L"FireBoy_Charge_Hand_Fire"		, L"Bone_LeftHand"	, 0.6, 50, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ���� - �ٴڿ� ��"	, L"FireBoy_FireTornade_ReadyFire"	, L"SelfPos"		, 0.55, 1, 0, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("���� ���� - �ұ��"		, L"FireBoy_FireTornade_ReadyFire_Up", L"SelfPos"		, 1.15, 3, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("����̵�"					, L"FireBoy_FireTornade_Mesh"		, L"SelfPos"		, 1.3, 1, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("����̵�"					, L"FireBoy_FireTornade_Mesh"		, L"SelfPos"		, 1.5, 1, 0, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("����̵�"				, L"FireBoy_FireTornade_Mesh"		, L"SelfPos"		, 1.7, 1, 0, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("����̵�"				, L"FireBoy_FireTornade_Mesh_2"		, L"SelfPos"		, 1.4, 1, 0, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("����̵�"				, L"FireBoy_FireTornade_Mesh_2"		, L"SelfPos"		, 1.6, 1, 0, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("����̵�"				, L"FireBoy_FireTornade_Mesh_2"		, L"SelfPos"		, 1.8, 1, 0, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�ٴڿ� ��ϰ�����"		, L"FireBoy_FireTornade_Floor_01"	, L"SelfPos"		, 1.1, 30, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�ٴڿ� ��ϰ�����"		, L"FireBoy_FireTornade_Floor_02"	, L"SelfPos"		, 1.1, 30, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("�ٴڿ� ��ϰ�����"		, L"FireBoy_FireTornade_Floor_03"	, L"SelfPos"		, 1.1, 30, 0, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("�Ҷ�"						, L"FireBoy_Fire_Particle_01"		, L"SelfPos"		, 0.95, 60, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("�Ҷ�"						, L"FireBoy_Fire_Particle_02"		, L"SelfPos"		, 0.95, 60, 0, 0);

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

	CBT_CreateBullet* Col0 = Node_CreateBullet("����̵� �浹ü", L"Monster_FireTornadoCol", L"SelfPos", L"", 0, 0.8, 1.856, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Back_Dash()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�ڷ� �뽬");
	CBT_Play_Ani* Show_Ani9 = Node_Ani("�ڷ� �뽬", 9, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.0f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", -5.f, 0.333, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", -7.f, 0.433, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", -3.f, 0.517, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani9);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_Tracking()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�����ϸ鼭 �� �߻�");
	CBT_Play_Ani* Show_Ani27 = Node_Ani("�����ϸ鼭 �� �߻�", 27, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.0f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 4);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.883, 0);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����", L"Player_Pos", 2.467, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ѱ� �Ա� ����Ʈ", L"FireBoy_FireBullet_GunEff", L"Bone_Muzzle", 1.0, 70, 0, 0);

	Root_Parallel->Add_Service(Effect0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani27);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(ChaseDir0);

	//CBT_CreateBullet* Col0 = Node_CreateBullet("ȭ�� �߻�", L"Monster_FireBullet", L"Bone_Muzzle", L"StraightFireDir", 8, 1, 0.833, 12, 0.2, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col0 = Node_CreateBullet("ȭ�� �߻�", L"Monster_FireBullet", L"Bone_Muzzle", L"StraightFireDir", 8, 1, 0.833, 100, 0.025, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_Cone()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("��ä�÷� �� �߻�");
	CBT_Play_Ani* Show_Ani26 = Node_Ani("��ä�÷� �� �߻�", 26, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.0f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 4);
	CBT_Wait* Wait0 = Node_Wait("���0", 1.166, 0);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����", L"Player_Pos", 4.05, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ѱ� �Ա� ����Ʈ", L"FireBoy_FireBullet_GunEff", L"Bone_Muzzle", 1.0, 200, 0, 0);

	Root_Parallel->Add_Service(Effect0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani26);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(ChaseDir0);



	CBT_CreateBullet* Col0 = Node_CreateBullet("ȭ�� �߻�", L"Monster_FireBullet", L"Bone_Muzzle", L"FireDir", 8, 1, 1.166, 120, 0.025, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_BigSphere()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("���� ȭ���� �߻�");
	CBT_Play_Ani* Show_Ani24 = Node_Ani("���� ȭ���� �߻�", 24, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.0f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.283, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 1.f, 0.35, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����", L"Player_Pos", 0.183);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 3);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.884, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", -1.f, 0.35, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ѱ� �Ա� ����Ʈ", L"FireBoy_FireBullet_GunEff", L"Bone_Muzzle", 0.7, 10, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("�ѱ� �Ա� �� ����Ʈ", L"FireBoy_FireSphere_ShotCircle", L"Bone_Muzzle", 0.8, 1, 0, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani24);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);


	CBT_CreateBullet* Col0 = Node_CreateBullet("ȭ���� �߻�", L"Monster_FireSphere", L"Bone_Muzzle", L"FireDir", 12, 1.5, 0.816, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_Ground()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("Ÿ�� �ٴڿ��� ��");
	CBT_Play_Ani* Show_Ani22 = Node_Ani("Ÿ�� �ٴڿ��� ��", 22, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.0f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����", L"Player_Pos", 0.2);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.733, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 4.f, 0.233, 0);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 2);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.984, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", -4.f, 0.233, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�޼տ� ��", L"FireBoy_Charge_Hand_Fire", L"Bone_LeftHand", 0.6, 30, 0, 0);

	Root_Parallel->Add_Service(Effect0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani22);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);


	CBT_CreateBullet* Col0 = Node_CreateBullet("Ÿ�� �ٴڿ� �ҳ�", L"Monster_FireGround", L"Player_Pos", L"", 0, 5, 1.266, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col1 = Node_CreateBullet("Ÿ�� �ٴڿ� �ҳ�", L"Monster_FireGround", L"Random_FireGround_Pos0", L"", 0, 5, 1.266, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col2 = Node_CreateBullet("Ÿ�� �ٴڿ� �ҳ�", L"Monster_FireGround", L"Random_FireGround_Pos1", L"", 0, 5, 1.266, 1, 0, 0, CBT_Service_Node::Finite);

	Root_Parallel->Add_Service(Col0);
	Root_Parallel->Add_Service(Col1);
	Root_Parallel->Add_Service(Col2);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_Flame()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("���� ���ڰ� ��");
	CBT_Play_Ani* Show_Ani19 = Node_Ani("���� ���ڰ� ��", 19, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����", L"Player_Pos", 0.2);
	CBT_Wait* Wait0 = Node_Wait("���0", 1.133, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 1.5f, 0.633, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.484, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", -1.5f, 0.633, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�޼տ� ��", L"FireBoy_FireHandBall_Before_HandFire", L"Bone_LeftHand", 0.4, 1, 0, 0);

	Root_Parallel->Add_Service(Effect0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani19);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);


	/* 
	1. FireHandBall �տ��� ����
	2. FireHandBall�� ������ ��, �������鼭 FireFlame ����.
	*/

	CBT_CreateBuff* Bullet0 = Node_CreateBuff("ȭ���� �޼տ� ����", L"Monster_FireHandBall", 2, 0.716, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Bullet0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Start_Game()
{
	CBT_Selector* Root_Sel = Node_Selector("���� ����");

	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 4);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist4());

	Root_Sel->Add_Child(FarAttack());

	return Root_Sel;
}

CBT_Composite_Node * CFireBoy::NearAttack_Dist4()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� Dist4 ����");

	Root_Seq->Add_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CFireBoy::NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���� ����");

	Root_Sel->Add_Child(Arm_Attack());
	Root_Sel->Add_Child(Gun_Attack());
	Root_Sel->Add_Child(Fire_Tornado());
	Root_Sel->Add_Child(Back_Dash());

	return Root_Sel;
}

CBT_Composite_Node * CFireBoy::FarAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���Ÿ� ����");

	Root_Sel->Add_Child(Fire_Tracking());
	Root_Sel->Add_Child(Fire_Cone());
	Root_Sel->Add_Child(Fire_BigSphere());
	Root_Sel->Add_Child(Fire_Ground());
	Root_Sel->Add_Child(Fire_Flame());
	Root_Sel->Add_Child(Fire_BigSphere());

	return Root_Sel;
}

CBT_Composite_Node * CFireBoy::Start_Show()
{
	CBT_Selector* Root_Sel = Node_Selector("�ÿ�ȸ");

	CBT_CompareValue* Check_ShowValueN = Node_BOOL_A_Equal_Value("�ÿ�ȸ �ٰŸ� ���� üũ", L"Show_Near", true);

	Root_Sel->Add_Child(Check_ShowValueN);
	Check_ShowValueN->Set_Child(Show_NearAttack());

	Root_Sel->Add_Child(Show_FarAttack());

	return Root_Sel;
}

CBT_Composite_Node * CFireBoy::Show_NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("������� �ٰŸ� ����");

	CBT_Cooldown* Cool0 = Node_Cooldown("��0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("��1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("��2", 300);

	CBT_SetValue* Show_OffNearAttack = Node_BOOL_SetValue("�ÿ�ȸ OFF", L"Show_Near", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Arm_Attack());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Gun_Attack());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Fire_Tornado());

	Root_Sel->Add_Child(Show_OffNearAttack);

	return Root_Sel;
}

CBT_Composite_Node * CFireBoy::Show_FarAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("������� ���Ÿ� ����");
	
	CBT_Cooldown* Cool0 = Node_Cooldown("��0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("��1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("��2", 300);
	CBT_Cooldown* Cool3 = Node_Cooldown("��3", 300);
	CBT_Cooldown* Cool4 = Node_Cooldown("��4", 300);
	CBT_Cooldown* Cool5 = Node_Cooldown("��5", 300);

	CBT_Play_Ani* Show_Ani3 = Node_Ani("�⺻", Ani_Appearance_End, 0.95f);

	CBT_SetValue* Show_ValueOff = Node_BOOL_SetValue("�ÿ�ȸ OFF", L"Show", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Fire_Tracking());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Fire_Cone());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Fire_BigSphere());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(Fire_Ground());
	Root_Sel->Add_Child(Cool4);
	Cool4->Set_Child(Fire_Flame());
	Root_Sel->Add_Child(Cool5);
	Cool5->Set_Child(Show_Ani3);

	Root_Sel->Add_Child(Show_ValueOff);

	return Root_Sel;
}

void CFireBoy::Down()
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
			m_bDown_Finish = true;
			m_bAIController = true;

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
			m_pMeshCom->Reset_OldIndx();	//���� �ִ� �ʱ�ȭ

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
}

HRESULT CFireBoy::Update_Bone_Of_BlackBoard()
{
	m_vLeftForeArm = *(_v3*)(&(m_pLeftForeArmFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftForeArm", m_vLeftForeArm);

	m_vLeftHand = *(_v3*)(&(m_pLeftHandFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftHand", m_vLeftHand);

	m_vLeftHandAttach = *(_v3*)(&(m_pLeftHandAttachFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftHandAttach", m_vLeftHandAttach);

	m_vLeftHandMiddle2 = *(_v3*)(&(m_pLeftHandMiddle2Frame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftHandMiddle2", m_vLeftHandMiddle2);

	m_vMuzzle = *(_v3*)(&(m_pMuzzleFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Muzzle", m_vMuzzle);

	m_vRightHand = *(_v3*)(&(m_pRightHandFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_RightHand", m_vRightHand);

	return S_OK;
}

HRESULT CFireBoy::Update_Value_Of_BB()
{
	CGameObject* pPlayer = CMonster::Get_pTargetObject();

	if (nullptr == pPlayer)
		return E_FAIL;

	// 1. �÷��̾� ��ǥ ������Ʈ
	CTransform* pPlayerTransCom = TARGET_TO_TRANS(pPlayer);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Player_Pos", pPlayerTransCom->Get_Pos());
	// 2. ü�� ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HP", m_tObjParam.fHp_Cur);
	// 3. ü�� ���� ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HPRatio", _float(m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max) * 100);



	// ����
	// SFX_01
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_01_Stop"))	// ����
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_01_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::FireBoy_SFX_01);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_01_Play"))	// ���
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_01_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_01_Tag")]), CSoundManager::CHANNELID::FireBoy_SFX_01, CSoundManager::SOUND::Effect_Sound);
	}

	// SFX_02
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_02_Stop"))	// ����
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_02_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::FireBoy_SFX_02);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_02_Play"))	// ���
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_02_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_02_Tag")]), CSoundManager::CHANNELID::FireBoy_SFX_02, CSoundManager::SOUND::Effect_Sound);
	}

	// Voice
	if (true == m_pAIControllerCom->Get_BoolValue(L"Voice_Stop"))	// ����
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Voice_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::FireBoy_Voice);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"Voice_Play"))	// ���
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Voice_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"Voice_Tag")]), CSoundManager::CHANNELID::FireBoy_Voice, CSoundManager::SOUND::Effect_Sound);
	}


	// ī�޶� ����
	if (true == m_pAIControllerCom->Get_BoolValue(L"CamShake1"))
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"CamShake1", false);

		SHAKE_CAM_lv1;
	}



	// 1. �������� ������ Collider ��ǥ,  ���� ���İ��� �������� �浹�� �� �ȵǹǷ� �ݶ��̴� ��¦ �������� �浹�� ����.
	_mat matSelf = m_pTransformCom->Get_WorldMat();
	_v3 vSelfPos = *(_v3*)matSelf.m[3];
	_v3 vSelfLook = *D3DXVec3Normalize(&_v3(), (_v3*)matSelf.m[2]);
	_v3 vSelfRight = *D3DXVec3Normalize(&_v3(), (_v3*)matSelf.m[0]);

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"GunAttackPos", vSelfPos + vSelfLook * 1.5f + _v3(0.f, 1.f, 0.f));
	
	// 2. ���ſ��� ���ڷ� �� �߻��ϴ� ����

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"StraightFireDir", *D3DXVec3Normalize (&_v3(), &(m_vMuzzle - m_vRightHand + _v3(0.f, -0.2f, 0.f))));

	// 3. ���ſ��� ��ä��, ȭ���� �߻��ϴ� ����

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FireDir", *D3DXVec3Normalize(&_v3(), &(m_vMuzzle - m_vRightHand)));


	// 4. Flame, �޼տ��� ���� �� ����.
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlameHandBallDir", *D3DXVec3Normalize(&_v3(), &(m_vLeftHandMiddle2 - m_vLeftHandAttach)));


	// 5. ȭ�� ���̳׵�
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SelfPos", vSelfPos);

	// 6. �� �ٴڿ� �, �÷��̾� + ���� ���� ��ǥ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Random_FireGround_Pos0", pPlayerTransCom->Get_Pos() + _v3(_float(CALC::Random_Num_Double(-8, 8)), 0.f, _float(CALC::Random_Num_Double(-8, 8))));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Random_FireGround_Pos1", pPlayerTransCom->Get_Pos() + _v3(_float(CALC::Random_Num_Double(-8, 8)), 0.f, _float(CALC::Random_Num_Double(-8, 8))));


	return S_OK;
}

HRESULT CFireBoy::Update_NF()
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

		if (m_pMeshCom->Is_Finish_Animation(0.7f) && m_bHello)
		{
			m_bHello = false;

			g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::FireBoy_Voice);
			g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"Los_KetsugiWiderangeShoot_02_gate_m.ogg"), CSoundManager::CHANNELID::FireBoy_Voice, CSoundManager::SOUND::Effect_Sound);
		}

		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_pMeshCom->SetUp_Animation(Ani_Idle);
			m_bFight = true;

			// ����� �༮ ��׷� ����.
			Set_Target_Auto();
		}

		// �÷��̾� �߰� ��, UI Ȱ��ȭ(����)
		m_pBossUI->Set_Active(true);
	}

	return S_OK;
}

HRESULT CFireBoy::Update_Collider()
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

	///////// ���ݿ� �ݶ��̴�
	m_vecAttackCol[0]->Update(m_vLeftForeArm);

	m_pColliderCom->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pColliderCom->Get_Radius().y, 0.f));
	return S_OK;
}

void CFireBoy::Check_PhyCollider()
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
		m_tObjParam.bCanHit = true;

		m_dHitTime = 0;	// �ǰݰ��� Ÿ�� �ʱ�ȭ

		//m_bFight = true;		// �ο� ����
		m_bFindPlayer = true;

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			// ü�� ���� 70 ���ϵǸ� ����
			if (false == m_bDown_Finish)
			{
				if (0.7 >= (m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max))
				{
					g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::FireBoy_Voice);
					g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"Los_BossDown_01_gate_m.ogg"), CSoundManager::CHANNELID::FireBoy_Voice, CSoundManager::SOUND::Effect_Sound);


					m_bDown_Start = true;

					m_tObjParam.bDown = true;

					m_pMeshCom->SetUp_Animation(Ani_Down_Start);
					m_bDown_StartAni = true;	//down �Լ� ���ο��� �� ����.
					m_pAIControllerCom->Reset_BT();
					m_bAIController = false;

				}
			}

		}
		else
		{
			g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::FireBoy_Voice);
			g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"Los_BossDead_01_gate_m.ogg"), CSoundManager::CHANNELID::FireBoy_Voice, CSoundManager::SOUND::Effect_Sound);

			m_pMeshCom->SetUp_Animation(Ani_Death);	// ����ó�� ����
			Start_Dissolve(0.4f, false, true, 4.2f);
			for (_int i = 0; i < 20; i++)
				CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(_float(CCalculater::Random_Num(100, 300)), m_pTransformCom->Get_Pos(), 4.3f + (i * 0.08f)));
			CParticleMgr::Get_Instance()->Create_BossDeadParticle_Effect(m_pTransformCom->Get_Pos() + _v3(0.f, 1.3f, 0.f), 3.9f, 0.5f);
			g_pManagement->Create_ParticleEffect_Delay(L"SpawnParticle_ForBoss", 2.5f, 4.2f, m_pTransformCom->Get_Pos() + _v3(0.f, 0.5f, 0.f));
		}
	}
	else
	{
		m_dHitTime += DELTA_60;

		if (m_dHitTime > 0.5)
		{
			m_tObjParam.bIsHit = false;		// ���浹 ����
		}
	}
}

void CFireBoy::Push_Collider()
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

void CFireBoy::OnCollisionEnter()
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

void CFireBoy::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

HRESULT CFireBoy::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_FireBoy", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
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

HRESULT CFireBoy::SetUp_ConstantTable(CShader* pShader)
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

	//=============================================================================================
	// ���̴� �������� ��ġ �Է�
	//=============================================================================================
	_float	fEmissivePower = 5.f;	// �̹̽ú� : ���� ����, ��ü �߱��� ������.
	_float	fSpecularPower = 3.f;	// ��Ż�Ͻ� : ���� ����, ���ݻ簡 ������.
	_float	fRoughnessPower = 4.f;	// �����Ͻ� : ���� ����, �� ����� ������(���� ������).
	_float	fRimLightPower = 0.f;	// ��		: ���� ���� ���� ����(������Ʈ�� ������ �о����� , �������� ������).
	_float	fMinSpecular = 0.1f;	// �ּ� ��	: ���� ���� ���� ����(������Ʈ�� ������ �о����� , �������� ������).

	if (FAILED(m_pShaderCom->Set_Value("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fSpecularPower", &fSpecularPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fRoughnessPower", &fRoughnessPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fRimAlpha", &fRimLightPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fMinSpecular", &fMinSpecular, sizeof(_float))))
		return E_FAIL;
	//=============================================================================================


	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CFireBoy::Ready_Weapon()
{
	return NOERROR;
}

HRESULT CFireBoy::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	// �� �ּ�
	m_pLeftForeArmFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftForeArm");
	m_pLeftHandFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHand");
	m_pLeftHandAttachFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHandAttach");
	m_pLeftHandMiddle2Frame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHandMiddle2");
	m_pMuzzleFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Muzzle");
	m_pRightHandFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHand");

	return S_OK;
}

HRESULT CFireBoy::Ready_Collider()
{
	m_vecPhysicCol.reserve(10);

	//���üũ��
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.4f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Range]->_41, m_matBones[Bone_Range]->_42, m_matBones[Bone_Range]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//��
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 0.95f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Body]->_41, m_matBones[Bone_Body]->_42, m_matBones[Bone_Body]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//�Ӹ�
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 0.2f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Head]->_41, m_matBones[Bone_Head]->_42, m_matBones[Bone_Head]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);


	//////////////// ���ݿ� �ݶ��̴�
	m_vecAttackCol.reserve(5);

	// �Ȳ�ġ
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 1.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(m_vLeftForeArm);
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CFireBoy::Ready_NF(void * pArg)
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

HRESULT CFireBoy::Ready_Sound()
{
	m_mapSound.emplace(0, L"SE_GATE_KEEPER_MAN_MOVE_001.ogg");
	m_mapSound.emplace(1, L"SE_GATE_WOMAN_KETUGI_ATTACK_FIRE_002.ogg");
	m_mapSound.emplace(2, L"SE_GATE_KEEPER_MAN_FLAME_RADIATION_000.ogg");
	m_mapSound.emplace(3, L"SE_GATE_KEEPER_MAN_KETSUGI_IMPACT_FIRE_002.ogg");
	m_mapSound.emplace(4, L"SE_GATE_KEEPER_MAN_KETSUGI_IMPACT_FIRE_006.ogg");
	m_mapSound.emplace(5, L"SE_GATE_KEEPER_MAN_KETSUGI_IMPACT_FIRE_000.ogg");
	m_mapSound.emplace(6, L"SE_GATE_KEEPER_MAN_ATTACK_IMPACT_000.ogg");
	m_mapSound.emplace(7, L"SE_GATE_KEEPER_MAN_ATTACK_TACKLE_000.ogg");
	
	
	m_mapSound.emplace(10, L"Los_AttackFlame03_01_gate_m.ogg");
	m_mapSound.emplace(11, L"Los_AttackTackle01_01_gate_m.ogg");
	
	return S_OK;
}

CFireBoy * CFireBoy::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFireBoy* pInstance = new CFireBoy(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CFireBoy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFireBoy::Clone_GameObject(void * pArg)
{
	CFireBoy* pInstance = new CFireBoy(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CFireBoy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFireBoy::Free()
{

	CMonster::Free();
}