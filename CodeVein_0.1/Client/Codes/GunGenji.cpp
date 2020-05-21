#include "stdafx.h"
#include "..\Headers\GunGenji.h"

CGunGenji::CGunGenji(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
	ZeroMemory(m_matBones, sizeof(_mat*) * Bone_End);
}

CGunGenji::CGunGenji(const CGunGenji & rhs)
	: CMonster(rhs)
{
	ZeroMemory(m_matBones, sizeof(_mat*) * Bone_End);
}

HRESULT CGunGenji::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CGunGenji::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	Ready_NF(pArg);

	Ready_Weapon();
	Ready_BoneMatrix();
	Ready_Collider();
	Ready_Sound();
	Ready_Rigid();

	m_tObjParam.bCanRepel = true;			// ƨ�ܳ��� ���ɼ� (���� ������ �����Լ�)
	m_tObjParam.bCanCounter = true;			// �ݰݰ��ɼ�
	m_tObjParam.bCanExecution = true;		// ó��
	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 5000.f;
	m_tObjParam.fHp_Max = m_tObjParam.fHp_Cur;

	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	/////////////////////////////////////////////////////////

	m_pMonsterUI = static_cast<CMonsterUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MonsterHPUI", pArg));
	m_pMonsterUI->Set_Target(this);
	m_pMonsterUI->Set_Bonmatrix(m_matBones[Bone_Head]);
	m_pMonsterUI->Ready_GameObject(NULL);

	////////////////// �ൿƮ�� init

	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL);

	if (nullptr == pPlayer)
		return E_FAIL;

	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create();

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(pPlayer)->Get_Pos());
	pBlackBoard->Set_Value(L"HP", m_tObjParam.fHp_Cur);
	pBlackBoard->Set_Value(L"MAXHP", m_tObjParam.fHp_Max);
	pBlackBoard->Set_Value(L"Show", true);

	pBlackBoard->Set_Value(L"TrailOn", false);
	pBlackBoard->Set_Value(L"TrailOff", false);

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
	//CBT_Sequence* Start_Sel = Node_Sequence("�ൿ ����"); // �׽�Ʈ

	pBehaviorTree->Set_Child(Start_Sel);

	//CBT_CompareValue* Check_ShowValue = Node_BOOL_A_Equal_Value("�ÿ�ȸ ���� üũ", L"Show", true);
	//Check_ShowValue->Set_Child(Start_Show());
	//Start_Sel->Add_Child(Check_ShowValue);
	Start_Sel->Add_Child(Start_Game());

	//Start_Sel->Add_Child(Arm_Attack());

	//CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.2);
	//Start_Sel->Add_Child(Rotation0);

	///////////�����ֱ��

	//CBT_RotationDir* TurnDir0 = Node_RotationDir("Look ȸ��", L"Player_Pos", 0.15);
	//Start_Sel->Add_Child(TurnDir0);

	//Start_Sel->Add_Child(Shot());

	//CBT_RotationDir* TurnDir1 = Node_RotationDir("Look ȸ��", L"Player_Pos", 0.15);
	//Start_Sel->Add_Child(TurnDir1);

	//Start_Sel->Add_Child(Tumbling_Shot());

	//CBT_RotationDir* TurnDir2 = Node_RotationDir("Look ȸ��", L"Player_Pos", 0.15);
	//Start_Sel->Add_Child(TurnDir2);

	//Start_Sel->Add_Child(Sudden_Shot());

	//CBT_RotationDir* TurnDir3 = Node_RotationDir("Look ȸ��", L"Player_Pos", 0.15);
	//Start_Sel->Add_Child(TurnDir3);

	//Start_Sel->Add_Child(Upper_Slash());

	//CBT_RotationDir* TurnDir4 = Node_RotationDir("Look ȸ��", L"Player_Pos", 0.15);
	//Start_Sel->Add_Child(TurnDir4);

	//Start_Sel->Add_Child(Arm_Attack());

	//CBT_RotationDir* TurnDir5 = Node_RotationDir("Look ȸ��", L"Player_Pos", 0.15);
	//Start_Sel->Add_Child(TurnDir5);

	//Start_Sel->Add_Child(Sting_Attack());

	//CBT_RotationDir* TurnDir6 = Node_RotationDir("Look ȸ��", L"Player_Pos", 0.15);
	//Start_Sel->Add_Child(TurnDir6);

	//Start_Sel->Add_Child(Cut_To_Right());

	////////////////////////

	//m_pMeshCom->SetUp_Animation(Ani_Idle);


	return NOERROR;
}

