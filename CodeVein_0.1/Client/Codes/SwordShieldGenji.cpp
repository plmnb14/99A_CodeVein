#include "stdafx.h"
#include "..\Headers\SwordShieldGenji.h"

CSwordShieldGenji::CSwordShieldGenji(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CSwordShieldGenji::CSwordShieldGenji(const CSwordShieldGenji & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSwordShieldGenji::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CSwordShieldGenji::Ready_GameObject(void * pArg)
{

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(3.f, 0.f, 3.f));
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));


	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create(true);

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", 100);
	pBlackBoard->Set_Value(L"MAXHP", 100);
	pBlackBoard->Set_Value(L"HPRatio", 100);
	pBlackBoard->Set_Value(L"Show", true);

	//CBT_Selector* Start_Sel = Node_Selector("�ൿ ����"); // ��
	CBT_Sequence* Start_Sel = Node_Sequence("�ൿ ����"); // �׽�Ʈ
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


	// �����ֱ��
	/*Start_Sel->Add_Child(ShortDelay_Sting());

	CBT_RotationDir* TurnDir0 = Node_RotationDir("Look ȸ��", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir0);

	Start_Sel->Add_Child(Upper_Slash());

	CBT_RotationDir* TurnDir1 = Node_RotationDir("Look ȸ��", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir1);

	Start_Sel->Add_Child(LongDelay_Sting());

	CBT_RotationDir* TurnDir2 = Node_RotationDir("Look ȸ��", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir2);

	Start_Sel->Add_Child(Shield_Attack());

	CBT_RotationDir* TurnDir3 = Node_RotationDir("Look ȸ��", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir3);

	Start_Sel->Add_Child(Turning_Cut());

	CBT_RotationDir* TurnDir4 = Node_RotationDir("Look ȸ��", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir4);

	Start_Sel->Add_Child(ShortDelay_Sting());

	CBT_RotationDir* TurnDir5 = Node_RotationDir("Look ȸ��", L"Player_Pos", 0.15);
	Start_Sel->Add_Child(TurnDir5);

	Start_Sel->Add_Child(Guard(3));*/

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



	return NOERROR;
}

_int CSwordShieldGenji::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	m_pAIControllerCom->Update_AIController(TimeDelta);

	return _int();
}

_int CSwordShieldGenji::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

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
			m_pShaderCom->Begin_Pass(0);

			if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE))))
				return E_FAIL;

			m_pShaderCom->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			m_pShaderCom->End_Pass();
		}
	}

	m_pShaderCom->End_Shader();

	return NOERROR;
}

CBT_Composite_Node * CSwordShieldGenji::Upper_Slash()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");	
	CBT_Sequence* MainSeq = Node_Sequence("���۽�����");
	CBT_Play_Ani* Show_Ani25 = Node_Ani("���۽�����", 25, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.1f);

	//2.5�� ���� �� ����;
	CBT_Sequence* SubSeq = Node_Sequence("����");
	CBT_Wait* Wait0 = Node_Wait("���", 1.1, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("����", 20, 0.2, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani25);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::LongDelay_Sting()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("�� ������ ���");
	CBT_Sequence* MainSeq = Node_Sequence("�� ������ ���");
	CBT_Play_Ani* Show_Ani24 = Node_Ani("��� ���", 24, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.5, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", 2, 0.5, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani24);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Shield_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����ġ��");
	CBT_Sequence* MainSeq = Node_Sequence("����ġ��");
	CBT_Play_Ani* Show_Ani23 = Node_Ani("����ġ��", 23, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.5, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", 1, 0.5, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani23);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Turning_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("�ѹ��� ���鼭 ����");
	CBT_Sequence* MainSeq = Node_Sequence("�ѹ��� ���鼭 ����");
	CBT_Play_Ani* Show_Ani22 = Node_Ani("�ѹ��� ���鼭 ����", 22, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 1.0, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", 4, 0.5, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani22);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::ShortDelay_Sting()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("ª�� ������ ���");
	CBT_Sequence* MainSeq = Node_Sequence("ª�� ������ ���");
	CBT_Play_Ani* Show_Ani21 = Node_Ani("���", 21, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.5, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", 2, 0.3, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani21);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Hit_Weakly()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���е�� ���ϰ� ����");

	CBT_Play_Ani* Show_Ani2 = Node_Ani("���е�� ���ϰ� ����", 2, 0.95f);

	Root_Seq->Add_Child(Show_Ani2);

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::Hit_Strongly()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���е�� ���ϰ� ����");

	CBT_Play_Ani* Show_Ani1 = Node_Ani("���е�� ���ϰ� ����", 1, 0.95f);

	Root_Seq->Add_Child(Show_Ani1);

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::Guard(_double dGuardTime)
{
	//345
	CBT_Sequence* Root_Seq = Node_Sequence("���е��");

	CBT_Play_Ani* Show_Ani3 = Node_Ani("����", 3, 0.8f);
	CBT_Play_Ani* Show_Ani4 = Node_Ani("����", 4, 0.95f);
	CBT_Wait* Wait0 = Node_Wait("���� ���", dGuardTime, 0);
	CBT_Play_Ani* Show_Ani5 = Node_Ani("��", 5, 0.8f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 1.f);
	
	Root_Seq->Add_Child(Show_Ani3);
	Root_Seq->Add_Child(Show_Ani4);
	Root_Seq->Add_Child(Wait0);
	Root_Seq->Add_Child(Show_Ani5);
	Root_Seq->Add_Child(Show_Ani42);

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::Start_Game()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� ����");

	Root_Seq->Add_Child(ChaseAndNearAttack());
	//Root_Seq->Add_Child(TurnAndFarAttack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::ChaseAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� ���� �Ǵ� ����");

	Root_Seq->Add_Child(Chase());
	Root_Seq->Add_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::Chase()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_MoveDirectly* pChase = Node_MoveDirectly_Chase("����", L"Player_Pos", 3.f, 2.f);

	CBT_Play_Ani* Show_Ani139 = Node_Ani("����", 139, 1.f);

	Root_Parallel->Set_Main_Child(pChase);

	Root_Parallel->Set_Sub_Child(Show_Ani139);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� �ٰŸ� ����");

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
	return E_NOTIMPL;
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

	Safe_Release(pManagement);

	return NOERROR;
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
	// ��, ��, ����
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
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
