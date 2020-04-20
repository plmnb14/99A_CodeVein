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

	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 2000.f;
	m_tObjParam.fHp_Max = m_tObjParam.fHp_Cur;
	m_tObjParam.fDamage = 20.f;

	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));


	///////////////// �ൿƮ�� init

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

	CBT_Selector* Start_Sel = Node_Selector("�ൿ ����");
	//CBT_Sequence* Start_Sel = Node_Sequence("�ൿ ����");	//�׽�Ʈ

	pBehaviorTree->Set_Child(Start_Sel);


	//////////// �Ʒ��� �ּ��س��� 4���� �����ӿ��� �� ����, ���ʴ�� ������.

	//CBT_CompareValue* Check_ShowValue = Node_BOOL_A_Equal_Value("�ÿ�ȸ ���� üũ", L"Show", true);
	//Check_ShowValue->Set_Child(Start_Show());
	//Start_Sel->Add_Child(Check_ShowValue);
	//Start_Sel->Add_Child(Start_Game());

	////////////

	// ���� Ȯ�ο�,  �� ���� �Լ��� �Ʒ��� ������ �����

	Start_Sel->Add_Child(Start_Game());

	//CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.2);
	//Start_Sel->Add_Child(Rotation0);

	//CBT_Wait* Wait0 = Node_Wait("���", 1, 0);
	//Start_Sel->Add_Child(Wait0);


	/////////////
	// UI �߰�(����)
	m_pBossUI = static_cast<CBossHP*>(g_pManagement->Clone_GameObject_Return(L"GameObject_BossHP", nullptr));
	m_pBossUI->Set_UI_Pos(WINCX * 0.5f, WINCY * 0.1f);
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
		// �� ��ġ ������Ʈ
		Update_Bone_Of_BlackBoard();
		// BB ���� ������Ʈ
		Update_Value_Of_BB();

		if (true == m_bAIController)
			m_pAIControllerCom->Update_AIController(TimeDelta);

		// �÷��̾� �߰� ��, UI Ȱ��ȭ(����)
		m_pBossUI->Set_Active(true);
			
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

	m_pTransformCom->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	return NOERROR;
}

_int CFireBoy::Late_Update_GameObject(_double TimeDelta)
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

	return NOERROR;
}

HRESULT CFireBoy::Render_GameObject()
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

	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

HRESULT CFireBoy::Render_GameObject_SetPass(CShader * pShader, _int iPass)
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