_int CGunGenji::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	Push_Collider();

	CGameObject::Update_GameObject(TimeDelta);
	m_pRigidCom->Update_Component_Self(TimeDelta);

	// �������� 1�� �� ����.
	if (m_pRigidCom->Get_CurTime() > 1.f)
	{
		m_bIsDead = true;

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::GunGenji_SFX_02);
		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"SE_NEW_BARK_DEATH_MV_V3_002.ogg"), CSoundManager::CHANNELID::GunGenji_SFX_02, CSoundManager::SOUND::Effect_Sound);
	}

	// �׾��� ���
	if (m_bIsDead)
	{
		m_bEnable = false;

		Check_DropItem(MONSTER_NAMETYPE::M_GunGenji);
		CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(100.f, m_pTransformCom->Get_Pos(), 0.f));
	}

	if (m_bReadyDead)
	{
		CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle", 0.1f, m_vRightToeBase, m_vHead);
		CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle_Sub", 0.1f, m_vRightToeBase, m_vHead);
	}

	// MonsterHP UI
	if(nullptr != m_pMonsterUI)
		m_pMonsterUI->Update_GameObject(TimeDelta);


	// ó���� �ƴϰ�, �ݰ� ���� �� ���� ���
	if (true == m_tObjParam.bCanExecution && true == m_tObjParam.bCanRepel)
	{
		// �÷��̾� �̹߰�
		if (false == m_bFight)
		{
			//cout << "�߰� ���߽��ϴ�" << endl;
			Update_NF();
		}
		// �÷��̾� �߰�
		else
		{
			// ����� �༮ ��׷� ����.
			Set_Target_Auto(true);

			// �� ��ġ ������Ʈ
			Update_Bone_Of_BlackBoard();
			// BB ���� ������Ʈ
			Update_Value_Of_BB();

			if (true == m_bAIController)
				m_pAIControllerCom->Update_AIController(TimeDelta);

			// Ÿ�ٰ� �Ÿ� ���� �� �ʱ���·� ���ư��� ����.
			Check_TargetDist();
		}
	}

	// �� �� �ϳ��� Ȱ��ȭ �Ǹ�,
	else
	{
		// ���� ������Ʈ
		Update_Bone_Of_BlackBoard();

		if (false == m_tObjParam.bCanExecution)
		{
			// ó�� üũ
			Check_Execution();
		}

		else if (false == m_tObjParam.bCanRepel)
		{
			// �ݰ� üũ
			Check_Repel();
		}
	}


	if (false == m_bReadyDead)
		Check_PhyCollider();
	else
		Check_DeadEffect(TimeDelta);


	m_pTransformCom->Set_Pos(m_pNavMeshCom->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	//====================================================================================================
	// �ø�
	//====================================================================================================
	m_bInFrustum = m_pOptimizationCom->Check_InFrustumforObject(&m_pTransformCom->Get_Pos(), 2.f);
	//====================================================================================================

	return NO_EVENT;
}

_int CGunGenji::Late_Update_GameObject(_double TimeDelta)
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

	m_pGun->Late_Update_GameObject(TimeDelta);

	return NO_EVENT;
}

HRESULT CGunGenji::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	m_pMeshCom->Play_Animation(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60")); // * alpha


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

	m_pGun->Update_GameObject(m_dTimeDelta);
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CGunGenji::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	IF_NULL_VALUE_RETURN(pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

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

	m_pGun->Update_GameObject(m_dTimeDelta);
	Update_Collider();
	Draw_Collider();

	return S_OK;
}

HRESULT CGunGenji::Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur)
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

	return S_OK;
}

