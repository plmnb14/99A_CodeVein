#include "stdafx.h"
#include "..\Headers\QueensKnight.h"
#include "..\Headers\Weapon.h"


CQueensKnight::CQueensKnight(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster(pGraphic_Device)
{
}

CQueensKnight::CQueensKnight(const CQueensKnight & rhs)
	: CMonster(rhs)
{
}

HRESULT CQueensKnight::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CQueensKnight::Ready_GameObject(void * pArg)
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
	//pBlackBoard->Set_Value(L"HPRatio", 100);
	pBlackBoard->Set_Value(L"Show", true);
	pBlackBoard->Set_Value(L"Show_Near", true);

	pBlackBoard->Set_Value(L"PushCol", true);	// �浹���� �����
	pBlackBoard->Set_Value(L"PhyCol", true);	// �ǰ����� ���� ����
	pBlackBoard->Set_Value(L"TrailOn", false);	// Ʈ���� ����
	pBlackBoard->Set_Value(L"TrailOff", true);	// Ʈ���� ��

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

	Start_Sel->Add_Child(Flash_Middle_Ground());

	//CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.2);
	//Start_Sel->Add_Child(Rotation0);

	//CBT_Wait* Wait0 = Node_Wait("���", 1, 0);
	//Start_Sel->Add_Child(Wait0);

	return S_OK;
}

_int CQueensKnight::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if ( true == m_pAIControllerCom->Get_BoolValue(L"PushCol"))
		Push_Collider();

	CGameObject::Update_GameObject(TimeDelta);

	// �׾��� ���
	if (m_bIsDead)
		return DEAD_OBJ;

	// ���� �ִϸ��̼�
	if (m_bReadyDead)
		return NO_EVENT;

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

	}

	if (false == m_bReadyDead && true == m_pAIControllerCom->Get_BoolValue(L"PhyCol"))
		Check_PhyCollider();

	OnCollisionEnter();

	return NOERROR;
}

_int CQueensKnight::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == m_pRendererCom)
		return E_FAIL;

	RENDERID eRenderID = RENDER_NONALPHA;
	//if (m_bDissolve)
	//	eRenderID = RENDER_ALPHA;

	if (FAILED(m_pRendererCom->Add_RenderList(eRenderID, this)))
		return E_FAIL;
	//if (FAILED(m_pRendererCom->Add_RenderList(RENDER_SHADOWTARGET, this)))
	//	return E_FAIL;
	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	m_pSword->Late_Update_GameObject(TimeDelta);
	m_pShield->Late_Update_GameObject(TimeDelta);

	return NOERROR;
}

HRESULT CQueensKnight::Render_GameObject()
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
			if (false == m_bReadyDead && !m_bDissolve)
				m_iPass  = m_iTempPass = m_pMeshCom->Get_MaterialPass(i, j);

			m_pShaderCom->Begin_Pass(m_iPass);

			m_pShaderCom->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

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

HRESULT CQueensKnight::Render_GameObject_SetPass(CShader * pShader, _int iPass)
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

