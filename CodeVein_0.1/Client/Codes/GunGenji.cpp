#include "stdafx.h"
#include "..\Headers\GunGenji.h"
#include "..\Headers\Weapon.h"

CGunGenji::CGunGenji(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CGunGenji::CGunGenji(const CGunGenji & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGunGenji::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CGunGenji::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	//m_pNavMesh->Ready_NaviMesh(m_pGraphic_Dev, L"Navmesh_StageBase.dat");
	//m_pNavMesh->Set_SubsetIndex(0);

	Ready_Weapon();
	Ready_BoneMatrix();
	Ready_Collider();

	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 3.f;

	m_pTransformCom->Set_Pos(_v3(-3.f, 0.f, -3.f));
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));


	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create(false);

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", 100);
	pBlackBoard->Set_Value(L"MAXHP", 100);
	pBlackBoard->Set_Value(L"HPRatio", 100);
	pBlackBoard->Set_Value(L"Show", true);

	CBT_Selector* Start_Sel = Node_Selector("�ൿ ����");
	//CBT_Sequence* Start_Sel = Node_Sequence("�ൿ ����"); // �׽�Ʈ

	CBT_UpdatePos* UpdatePlayerPosService = Node_UpdatePos("Update_Player_Pos", L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE)), 0, 0.01, 0, CBT_Service_Node::Infinite);
	CBT_UpdateGageRatio* UpdatePlayerHPservice = Node_UpdateGageRatio("Update_Player_Pos", L"HPRatio", L"MaxHP", L"HP", 0, 0.01, 0, CBT_Service_Node::Infinite);
	CBT_UpdateGageRatio* UpdateHPRatioService = Node_UpdateGageRatio("ü�� ����", L"HPRatio", L"MAXHP", L"HP", 1, 0.01, 0, CBT_Service_Node::Infinite);

	pBehaviorTree->Set_Child(Start_Sel);

	Start_Sel->Add_Service(UpdatePlayerPosService);
	Start_Sel->Add_Service(UpdatePlayerHPservice);
	Start_Sel->Add_Service(UpdateHPRatioService);

	//CBT_CompareValue* Check_ShowValue = Node_BOOL_A_Equal_Value("�ÿ�ȸ ���� üũ", L"Show", true);
	//Check_ShowValue->Set_Child(Start_Show());
	//Start_Sel->Add_Child(Check_ShowValue);
	Start_Sel->Add_Child(Start_Game());


	//Start_Sel->Add_Child(Dodge_B());


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


	////���� ������
	//CBT_Play_Ani* pAni0 = Node_Ani("���� ������ ", 0, 0.9f);
	//pSequence->Add_Child(pAni0);

	////���� Ǯ��
	//CBT_Play_Ani* pAni1 = Node_Ani("���� Ǯ�� ", 1, 0.9f);
	//pSequence->Add_Child(pAni1);

	////���� ƽƽ
	//CBT_Play_Ani* pAni2 = Node_Ani("���� ƽƽ ", 2, 0.9f);
	//pSequence->Add_Child(pAni2);

	////���� ����
	//CBT_Play_Ani* pAni3 = Node_Ani("���� ���� ", 3, 0.9f);
	//pSequence->Add_Child(pAni3);

	////���� ����
	//CBT_Play_Ani* pAni4 = Node_Ani("���� ���� ", 4, 0.9f);
	//pSequence->Add_Child(pAni4);

	////���� ��
	//CBT_Play_Ani* pAni5 = Node_Ani("���� �� ", 5, 0.9f);
	//pSequence->Add_Child(pAni5);

	////���� �޸���
	//CBT_Play_Ani* pAni6 = Node_Ani("���� �޸��� ", 6, 0.9f);
	//pSequence->Add_Child(pAni6);

	////���� ������ �ȱ�
	//CBT_Play_Ani* pAni7 = Node_Ani("���� �ڷ� �ȱ� ", 7, 0.9f);
	//pSequence->Add_Child(pAni7);

	////���� ���� �ȱ�
	//CBT_Play_Ani* pAni8 = Node_Ani("���� ���� �ȱ� ", 8, 0.9f);
	//pSequence->Add_Child(pAni8);

	////���� �տ����U �ȱ�
	//CBT_Play_Ani* pAni9 = Node_Ani("���� �տ����U �ȱ� ", 9, 0.9f);
	//pSequence->Add_Child(pAni9);

	////���� �տ��ʿ����� �ȱ�
	//CBT_Play_Ani* pAni10 = Node_Ani("���� �տ��ʿ����� �ȱ� ", 10, 0.9f);
	//pSequence->Add_Child(pAni10);

	////���� �տ��ʿ޹� �ȱ�
	//CBT_Play_Ani* pAni11 = Node_Ani("���� �տ��ʿ޹� �ȱ� ", 11, 0.9f);
	//pSequence->Add_Child(pAni11);

	////���� ������ �ȱ�
	//CBT_Play_Ani* pAni12 = Node_Ani("���� ������ �ȱ� ", 12, 0.9f);
	//pSequence->Add_Child(pAni12);

	////���� �ڿ����ʿ����� �ȱ�
	//CBT_Play_Ani* pAni13 = Node_Ani("���� �ڿ����ʿ����� �ȱ� ", 13, 0.9f);
	//pSequence->Add_Child(pAni13);

	////���� �� �����ʿ޹� �ȱ�
	//CBT_Play_Ani* pAni14 = Node_Ani("���� �� �����ʿ޹� �ȱ� ", 14, 0.9f);
	//pSequence->Add_Child(pAni14);

	////���� �� ���� �ȱ�
	//CBT_Play_Ani* pAni15 = Node_Ani("���� �� ���� �ȱ� ", 15, 0.9f);
	//pSequence->Add_Child(pAni15);

	////���� �� �ȱ�
	//CBT_Play_Ani* pAni16 = Node_Ani("���� �� �ȱ� ", 16, 0.9f);
	//pSequence->Add_Child(pAni16);

	////���� ������180��
	//CBT_Play_Ani* pAni17 = Node_Ani("���� ������180�� ", 17, 0.9f);
	//pSequence->Add_Child(pAni17);

	////���� ������ 90��
	//CBT_Play_Ani* pAni18 = Node_Ani("���� ������ 90�� ", 18, 0.9f);
	//pSequence->Add_Child(pAni18);

	////���� ���� 180��
	//CBT_Play_Ani* pAni19 = Node_Ani("���� ���� 180�� ", 19, 0.9f);
	//pSequence->Add_Child(pAni19);

	////���� ���� 90��
	//CBT_Play_Ani* pAni20 = Node_Ani("���� ���� 90�� ", 20, 0.9f);
	//pSequence->Add_Child(pAni20);

	////���� ������
	//CBT_Play_Ani* pAni21 = Node_Ani("���� ������ ", 21, 0.9f);
	//pSequence->Add_Child(pAni21);

	////���е�� �ֵ���ġ��
	//CBT_Play_Ani* pAni22 = Node_Ani("���е�� �ֵ���ġ�� ", 22, 0.9f);
	//pSequence->Add_Child(pAni22);

	////���з� �ķ�ġ��
	//CBT_Play_Ani* pAni23 = Node_Ani("���з� �ķ�ġ�� ", 23, 0.9f);
	//pSequence->Add_Child(pAni23);

	////���е�� ������
	//CBT_Play_Ani* pAni24 = Node_Ani("���е�� ������ ", 24, 0.9f);
	//pSequence->Add_Child(pAni24);

	////���з� ġ�� ����ġ��
	//CBT_Play_Ani* pAni25 = Node_Ani("���з� ġ�� ����ġ�� ", 25, 0.9f);
	//pSequence->Add_Child(pAni25);

	////�ָӴϿ��� ǥâ
	//CBT_Play_Ani* pAni26 = Node_Ani("�ָӴϿ��� ǥâ ", 26, 0.9f);
	//pSequence->Add_Child(pAni26);

	////��� ǥâ��������
	//CBT_Play_Ani* pAni27 = Node_Ani("��� ǥâ �������� ", 27, 0.9f);
	//pSequence->Add_Child(pAni27);

	////�缱�ķ�����
	//CBT_Play_Ani* pAni28 = Node_Ani("�缱 �ķ����� ", 28, 0.9f);
	//pSequence->Add_Child(pAni28);

	////ũ�� ����������
	//CBT_Play_Ani* pAni29 = Node_Ani("ũ�� ������ ����", 29, 0.9f);
	//pSequence->Add_Child(pAni29);

	////�״Ͻ� �佺 �ι�����
	//CBT_Play_Ani* pAni30 = Node_Ani("�״Ͻ� �佺 �ι����� ", 30, 0.9f);
	//pSequence->Add_Child(pAni30);

	////���� �������ô�ϸ鼭 ���
	//CBT_Play_Ani* pAni31 = Node_Ani("���� �������ô �ϸ鼭 ��� ", 31, 0.9f);
	//pSequence->Add_Child(pAni31);

	////�ɾƼ� ���ʿ����� ����
	//CBT_Play_Ani* pAni32 = Node_Ani("�ɾƼ� ���ʿ��������� ", 32, 0.9f);
	//pSequence->Add_Child(pAni32);

	////�齺���ϸ� ǥâ
	//CBT_Play_Ani* pAni33 = Node_Ani("�齺���ϸ� ǥâ ", 33, 0.9f);
	//pSequence->Add_Child(pAni33);

	////�޷��� ������ ����
	//CBT_Play_Ani* pAni34 = Node_Ani("�޷��� ������ ���� ", 34, 0.9f);
	//pSequence->Add_Child(pAni34);

	////�޷��� ������ ����
	//CBT_Play_Ani* pAni35 = Node_Ani("�޷��� ������ ���� ", 35, 0.9f);
	//pSequence->Add_Child(pAni35);

	////�޷��� ������ ��
	//CBT_Play_Ani* pAni36 = Node_Ani("�޷��� ������ �� ", 36, 0.9f);
	//pSequence->Add_Child(pAni36);

	////�״Ͻ� �佺 ��������
	//CBT_Play_Ani* pAni37 = Node_Ani("�״Ͻ� �佺 �������� ", 37, 0.9f);
	//pSequence->Add_Child(pAni37);

	////��û�Ÿ��� ũ�� ����
	//CBT_Play_Ani* pAni38 = Node_Ani("��û�Ÿ��� ũ�� ���� ", 38, 0.9f);
	//pSequence->Add_Child(pAni38);

	////�ճ�ġ����
	//CBT_Play_Ani* pAni39 = Node_Ani("�ճ�ġ���� ", 39, 0.9f);
	//pSequence->Add_Child(pAni39);

	////�յ����� ��ġ����
	//CBT_Play_Ani* pAni40 = Node_Ani("�յ����� ��ġ���� ", 40, 0.9f);
	//pSequence->Add_Child(pAni40);

	////�̿�� �ܹ��е���
	//CBT_Play_Ani* pAni41 = Node_Ani("�̿�� �ܹ��е���", 41, 0.9f);
	//pSequence->Add_Child(pAni41);

	////�� �⺻
	//CBT_Play_Ani* pAni42 = Node_Ani("�� �⺻", 42, 0.9f);
	//pSequence->Add_Child(pAni42);

	////���Ӹ��� ġ��
	//CBT_Play_Ani* pAni43 = Node_Ani("���Ӹ��� ġ�� ", 43, 0.9f);
	//pSequence->Add_Child(pAni43);

	////������
	//CBT_Play_Ani* pAni44 = Node_Ani("������ ", 44, 0.9f);
	//pSequence->Add_Child(pAni44);

	////�Ȳ�ġ ġ��
	//CBT_Play_Ani* pAni45 = Node_Ani("�Ȳ�ġ ġ�� ", 45, 0.9f);
	//pSequence->Add_Child(pAni45);

	////�Ѽ� �� ���
	//CBT_Play_Ani* pAni46 = Node_Ani("�Ѽ� �� ��� ", 46, 0.9f);
	//pSequence->Add_Child(pAni46);

	////�� ���ʺ��� ����
	//CBT_Play_Ani* pAni47 = Node_Ani("�� ���ʺ��� ���� ", 47, 0.9f);
	//pSequence->Add_Child(pAni47);

	////������� 2ȸ ���
	//CBT_Play_Ani* pAni48 = Node_Ani("������� 2ȸ ��� ", 48, 0.9f);
	//pSequence->Add_Child(pAni48);

	////�� ���
	//CBT_Play_Ani* pAni49 = Node_Ani("�� ��� ", 49, 0.9f);
	//pSequence->Add_Child(pAni49);


	m_pMeshCom->SetUp_Animation(Ani_Idle);

	return NOERROR;
}