CBT_Composite_Node * CGunGenji::Shot()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("�Ϲ� �ѽ��");
	CBT_Sequence* MainSeq = Node_Sequence("�Ϲ� �ѽ��");
	CBT_Play_Ani* Show_Ani101 = Node_Ani("�Ϲ� �ѽ��", 101, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("�⺻", 1, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����", L"Player_Pos", 1.433, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ������", L"Player_Pos", 0.2);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 0);

	CBT_CreateBullet* Bullet0 = Node_CreateBullet("���� �Ѿ�", L"Monster_GunGenjiBullet", L"CreateBulletPos", L"NormalShotDir", 20, 1.5, 1.633, 1, 1, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Bullet0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�غ� ������"	, L"Bullet_Ready_Aura"	, L"RightHandAttach", 0.3, 5, 1, 0);
	//CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("�غ� ����"	, L"Bullet_Ready_Flash"	, L"RightHandAttach", 0.3, 1, 0.0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("�߻� ����"	, L"Bullet_Fire_Flash"	, L"CreateBulletPos", 1.5f, 1, 0.0, 0);
	Root_Parallel->Add_Service(Effect0);
	//Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani101);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::Tumbling_Shot()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("�Һ� �ѽ��");
	CBT_Sequence* MainSeq = Node_Sequence("�Һ� �ѽ��");
	CBT_Play_Ani* Show_Ani102 = Node_Ani("�Һ� �ѽ��", 102, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("�⺻", 1, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.3, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", -1, 1.3, 0);

	CBT_CreateBullet* Bullet0 = Node_CreateBullet("���� �Ѿ�", L"Monster_GunGenjiBullet", L"CreateBulletPos", L"TumblingShotDir", 12, 1, 0.65, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Bullet0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani102);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::Sudden_Shot()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("���ڱ� �ѽ��");
	CBT_Sequence* MainSeq = Node_Sequence("���ڱ� �ѽ��");
	CBT_Play_Ani* Show_Ani103 = Node_Ani("���ڱ� �ѽ��", 103, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("�⺻", 1, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����", L"Player_Pos", 3.433, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ������", L"Player_Pos", 0.2);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 0);

	CBT_CreateBullet* Bullet0 = Node_CreateBullet("���� �Ѿ�", L"Monster_GunGenjiBullet", L"CreateBulletPos", L"NormalShotDir", 20, 1.5, 3.633, 1, 1, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Bullet0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani103);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::Upper_Slash()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");
	CBT_Sequence* MainSeq = Node_Sequence("���Ӹ��� ġ��");
	CBT_Play_Ani* Show_Ani97 = Node_Ani("���Ӹ��� ġ��", 97, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("�⺻", 1, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.083, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 2, 0.434, 0);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 1);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("Ʈ���� On", L"TrailOn", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 2, 0.1, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.2, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("Ʈ���� Off", L"TrailOff", true);
	CBT_Wait* Wait2 = Node_Wait("���1", 0.25, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 0.6f, 0.566, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani97);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(TrailOff);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pGun->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.5, 1, 0.20, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::Arm_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");
	CBT_Sequence* MainSeq = Node_Sequence("�ȱ�ġ ġ��");
	CBT_Play_Ani* Show_Ani98 = Node_Ani("�ȱ�ġ ġ��", 98, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("�⺻", 1, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.25, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 2, 0.75, 0);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 1);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("Ʈ���� On", L"TrailOn", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 2, 0.3, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("Ʈ���� Off", L"TrailOff", true);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.317, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", -0.6f, 1.066, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani98);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(TrailOff);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pGun->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1, 1, 0.2, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::Sting_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");
	CBT_Sequence* MainSeq = Node_Sequence("���");
	CBT_Play_Ani* Show_Ani99 = Node_Ani("���", 99, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("�⺻", 1, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.42, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 2.f, 0.28, 0);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 1);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.95, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 0.6f, 0.537, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani99);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pGun->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.7, 1, 0.083, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::Cut_To_Right()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");
	CBT_Sequence* MainSeq = Node_Sequence("���������� ����");
	CBT_Play_Ani* Show_Ani100 = Node_Ani("���������� ����", 100, 0.95f);
	CBT_Play_Ani* Show_Ani1 = Node_Ani("�⺻", 1, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.3, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 1, 0.4, 0);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 1);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("Ʈ���� On", L"TrailOn", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 2, 0.35, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("Ʈ���� Off", L"TrailOff", true);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.367, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", -0.2f, 0.5, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani100);
	MainSeq->Add_Child(Show_Ani1);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(TrailOff);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pGun->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.8, 1, 0.25, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::Dodge_B()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");
	CBT_Sequence* MainSeq = Node_Sequence("�ڷ� ȸ��");
	CBT_Play_Ani* Show_Ani162 = Node_Ani("�ڷ� ȸ��", 159, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 0.6f, 0.183, 0);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.117, 0);
	CBT_SetValue* Sound1Stop = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Stop", true);
	CBT_SetValue* Sound1Play = Node_BOOL_SetValue("�Ҹ�1 ���", L"SFX_01_Play", true);
	CBT_SetValue* Sound1Tag = Node_INT_SetValue("�Ҹ�1 �̸� ����", L"SFX_01_Tag", 2);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", -4, 0.483, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", 0.5, 0.117, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani162);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Sound1Stop);
	SubSeq->Add_Child(Sound1Play);
	SubSeq->Add_Child(Sound1Tag);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::Start_Game()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� ����");

	Root_Seq->Add_Child(Dist_Attack());

	return Root_Seq;
}

CBT_Composite_Node * CGunGenji::Dist_Attack()
{
	CBT_Selector* Root_Sel = Node_Selector("�ٰŸ� ���Ÿ� ���� ����");

	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 3);
	CBT_DistCheck* Dist1 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 6);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(LookPlayer_NearAttack());

	Root_Sel->Add_Child(Dist1);
	Dist1->Set_Child(LookPlayer_FarAttack());

	Root_Sel->Add_Child(Chase());

	return Root_Sel;
}

CBT_Composite_Node * CGunGenji::LookPlayer_NearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("�÷��̾� �ٶ� �� ���� ���� ����");

	CBT_RotationDir* Rotation0 = Node_RotationDir("�÷��̾� �ٶ󺸱�", L"Player_Pos", 0.1);

	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CGunGenji::LookPlayer_FarAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("�÷��̾� �ٶ� �� ���� ���Ÿ� ����");

	CBT_RotationDir* Rotation0 = Node_RotationDir("�÷��̾� �ٶ󺸱�", L"Player_Pos", 0.1);

	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(FarAttack());

	return Root_Seq;
}

