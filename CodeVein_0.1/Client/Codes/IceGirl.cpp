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

	pBlackBoard->Set_Value(L"PushCol", true);	// �浹���� �����
	pBlackBoard->Set_Value(L"PhyCol", true); // �ǰ����� ���� ����
	pBlackBoard->Set_Value(L"Ice_Barrier_On", false);	// ���� ��ȣ�� ����

	//CBT_Selector* Start_Sel = Node_Selector("�ൿ ����");
	CBT_Sequence* Start_Sel = Node_Sequence("�ൿ ����");	//�׽�Ʈ

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

	CBT_Wait* Wait0 = Node_Wait("���", 1, 0);
	Start_Sel->Add_Child(Wait0);

	/////////////
	// UI �߰�(����)
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

	// �׾��� ���
	if (m_bIsDead)
		return DEAD_OBJ;
	
		

	// ���� �ִϸ��̼�
	if (m_bReadyDead)
	{
		// �����鼭 UI ��Ȱ��ȭ
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
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("ȸ�� ����");
	CBT_Play_Ani* Show_Ani34 = Node_Ani("���鼭 ����", 34, fWeight);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.116, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("����2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", 3.f, 0.534, 0);

	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("�ٴ� ���� ������", L"IceFloorAura_01", L"Self_Foot", 0.2, 25, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("�ٴ� ���� ������", L"IceFloorAura_02", L"Self_Foot", 0.2, 25, 0, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�ٴ� ���� ������", L"IceFloorAura_03", L"Self_Foot", 0.2, 25, 0, 0);

	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("Į �ñ� ����ũ1", L"IceSmoke_01", L"Sword_MidPos", 0.2, 150, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("Į �ñ� ����ũ2", L"IceSmoke_02", L"Sword_MidPos", 0.2, 150, 0, 0);

	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("�Ķ� �� ��ƼŬ", L"IceGirl_PointParticle_Blue", L"Sword_MidPos"		, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("�ʷ� �� ��ƼŬ", L"IceGirl_PointParticle_Green", L"Sword_MidPos"		, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("�Ķ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Blue", L"Sword_MidPos"	, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("�ʷ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Green", L"Sword_MidPos"	, 0.38, 80, 0, 0);

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

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.833, 1, 0.217, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::ThreeCombo_Cut1()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("3���� ����");
	CBT_Play_Ani* Show_Ani38 = Node_Ani("����1", 38, 0.4f);
	CBT_Play_Ani* Show_Ani37 = Node_Ani("����2", 37, 0.4f);
	CBT_Play_Ani* Show_Ani35 = Node_Ani("����3", 35, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.083, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("����0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.45, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.333 + 0.016, 0);
	// �ִ�38 * 0.4f = 0.966
	CBT_RotationDir* Rotation1 = Node_RotationDir("����1", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 3.f, 0.35, 0);
	CBT_Wait* Wait2 = Node_Wait("���2", 0.507 + 0.016, 0);
	// �ִ�37 * 0.4f = 0.973
	CBT_RotationDir* Rotation2 = Node_RotationDir("����2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", 3.f, 0.5, 0);
	CBT_Wait* Wait3 = Node_Wait("���3", 0.767, 0);
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("�̵�3", L"Monster_Speed", L"Monster_Dir", 1.f, 0.317, 0);
	CBT_Wait* Wait4 = Node_Wait("���4", 0.25, 0);
	CBT_MoveDirectly* Move4 = Node_MoveDirectly_Rush("�̵�4", L"Monster_Speed", L"Monster_Dir", -0.6f, 0.716, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ٴ� ���� ������ 1Ÿ", L"IceFloorAura_01", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("�ٴ� ���� ������ 1Ÿ", L"IceFloorAura_02", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("�ٴ� ���� ������ 1Ÿ", L"IceFloorAura_03", L"Self_Foot", 0.3, 20, 0, 0);

	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�ٴ� ���� ������ 2Ÿ", L"IceFloorAura_01", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�ٴ� ���� ������ 2Ÿ", L"IceFloorAura_02", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("�ٴ� ���� ������ 2Ÿ", L"IceFloorAura_03", L"Self_Foot", 1, 20, 0, 0);

	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("�ٴ� ���� ������ 3Ÿ", L"IceFloorAura_01", L"Self_Foot", 1.8, 25, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("�ٴ� ���� ������ 3Ÿ", L"IceFloorAura_02", L"Self_Foot", 1.8, 25, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("�ٴ� ���� ������ 3Ÿ", L"IceFloorAura_03", L"Self_Foot", 1.8, 25, 0, 0);

	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("Į �ñ� ����ũ1", L"IceSmoke_01"	, L"Sword_MidPos"	, 0.2, 150, 0, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("Į �ñ� ����ũ2", L"IceSmoke_02"	, L"Sword_MidPos"	, 0.2, 150, 0, 0);
	
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("�Ķ� �� ��ƼŬ", L"IceGirl_PointParticle_Blue"			, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("�ʷ� �� ��ƼŬ", L"IceGirl_PointParticle_Green"		, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("�Ķ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Blue"		, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("�ʷ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Green"	, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("�Ķ� �� ��ƼŬ", L"IceGirl_PointParticle_Blue"			, L"Sword_MidPos"	, 1, 80, 0, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("�ʷ� �� ��ƼŬ", L"IceGirl_PointParticle_Green"		, L"Sword_MidPos"	, 1, 80, 0, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("�Ķ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Blue"		, L"Sword_MidPos"	, 1, 80, 0, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("�ʷ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Green"	, L"Sword_MidPos"	, 1, 80, 0, 0);
	
	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("�Ķ� �� ��ƼŬ", L"IceGirl_PointParticle_Blue"			, L"Sword_MidPos"	, 1.6, 80, 0, 0);
	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("�ʷ� �� ��ƼŬ", L"IceGirl_PointParticle_Green"		, L"Sword_MidPos"	, 1.6, 80, 0, 0);
	CBT_CreateEffect* Effect21 = Node_CreateEffect_Finite("�Ķ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Blue"		, L"Sword_MidPos"	, 1.6, 80, 0, 0);
	CBT_CreateEffect* Effect22 = Node_CreateEffect_Finite("�ʷ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Green"	, L"Sword_MidPos"	, 1.6, 80, 0, 0);

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

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.616, 1, 0.084, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.45, 1, 0.166, 0);
	CBT_UpdateParam* pHitCol2 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.973 + 0.55, 1, 0.166, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);
	Root_Parallel->Add_Service(pHitCol2);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::ThreeCombo_Cut2()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("3���� ����");
	CBT_Play_Ani* Show_Ani38 = Node_Ani("����1", 38, 0.4f);
	CBT_Play_Ani* Show_Ani37 = Node_Ani("����2", 37, 0.4f);
	CBT_Play_Ani* Show_Ani34 = Node_Ani("���鼭 ����", 34, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.f);


	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.083, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("����0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.45, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.333 + 0.016, 0);
	// �ִ�38 * 0.4f = 0.966
	CBT_RotationDir* Rotation1 = Node_RotationDir("����1", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 3.f, 0.35, 0);
	CBT_Wait* Wait2 = Node_Wait("���2", 0.507 + 0.116, 0);
	// �ִ�37 * 0.4f = 0.973
	CBT_RotationDir* Rotation2 = Node_RotationDir("����2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", 3.f, 0.534, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ٴ� ���� ������ 1Ÿ", L"IceFloorAura_01", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("�ٴ� ���� ������ 1Ÿ", L"IceFloorAura_02", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("�ٴ� ���� ������ 1Ÿ", L"IceFloorAura_03", L"Self_Foot", 0.3, 20, 0, 0);

	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�ٴ� ���� ������ 2Ÿ", L"IceFloorAura_01", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�ٴ� ���� ������ 2Ÿ", L"IceFloorAura_02", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("�ٴ� ���� ������ 2Ÿ", L"IceFloorAura_03", L"Self_Foot", 1, 20, 0, 0);

	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("�ٴ� ���� ������ 3Ÿ", L"IceFloorAura_01", L"Self_Foot", 2.0, 25, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("�ٴ� ���� ������ 3Ÿ", L"IceFloorAura_02", L"Self_Foot", 2.0, 25, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("�ٴ� ���� ������ 3Ÿ", L"IceFloorAura_03", L"Self_Foot", 2.0, 25, 0, 0);

	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("Į �ñ� ����ũ1", L"IceSmoke_01"	, L"Sword_MidPos"	, 0.2, 180, 0, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("Į �ñ� ����ũ2", L"IceSmoke_02"	, L"Sword_MidPos"	, 0.2, 180, 0, 0);
	
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("�Ķ� ����� ��ƼŬ", L"IceGirl_PointParticle_Blue"	, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("�ʷ� ����� ��ƼŬ", L"IceGirl_PointParticle_Green"	, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("�Ķ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Blue"	, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("�ʷ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Green"	, L"Sword_MidPos"	, 0.38, 80, 0, 0);
	
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("�Ķ� ����� ��ƼŬ", L"IceGirl_PointParticle_Blue"	, L"Sword_MidPos"	, 1, 80, 0, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("�ʷ� ����� ��ƼŬ", L"IceGirl_PointParticle_Green"	, L"Sword_MidPos"	, 1, 80, 0, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("�Ķ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Blue"	, L"Sword_MidPos"	, 1, 80, 0, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("�ʷ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Green"	, L"Sword_MidPos"	, 1, 80, 0, 0);
	
	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("�Ķ� ����� ��ƼŬ", L"IceGirl_PointParticle_Blue"	, L"Sword_MidPos"	, 1.95, 80, 0, 0);
	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("�ʷ� ����� ��ƼŬ", L"IceGirl_PointParticle_Green"	, L"Sword_MidPos"	, 1.95, 80, 0, 0);
	CBT_CreateEffect* Effect21 = Node_CreateEffect_Finite("�Ķ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Blue"	, L"Sword_MidPos"	, 1.95, 80, 0, 0);
	CBT_CreateEffect* Effect22 = Node_CreateEffect_Finite("�ʷ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Green"	, L"Sword_MidPos"	, 1.95, 80, 0, 0);

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

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.616, 1, 0.084, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.45, 1, 0.166, 0);
	CBT_UpdateParam* pHitCol2 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.973 + 0.833, 1, 0.217, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);
	Root_Parallel->Add_Service(pHitCol2);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Jump_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("���� ����");
	CBT_Play_Ani* Show_Ani39 = Node_Ani("���� ����", 39, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("����0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 2.f, 1.0, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 10.f, 0.183, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("��¡ ������ ��ƼŬ" , L"IceGirl_Charge_Hand_Particle", L"CreateSwordBulletPos", 0.1, 60, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("��¡ ������ ����ũ1", L"IceGirl_Charge_Hand_Smoke", L"CreateSwordBulletPos", 0.1, 60, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("��¡ ������ ����ũ2", L"IceGirl_Charge_Hand_Smoke_2", L"CreateSwordBulletPos", 0.1, 60, 0, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("Į �ñ� ����ũ1", L"IceSmoke_01", L"Sword_MidPos", 0.2, 60, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("Į �ñ� ����ũ2", L"IceSmoke_02", L"Sword_MidPos", 0.2, 60, 0, 0);

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

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.316, 1, 0.117, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Ice_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("3���� ����");
	CBT_Play_Ani* Show_Ani38 = Node_Ani("����1", 38, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.f);
	
	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.083, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("����0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.45, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.417, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", -0.6f, 0.483, 0);

	// ���� ������ Į �ֵθ��鼭 ���ʴ�� ��ȯ�ϰ�


	// ���� ����� Į �� �ֵθ��� �ϰ������� ��ȯ(ColdBeam Ŭ�������� ��ȯ)
	CBT_CreateBullet* Ice0 = Node_CreateBullet("���� ��ȯ", L"Monster_ColdBeam", L"IceCut_Pos0", L"IceCut_Dir0", 0, 1.0, 0.73, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Ice1 = Node_CreateBullet("���� ��ȯ", L"Monster_ColdBeam", L"IceCut_Pos1", L"IceCut_Dir1", 0, 1.0, 0.76, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Ice2 = Node_CreateBullet("���� ��ȯ", L"Monster_ColdBeam", L"IceCut_Pos2", L"IceCut_Dir2", 0, 1.0, 0.79, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Ice3 = Node_CreateBullet("���� ��ȯ", L"Monster_ColdBeam", L"IceCut_Pos3", L"IceCut_Dir3", 0, 1.0, 0.82, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Ice4 = Node_CreateBullet("���� ��ȯ", L"Monster_ColdBeam", L"IceCut_Pos4", L"IceCut_Dir4", 0, 1.0, 0.85, 1, 1, 0, CBT_Service_Node::Finite);

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

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.616, 1, 0.084, 0);
	Root_Parallel->Add_Service(pHitCol0);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Ice_Barrier()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("���� ��ȣ�� ����");
	CBT_Play_Ani* Show_Ani29 = Node_Ani("���� ��ȣ��", 29, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("��ȣ�� ���� On");
	CBT_SetValue* BarrierOn = Node_BOOL_SetValue("���� ��ȣ�� ���� On", L"Ice_Barrier_On", true);

	CBT_CreateBuff* Barrier0 = Node_CreateBuff("���� ��ȣ�� ����", L"Monster_IceBarrier", 10, 1.5, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Barrier0);
	CBT_UpdateParam* BarrierValue0 = Node_UpdateParam("���� On", &m_tObjParam, CBT_UpdateParam::Dodge, 1.5, 1, 0, 0);
	Root_Parallel->Add_Service(BarrierValue0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� �� ������ ������", L"IceGirl_Buff_Charge_Smoke_01", L"Bone_LeftHand", 0.1, 45, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("����ũ1", L"IceGirl_Buff_Bubble_BreakSmoke", L"Self_Foot", 0.8, 30, 0, 0);

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
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("���Ƽ� ����");
	CBT_Play_Ani* Show_Ani32 = Node_Ani("���Ƽ� ����", 32, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.f);
	
	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("�÷��̾� ����", L"Player_Pos", 2, 0);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_RotationDir* Rotation0 = Node_RotationDir("����0", L"Player_Pos", 0.133);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 30.f, 0.183, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);


	CBT_CreateBullet* Bullet0 = Node_CreateBullet("�˱� �߻�",L"Monster_SwordBullet", L"CreateSwordBulletPos", L"IceCut_Dir2"/*���� ����*/, 23, 0.9, 2.3, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Parallel->Add_Service(Bullet0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("��¡ ������ ��ƼŬ", L"IceGirl_Charge_Hand_Particle", L"CreateSwordBulletPos", 0.1, 60, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("��¡ ������ ����ũ1", L"IceGirl_Charge_Hand_Smoke", L"CreateSwordBulletPos", 0.1, 60, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("��¡ ������ ����ũ2", L"IceGirl_Charge_Hand_Smoke_2", L"CreateSwordBulletPos", 0.1, 60, 0, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�ٴ� ���� ������", L"IceFloorAura_01", L"Self_Foot", 1.9, 25, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�ٴ� ���� ������", L"IceFloorAura_02", L"Self_Foot", 1.9, 25, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("�ٴ� ���� ������", L"IceFloorAura_03", L"Self_Foot", 1.9, 25, 0, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("Į �ñ� ����ũ1", L"IceSmoke_01", L"Sword_MidPos", 0.1, 160, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("Į �ñ� ����ũ2", L"IceSmoke_02", L"Sword_MidPos", 0.1, 160, 0, 0);

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

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 2.133, 1, 0.183, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Dodge_Left()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");
	
	CBT_Sequence* MainSeq = Node_Sequence("�� ȸ��");
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�� ȸ��", 15, 0.5f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("����0", L"Player_Pos", 0.1);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("�̵�", L"DodgePosL", 0.4);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 5.f, 0.3, 0);
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
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�� ȸ��");
	CBT_Play_Ani* Show_Ani14 = Node_Ani("�� ȸ��", 14, 0.5f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("����0", L"Player_Pos", 0.1);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("�̵�", L"DodgePosR", 0.4);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 5.f, 0.3, 0);
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
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�� ȸ��");
	CBT_Play_Ani* Show_Ani16 = Node_Ani("�� ȸ��", 16, 0.5f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("����0", L"Player_Pos", 0.1);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("�̵�", L"DodgePosF", 0.4);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 5.f, 0.3, 0);
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
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�� ȸ��");
	CBT_Play_Ani* Show_Ani17 = Node_Ani("�� ȸ��", 17, 0.5f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("����0", L"Player_Pos", 0.1);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("�̵�", L"DodgePosB", 0.4);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 5.f, 0.3, 0);
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
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("3���� ����");
	CBT_Play_Ani* Show_Ani38 = Node_Ani("����1", 38, 0.4f);
	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 6);
	CBT_Play_Ani* Show_Ani37 = Node_Ani("����2", 37, 0.4f);
	CBT_DistCheck* Dist1 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 6);
	CBT_Play_Ani* Show_Ani35 = Node_Ani("����3", 35, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.083, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("����0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.45, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.333 + 0.016, 0);
	// �ִ�38 * 0.4f = 0.966
	CBT_RotationDir* Rotation1 = Node_RotationDir("����1", L"Player_Pos", 0.1);
	//CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 3.f, 0.35, 0);
	CBT_Wait* Wait2 = Node_Wait("���2", 0.507 + 0.016, 0);
	// �ִ�37 * 0.4f = 0.973
	CBT_RotationDir* Rotation2 = Node_RotationDir("����2", L"Player_Pos", 0.1);
	//CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", 3.f, 0.5, 0);
	CBT_Wait* Wait3 = Node_Wait("���3", 0.767, 0);
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("�̵�3", L"Monster_Speed", L"Monster_Dir", 1.f, 0.317, 0);
	CBT_Wait* Wait4 = Node_Wait("���4", 0.25, 0);
	CBT_MoveDirectly* Move4 = Node_MoveDirectly_Rush("�̵�4", L"Monster_Speed", L"Monster_Dir", -0.6f, 0.716, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ٴ� ���� ������ 1Ÿ", L"IceFloorAura_01", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("�ٴ� ���� ������ 1Ÿ", L"IceFloorAura_02", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("�ٴ� ���� ������ 1Ÿ", L"IceFloorAura_03", L"Self_Foot", 0.3, 20, 0, 0);

	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�ٴ� ���� ������ 2Ÿ", L"IceFloorAura_01", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�ٴ� ���� ������ 2Ÿ", L"IceFloorAura_02", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("�ٴ� ���� ������ 2Ÿ", L"IceFloorAura_03", L"Self_Foot", 1, 20, 0, 0);

	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("�ٴ� ���� ������ 3Ÿ", L"IceFloorAura_01", L"Self_Foot", 1.8, 25, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("�ٴ� ���� ������ 3Ÿ", L"IceFloorAura_02", L"Self_Foot", 1.8, 25, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("�ٴ� ���� ������ 3Ÿ", L"IceFloorAura_03", L"Self_Foot", 1.8, 25, 0, 0);

	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("Į �ñ� ����ũ1", L"IceSmoke_01", L"Sword_MidPos", 0.2, 150, 0, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("Į �ñ� ����ũ2", L"IceSmoke_02", L"Sword_MidPos", 0.2, 150, 0, 0);

	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("�Ķ� �� ��ƼŬ", L"IceGirl_PointParticle_Blue", L"Sword_MidPos", 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("�ʷ� �� ��ƼŬ", L"IceGirl_PointParticle_Green", L"Sword_MidPos", 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("�Ķ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Blue", L"Sword_MidPos", 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("�ʷ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Green", L"Sword_MidPos", 0.38, 80, 0, 0);

	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("�Ķ� �� ��ƼŬ", L"IceGirl_PointParticle_Blue", L"Sword_MidPos", 1, 80, 0, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("�ʷ� �� ��ƼŬ", L"IceGirl_PointParticle_Green", L"Sword_MidPos", 1, 80, 0, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("�Ķ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Blue", L"Sword_MidPos", 1, 80, 0, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("�ʷ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Green", L"Sword_MidPos", 1, 80, 0, 0);

	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("�Ķ� �� ��ƼŬ", L"IceGirl_PointParticle_Blue", L"Sword_MidPos", 1.6, 80, 0, 0);
	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("�ʷ� �� ��ƼŬ", L"IceGirl_PointParticle_Green", L"Sword_MidPos", 1.6, 80, 0, 0);
	CBT_CreateEffect* Effect21 = Node_CreateEffect_Finite("�Ķ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Blue", L"Sword_MidPos", 1.6, 80, 0, 0);
	CBT_CreateEffect* Effect22 = Node_CreateEffect_Finite("�ʷ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Green", L"Sword_MidPos", 1.6, 80, 0, 0);

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

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.616, 1, 0.084, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.45, 1, 0.166, 0);
	CBT_UpdateParam* pHitCol2 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.973 + 0.55, 1, 0.166, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);
	Root_Parallel->Add_Service(pHitCol2);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Chase_ThreeCombo_Cut2()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("3���� ����");
	CBT_Play_Ani* Show_Ani38 = Node_Ani("����1", 38, 0.4f);
	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 6);
	CBT_Play_Ani* Show_Ani37 = Node_Ani("����2", 37, 0.4f);
	CBT_DistCheck* Dist1 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 6);
	CBT_Play_Ani* Show_Ani34 = Node_Ani("���鼭 ����", 34, 0.95f);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻", 0, 0.f);


	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.083, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("����0", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 3.f, 0.45, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.333 + 0.016, 0);
	// �ִ�38 * 0.4f = 0.966
	CBT_RotationDir* Rotation1 = Node_RotationDir("����1", L"Player_Pos", 0.1);
	//CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 3.f, 0.35, 0);
	CBT_Wait* Wait2 = Node_Wait("���2", 0.507 + 0.116, 0);
	// �ִ�37 * 0.4f = 0.973
	CBT_RotationDir* Rotation2 = Node_RotationDir("����2", L"Player_Pos", 0.1);
	//CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", 3.f, 0.534, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ٴ� ���� ������ 1Ÿ", L"IceFloorAura_01", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("�ٴ� ���� ������ 1Ÿ", L"IceFloorAura_02", L"Self_Foot", 0.3, 20, 0, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("�ٴ� ���� ������ 1Ÿ", L"IceFloorAura_03", L"Self_Foot", 0.3, 20, 0, 0);

	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�ٴ� ���� ������ 2Ÿ", L"IceFloorAura_01", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�ٴ� ���� ������ 2Ÿ", L"IceFloorAura_02", L"Self_Foot", 1, 20, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("�ٴ� ���� ������ 2Ÿ", L"IceFloorAura_03", L"Self_Foot", 1, 20, 0, 0);

	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("�ٴ� ���� ������ 3Ÿ", L"IceFloorAura_01", L"Self_Foot", 2.0, 25, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("�ٴ� ���� ������ 3Ÿ", L"IceFloorAura_02", L"Self_Foot", 2.0, 25, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("�ٴ� ���� ������ 3Ÿ", L"IceFloorAura_03", L"Self_Foot", 2.0, 25, 0, 0);

	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("Į �ñ� ����ũ1", L"IceSmoke_01", L"Sword_MidPos", 0.2, 180, 0, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("Į �ñ� ����ũ2", L"IceSmoke_02", L"Sword_MidPos", 0.2, 180, 0, 0);

	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("�Ķ� ����� ��ƼŬ", L"IceGirl_PointParticle_Blue", L"Sword_MidPos", 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("�ʷ� ����� ��ƼŬ", L"IceGirl_PointParticle_Green", L"Sword_MidPos", 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("�Ķ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Blue", L"Sword_MidPos", 0.38, 80, 0, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("�ʷ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Green", L"Sword_MidPos", 0.38, 80, 0, 0);

	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("�Ķ� ����� ��ƼŬ", L"IceGirl_PointParticle_Blue", L"Sword_MidPos", 1, 80, 0, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("�ʷ� ����� ��ƼŬ", L"IceGirl_PointParticle_Green", L"Sword_MidPos", 1, 80, 0, 0);
	CBT_CreateEffect* Effect17 = Node_CreateEffect_Finite("�Ķ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Blue", L"Sword_MidPos", 1, 80, 0, 0);
	CBT_CreateEffect* Effect18 = Node_CreateEffect_Finite("�ʷ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Green", L"Sword_MidPos", 1, 80, 0, 0);

	CBT_CreateEffect* Effect19 = Node_CreateEffect_Finite("�Ķ� ����� ��ƼŬ", L"IceGirl_PointParticle_Blue", L"Sword_MidPos", 1.95, 80, 0, 0);
	CBT_CreateEffect* Effect20 = Node_CreateEffect_Finite("�ʷ� ����� ��ƼŬ", L"IceGirl_PointParticle_Green", L"Sword_MidPos", 1.95, 80, 0, 0);
	CBT_CreateEffect* Effect21 = Node_CreateEffect_Finite("�Ķ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Blue", L"Sword_MidPos", 1.95, 80, 0, 0);
	CBT_CreateEffect* Effect22 = Node_CreateEffect_Finite("�ʷ� ��¦�� ��ƼŬ", L"IceGirl_FlashParticle_Green", L"Sword_MidPos", 1.95, 80, 0, 0);

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

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.616, 1, 0.084, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.45, 1, 0.166, 0);
	CBT_UpdateParam* pHitCol2 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.966 + 0.973 + 0.833, 1, 0.217, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);
	Root_Parallel->Add_Service(pHitCol2);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Chase_Timer(_double dRunTime, _float fSpeed)
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", dRunTime, 0);

	CBT_Sequence* SubSeq = Node_Sequence("����");
	CBT_MoveDirectly* Chase0 = Node_MoveDirectly_Chase("����", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", fSpeed, 3);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Wait0);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Chase0);

	return Root_Parallel;
}

CBT_Composite_Node * CIceGirl::Start_Game()
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

CBT_Composite_Node * CIceGirl::More_Than_HP_70()
{
	CBT_Selector* Root_Sel = Node_Selector("�ٰŸ� ���Ÿ� ���� ����");

	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_More_Than_HP70());

	Root_Sel->Add_Child(FarAttack_More_Than_HP70());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::NearAttack_Dist5_More_Than_HP70()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���� ����");

	Root_Sel->Add_Child(Turn_Cut(0.95f));
	Root_Sel->Add_Child(ThreeCombo_Cut1());
	Root_Sel->Add_Child(ThreeCombo_Cut2());
	Root_Sel->Add_Child(Jump_Attack());
	Root_Sel->Add_Child(Random_Dodge());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::FarAttack_More_Than_HP70()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���Ÿ� ����");

	Root_Sel->Add_Child(Charge_Rush());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::More_Than_HP_40()
{
	CBT_Selector* Root_Sel = Node_Selector("�ٰŸ� ���Ÿ� ���� ����");

	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_More_Than_HP40());

	Root_Sel->Add_Child(FarAttack_More_Than_HP40());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::NearAttack_Dist5_More_Than_HP40()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���� ����");

	Root_Sel->Add_Child(Chase_ThreeCombo_Cut1());
	Root_Sel->Add_Child(Chase_ThreeCombo_Cut2());
	Root_Sel->Add_Child(Ice_Cut());
	Root_Sel->Add_Child(Turn_Cut(0.95f));
	Root_Sel->Add_Child(Random_Dodge());
	
	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::FarAttack_More_Than_HP40()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���Ÿ� ����");

	Root_Sel->Add_Child(Charge_Rush());
	Root_Sel->Add_Child(Dash_To_Target());
	Root_Sel->Add_Child(Create_IceBarrier_Or_Not());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::HP_Final()
{
	CBT_Selector* Root_Sel = Node_Selector("�ٰŸ� ���Ÿ� ���� ����");

	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_Final());

	Root_Sel->Add_Child(FarAttack_Fianl());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::NearAttack_Dist5_Final()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���� ����");

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
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���Ÿ� ����");

	Root_Sel->Add_Child(Charge_Rush());
	Root_Sel->Add_Child(Create_IceBarrier_Or_Not());
	Root_Sel->Add_Child(Dash_To_Target());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::Create_IceBarrier_Or_Not()
{
	/*
	���̽� ������ �������? -> ���� -> �⺻ ���
	-> ���� -> ���� ��ȣ�� ����
	*/

	CBT_Selector* Root_Sel = Node_Selector("���� ��ȣ�� ����");

	CBT_CompareValue* Check_Ice_Barrier = Node_BOOL_A_Equal_Value("�踮�� �������?", L"Ice_Barrier_On", true);
	CBT_Play_Ani* Show_Ani0 = Node_Ani("�⺻1", 0, 0.f);

	Root_Sel->Add_Child(Check_Ice_Barrier);
	Check_Ice_Barrier->Set_Child(Show_Ani0);

	Root_Sel->Add_Child(Ice_Barrier());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::Cut_BackDodge()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� �ڷ� ȸ��");
	
	Root_Seq->Add_Child(Turn_Cut(0.4f));
	Root_Seq->Add_Child(Dodge_Back());

	return Root_Seq;
}

CBT_Composite_Node * CIceGirl::Dash_To_Target()
{
	CBT_Sequence* Root_Seq = Node_Sequence("Ÿ�ٿ��� �뽬");

	CBT_RotationDir* Rotation0 = Node_RotationDir("Ÿ�ٿ��� �뽬", L"Player_Pos", 0.1);
	
	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(Dodge_Front());

	return Root_Seq;
}

CBT_Composite_Node * CIceGirl::Random_Dodge()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ȸ��");

	Root_Sel->Add_Child(Dodge_Left());
	Root_Sel->Add_Child(Dodge_Right());
	Root_Sel->Add_Child(Dodge_Front());
	Root_Sel->Add_Child(Dodge_Back());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::Start_Show()
{
	CBT_Selector* Root_Sel = Node_Selector("�ÿ�ȸ");

	CBT_CompareValue* Check_ShowValueN = Node_BOOL_A_Equal_Value("�ÿ�ȸ �ٰŸ� ���� üũ", L"Show_Near", true);

	Root_Sel->Add_Child(Check_ShowValueN);
	Check_ShowValueN->Set_Child(Show_RotationAndNearAttack());

	Root_Sel->Add_Child(Show_FarAttack());

	return Root_Sel;
}

CBT_Composite_Node * CIceGirl::Show_RotationAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� �� ������� ����");
	CBT_RotationDir* Rotation0 = Node_RotationDir("�÷��̾� �ٶ󺸱�", L"Player_Pos", 0.2);

	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(Show_NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CIceGirl::Show_NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("������� �ٰŸ� ����");

	CBT_Cooldown* Cool0 = Node_Cooldown("��0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("��1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("��2", 300);
	CBT_Cooldown* Cool3 = Node_Cooldown("��3", 300);
	CBT_Cooldown* Cool4 = Node_Cooldown("��4", 300);

	CBT_SetValue* Show_OffNearAttack = Node_BOOL_SetValue("�ÿ�ȸ OFF", L"Show_Near", false);

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
	CBT_Selector* Root_Sel = Node_Selector("������� ���Ÿ� ����");

	CBT_Cooldown* Cool0 = Node_Cooldown("��0", 300);

	CBT_SetValue* Show_ValueOff = Node_BOOL_SetValue("�ÿ�ȸ OFF", L"Show", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Charge_Rush());

	Root_Sel->Add_Child(Show_ValueOff);

	return Root_Sel;
}

void CIceGirl::Down()
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
	// 1. �÷��̾� ��ǥ ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_Pos());
	// 2. ü�� ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HP", m_tObjParam.fHp_Cur);
	// 3. ü�� ���� ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HPRatio", _float(m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max) * 100);


	// 1. ���� -> ���� ��ȯ ��ǥ
	_v3 vSelfLook = *D3DXVec3Normalize(&_v3(), (_v3*)&m_pTransformCom->Get_WorldMat().m[2]);
	_v3 vSelfPos = m_pTransformCom->Get_Pos();

	// ���� ��ȯ ����
	_v3 vDir0 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(50)));
	_v3 vDir1 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(25)));
	_v3 vDir3 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-25)));
	_v3 vDir4 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-50)));
	
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Dir0", vDir0);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Dir1", vDir1);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Dir2", vSelfLook);	// �߾�
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Dir3", vDir3);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Dir4", vDir4);

	// ���� ��ȯ ��ǥ = Position + Look * Alpha
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Pos0", vSelfPos + vDir0 * 3.4f);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Pos1", vSelfPos + vDir1 * 3.2f);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Pos2", vSelfPos + vSelfLook * 3.f);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Pos3", vSelfPos + vDir3 * 3.2f);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"IceCut_Pos4", vSelfPos + vDir4 * 3.4f);

	

	// 2. �÷��̾� �߿� ��������� ��ǥ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Foot", vSelfPos);


	// 3. ��� �˱� ���� ��ġ
	_mat matSwordBullet = static_cast<CTransform*>(m_pSword->Get_Component(L"Com_Transform"))->Get_WorldMat();
	// Į �� ��ǥ
	_v3 vSwordBulletPos = _v3(matSwordBullet.m[3][0], matSwordBullet.m[3][1], matSwordBullet.m[3][2]) + (_v3(0.f, 0.3f, 0.f));

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"CreateSwordBulletPos", vSwordBulletPos);
	
	// Į ��� ��ǥ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Sword_MidPos", vSwordBulletPos +_v3(matSwordBullet.m[2][0], matSwordBullet.m[2][1], matSwordBullet.m[2][2]) * 2.35f);

	// 4. ȸ�� ���� 4��, // ȸ�ǹ��⵵ �߰��ؾ���.
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
	// �ٿ������ ���� ���Լ��� ź��.
	if (true == m_bDown_Start)
	{
		// �� �ȿ��� �浹ó���� �� ����.
		Down();
	}


	// �Ϲ� �浹ó��, bCanHit�� ���Ⱑ false�����ٰ���.
	else if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		m_dHitTime = 0;	// �ǰݰ��� Ÿ�� �ʱ�ȭ

		m_bFight = true;		// �ο� ����

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			// ü�� ���� 70 ���ϵǸ� ����
			if (0 == m_iDownCount)
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

			if (1 == m_iDownCount)
			{
				if (0.4 >= (m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max))
				{
					m_bDown_Start = true;

					m_tObjParam.bDown = true;

					m_pMeshCom->SetUp_Animation(Ani_Down_Start);
					m_bDown_StartAni = true;	//down �Լ� ���ο��� �� ����.
					m_pAIControllerCom->Reset_BT();
					m_bAIController = false;

				}
			}

			//cout << "IceGirl - Check_PhyCollider : " << m_tObjParam.fHp_Cur << endl;

		}
		else
		{
			m_pMeshCom->SetUp_Animation(Ani_Death);	// ����ó�� ����
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
			m_tObjParam.bIsHit = false;		// ���浹 ����
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

void CIceGirl::OnCollisionEnter()
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

void CIceGirl::OnCollisionEvent(list<CGameObject*> plistGameObject)
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
	// ������ ����
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