_int CGunGenji::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	// �׾��� ���
	if (m_bIsDead)
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			return DEAD_OBJ;
		}
	}
	else
	{
		if (true == m_bAIController)
			m_pAIControllerCom->Update_AIController(TimeDelta);

		Check_Collider();
	}

	// �׺�޽� �¿��
	if (-1 == m_pNavMesh->Get_CellIndex())
	{
		// ������ ���
	}
	else
		m_pTransformCom->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransformCom->Get_Pos()));

	return _int();
}

_int CGunGenji::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	m_pGun->Late_Update_GameObject(TimeDelta);

	return _int();
}

HRESULT CGunGenji::Render_GameObject()
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

	m_pGun->Update_GameObject(m_dTimeDelta);
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}

CBT_Composite_Node * CGunGenji::Shot()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("�Ϲ� �ѽ��");
	CBT_Sequence* MainSeq = Node_Sequence("�Ϲ� �ѽ��");
	CBT_Play_Ani* Show_Ani44 = Node_Ani("�Ϲ� �ѽ��", 44, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_ChaseDir* FixDir0 = Node_ChaseDir("���� ����", L"Player_Pos", 1, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ������", L"Player_Pos", 0.2);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani44);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(FixDir0);
	SubSeq->Add_Child(Rotation0);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::Tumbling_Shot()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("�Һ� �ѽ��");
	CBT_Sequence* MainSeq = Node_Sequence("�Һ� �ѽ��");
	CBT_Play_Ani* Show_Ani48 = Node_Ani("�Һ� �ѽ��", 48, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.3, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", -1, 1.3, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani48);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::Sudden_Shot()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���ڱ� �ѽ��");

	CBT_Play_Ani* Show_Ani49 = Node_Ani("���ڱ� �ѽ��", 49, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.3f);

	Root_Seq->Add_Child(Show_Ani49);
	Root_Seq->Add_Child(Show_Ani42);

	return Root_Seq;
}

CBT_Composite_Node * CGunGenji::Upper_Slash()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");
	CBT_Sequence* MainSeq = Node_Sequence("���Ӹ��� ġ��");
	CBT_Play_Ani* Show_Ani43 = Node_Ani("���Ӹ��� ġ��", 43, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.3f);

	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", 2, 0.8, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani43);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::Arm_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");
	CBT_Sequence* MainSeq = Node_Sequence("�ȱ�ġ ġ��");
	CBT_Play_Ani* Show_Ani45 = Node_Ani("�ȱ�ġ ġ��", 45, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.7, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", 2, 0.5, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani45);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::Sting_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");
	CBT_Sequence* MainSeq = Node_Sequence("���");
	CBT_Play_Ani* Show_Ani46 = Node_Ani("���", 46, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.4, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", 1, 0.3, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani46);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::Cut_To_Right()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");
	CBT_Sequence* MainSeq = Node_Sequence("���������� ����");
	CBT_Play_Ani* Show_Ani47 = Node_Ani("���������� ����", 47, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.4, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", 1, 0.5, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani47);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::Dodge_B()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");
	CBT_Sequence* MainSeq = Node_Sequence("�ڷ� ȸ��");
	CBT_Play_Ani* Show_Ani162 = Node_Ani("�ڷ� ȸ��", 162, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.4, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", -10, 0.1, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani162);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::Start_Game()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� ����");



	//Root_Seq->Add_Child(NearAttack());
	Root_Seq->Add_Child(Dist_Attack());

	return Root_Seq;
}