CBT_Composite_Node * CGunGenji::Chase()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_MoveDirectly* pChase = Node_MoveDirectly_Chase("����", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 3.f, 5.f);

	CBT_Play_Ani* Show_Ani16 = Node_Ani("����", 16, 1.f);

	Root_Parallel->Set_Main_Child(pChase);

	Root_Parallel->Set_Sub_Child(Show_Ani16);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� �ٰŸ� ����");

	Root_Sel->Add_Child(Upper_Slash());
	Root_Sel->Add_Child(Arm_Attack());
	Root_Sel->Add_Child(Sting_Attack());
	Root_Sel->Add_Child(Cut_To_Right());
	//Root_Sel->Add_Child(Tumbling_Shot());

	return Root_Sel;
}

CBT_Composite_Node * CGunGenji::FarAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���Ÿ� ����");

	Root_Sel->Add_Child(Shot());
	Root_Sel->Add_Child(Sudden_Shot());

	return Root_Sel;
}

CBT_Composite_Node * CGunGenji::Start_Show()
{
	CBT_Sequence* Root_Seq = Node_Sequence("�ÿ�ȸ");

	Root_Seq->Add_Child(Show_ChaseAttack());

	return Root_Seq;
}

CBT_Composite_Node * CGunGenji::Show_ChaseAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� �Ǵ� ����");
	CBT_MoveDirectly* Chase = Node_MoveDirectly_Chase("����", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 3.f, 2.f);

	Root_Seq->Add_Child(Chase);
	Root_Seq->Add_Child(Show_Attack());

	return Root_Seq;
}

CBT_Composite_Node * CGunGenji::Show_Attack()
{
	CBT_Selector* Root_Sel = Node_Selector("������� ����");

	CBT_Cooldown* Cool0 = Node_Cooldown("��0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("��1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("��2", 300);
	CBT_Cooldown* Cool3 = Node_Cooldown("��3", 300);
	CBT_Cooldown* Cool4 = Node_Cooldown("��4", 300);
	CBT_Cooldown* Cool5 = Node_Cooldown("��5", 300);
	CBT_Cooldown* Cool6 = Node_Cooldown("��6", 300);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Shot());
	Root_Sel->Add_Child(Cool1);
	Cool0->Set_Child(Tumbling_Shot());
	Root_Sel->Add_Child(Cool2);
	Cool0->Set_Child(Sudden_Shot());
	Root_Sel->Add_Child(Cool3);
	Cool0->Set_Child(Upper_Slash());
	Root_Sel->Add_Child(Cool4);
	Cool0->Set_Child(Arm_Attack());
	Root_Sel->Add_Child(Cool5);
	Cool0->Set_Child(Sting_Attack());
	Root_Sel->Add_Child(Cool6);
	Cool0->Set_Child(Cut_To_Right());

	return Root_Sel;
}