CBT_Composite_Node * CQueensKnight::Normal_HorizontalCut1()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�Ϲ� ���κ���1");
	CBT_Play_Ani* Show_Ani50 = Node_Ani("�Ϲ� ���κ���1", 50, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�⺻", 15, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.166, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.567, 0);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("Ʈ���� On", L"TrailOn", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 7.f, 0.333, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("Ʈ���� Off", L"TrailOff", true);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"	, 0.5, 13, 0.1, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"	, 0.5, 13, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos"	, 0.5, 13, 0.1, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos", 0.5, 13, 0.1, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"	, 0.5, 13, 0.1, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"	, 0.5, 13, 0.1, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_0", L"Sword_BottomPos"	, 0.5, 25, 0.1, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_1", L"Sword_BottomPos"	, 0.5, 25, 0.1, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani50);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(TrailOff);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.866, 1, 0.317, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Normal_VerticalCut1()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�Ϲ� ���κ���1");
	CBT_Play_Ani* Show_Ani49 = Node_Ani("�Ϲ� ���κ���1", 49, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�⺻", 15, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.333, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 6, 0.4, 0);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("Ʈ���� On", L"TrailOn", true);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.067, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("Ʈ���� Off", L"TrailOff", true);
	CBT_Wait* Wait2 = Node_Wait("���2", 1.05, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", -2, 0.216, 0);
	CBT_Wait* Wait3 = Node_Wait("���3", 0.15, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", 1, 0.484, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos", 0.3, 13, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos", 0.3, 13, 0.1, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_0", L"Sword_BottomPos", 0.3, 20, 0.1, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos", 0.3, 13, 0.1, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos", 0.3, 13, 0.1, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("������� ��ƼŬ", L"QueensKnight_SwordCrash_Particle", L"Sword_TopPos"		, 0.9, 5, 0.01, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("������� ��ƼŬ", L"QueensKnight_SwordCrash_Particle", L"Sword_MidPos"		, 0.9, 5, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("������� ��ƼŬ", L"QueensKnight_SwordCrash_Particle_Orange", L"Sword_MidPos"	, 0.9, 5, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani49);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(TrailOn);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(TrailOff);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait3);
	SubSeq->Add_Child(Move2);


	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.75, 1, 0.15, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::TwoCombo_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("2���� ����");
	CBT_Play_Ani* Show_Ani50 = Node_Ani("�Ϲ� ���κ���1", 50, 0.5);
	CBT_Play_Ani* Show_Ani48 = Node_Ani("�Ϲ� ���κ���", 48, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�⺻", 15, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.166, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.567, 0);
	CBT_SetValue* TrailOn0 = Node_BOOL_SetValue("Ʈ���� On", L"TrailOn", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 7.f, 0.333, 0);
	CBT_SetValue* TrailOff0 = Node_BOOL_SetValue("Ʈ���� Off", L"TrailOff", true);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.45, 0);
	//50���ִ� * 0.5 = 1.616
	CBT_RotationDir* Rotation1 = Node_RotationDir("�÷��̾� �ٶ󺸱�", L"Player_Pos", 0.09);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("�÷��̾� �ٶ󺸱�", L"Player_Pos", 0.01, 0);
	CBT_Wait* Wait2 = Node_Wait("���2", 0.15, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", 6.f, 0.366, 0);
	CBT_SetValue* TrailOn1 = Node_BOOL_SetValue("Ʈ���� On", L"TrailOn", true);
	CBT_Wait* Wait3 = Node_Wait("���3", 0.15, 0);
	CBT_SetValue* TrailOff1 = Node_BOOL_SetValue("Ʈ���� Off", L"TrailOff", true);
	CBT_Wait* Wait4 = Node_Wait("���4", 0.784, 0);
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("�̵�3", L"Monster_Speed", L"Monster_Dir", -1.f, 0.483, 0);
	CBT_MoveDirectly* Move4 = Node_MoveDirectly_Rush("�̵�4", L"Monster_Speed", L"Monster_Dir", 1.f, 0.5, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"	, 0.6, 15, 0.5, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"	, 0.6, 15, 0.5, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos"	, 0.6, 15, 0.5, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"	, 2.1, 15, 0.5, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"	, 2.1, 15, 0.5, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos"	, 2.1, 15, 0.5, 0);
	CBT_CreateEffect* Effect8  = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos"	, 0.6, 10, 0.5, 0);
	CBT_CreateEffect* Effect9  = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"	, 0.6, 10, 0.5, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"	, 0.6, 10, 0.5, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos"	, 2.1, 10, 0.5, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"	, 2.1, 10, 0.5, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"	, 2.1, 10, 0.5, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("������� ��ƼŬ", L"QueensKnight_SwordCrash_Particle", L"Sword_TopPos", 2.4, 5, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("������� ��ƼŬ", L"QueensKnight_SwordCrash_Particle", L"Sword_MidPos", 2.4, 5, 0.01, 0);

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
	MainSeq->Add_Child(Show_Ani50);
	MainSeq->Add_Child(Show_Ani48);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(TrailOn0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(TrailOff0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Rotation1);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(TrailOn1);
	SubSeq->Add_Child(Wait3);
	SubSeq->Add_Child(TrailOff1);
	SubSeq->Add_Child(Wait4);
	SubSeq->Add_Child(Move3);
	SubSeq->Add_Child(Move4);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.866, 1, 0.317, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.616 + 0.533, 1, 0.15, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);
	
	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::ThreeCombo_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("2���� ����");
	CBT_Play_Ani* Show_Ani50 = Node_Ani("�Ϲ� ���κ���1", 50, 0.5f);
	CBT_Play_Ani* Show_Ani48 = Node_Ani("�Ϲ� ���κ���", 48, 0.4f);
	CBT_Play_Ani* Show_Ani45 = Node_Ani("����ġ��", 45, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�⺻", 15, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.166, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.567, 0);
	CBT_SetValue* TrailOn0 = Node_BOOL_SetValue("Ʈ���� On", L"TrailOn", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 7.f, 0.333, 0);
	CBT_SetValue* TrailOff0 = Node_BOOL_SetValue("Ʈ���� Off", L"TrailOff", true);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.45, 0);
	//50���ִ� * 0.5 = 1.616
	CBT_RotationDir* Rotation1 = Node_RotationDir("�÷��̾� �ٶ󺸱�1", L"Player_Pos", 0.09);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("�÷��̾� �ٶ󺸱�", L"Player_Pos", 0.01, 0);
	CBT_Wait* Wait2 = Node_Wait("���2", 0.15, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", 6.f, 0.366, 0);
	CBT_SetValue* TrailOn1 = Node_BOOL_SetValue("Ʈ���� On", L"TrailOn", true);
	CBT_Wait* Wait3 = Node_Wait("���3", 0.857, 0);
	CBT_SetValue* TrailOff1 = Node_BOOL_SetValue("Ʈ���� Off", L"TrailOff", true);
	CBT_RotationDir* Rotation2 = Node_RotationDir("�÷��̾� �ٶ󺸱�2", L"Player_Pos", 0.09);
	CBT_ChaseDir* ChaseDir1 = Node_ChaseDir("�÷��̾� �ٶ󺸱�1", L"Player_Pos", 0.01, 0);
	//48���ִ� * 0.4 = 1.473
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("�̵�3", L"Monster_Speed", L"Monster_Dir", 6.f, 0.45, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"	, 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"	, 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos"	, 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"	, 2.1, 15, 0.1, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"	, 2.1, 15, 0.1, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos"	, 2.1, 15, 0.1, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos"	, 0.6, 12, 0.1, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"	, 0.6, 12, 0.1, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"	, 0.6, 12, 0.1, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos"	, 2.1, 12, 0.1, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"	, 2.1, 12, 0.1, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"	, 2.1, 12, 0.1, 0);
	
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("������� ��ƼŬ", L"QueensKnight_SwordCrash_Particle", L"Sword_TopPos"	, 2.36, 5, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("������� ��ƼŬ", L"QueensKnight_SwordCrash_Particle", L"Sword_MidPos"	, 2.36, 5, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("����ġ�� ��ƼŬ", L"QueensKnight_ShieldAttack_Particle", L"ShieldPos"		, 3.45, 5, 0.01, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("����ġ�� �ְ�", L"QueensKnight_ShieldAttack_Distortion", L"ShieldPos"		, 3.5, 10, 0.1, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("����ġ�� ���� ��", L"QueensKnight_ShieldAttack_RedLight", L"ShieldPos"	, 3.45, 1, 0.01, 0);

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

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani50);
	MainSeq->Add_Child(Show_Ani48);
	MainSeq->Add_Child(Show_Ani45);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);	
	SubSeq->Add_Child(TrailOn0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(TrailOff0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Rotation1);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(TrailOn1);
	SubSeq->Add_Child(Wait3);
	SubSeq->Add_Child(TrailOff1);
	SubSeq->Add_Child(Rotation2);
	SubSeq->Add_Child(ChaseDir1);
	SubSeq->Add_Child(Move3);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.866, 1, 0.317, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.616 + 0.533, 1, 0.15, 0);
	CBT_UpdateParam* pHitCol2 = Node_UpdateParam("���� ��Ʈ On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.616 + 1.473 + 0.333, 1, 0.167, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);
	Root_Parallel->Add_Service(pHitCol2);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::BackStep_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�齺�� ����");
	CBT_Play_Ani* Show_Ani55 = Node_Ani("�齺�� ����", 55, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�⺻", 15, 0.f);
	
	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.8, 0);
	CBT_SetValue* TrailOn0 = Node_BOOL_SetValue("Ʈ���� On", L"TrailOn", true);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.066, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", -8, 0.484, 0);
	CBT_SetValue* TrailOff0 = Node_BOOL_SetValue("Ʈ���� Off", L"TrailOff", true);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos"			, 0.6, 30, 0.8, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"			, 0.6, 30, 0.8, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"			, 0.6, 30, 0.8, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"		, 0.6, 22, 0.8, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"		, 0.6, 22, 0.8, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos"	, 0.6, 22, 0.8, 0);
	
	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani55);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(TrailOn0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(TrailOff0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.9, 1, 0.183, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Sting()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("���");
	CBT_Play_Ani* Show_Ani42 = Node_Ani("���", 42, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�⺻", 15, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.366, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 6, 0.317, 0);
	CBT_SetValue* TrailOn0 = Node_BOOL_SetValue("Ʈ���� On", L"TrailOn", true);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.133, 0);
	CBT_SetValue* TrailOff0 = Node_BOOL_SetValue("Ʈ���� Off", L"TrailOff", true);
	CBT_Wait* Wait2 = Node_Wait("���2", 0.85, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", -1, 0.317, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", 1, 0.35, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos", 0.6, 17, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos", 0.6, 17, 0.1, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos", 0.6, 7, 0.1, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos", 0.6, 7, 0.1, 0);
	//CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("�� �ҿ뵹��", L"QueensKnight_Sting_Tornade", L"Sword_TopPos", 0.65, 1, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	//Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani42);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(TrailOn0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(TrailOff0);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.683, 1, 0.133, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Wing_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");
	CBT_Sequence* MainSeq = Node_Sequence("���鼭 ����ġ��");
	CBT_Play_Ani* Show_Ani43 = Node_Ani("���鼭 ����ġ��", 43, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�⺻", 15, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.1);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.05, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", -6, 0.25, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.85, 0);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation1 = Node_RotationDir("���� ����2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 8, 0.483, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("����", L"QueensKnight_WhirlWind_Smoke", L"Self_Pos", 0.63, 10, 0.5, 0);

	Root_Parallel->Add_Service(Effect0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani43);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation1);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.533, 1, 0.45, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Rush()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("���� ���");
	CBT_Play_Ani* Show_Ani42 = Node_Ani("���� ���", 42, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�⺻", 15, 0.f);
	
	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.276, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����2", L"Player_Pos", 0.2);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 30, 0.307, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.983, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", -1, 0.317, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", 1, 0.35, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos"		, 0.6, 16, 0.1, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"		, 0.6, 16, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"		, 0.6, 16, 0.1, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"	, 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"	, 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos", 0.6, 15, 0.1, 0);
	//CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("�� �ҿ뵹��", L"QueensKnight_Sting_Tornade", L"Sword_TopPos", 0.65, 1, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	//Root_Parallel->Add_Service(Effect6);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani42);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(PushColOn);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.456, 1, 0.36, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Shield_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("����ġ��");
	CBT_Play_Ani* Show_Ani46 = Node_Ani("����ġ��", 46, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�⺻", 15, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.083, 0);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 4.f, 0.6, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.617, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", -2.f, 0.333, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("����ġ�� ��ƼŬ", L"QueensKnight_ShieldAttack_Particle", L"ShieldPos", 0.65, 5, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("����ġ�� �ְ�", L"QueensKnight_ShieldAttack_Distortion", L"ShieldPos", 0.7, 10, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("����ġ�� ���� ��", L"QueensKnight_ShieldAttack_RedLight", L"ShieldPos", 0.65, 1, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani46);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);


	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("���� ��Ʈ On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.683, 1, 0.133, 0);
	Root_Parallel->Add_Service(pHitCol0);


	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Flash()
{
	CBT_Sequence* MainSeq = Node_Sequence("�̵�");

	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("�̵�", L"FlashPos", 0.2);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOn", L"PhyCol", true);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);

	MainSeq->Add_Child(PhyColOff);
	MainSeq->Add_Child(PushColOff);
	MainSeq->Add_Child(MoveTo0);
	MainSeq->Add_Child(PhyColOn);
	MainSeq->Add_Child(PushColOn);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� �ְ�", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.0, 1, 0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� �ְ�", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.16, 1, 0, 0);

	CBT_StartDissolve* Dissolve0 = Node_StartDissolve("������", this, 12.7f, false, 0.01);
	CBT_StartDissolve* Dissolve1 = Node_StartDissolve("������", this, 12.7f, true, 0.17);
	CBT_StartDissolve* Dissolve2 = Node_StartDissolve("������", m_pSword, 12.7f, false, 0.01);
	CBT_StartDissolve* Dissolve3 = Node_StartDissolve("������", m_pSword, 12.7f, true, 0.17);
	CBT_StartDissolve* Dissolve4 = Node_StartDissolve("������", m_pShield, 12.7f, false, 0.01);
	CBT_StartDissolve* Dissolve5 = Node_StartDissolve("������", m_pShield, 12.7f, true, 0.17);

	MainSeq->Add_Service(Effect0);
	MainSeq->Add_Service(Effect1);

	MainSeq->Add_Service(Dissolve0);
	MainSeq->Add_Service(Dissolve1);
	MainSeq->Add_Service(Dissolve2);
	MainSeq->Add_Service(Dissolve3);
	MainSeq->Add_Service(Dissolve4);
	MainSeq->Add_Service(Dissolve5);

	return MainSeq;
}

CBT_Composite_Node * CQueensKnight::Flash_Wing_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");
	CBT_Sequence* MainSeq = Node_Sequence("���鼭 ����ġ��");
	CBT_Play_Ani* Show_Ani43 = Node_Ani("���鼭 ����ġ��", 43, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�⺻", 15, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.15, 0);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", -6, 0.25, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.75, 0);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("���� �̵�", L"FlashPos", 0.1);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOn", L"PhyCol", true);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 8, 0.483, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� ��ƼŬ", L"QueensKnight_Teleport_Particle", L"Self_MidPos", 0, 150, 0.15, 0);
	//CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ���� ����", L"QueensKnight_Teleport_Smoke", L"Self_MidPos", 0, 50, 0.35, 0);
	//CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ���� ����", L"QueensKnight_Teleport_Smoke_Red", L"Self_MidPos", 0, 50, 0.35, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("����", L"QueensKnight_WhirlWind_Smoke", L"Self_Pos", 1.3, 20, 0.1, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("���� �ְ�", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.2, 1, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("���� �ְ�", L"QueensKnight_DistortionCircle", L"Self_MidPos", 1.3, 1, 0, 0);

	CBT_StartDissolve* Dissolve0 = Node_StartDissolve("������", this		, 3.7f, false, 0.2);
	CBT_StartDissolve* Dissolve1 = Node_StartDissolve("������", this		, 3.7f, true, 1.3);
	CBT_StartDissolve* Dissolve2 = Node_StartDissolve("������", m_pSword	, 3.7f, false, 0.2);
	CBT_StartDissolve* Dissolve3 = Node_StartDissolve("������", m_pSword	, 3.7f, true, 1.3);
	CBT_StartDissolve* Dissolve4 = Node_StartDissolve("������", m_pShield	, 3.7f, false, 0.2);
	CBT_StartDissolve* Dissolve5 = Node_StartDissolve("������", m_pShield	, 3.7f, true, 1.3);

	Root_Parallel->Add_Service(Effect0);
	//Root_Parallel->Add_Service(Effect1);
	//Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Dissolve0);
	Root_Parallel->Add_Service(Dissolve1);
	Root_Parallel->Add_Service(Dissolve2);
	Root_Parallel->Add_Service(Dissolve3);
	Root_Parallel->Add_Service(Dissolve4);
	Root_Parallel->Add_Service(Dissolve5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani43);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(PhyColOn);
	SubSeq->Add_Child(PushColOn);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.533, 1, 0.45, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Flash_Rush()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("���� ���");
	CBT_Play_Ani* Show_Ani42 = Node_Ani("���� ���", 42, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�⺻", 15, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_Wait* Wait0 = Node_Wait("���0", 0.176, 0);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("���� �̵�", L"FlashPos", 0.1);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOff", L"PhyCol", true);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 30, 0.307, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.983, 0);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", -1, 0.317, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", 1, 0.35, 0);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� ��ƼŬ", L"QueensKnight_Teleport_Particle", L"Self_MidPos", 0, 150, 0.15, 0);
	//CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ���� ����", L"QueensKnight_Teleport_Smoke", L"Self_MidPos", 0, 50, 0.35, 0);
	//CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ���� ����", L"QueensKnight_Teleport_Smoke_Red", L"Self_MidPos", 0, 50, 0.35, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos"			, 0.2, 17, 0.1, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"			, 0.2, 17, 0.1, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"			, 0.2, 17, 0.1, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos"		, 0.2, 13, 0.1, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"		, 0.2, 13, 0.1, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos"	, 0.2, 13, 0.1, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("���� �ְ�", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.17, 1, 0, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("���� �ְ�", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.6, 1, 0, 0);

	CBT_StartDissolve* Dissolve0 = Node_StartDissolve("������", this,  3.7f, false, 0.17);
	CBT_StartDissolve* Dissolve1 = Node_StartDissolve("������", this, 3.7f, true, 0.6);
	CBT_StartDissolve* Dissolve2 = Node_StartDissolve("������", m_pSword, 3.7f, false, 0.17);
	CBT_StartDissolve* Dissolve3 = Node_StartDissolve("������", m_pSword, 3.7f, true, 0.6);
	CBT_StartDissolve* Dissolve4 = Node_StartDissolve("������", m_pShield, 3.7f, false, 0.17);
	CBT_StartDissolve* Dissolve5 = Node_StartDissolve("������", m_pShield, 3.7f, true, 0.6);

	Root_Parallel->Add_Service(Effect0);
	//Root_Parallel->Add_Service(Effect1);
	//Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);
	Root_Parallel->Add_Service(Effect8);
	Root_Parallel->Add_Service(Effect9);
	Root_Parallel->Add_Service(Effect10);
	Root_Parallel->Add_Service(Dissolve0);
	Root_Parallel->Add_Service(Dissolve1);
	Root_Parallel->Add_Service(Dissolve2);
	Root_Parallel->Add_Service(Dissolve3);
	Root_Parallel->Add_Service(Dissolve4);
	Root_Parallel->Add_Service(Dissolve5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani42);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(PhyColOn);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(PushColOn);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.476, 1, 0.34, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Flash_Jump_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("������ �������");
	CBT_Play_Ani* Show_Ani38 = Node_Ani("���� ����", 38, 0.7f);
	CBT_Play_Ani* Show_Ani40 = Node_Ani("���� ���", 40, 0.9f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�⺻", 15, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.871, 0);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("���� �̵�", L"FlashPos", 0.05);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOff", L"PhyCol", true);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����2", L"Player_Pos", 0.1);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� ��ƼŬ", L"QueensKnight_Teleport_Particle", L"Self_MidPos", 0, 150, 0.15, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ���� ����", L"QueensKnight_Teleport_Smoke", L"Self_MidPos", 0, 10, 0.35, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("���� �ְ�", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.42, 1, 0, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("���� �ְ�", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.68, 1, 0, 0);
	//CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("���� �ְ� ����", L"QueensKnight_Teleport_DistortionSmoke", L"Self_MidPos", 0.3, 20, 0.1, 0);
	//CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("������� ����", L"QueensKnight_JumpDown_ShockWave", L"Self_Pos"			, 1.45, 1, 0.01, 0);
	//CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("������� ���", L"QueensKnight_2Phase_SwordCrash_Chunk", L"Sword_TopPos", 1.45, 2, 0.01, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("������� ���", L"QueensKnight_2Phase_SwordCrash_Chunk", L"Sword_MidPos", 1.45, 2, 0.01, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("������� ���", L"QueensKnight_2Phase_SwordCrash_Chunk_Dark", L"Sword_MidPos", 1.45, 2, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("������� ��ƼŬ", L"QueensKnight_JumpDown_Particle_Red", L"Sword_TopPos"	, 1.45, 2, 0.01, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("������� ��ƼŬ", L"QueensKnight_JumpDown_Particle_Red", L"Sword_MidPos"	, 1.45, 2, 0.01, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("������� ��ƼŬ", L"QueensKnight_SwordCrash_Particle", L"Sword_TopPos"	, 1.45, 5, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("������� ��ƼŬ", L"QueensKnight_SwordCrash_Particle", L"Sword_MidPos"	, 1.45, 5, 0.01, 0);
	//CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("����ũ", L"QueensKnight_JumpDown_Smoke_Black"	, L"Sword_MidPos"	, 1.45, 10, 0.01, 0);
	//CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("����ũ", L"QueensKnight_JumpDown_Smoke_Red"		, L"Sword_MidPos"	, 1.45, 10, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos", 1.12, 20, 0.01, 0);

	CBT_StartDissolve* Dissolve0 = Node_StartDissolve("������", this		, 3.7f, false, 0.45);
	CBT_StartDissolve* Dissolve1 = Node_StartDissolve("������", this		, 3.7f, true, 0.7);
	CBT_StartDissolve* Dissolve2 = Node_StartDissolve("������", m_pSword	, 3.7f, false, 0.45);
	CBT_StartDissolve* Dissolve3 = Node_StartDissolve("������", m_pSword	, 3.7f, true, 0.7);
	CBT_StartDissolve* Dissolve4 = Node_StartDissolve("������", m_pShield	, 3.7f, false, 0.45);
	CBT_StartDissolve* Dissolve5 = Node_StartDissolve("������", m_pShield	, 3.7f, true, 0.7);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	//Root_Parallel->Add_Service(Effect2);
	//Root_Parallel->Add_Service(Effect3);
	//Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);
	Root_Parallel->Add_Service(Effect6);
	Root_Parallel->Add_Service(Effect7);
	Root_Parallel->Add_Service(Effect8);
	Root_Parallel->Add_Service(Effect9);
	Root_Parallel->Add_Service(Effect10);
	Root_Parallel->Add_Service(Effect11);
	//Root_Parallel->Add_Service(Effect12);
	//Root_Parallel->Add_Service(Effect13);
	Root_Parallel->Add_Service(Effect14);
	Root_Parallel->Add_Service(Effect15);
	Root_Parallel->Add_Service(Dissolve0);
	Root_Parallel->Add_Service(Dissolve1);
	Root_Parallel->Add_Service(Dissolve2);
	Root_Parallel->Add_Service(Dissolve3);
	Root_Parallel->Add_Service(Dissolve4);
	Root_Parallel->Add_Service(Dissolve5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani38);
	MainSeq->Add_Child(Show_Ani40);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(PushColOn);
	SubSeq->Add_Child(PhyColOn);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.921 + 0.416, 1, 0.15, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Flash_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("���� ����");
	CBT_Play_Ani* Show_Ani37 = Node_Ani("���� ����", 37, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�⺻", 15, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.2, 0);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.533, 0);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("���� �̵�", L"FlashPos", 0.1);
	CBT_SetValue* TrailOn = Node_BOOL_SetValue("Ʈ���� On", L"TrailOn", true);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOff", L"PhyCol", true);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����1", L"Player_Pos", 0.1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����2", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 5.f, 0.267, 0);
	CBT_Wait* Wait2 = Node_Wait("���2", 1.55, 0);
	CBT_SetValue* TrailOff = Node_BOOL_SetValue("Ʈ���� Off", L"TrailOff", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", -0.3f, 0.667, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� ��ƼŬ", L"QueensKnight_Teleport_Particle", L"Self_MidPos", 0, 50, 0.15, 0);
	//CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ���� ����", L"QueensKnight_Teleport_Smoke", L"Self_MidPos", 0, 40, 0, 0);
	//CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ���� ����", L"QueensKnight_Teleport_Smoke_Red", L"Self_MidPos", 0, 40, 0, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("���� �ְ�", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.2, 1, 0.35, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("���� �ְ�", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.8, 1, 0.35, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("���� ��ƼŬ ��", L"QueensKnight_Teleport_Particle_Black", L"Self_MidPos", 0, 2, 0, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("���� ��ƼŬ ��", L"QueensKnight_Teleport_Particle_Black", L"Self_MidPos", 0.8, 2, 0, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos"		, 0.85, 17, 0, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos"		, 0.85, 17, 0, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_TomPos"		, 0.85, 17, 0, 0);
	CBT_CreateEffect* Effect9  = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos"		, 0.85, 15, 0, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos"		, 0.85, 15, 0, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TomPos"		, 0.85, 15, 0, 0);

	CBT_StartDissolve* Dissolve0 = Node_StartDissolve("������", this		, 3.7f, false, 0.2);
	CBT_StartDissolve* Dissolve1 = Node_StartDissolve("������", this		, 3.7f, true, 0.8);
	CBT_StartDissolve* Dissolve2 = Node_StartDissolve("������", m_pSword	, 3.7f, false, 0.2);
	CBT_StartDissolve* Dissolve3 = Node_StartDissolve("������", m_pSword	, 3.7f, true, 0.8);
	CBT_StartDissolve* Dissolve4 = Node_StartDissolve("������", m_pShield	, 3.7f, false, 0.2);
	CBT_StartDissolve* Dissolve5 = Node_StartDissolve("������", m_pShield	, 3.7f, true, 0.8);

	Root_Parallel->Add_Service(Effect0);
	//Root_Parallel->Add_Service(Effect1);
	//Root_Parallel->Add_Service(Effect2);
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
	Root_Parallel->Add_Service(Dissolve0);
	Root_Parallel->Add_Service(Dissolve1);
	Root_Parallel->Add_Service(Dissolve2);
	Root_Parallel->Add_Service(Dissolve3);
	Root_Parallel->Add_Service(Dissolve4);
	Root_Parallel->Add_Service(Dissolve5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani37);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(TrailOn);	
	SubSeq->Add_Child(PushColOn);
	SubSeq->Add_Child(PhyColOn);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(TrailOff);
	SubSeq->Add_Child(Move1);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.283, 1, 0.133, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Flash_Middle_Ground()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("�߾����� ���� �� �������");
	CBT_Play_Ani* Show_Ani38 = Node_Ani("�߾����� ���� ����", 38, 0.7f);
	CBT_Play_Ani* Show_Ani40 = Node_Ani("���� ���", 39, 0.9f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�⺻", 15, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.821, 0);
	CBT_SetValue* PushColOff = Node_BOOL_SetValue("PushColOff", L"PushCol", false);
	CBT_SetValue* PhyColOff = Node_BOOL_SetValue("PhyColOff", L"PhyCol", false);
	CBT_MoveTo* MoveTo0 = Node_MoveTo("���� �̵�", L"Field_MidPos", 0.1);
	CBT_SetValue* PushColOn = Node_BOOL_SetValue("PushColOn", L"PushCol", true);
	CBT_SetValue* PhyColOn = Node_BOOL_SetValue("PhyColOn", L"PhyCol", true);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� ��ƼŬ", L"QueensKnight_Teleport_Particle", L"Self_MidPos", 0, 70, 0.0, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� �ְ�", L"QueensKnight_DistortionCircle", L"Self_MidPos", 0.2, 1, 0.35, 0);

	CBT_StartDissolve* Dissolve0 = Node_StartDissolve("������", this, 3.7f, false		, 0.2);
	CBT_StartDissolve* Dissolve2 = Node_StartDissolve("������", m_pSword, 3.7f, false	, 0.2);
	CBT_StartDissolve* Dissolve4 = Node_StartDissolve("������", m_pShield, 3.7f, false	, 0.2);
	CBT_StartDissolve* Dissolve1 = Node_StartDissolve("������", this, 3.7f, true		, 1.25);
	CBT_StartDissolve* Dissolve3 = Node_StartDissolve("������", m_pSword, 3.7f, true	, 1.25);
	CBT_StartDissolve* Dissolve5 = Node_StartDissolve("������", m_pShield, 3.7f, true	, 1.25);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);

	Root_Parallel->Add_Service(Dissolve0);
	Root_Parallel->Add_Service(Dissolve1);
	Root_Parallel->Add_Service(Dissolve2);
	Root_Parallel->Add_Service(Dissolve3);
	Root_Parallel->Add_Service(Dissolve4);
	Root_Parallel->Add_Service(Dissolve5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani38);
	MainSeq->Add_Child(Show_Ani40);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(PushColOff);
	SubSeq->Add_Child(PhyColOff);
	SubSeq->Add_Child(MoveTo0);
	SubSeq->Add_Child(PushColOn);
	SubSeq->Add_Child(PhyColOn);

	
	CBT_CreateBullet* Col0 = Node_CreateBullet("�ٴڿ��� � ����Ѿ�", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir0", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col1 = Node_CreateBullet("�ٴڿ��� � ����Ѿ�", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir1", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col2 = Node_CreateBullet("�ٴڿ��� � ����Ѿ�", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir2", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col3 = Node_CreateBullet("�ٴڿ��� � ����Ѿ�", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir3", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col4 = Node_CreateBullet("�ٴڿ��� � ����Ѿ�", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir4", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col5 = Node_CreateBullet("�ٴڿ��� � ����Ѿ�", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir5", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col6 = Node_CreateBullet("�ٴڿ��� � ����Ѿ�", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir6", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* Col7 = Node_CreateBullet("�ٴڿ��� � ����Ѿ�", L"Monster_DarkGroundBullet", L"Self_Pos", L"DarkBulletDir7", 20, 0.5, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);
	
	CBT_CreateBullet* ColMid = Node_CreateBullet("���� ������ ���", L"Monster_DarkMidCol", L"Self_Pos", L"", 0, 0.5f, 0.921 + 0.8, 1, 0, 0, CBT_Service_Node::Finite);

	Root_Parallel->Add_Service(Col0);
	Root_Parallel->Add_Service(Col1);
	Root_Parallel->Add_Service(Col2);
	Root_Parallel->Add_Service(Col3);
	Root_Parallel->Add_Service(Col4);
	Root_Parallel->Add_Service(Col5);
	Root_Parallel->Add_Service(Col6);
	Root_Parallel->Add_Service(Col7);

	Root_Parallel->Add_Service(ColMid);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Smart_ThreeCombo_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_Sequence* MainSeq = Node_Sequence("2���� ����");
	CBT_Play_Ani* Show_Ani50 = Node_Ani("�Ϲ� ���κ���1", 50, 0.5f);
	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 5);
	CBT_Play_Ani* Show_Ani48 = Node_Ani("�Ϲ� ���κ���", 48, 0.4f);
	CBT_DistCheck* Dist1 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 3);
	CBT_Play_Ani* Show_Ani45 = Node_Ani("����ġ��", 45, 0.95f);
	CBT_Play_Ani* Show_Ani15 = Node_Ani("�⺻", 15, 0.f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���0", 0.166, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.1);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�0", L"Monster_Speed", L"Monster_Dir", 2.f, 0.567, 0);
	CBT_SetValue* TrailOn0 = Node_BOOL_SetValue("Ʈ���� On", L"TrailOn", true);
	CBT_MoveDirectly* Move1 = Node_MoveDirectly_Rush("�̵�1", L"Monster_Speed", L"Monster_Dir", 7.f, 0.333, 0);
	CBT_Wait* Wait1 = Node_Wait("���1", 0.45, 0);
	CBT_SetValue* TrailOff0 = Node_BOOL_SetValue("Ʈ���� Off", L"TrailOff", true);
	//50���ִ� * 0.5 = 1.616
	CBT_RotationDir* Rotation1 = Node_RotationDir("�÷��̾� �ٶ󺸱�1", L"Player_Pos", 0.09);
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("�÷��̾� �ٶ󺸱�", L"Player_Pos", 0.01, 0);
	CBT_Wait* Wait2 = Node_Wait("���2", 0.15, 0);
	CBT_MoveDirectly* Move2 = Node_MoveDirectly_Rush("�̵�2", L"Monster_Speed", L"Monster_Dir", 6.f, 0.366, 0);
	CBT_SetValue* TrailOn1 = Node_BOOL_SetValue("Ʈ���� On", L"TrailOn", true);
	CBT_Wait* Wait3 = Node_Wait("���3", 0.857, 0);
	CBT_SetValue* TrailOff1 = Node_BOOL_SetValue("Ʈ���� Off", L"TrailOff", true);
	CBT_RotationDir* Rotation2 = Node_RotationDir("�÷��̾� �ٶ󺸱�2", L"Player_Pos", 0.09);
	CBT_ChaseDir* ChaseDir1 = Node_ChaseDir("�÷��̾� �ٶ󺸱�1", L"Player_Pos", 0.01, 0);
	//48���ִ� * 0.4 = 1.473
	CBT_MoveDirectly* Move3 = Node_MoveDirectly_Rush("�̵�3", L"Monster_Speed", L"Monster_Dir", 6.f, 0.45, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos", 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos", 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos", 0.6, 15, 0.1, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_BottomPos", 2.1, 15, 0.1, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_MidPos", 2.1, 15, 0.1, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("���� ����", L"QueensKnight_Trail_Lightning_2", L"Sword_TopPos", 2.1, 15, 0.1, 0);
	CBT_CreateEffect* Effect11 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos", 0.6, 12, 0.1, 0);
	CBT_CreateEffect* Effect12 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos", 0.6, 12, 0.1, 0);
	CBT_CreateEffect* Effect13 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos", 0.6, 12, 0.1, 0);
	CBT_CreateEffect* Effect14 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_BottomPos", 2.1, 12, 0.1, 0);
	CBT_CreateEffect* Effect15 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_MidPos", 2.1, 12, 0.1, 0);
	CBT_CreateEffect* Effect16 = Node_CreateEffect_Finite("�˺��� ����", L"QueensKnight_Trail_Lightning_2_Dark", L"Sword_TopPos", 2.1, 12, 0.1, 0);

	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("������� ��ƼŬ", L"QueensKnight_SwordCrash_Particle", L"Sword_TopPos", 2.36, 5, 0.01, 0);
	CBT_CreateEffect* Effect10 = Node_CreateEffect_Finite("������� ��ƼŬ", L"QueensKnight_SwordCrash_Particle", L"Sword_MidPos", 2.36, 5, 0.01, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("����ġ�� ��ƼŬ", L"QueensKnight_ShieldAttack_Particle", L"ShieldPos", 3.45, 5, 0.01, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("����ġ�� �ְ�", L"QueensKnight_ShieldAttack_Distortion", L"ShieldPos", 3.5, 10, 0.1, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("����ġ�� ���� ��", L"QueensKnight_ShieldAttack_RedLight", L"ShieldPos", 3.45, 1, 0.01, 0);

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

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani50);
	MainSeq->Add_Child(Dist0);
	Dist0->Set_Child(Show_Ani48);
	MainSeq->Add_Child(Dist1);
	Dist1->Set_Child(Show_Ani45);
	MainSeq->Add_Child(Show_Ani15);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Rotation0);
	SubSeq->Add_Child(Move0);
	SubSeq->Add_Child(TrailOn0);
	SubSeq->Add_Child(Move1);
	SubSeq->Add_Child(Wait1);
	SubSeq->Add_Child(TrailOff0);
	SubSeq->Add_Child(Rotation1);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Wait2);
	SubSeq->Add_Child(Move2);
	SubSeq->Add_Child(TrailOn1);
	SubSeq->Add_Child(Wait3);
	SubSeq->Add_Child(TrailOff1);
	SubSeq->Add_Child(Rotation2);
	SubSeq->Add_Child(ChaseDir1);
	SubSeq->Add_Child(Move3);

	CBT_UpdateParam* pHitCol0 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.866, 1, 0.317, 0);
	CBT_UpdateParam* pHitCol1 = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.616 + 0.533, 1, 0.15, 0);
	CBT_UpdateParam* pHitCol2 = Node_UpdateParam("���� ��Ʈ On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.616 + 1.473 + 0.333, 1, 0.167, 0);
	Root_Parallel->Add_Service(pHitCol0);
	Root_Parallel->Add_Service(pHitCol1);
	Root_Parallel->Add_Service(pHitCol2);

	return Root_Parallel;
}

CBT_Composite_Node * CQueensKnight::Start_Game()
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

CBT_Composite_Node * CQueensKnight::More_Than_HP_70()
{
	CBT_Selector* Root_Sel = Node_Selector("�ٰŸ� ���Ÿ� ���� ����");

	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_More_Than_HP70());

	Root_Sel->Add_Child(FarAttack_More_Than_HP70());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::NearAttack_Dist5_More_Than_HP70()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���� ����");

	Root_Sel->Add_Child(Normal_HorizontalCut1());
	Root_Sel->Add_Child(Normal_VerticalCut1());
	Root_Sel->Add_Child(TwoCombo_Cut());
	Root_Sel->Add_Child(ThreeCombo_Cut());
	Root_Sel->Add_Child(BackStep_Cut());
	Root_Sel->Add_Child(Sting());
	Root_Sel->Add_Child(Wing_Attack());
	Root_Sel->Add_Child(Shield_Attack());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::FarAttack_More_Than_HP70()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���Ÿ� ����");

	Root_Sel->Add_Child(Rush());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::More_Than_HP_40()
{
	CBT_Selector* Root_Sel = Node_Selector("�ٰŸ� ���Ÿ� ���� ����");

	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_More_Than_HP40());

	Root_Sel->Add_Child(FarAttack_More_Than_HP40());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::NearAttack_Dist5_More_Than_HP40()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���� ����");

	//Root_Sel->Add_Child(Normal_HorizontalCut1());
	//Root_Sel->Add_Child(Normal_VerticalCut1());
	//Root_Sel->Add_Child(TwoCombo_Cut());
	//Root_Sel->Add_Child(ThreeCombo_Cut());
	Root_Sel->Add_Child(BackStep_Cut());
	Root_Sel->Add_Child(Sting());
	Root_Sel->Add_Child(Wing_Attack());
	Root_Sel->Add_Child(Flash());
	Root_Sel->Add_Child(Smart_ThreeCombo_Cut());
	Root_Sel->Add_Child(Shield_Attack());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::FarAttack_More_Than_HP40()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���Ÿ� ����");
	
	Root_Sel->Add_Child(Rush());
	Root_Sel->Add_Child(Flash_Rush());
	Root_Sel->Add_Child(Flash_Jump_Attack());
	Root_Sel->Add_Child(Flash_Cut());
	Root_Sel->Add_Child(Flash());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::HP_Final()
{
	CBT_Selector* Root_Sel = Node_Selector("�ٰŸ� ���Ÿ� ���� ����");

	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 5);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist5_Final());

	Root_Sel->Add_Child(FarAttack_Fianl());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::NearAttack_Dist5_Final()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���� ����");

	//Root_Sel->Add_Child(Normal_HorizontalCut1());
	//Root_Sel->Add_Child(Normal_VerticalCut1());
	//Root_Sel->Add_Child(TwoCombo_Cut());
	//Root_Sel->Add_Child(ThreeCombo_Cut());
	Root_Sel->Add_Child(Smart_ThreeCombo_Cut());
	Root_Sel->Add_Child(BackStep_Cut());
	//Root_Sel->Add_Child(Sting());
	Root_Sel->Add_Child(Flash_Wing_Attack());
	Root_Sel->Add_Child(Flash());
	Root_Sel->Add_Child(Flash());
	Root_Sel->Add_Child(Shield_Attack());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::FarAttack_Fianl()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� ���Ÿ� ����");

	Root_Sel->Add_Child(Flash_Rush());
	Root_Sel->Add_Child(Flash_Jump_Attack());
	Root_Sel->Add_Child(Flash_Cut());
	Root_Sel->Add_Child(Flash());
	Root_Sel->Add_Child(Flash_Middle_Ground());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::Start_Show()
{
	CBT_Selector* Root_Sel = Node_Selector("�ÿ�ȸ");

	CBT_CompareValue* Check_ShowValueN = Node_BOOL_A_Equal_Value("�ÿ�ȸ �ٰŸ� ���� üũ", L"Show_Near", true);

	Root_Sel->Add_Child(Check_ShowValueN);
	Check_ShowValueN->Set_Child(Show_ChaseAndNearAttack());

	Root_Sel->Add_Child(Show_FarAttack());

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::Show_ChaseAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� �� ������� ����");
	CBT_Play_Ani* Show_Ani14 = Node_Ani("�������", 14, 0.05f);
	CBT_MoveDirectly* Chase = Node_MoveDirectly_Chase("����", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 5.f, 3.f);
	CBT_RotationDir* Rotation0 = Node_RotationDir("�÷��̾� �ٶ󺸱�", L"Player_Pos", 0.2);

	Root_Seq->Add_Child(Show_Ani14);
	Root_Seq->Add_Child(Chase);
	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(Show_NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CQueensKnight::Show_NearAttack()
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
	Cool0->Set_Child(Normal_HorizontalCut1());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Normal_VerticalCut1());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(TwoCombo_Cut());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(ThreeCombo_Cut());
	Root_Sel->Add_Child(Cool4);
	Cool4->Set_Child(BackStep_Cut());
	Root_Sel->Add_Child(Cool5);
	Cool5->Set_Child(Sting());
	Root_Sel->Add_Child(Cool6);
	Cool6->Set_Child(Wing_Attack());

	Root_Sel->Add_Child(Show_OffNearAttack);

	return Root_Sel;
}

CBT_Composite_Node * CQueensKnight::Show_FarAttack()
{
	CBT_Selector* Root_Sel = Node_Selector("������� ���Ÿ� ����");

	CBT_Cooldown* Cool0 = Node_Cooldown("��0", 300);
	CBT_Cooldown* Cool1 = Node_Cooldown("��1", 300);
	CBT_Cooldown* Cool2 = Node_Cooldown("��2", 300);
	CBT_Cooldown* Cool3 = Node_Cooldown("��3", 300);
	CBT_Cooldown* Cool4 = Node_Cooldown("��4", 300);

	CBT_SetValue* Show_ValueOff = Node_BOOL_SetValue("�ÿ�ȸ OFF", L"Show", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Rush());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Flash_Wing_Attack());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Flash_Rush());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(Flash_Jump_Attack());
	Root_Sel->Add_Child(Cool4);
	Cool4->Set_Child(Flash_Cut());

	Root_Sel->Add_Child(Show_ValueOff);

	return Root_Sel;
}

void CQueensKnight::Down()
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
		if (m_pMeshCom->Is_Finish_Animation(0.5f))
		{
			m_pMeshCom->Reset_OldIndx();
			m_pMeshCom->SetUp_Animation(Ani_Down_Loop);
		}
	}
}

HRESULT CQueensKnight::Update_Bone_Of_BlackBoard()
{
	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine3_WingB4");
	m_vWing = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Bone_Wing", m_vWing);

	
	return S_OK;
}

HRESULT CQueensKnight::Update_Value_Of_BB()
{
	// 1. �÷��̾� ��ǥ ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_Pos());
	// 2. ü�� ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HP", m_tObjParam.fHp_Cur);
	// 3. ü�� ���� ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"HPRatio", _float(m_tObjParam.fHp_Cur / m_tObjParam.fHp_Max) * 100);


	////////
	// 1. Ʈ���� ������Ʈ
	if (true == m_pAIControllerCom->Get_BoolValue(L"TrailOn"))
	{
		m_pSword->Set_Enable_Trail(true);
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"TrailOn", false);
	}

	if (true == m_pAIControllerCom->Get_BoolValue(L"TrailOff"))
	{
		m_pSword->Set_Enable_Trail(false);
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"TrailOff", false);
	}



	////////

	// ���� �����Ʈ����
	_mat matSelf = m_pTransformCom->Get_WorldMat();
	_v3 vSelfPos = *(_v3*)&matSelf.m[3];
	_v3 vSelfLook = *D3DXVec3Normalize(&_v3(), (_v3*)&matSelf.m[2]);
	_v3 vSelfRight = *D3DXVec3Normalize(&_v3(), (_v3*)&matSelf.m[0]);

	// 1. ������ ���� �÷��̾� 4�� ��ġ.
	_mat matPlayer = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL))->Get_WorldMat();
	_v3 vPlayerPos = *(_v3*)&matPlayer.m[3];
	_v3 vPlayerLook = *D3DXVec3Normalize(&_v3(), (_v3*)&matPlayer.m[2]);
	_v3 vPlayerRight = *D3DXVec3Normalize(&_v3(), (_v3*)&matPlayer.m[0]);

	_float fLength = 3.f;

	switch (CALC::Random_Num(0, 3))
	{
	case 0:
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlashPos", vPlayerPos + fLength * vPlayerLook);
		break;
	case 1:
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlashPos", vPlayerPos - fLength * vPlayerLook);
		break;
	case 2:
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlashPos", vPlayerPos + fLength * vPlayerRight);
		break;
	case 3:
		m_pAIControllerCom->Set_Value_Of_BlackBoard(L"FlashPos", vPlayerPos - fLength * vPlayerRight);
		break;
	default:
		break;
	}

	// 2. ���� ��ǥ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_Pos", vSelfPos);

	// 2-1. ���� ��ǥ + Y ���
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Self_MidPos", vSelfPos + _v3(0, 1.3f, 0.f));

	// 3. Į ��������
	CTransform* pSwordTrans = static_cast<CTransform*>(m_pSword->Get_Component(L"Com_Transform"));
	_mat matSword = pSwordTrans->Get_WorldMat();
	_v3 vSwordPos = _v3(matSword.m[3][0], matSword.m[3][1], matSword.m[3][2]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Sword_BottomPos", vSwordPos);

	// 3-1. Į �� ��
	vSwordPos = _v3(matSword.m[3][0], matSword.m[3][1], matSword.m[3][2]) + _v3(matSword.m[2][0], matSword.m[2][1], matSword.m[2][2]) * 3.75f;
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Sword_TopPos", vSwordPos);

	// 3-2. Į ���
	vSwordPos = _v3(matSword.m[3][0], matSword.m[3][1], matSword.m[3][2]) + _v3(matSword.m[2][0], matSword.m[2][1], matSword.m[2][2]) * 2.35f;
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Sword_MidPos", vSwordPos);

	// 4. ����
	CTransform* pShieldTrans = static_cast<CTransform*>(m_pShield->Get_Component(L"Com_Transform"));
	_mat matShield = pShieldTrans->Get_WorldMat();
	_v3 vShieldPos = _v3(matShield.m[3][0], matShield.m[3][1], matShield.m[3][2]);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"ShieldPos", vShieldPos);

	// 5. �߾ӿ��� ������⸦ ���� �ʵ� �߾� ��ǥ
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"Field_MidPos", _v3(0.f, 0.f, 0.f));

	// 5.1 ������� ����Ѿ� ����
	_v3 vDir0 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(45)));
	_v3 vDir1 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(90)));
	_v3 vDir2 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(135)));
	_v3 vDir3 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(180)));
	_v3 vDir4 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(225)));
	_v3 vDir5 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(270)));
	_v3 vDir6 = *D3DXVec3TransformNormal(&_v3(), &vSelfLook, D3DXMatrixRotationY(&_mat(), D3DXToRadian(315)));

	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir0", vSelfLook);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir1", vDir0);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir2", vDir1);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir3", vDir2);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir4", vDir3);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir5", vDir4);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir6", vDir5);
	m_pAIControllerCom->Set_Value_Of_BlackBoard(L"DarkBulletDir7", vDir6);



	return S_OK;
}