CBT_Composite_Node * CGunGenji::Dist_Attack()
{
	CBT_Selector* Root_Sel = Node_Selector("�ٰŸ� ���Ÿ� ���� ����");

	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 3);
	CBT_DistCheck* Dist1 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 6);

	// �Ĵٺ��Ⱑ ����,  �״��� �Ÿ�üũ�� ����

	//�Ÿ��� �Ǵ��ϰ� ����, selector

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

	CBT_MoveDirectly* pChase = Node_MoveDirectly_Chase("����", L"Player_Pos", 3.f, 5.f);

	CBT_Play_Ani* Show_Ani139 = Node_Ani("����", 139, 1.f);

	Root_Parallel->Set_Main_Child(pChase);

	Root_Parallel->Set_Sub_Child(Show_Ani139);

	return Root_Parallel;
}

CBT_Composite_Node * CGunGenji::NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� �ٰŸ� ����");

	Root_Sel->Add_Child(Upper_Slash());
	Root_Sel->Add_Child(Arm_Attack());
	Root_Sel->Add_Child(Sting_Attack());
	Root_Sel->Add_Child(Cut_To_Right());
	Root_Sel->Add_Child(Tumbling_Shot());

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
	CBT_MoveDirectly* Chase = Node_MoveDirectly_Chase("����", L"Player_Pos", 3.f, 2.f);

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
	return E_NOTIMPL;
}