HRESULT CGunGenji::Update_Bone_Of_BlackBoard()
{
	m_vRightHandAttach = *(_v3*)(&(m_pRightHandAttachFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"RightHandAttach", m_vRightHandAttach);

	m_vHead = *(_v3*)(&(m_pHeadFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Head", m_vHead);

	m_vRightToeBase = *(_v3*)(&(m_pRightToeBaseFrame->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_RightToeBase", m_vRightToeBase);

	return S_OK;
}

HRESULT CGunGenji::Update_Value_Of_BB()
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


	// 1. Ʈ���� ������Ʈ
	if (true == m_pAIControllerCom->Get_BoolValue(L"TrailOn"))
	{
		m_pGun->Set_Enable_Trail(true);
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"TrailOn", false);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"TrailOff"))
	{
		m_pGun->Set_Enable_Trail(false);
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"TrailOff", false);
	}


	// ����
	// SFX_01
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_01_Stop"))	// ����
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_01_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::GunGenji_SFX_01);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_01_Play"))	// ���
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_01_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_01_Tag")]), CSoundManager::CHANNELID::GunGenji_SFX_01, CSoundManager::SOUND::Effect_Sound);
	}

	// SFX_02
	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_02_Stop"))	// ����
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_02_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::GunGenji_SFX_02);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"SFX_02_Play"))	// ���
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SFX_02_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"SFX_02_Tag")]), CSoundManager::CHANNELID::GunGenji_SFX_02, CSoundManager::SOUND::Effect_Sound);
	}

	// Voice
	if (true == m_pAIControllerCom->Get_BoolValue(L"Voice_Stop"))	// ����
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Voice_Stop", false);

		g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::GunGenji_Voice);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"Voice_Play"))	// ���
	{
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Voice_Play", false);

		g_pSoundManager->Play_Sound(const_cast<TCHAR*>(m_mapSound[m_pAIControllerCom->Get_IntValue(L"Voice_Tag")]), CSoundManager::CHANNELID::GunGenji_Voice, CSoundManager::SOUND::Effect_Sound);
	}





	// 1. ���� �� �߻� ����
	_mat matGunDir = m_pTransformCom->Get_WorldMat();
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"NormalShotDir", _v3(matGunDir.m[2][0], matGunDir.m[2][1], matGunDir.m[2][2]));

	// 2. �Һ��� �� �߻� ����
	_v3 vSelfDir = *(_v3*)&m_pTransformCom->Get_WorldMat().m[2];	// ���� Look
	_v3 vTumblingShotDir;

	_v3 vFrontDir = _v3(0.f, 0.f, 1.f);
	_float fRadian = D3DXVec3Dot(&vSelfDir, &vFrontDir);

	if (fRadian >= 0)
		D3DXVec3TransformNormal(&vTumblingShotDir, &vSelfDir, D3DXMatrixRotationX(&_mat(), D3DXToRadian(30)));
	else if (fRadian < 0)
		D3DXVec3TransformNormal(&vTumblingShotDir, &vSelfDir, D3DXMatrixRotationX(&_mat(), D3DXToRadian(-30)));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"TumblingShotDir", vTumblingShotDir);

	// 3. �Ѿ��� ���� ��ġ
	_mat matBulletCreate = static_cast<CTransform*>(m_pGun->Get_Component(L"Com_Transform"))->Get_WorldMat();
	// ���� ������ǥ + ���� ���� �� �÷��� ����
	_v3 vCreateBulletPos = _v3(matBulletCreate.m[3][0], matBulletCreate.m[3][1], matBulletCreate.m[3][2]) + (_v3(0.f, 0.3f, 0.f));

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"CreateBulletPos", vCreateBulletPos);

	return S_OK;
}

HRESULT CGunGenji::Update_NF()
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
				//cout << "�߰� ���� : " << m_eNF_Ani << endl;

				m_pMeshCom->SetUp_Animation(m_eNF_Ani);
			}
		}
		// �÷��̾ �ִ�Ÿ� �ۿ� �ִ°�?
		else
			m_pMeshCom->SetUp_Animation(m_eNF_Ani);
	}
	// �÷��̾� �߰�
	else
	{
		switch (m_eNF_Ani)
		{
		case Client::CGunGenji::Sit1:
			m_pMeshCom->SetUp_Animation(Ani_StandUp1);
			break;
		case Client::CGunGenji::Sit2:
			m_pMeshCom->SetUp_Animation(Ani_StandUp2);
			break;
		default:
			m_bFight = true;
			break;
		}

		// �Ͼ�� ���۵鸸 ���� ����
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_pMeshCom->SetUp_Animation(Ani_Idle);
			m_bFight = true;
		}
	}
	return S_OK;
}

HRESULT CGunGenji::Update_Collider()
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

	m_pColliderCom->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pColliderCom->Get_Radius().y, 0.f));

	return S_OK;
}