CBT_Composite_Node * CFireBoy::Arm_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�Ȳ�ġ ġ��");
	CBT_Play_Ani* Show_Ani32 = Node_Ani("�Ȳ�ġ ġ��", 32, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.183, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.417f, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ٴ� ����", L"FireBoy_ArmAttack_FloorDust", L"Bone_LeftForeArm"	, 0.76, 3, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ����", L"FireBoy_ArmAttack_Dust", L"Bone_LeftForeArm"		, 0.8, 1, 0, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani32);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);




	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 0.65, 1, 0.1, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Gun_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�������� ġ��");
	CBT_Play_Ani* Show_Ani39 = Node_Ani("�������� ġ��", 39, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.116, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.767, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.847, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 0.6f, 0.17, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani39);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
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
	CBT_Play_Ani* Show_Ani30 = Node_Ani("�� ����̵�", 30, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����", L"Player_Pos", 0.2);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.1, 0);
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
	MainSeq->Add_Child(Show_Ani30);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);

	CBT_CreateBullet* Col0 = Node_CreateBullet("����̵� �浹ü", L"Monster_FireTornadoCol", L"SelfPos", L"", 0, 2, 1.856, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Back_Dash()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�ڷ� �뽬");
	CBT_Play_Ani* Show_Ani14 = Node_Ani("�ڷ� �뽬", 14, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", -5.f, 0.333, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", -7.f, 0.433, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", -3.f, 0.517, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani14);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_Tracking()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�����ϸ鼭 �� �߻�");
	CBT_Play_Ani* Show_Ani36 = Node_Ani("�����ϸ鼭 �� �߻�", 36, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.883, 0);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����", L"Player_Pos", 2.467, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ѱ� �Ա� ����Ʈ", L"FireBoy_FireBullet_GunEff", L"Bone_Muzzle", 1.0, 70, 0, 0);

	Root_Parallel->Add_Service(Effect0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani36);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(ChaseDir0);




	CBT_CreateBullet* Col0 = Node_CreateBullet("ȭ�� �߻�", L"Monster_FireBullet", L"Bone_Muzzle", L"StraightFireDir", 8, 1, 0.833, 12, 0.2, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_Cone()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("��ä�÷� �� �߻�");
	CBT_Play_Ani* Show_Ani35 = Node_Ani("��ä�÷� �� �߻�", 35, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 1.166, 0);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����", L"Player_Pos", 4.05, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ѱ� �Ա� ����Ʈ", L"FireBoy_FireBullet_GunEff", L"Bone_Muzzle", 1.0, 70, 0, 0);

	Root_Parallel->Add_Service(Effect0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani35);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(ChaseDir0);



	CBT_CreateBullet* Col0 = Node_CreateBullet("ȭ�� �߻�", L"Monster_FireBullet", L"Bone_Muzzle", L"FireDir", 8, 1, 1.166, 20, 0.2, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_BigSphere()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("���� ȭ���� �߻�");
	CBT_Play_Ani* Show_Ani33 = Node_Ani("���� ȭ���� �߻�", 33, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.283, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 1.f, 0.35, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����", L"Player_Pos", 0.183);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.884, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", -1.f, 0.35, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ѱ� �Ա� ����Ʈ", L"FireBoy_FireBullet_GunEff", L"Bone_Muzzle", 0.7, 10, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("�ѱ� �Ա� �� ����Ʈ", L"FireBoy_FireSphere_ShotCircle", L"Bone_Muzzle", 0.8, 1, 0, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani33);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Rotation0);
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
	CBT_Play_Ani* Show_Ani31 = Node_Ani("Ÿ�� �ٴڿ��� ��", 31, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����", L"Player_Pos", 0.2);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.733, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 4.f, 0.233, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.984, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", -4.f, 0.233, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�޼տ� ��", L"FireBoy_Charge_Hand_Fire", L"Bone_LeftHand", 0.6, 30, 0, 0);

	Root_Parallel->Add_Service(Effect0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani31);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);




	CBT_CreateBullet* Col0 = Node_CreateBullet("Ÿ�� �ٴڿ� �ҳ�", L"Monster_FireGround", L"Player_Pos", L"", 0, 5, 1.266, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Col0);

	return Root_Parallel;
}

CBT_Composite_Node * CFireBoy::Fire_Flame()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("���� ���ڰ� ��");
	CBT_Play_Ani* Show_Ani28 = Node_Ani("���� ���ڰ� ��", 28, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����", L"Player_Pos", 0.2);
	CBT_Wait* Wait0 = Node_Wait("���0", 1.133, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 1.5f, 0.633, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.484, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", -1.5f, 0.633, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani28);
	MainSeq->Add_Child(Show_Ani0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);




	CBT_CreateBullet* Col0 = Node_CreateBullet("���߿��� �� ����", L"Monster_FireBullet", L"FlamePos0", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col1 = Node_CreateBullet("���߿��� �� ����", L"Monster_FireBullet", L"FlamePos1", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col2 = Node_CreateBullet("���߿��� �� ����", L"Monster_FireBullet", L"FlamePos2", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col3 = Node_CreateBullet("���߿��� �� ����", L"Monster_FireBullet", L"FlamePos3", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col4 = Node_CreateBullet("���߿��� �� ����", L"Monster_FireBullet", L"FlamePos4", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col5 = Node_CreateBullet("���߿��� �� ����", L"Monster_FireBullet", L"FlamePos5", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col6 = Node_CreateBullet("���߿��� �� ����", L"Monster_FireBullet", L"FlamePos6", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col7 = Node_CreateBullet("���߿��� �� ����", L"Monster_FireBullet", L"FlamePos7", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col8 = Node_CreateBullet("���߿��� �� ����", L"Monster_FireBullet", L"FlamePos8", L"FlameDir", 6, 5, 2.166, 1, 0, 0, CBT_Service_Node::Finite);

	Root_Parallel->Add_Service(Col0);
	Root_Parallel->Add_Service(Col1);
	Root_Parallel->Add_Service(Col2);
	Root_Parallel->Add_Service(Col3);
	Root_Parallel->Add_Service(Col4);
	Root_Parallel->Add_Service(Col5);
	Root_Parallel->Add_Service(Col6);
	Root_Parallel->Add_Service(Col7);
	Root_Parallel->Add_Service(Col8);

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
	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftForeArm");
	m_vLeftForeArm = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftForeArm", m_vLeftForeArm);

	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHand");
	m_vLeftHand = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_LeftHand", m_vLeftHand);

	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Muzzle");
	m_vMuzzle = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Muzzle", m_vMuzzle);

	return S_OK;
}

HRESULT CFireBoy::Update_Value_Of_BB()
{
	// 1. �÷��̾� ��ǥ ������Ʈ
	CTransform* pPlayerTransCom = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Player_Pos", pPlayerTransCom->Get_Pos());
	// 2. ü�� ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HP", m_tObjParam.fHp_Cur);
	// 3. ü�� ���� ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HPRatio", _float(m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max) * 100);



	// 1. �������� ������ Collider ��ǥ,  ���� ���İ��� �������� �浹�� �� �ȵǹǷ� �ݶ��̴� ��¦ �������� �浹�� ����.
	_mat matSelf = m_pTransformCom->Get_WorldMat();
	_v3 vSelfPos = *(_v3*)matSelf.m[3];
	_v3 vSelfLook = *D3DXVec3Normalize(&_v3(), (_v3*)matSelf.m[2]);
	_v3 vSelfRight = *D3DXVec3Normalize(&_v3(), (_v3*)matSelf.m[0]);

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"GunAttackPos", vSelfPos + vSelfLook * 1.5f + _v3(0.f, 1.f, 0.f));
	
	// 2. ���ſ��� ���ڷ� �� �߻��ϴ� ����

	_v3 vDirTemp0;

	_v3 vTempFrontDir = _v3(0.f, 0.f, 1.f);
	_float fRadian = D3DXVec3Dot(&vSelfLook, &vTempFrontDir);

	if (fRadian >= 0)
		D3DXVec3TransformNormal(&vDirTemp0, &vSelfLook, D3DXMatrixRotationX(&_mat(), D3DXToRadian(15)));
	else if (fRadian < 0)
		D3DXVec3TransformNormal(&vDirTemp0, &vSelfLook, D3DXMatrixRotationX(&_mat(), D3DXToRadian(-15)));

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"StraightFireDir", vDirTemp0);

	// 3. ���ſ��� ��ä��, ȭ���� �߻��ϴ� ����

	if (fRadian >= 0)
		D3DXVec3TransformNormal(&vDirTemp0, &vSelfLook, D3DXMatrixRotationX(&_mat(), D3DXToRadian(3)));
	else if (fRadian < 0)
		D3DXVec3TransformNormal(&vDirTemp0, &vSelfLook, D3DXMatrixRotationX(&_mat(), D3DXToRadian(-3)));

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FireDir", vDirTemp0);


	// 4. Flame, �÷��̾� ������ ���ڰ��� �� ������
	_v3 vReverseLook = -vSelfLook;
	_v3 vReverseRight = -vSelfRight;

	_v3 vPlayerPos = pPlayerTransCom->Get_Pos();
	_float fLength = 2.f;

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos0", _v3(0.f, 5.f, 0.f) + vPlayerPos);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos1", _v3(0.f, 5.f, 0.f) + vPlayerPos + vSelfLook * fLength);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos2", _v3(0.f, 5.f, 0.f) + vPlayerPos + vSelfRight * fLength);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos3", _v3(0.f, 5.f, 0.f) + vPlayerPos + vReverseLook * fLength);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos4", _v3(0.f, 5.f, 0.f) + vPlayerPos + vReverseRight * fLength);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos5", _v3(0.f, 5.f, 0.f) + vPlayerPos + vSelfLook * fLength * 2);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos6", _v3(0.f, 5.f, 0.f) + vPlayerPos + vSelfRight * fLength * 2);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos7", _v3(0.f, 5.f, 0.f) + vPlayerPos + vReverseLook * fLength * 2);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlamePos8", _v3(0.f, 5.f, 0.f) + vPlayerPos + vReverseRight * fLength * 2);

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlameDir", _v3(0.f, -1.f, 0.f));


	// 5. ȭ�� ���̳׵�
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"SelfPos", vSelfPos);

	return S_OK;
}

HRESULT CFireBoy::Update_NF()
{
	// �÷��̾� �߰� �� ���� ��
	if (false == m_bFindPlayer)
	{
		// �÷��̾� ��ǥ ����.
		_v3 vPlayer_Pos = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_Pos();

		// �÷��̾�� ������ �Ÿ�
		_v3 vLengthTemp = vPlayer_Pos - m_pTransformCom->Get_Pos();
		vLengthTemp.y = 0.f;
		_float fLength = D3DXVec3Length(&vLengthTemp);

		//cout << "�Ÿ� : " << fLength << endl;

		// �÷��̾ �ּҰŸ��ȿ� �ִ°�?
		if (fLength < m_fMinLength)
		{
			// �÷��̾� �߰�
			m_bFindPlayer = true;
		}
		// �÷��̾ �ִ�Ÿ� �ȿ� �ִ°�?
		else if (fLength < m_fMaxLength)
		{
			// �÷��̾ �þ߰� �ȿ� �ִ°�?
			if (Is_InFov(m_fFov, m_pTransformCom, vPlayer_Pos))
			{
				// �÷��̾� �߰�
				m_bFindPlayer = true;
			}
			else
			{
				m_pMeshCom->SetUp_Animation(Ani_Idle);
			}
		}
		// �÷��̾ �ִ�Ÿ� �ۿ� �ִ°�?
		else
			m_pMeshCom->SetUp_Animation(Ani_Idle);
	}
	// �÷��̾� �߰�
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

	m_pCollider->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pCollider->Get_Radius().y, 0.f));
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

		m_bFight = true;		// �ο� ����

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			// ü�� ���� 70 ���ϵǸ� ����
			if (false == m_bDown_Finish)
			{
				if (0.7 >= (m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max))
				{
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
			m_pMeshCom->SetUp_Animation(Ani_Death);	// ����ó�� ����
			Start_Dissolve(0.4f, false, true, 4.2f);
			g_pManagement->Create_Effect_Delay(L"Boss_Dead_Particle", 3.9f, _v3(0.f, 1.3f, 0.f), m_pTransformCom);
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
	list<CGameObject*> tmpList[3];

	tmpList[0] = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);
	tmpList[1] = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
	tmpList[2] = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);

	for (auto& ListObj : tmpList)
	{
		for (auto& iter : ListObj)
		{
			CCollider* pCollider = TARGET_TO_COL(iter);

			// ���� �ӵ��� ���Ƿ� �־��µ� ���ؼ� �־������ - �Ϸ�
			if (m_pCollider->Check_Sphere(pCollider, m_pTransformCom->Get_Axis(AXIS_Z), m_pAIControllerCom->Get_FloatValue(L"Monster_Speed")))
			{
				CTransform* pTrans = TARGET_TO_TRANS(iter);
				CNavMesh*   pNav = TARGET_TO_NAV(iter);

				// ���� �����ְ�
				_v3 vDir = m_pTransformCom->Get_Pos() - pTrans->Get_Pos();
				V3_NORMAL_SELF(&vDir);

				// y�� �̵��� ��������
				vDir.y = 0;

				// �׺� �޽�Ÿ�� �� ����
				pTrans->Set_Pos(pNav->Move_OnNaviMesh(NULL, &pTrans->Get_Pos(), &vDir, m_pCollider->Get_Length().x));
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
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));


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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	m_pCollider->Set_Radius(_v3{ 1.5f, 1.5f, 1.5f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y , 0.f });


	return NOERROR;
}

HRESULT CFireBoy::SetUp_ConstantTable()
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

HRESULT CFireBoy::Ready_Weapon()
{
	return E_NOTIMPL;
}

HRESULT CFireBoy::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;

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
	Safe_Release(m_pCollider);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}