HRESULT CQueensKnight::Update_NF()
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

HRESULT CQueensKnight::Update_Collider()
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
	m_vecAttackCol[0]->Update(m_vWing);

	m_pCollider->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pCollider->Get_Radius().y, 0.f));
	return S_OK;
}

void CQueensKnight::Check_PhyCollider()
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
		
			//cout << "QueensKnight - Check_PhyCollider : " << m_tObjParam.fHp_Cur << endl;

		}
		else
		{
			m_pMeshCom->SetUp_Animation(Ani_Death);	// ����ó�� ����
			Start_Dissolve(				0.4f, false, true	, 2.7f);
			m_pSword->Start_Dissolve(	0.4f, false, false	, 2.7f);
			m_pShield->Start_Dissolve(	0.4f, false, false	, 2.7f);
			g_pManagement->Create_Effect_Delay(L"Boss_Dead_Particle"					, 2.5f, _v3(0.f, 1.3f, 0.f), m_pTransformCom);
			g_pManagement->Create_ParticleEffect_Delay(L"SpawnParticle_ForBoss"	, 2.5f	,  3.f, m_pTransformCom->Get_Pos() + _v3(0.f, 0.5f, 0.f));
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

void CQueensKnight::Push_Collider()
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

void CQueensKnight::OnCollisionEnter()
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

void CQueensKnight::OnCollisionEvent(list<CGameObject*> plistGameObject)
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

HRESULT CQueensKnight::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_QueensKnight", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
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

HRESULT CQueensKnight::SetUp_ConstantTable()
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

HRESULT CQueensKnight::Ready_Weapon()
{
	// ������ ����
	m_pSword = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pSword->Change_WeaponData(CWeapon::WPN_QueenLance);

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pSword->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pSword->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	// Trail
	m_pSword->Set_Enable_Trail(true);
	m_pSword->Set_TrailIndex(7);
	m_pSword->Set_TrailUseMask(0);

	// �޼� ����
	m_pShield = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pShield->Change_WeaponData(CWeapon::WPN_QueenShield);

	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHandAttach");
	m_pShield->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pShield->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	return S_OK;
}

HRESULT CQueensKnight::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

HRESULT CQueensKnight::Ready_Collider()
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

	//////////// ���ݿ� �ݶ��̴�
	m_vecAttackCol.reserve(5);

	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 1.f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(m_vWing);
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CQueensKnight::Ready_NF(void * pArg)
{
	INFO eTemp = *(INFO*)pArg;

	m_fFov = eTemp.fFov;
	m_fMaxLength = eTemp.fMaxLength;
	m_fMinLength = eTemp.fMinLength;

	return S_OK;
}

CQueensKnight * CQueensKnight::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CQueensKnight* pInstance = new CQueensKnight(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CQueensKnight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CQueensKnight::Clone_GameObject(void * pArg)
{
	CQueensKnight* pInstance = new CQueensKnight(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CQueensKnight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQueensKnight::Free()
{
	Safe_Release(m_pShield);
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