void CGunGenji::Skill_Movement(_float _fspeed, _v3 _vDir)
{
	_v3 tmpLook;
	_float fSpeed = _fspeed;

	tmpLook = _vDir;
	D3DXVec3Normalize(&tmpLook, &tmpLook);

	// �׺���̼� �����ϸ� 
	m_pTransformCom->Set_Pos((m_pNavMeshCom->Move_OnNaviMesh(m_pRigidCom, &m_pTransformCom->Get_Pos(), &tmpLook, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

}

void CGunGenji::Decre_Skill_Movement(_float _fMutiply)
{
	m_fSkillMoveSpeed_Cur -= (0.3f - m_fSkillMoveAccel_Cur * m_fSkillMoveAccel_Cur * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60")) * _fMutiply;
	m_fSkillMoveAccel_Cur += g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

	if (m_fSkillMoveSpeed_Cur < 0.f)
	{
		m_fSkillMoveAccel_Cur = 0.5f;
		m_fSkillMoveSpeed_Cur = 0.f;
	}
}

void CGunGenji::Check_PhyCollider()
{
	// �浹ó��, bCanHit�� ���Ⱑ false�����ٰ���.
	if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		//===========================================================
		// ���� �� ���� ������Ʈ ���� �ʱ�ȭ ��Ŵ
		m_pBattleAgentCom->Set_RimChangeData();
		//===========================================================

		m_bAIController = false;

		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		m_bFight = true;	// �ο� ����

		m_fSkillMoveSpeed_Cur = 4.f;
		m_fSkillMoveAccel_Cur = 0.f;
		m_fSkillMoveMultiply = 0.5f;

		m_pAIControllerCom->Reset_BT();
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"TrailOff", true);

		if (false == m_tObjParam.bIsExecution)
		{
			m_pMeshCom->Reset_OldIndx();	//�ִ� �ε��� �ʱ�ȭ

			// ������ �÷��̾��� ���� �޾ƿͼ� �� �������� ��.
			m_vPushDir_forHitting = (*(_v3*)&TARGET_TO_TRANS(CMonster::Get_pTargetObject())->Get_WorldMat().m[2]);
		}

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			_float fAngle = D3DXToDegree(m_pTransformCom->Chase_Target_Angle(&TARGET_TO_TRANS(CMonster::Get_pTargetObject())->Get_Pos()));

			g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::GunGenji_SFX_02);
			g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"SE_NEW_BARK_DAMAGE_SMALL_MV_008.ogg"), CSoundManager::CHANNELID::GunGenji_SFX_02, CSoundManager::SOUND::Effect_Sound);

			if (0.f <= fAngle && fAngle < 90.f)
				m_pMeshCom->SetUp_Animation(Ani_Dmg01_FR);
			else if (90.f <= fAngle && fAngle < 180.f)
				m_pMeshCom->SetUp_Animation(Ani_Dmg01_BR);
			else if (-90.f <= fAngle && fAngle < 0)
				m_pMeshCom->SetUp_Animation(Ani_Dmg01_FL);
			else
				m_pMeshCom->SetUp_Animation(Ani_Dmg01_BL);
		}
		else
		{
			g_pSoundManager->Stop_Sound(CSoundManager::CHANNELID::GunGenji_SFX_02);
			g_pSoundManager->Play_Sound(const_cast<TCHAR*>(L"SE_NEW_BARK_DEATH_MV_V3_002.ogg"), CSoundManager::CHANNELID::GunGenji_SFX_02, CSoundManager::SOUND::Effect_Sound);
			
			Ani eTmpAnim = (m_tObjParam.bIsExecution ? Ani_Death_F : Ani_Death);
			_float fDelay = (m_tObjParam.bIsExecution ? 0.5f : 0.1f);

			m_pMeshCom->SetUp_Animation(eTmpAnim);	// ����ó�� ����
			Start_Dissolve(0.5f, false, true, fDelay);

			m_pGun->Start_Dissolve();

			m_fDeadEffect_Delay = 0.5f;
			//CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_Haze", (void*)&CHaze::HAZE_INFO(100.f, m_pTransformCom->Get_Pos(), 0.5f));
		}
	}
	// �¾��� ��
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_bAIController = true;
			m_tObjParam.bIsHit = false;

			//m_pMeshCom->SetUp_Animation(Ani_Idle);
		}
		else if (m_pMeshCom->Is_Finish_Animation(0.5f))	// �̶����� ���浹 ����
		{
			m_tObjParam.bIsHit = false;
		}

		// �и� ó��
		if (m_tObjParam.bIsHit == true)
		{
			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, m_vPushDir_forHitting);
		}
	}
}

void CGunGenji::Push_Collider()
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
				pTrans->Set_Pos(pNav->Move_OnNaviMesh(m_pRigidCom, &pTrans->Get_Pos(), &vDir, m_pColliderCom->Get_Length().x));
			}
		}
	}
}

void CGunGenji::Check_TargetDist()
{
	// ���� Ÿ�� ������
	CGameObject* pPlayer = CMonster::Get_pTargetObject();

	if (nullptr == pPlayer)
		return;

	CTransform* pPlayer_Trans = TARGET_TO_TRANS(pPlayer);

	// Ÿ�ٰ� ������ �Ÿ�
	_v3 vLengthTemp = pPlayer_Trans->Get_Pos() - m_pTransformCom->Get_Pos();
	_float fLength = D3DXVec3Length(&vLengthTemp);

	// �ʱ���·� ����.
	if (fLength > 15.f)
	{
		m_bFight = false;
		m_bFindPlayer = false;
		m_pAIControllerCom->Reset_BT();
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"TrailOff", true);
	}
}

void CGunGenji::Check_DeadEffect(_double TimeDelta)
{
	m_fDeadEffect_Delay -= _float(TimeDelta);
	if (m_fDeadEffect_Delay > 0.f)
		return;

	m_fDeadEffect_Offset -= _float(TimeDelta);
	if (m_fDeadEffect_Offset > 0.f)
		return;

	m_fDeadEffect_Offset = 0.1f;

	CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle", 0.1f, m_vRightToeBase, m_vHead);
	CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle_Sub", 0.1f, m_vRightToeBase, m_vHead);

	D3DXFRAME_DERIVED* pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Hips");
	_v3 vHipPos = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);

	CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle", 0.1f, m_vRightToeBase, m_vHead);
	CParticleMgr::Get_Instance()->Create_Effect_FinishPos(L"SpawnParticle_Sub", 0.1f, m_vRightToeBase, m_vHead);

	CParticleMgr::Get_Instance()->Create_Effect(L"Monster_DeadSmoke_0", m_vHead);
	CParticleMgr::Get_Instance()->Create_Effect(L"Monster_DeadSmoke_0", vHipPos);
	CParticleMgr::Get_Instance()->Create_Effect(L"Monster_DeadSmoke_0", m_vRightToeBase);
}