HRESULT CGunGenji::Update_Value_Of_BB()
{
	return E_NOTIMPL;
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

	return S_OK;
}

void CGunGenji::Check_Collider()
{
	// �浹ó��, bCanHit�� ���Ⱑ false�����ٰ���.
	if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		m_pMeshCom->Reset_OldIndx();	//�ִ� �ε��� �ʱ�ȭ

		m_tObjParam.fHp_Cur -= 0.99f;	// ü�� ���Ƿ� ��� ����.

		m_bAIController = false;
		cout << "���� �ε��� ^^" << endl;
		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		m_pAIControllerCom->Reset_BT();

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			m_pMeshCom->SetUp_Animation(Ani_Dmg01_FL);	//���⿡ ���� ��� �������.
		}
		else
		{
			m_pMeshCom->SetUp_Animation(Ani_Death);	// ����ó�� ����
			m_bIsDead = true;
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

		else if (m_pMeshCom->Is_Finish_Animation(0.7f))	// �̶����� ���浹 ����
		{
			m_tObjParam.bIsHit = false;
		}
	}
}

HRESULT CGunGenji::Draw_Collider()
{
	for (auto& iter : m_vecPhysicCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	return S_OK;
}

HRESULT CGunGenji::Add_Component(void* pArg)
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

	_tchar name[256] = { 0, };
	INFO eTemp = *(INFO*)pArg;

	if (nullptr == pArg)
		lstrcpy(name, L"Mesh_NormalGenji");
	else
	{
		switch (eTemp.eType)
		{
		case CGunGenji::Jungle:
			lstrcpy(name, L"Mesh_JungleGenji");
			break;
		case CGunGenji::Normal:
			lstrcpy(name, L"Mesh_NormalGenji");
			break;
		case CGunGenji::White:
			lstrcpy(name, L"Mesh_WhiteGenji");
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;


	return NOERROR;
}

HRESULT CGunGenji::SetUp_ConstantTable()
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

HRESULT CGunGenji::Ready_Weapon()
{
	m_pGun = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pGun->Change_WeaponData(CWeapon::WPN_Gun_Normal);

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
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pGun);
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