HRESULT CGunGenji::Add_Component(void* pArg)
{
	/*if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_MonsterUI", SCENE_STAGE, L"Layer_MonsterHPUI")))
	return E_FAIL;*/

	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	_tchar name[STR_128] = { 0, };
	INFO eTemp = *(INFO*)pArg;

	if (nullptr == pArg)
		lstrcpy(name, L"Mesh_Genji_Normal");
	else
	{
		switch (eTemp.eColor)
		{
		case CGunGenji::Jungle:
			lstrcpy(name, L"Mesh_Genji_Green");
			break;
		case CGunGenji::Normal:
			lstrcpy(name, L"Mesh_Genji_Normal");
			break;
		case CGunGenji::White:
			lstrcpy(name, L"Mesh_Genji_White");
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

	// for.Com_RigidBody
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Rigidbody", L"Com_Rigidbody", (CComponent**)&m_pRigidCom)))
		return E_FAIL;

	m_pColliderCom->Set_Radius(_v3{ 1.f, 1.f, 1.f });
	m_pColliderCom->Set_Dynamic(true);
	m_pColliderCom->Set_Type(COL_SPHERE);
	m_pColliderCom->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pColliderCom->Get_Radius().y , 0.f });

	return NOERROR;
}

HRESULT CGunGenji::SetUp_ConstantTable(CShader * pShader)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	//=============================================================================================
	// �⺻ ��Ʈ����
	//=============================================================================================

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	//=============================================================================================
	// ������� ���
	//=============================================================================================
	if (FAILED(pShader->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// ���̴� �������� ��ġ �Է�
	//=============================================================================================
	_float	fEmissivePower = 5.f;	// �̹̽ú� : ���� ����, ��ü �߱��� ������.
	_float	fSpecularPower = 1.f;	// ��Ż�Ͻ� : ���� ����, ���ݻ簡 ������.
	_float	fRoughnessPower = 1.f;	// �����Ͻ� : ���� ����, �� ����� ������(���� ������).
	_float	fMinSpecular = 0.0f;	// �ּ� ��	: ���� ���� ���� ����(������Ʈ�� ������ �о����� , �������� ������).
	_float	fID_R = 1.0f;	// ID_R : Rä�� ID �� , 1�� �ִ�
	_float	fID_G = 0.f;	// ID_G : Gä�� ID �� , 1�� �ִ�
	_float	fID_B = 0.f;	// ID_B	: Bä�� ID �� , 1�� �ִ�

	if (FAILED(pShader->Set_Value("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fSpecularPower", &fSpecularPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fRoughnessPower", &fRoughnessPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fMinSpecular", &fMinSpecular, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fID_R_Power", &fID_R, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fID_G_Power", &fID_G, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fID_B_Power", &fID_B, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// ������Ʈ ������ ���̴��� ��Ͻ�Ŵ
	//=============================================================================================
	m_pBattleAgentCom->Update_RimParam_OnShader(pShader);
	//=============================================================================================

	return NOERROR;
}

HRESULT CGunGenji::Ready_Weapon()
{
	m_pGun = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pGun->Set_Target(this);
	m_pGun->Change_WeaponData(Wpn_Gun_Military);

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pGun->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pGun->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	return S_OK;
}

HRESULT CGunGenji::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	// �� �ּ� ����
	m_pRightHandAttachFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pHeadFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head");
	m_pRightToeBaseFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightToeBase");

	return S_OK;
}

HRESULT CGunGenji::Ready_Collider()
{
	m_vecPhysicCol.reserve(10);

	//���üũ��
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.2f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBones[Bone_Range]->_41, m_matBones[Bone_Range]->_42, m_matBones[Bone_Range]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//��
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 0.5f;

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

	return S_OK;
}

HRESULT CGunGenji::Ready_NF(void * pArg)
{
	INFO eTemp = *(INFO*)pArg;

	m_eNF_Ani = eTemp.eNF_Ani;
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

HRESULT CGunGenji::Ready_Sound()
{
	m_mapSound.emplace(0, L"SE_MILITARY_VAMPIRE_HAMMER_IMPACT_000.ogg");
	m_mapSound.emplace(1, L"SE_GATE_WOMAN_ATTACK_SWING_002.ogg");
	m_mapSound.emplace(2, L"SE_NEW_DODGE_MV_000.ogg");
	
	return S_OK;
}

HRESULT CGunGenji::Ready_Rigid()
{
	if (nullptr != m_pRigidCom)
	{
		m_pRigidCom->Set_UseGravity(true);							// �߷��� ���� ����

		m_pRigidCom->Set_IsFall(false);								// ���������� üũ

		m_pRigidCom->Set_fPower(2.f);								// ���� �Ŀ�

		m_pRigidCom->Set_Speed({ 10.f , 10.f , 10.f });				// �� �࿡ �ش��ϴ� �ӵ�
		m_pRigidCom->Set_Accel({ 1.f, 0.f, 0.f });					// �� �࿡ �ش��ϴ� Accel ��
		m_pRigidCom->Set_MaxAccel({ 2.f , 4.f , 2.f });				// �� �࿡ �ش��ϴ� MaxAccel ��
	}

	return S_OK;
}

void CGunGenji::Check_Execution()
{
	// ���� ó���� �����ϴٸ�, ����
	if (true == m_tObjParam.bCanExecution)
		return;

	// ó���� �Ұ����ѵ�, ó������ �ƴ� ���,
	if (false == m_tObjParam.bIsExecution)
	{
		// ó������ ���·� ������ְ�,
		m_tObjParam.bIsExecution = true;

		// ó�� ������ ���� ó�� �ִϸ��̼��� �����Ѵ�.
		Check_ExecutionAnim();

		m_pMeshCom->Reset_OldIndx();
	}

	// ó�� ���� ���,
	else if (true == m_tObjParam.bIsExecution)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.8f))
		{
			// ó���� ������ �������� ���� �ʱ�ȭ�� ���� ������
			//m_tObjParam.bIsExecution = false;
			//m_tObjParam.bCanExecution = true;

			//�װ� �����Ѵ�.
			//m_bIsDead = true;

			// Ȥ�� �𸣴� HP ��  ������
			m_tObjParam.bCanHit = false;
			//m_tObjParam.fHp_Cur = -1.0f;

			return;
		}
	}

	// ó�� ����� AniCtrl�� �������ش�.
	if(m_tObjParam.fHp_Cur > 0.f)
		m_pMeshCom->SetUp_Animation(m_eExecutionAnim);
}

void CGunGenji::Check_ExecutionAnim()
{
	switch (m_tObjParam.eExecutionWpn)
	{
	case EXE_WPN::EXECUTION_Wpn_Orge:
	{
		Check_OrgeExecution();
		break;
	}
	case EXE_WPN::EXECUTION_Wpn_Stinger:
	{
		Check_StingerExecution();
		break;
	}

	case EXE_WPN::EXECUTION_Wpn_Hounds:
	{
		Check_HoundsExecution();
		break;
	}

	case EXE_WPN::EXECUTION_Wpn_Ivy:
	{
		Check_IvyExecution();
		break;
	}

	}
}

void CGunGenji::Check_OrgeExecution()
{
}

void CGunGenji::Check_IvyExecution()
{
}

void CGunGenji::Check_StingerExecution()
{
	switch (m_tObjParam.eExecutionType)
	{
	case EXE_TYPE::EXECUTION_Back:
	{
		m_eExecutionAnim = Ani_Execution_LongCoat_B_S;
		break;
	}
	case EXE_TYPE::EXECUTION_BackCinema:
	{
		m_eExecutionAnim = Ani_Execution_LongCoat_B;
		break;
	}
	case EXE_TYPE::EXECUTION_Front:
	{
		break;
	}
	case EXE_TYPE::EXECUTION_FrontCinema:
	{
		break;
	}
	case EXE_TYPE::EXECUTION_ComboFront:
	{
		break;
	}
	case EXE_TYPE::EXECUTION_ComboBack:
	{
		break;
	}
	case EXE_TYPE::EXECUTION_ComboCinema:
	{
		break;
	}
	}
}

void CGunGenji::Check_HoundsExecution()
{
}

void CGunGenji::Check_Repel()
{
	if (m_tObjParam.bCanRepel)
		return;

	if (false == m_tObjParam.bIsRepel)
	{
		// �и����� ����
		m_tObjParam.bIsRepel = true;

		// �ݰ� �ִϸ��̼����� ����
		m_pMeshCom->SetUp_Animation(Ani_DmgRepel);

		m_fSkillMoveAccel_Cur = 0.f;
		m_fSkillMoveSpeed_Cur = 2.f;
		m_fSkillMoveMultiply = 0.5;
	}

	else if (m_tObjParam.bIsRepel)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			m_tObjParam.bIsRepel = false;
			m_tObjParam.bCanRepel = true;
		}

		Skill_Movement(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_Z));
		Decre_Skill_Movement(m_fSkillMoveMultiply);
	}
}

CGunGenji * CGunGenji::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CGunGenji* pInstance = new CGunGenji(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CGunGenji");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGunGenji::Clone_GameObject(void * pArg)
{
	CGunGenji* pInstance = new CGunGenji(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CGunGenji");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGunGenji::Free()
{
	//Safe_Release(m_pMonsterUI);

	Safe_Release(m_pGun);

	CMonster::Free();
}
