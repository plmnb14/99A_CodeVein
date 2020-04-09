#include "stdafx.h"
#include "..\Headers\SwordShieldGenji.h"
#include "..\Headers\Weapon.h"

#include "MonsterUI.h"
#include "DamegeNumUI.h"

CSwordShieldGenji::CSwordShieldGenji(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	ZeroMemory(m_matBones, sizeof(_mat*) * Bone_End);
}

CSwordShieldGenji::CSwordShieldGenji(const CSwordShieldGenji & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(m_matBones, sizeof(_mat*) * Bone_End);
}

HRESULT CSwordShieldGenji::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CSwordShieldGenji::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	Ready_NF(pArg);
	Ready_Weapon();
	Ready_BoneMatrix();
	Ready_Collider();

	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 100.f;
	m_tObjParam.fHp_Max = 100.f;

	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));


	//// MonsterHP UI
	m_pMonsterUI = static_cast<CMonsterUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MonsterHPUI", pArg));
	m_pMonsterUI->Set_Target(this);
	m_pMonsterUI->Set_Bonmatrix(m_matBones[Bone_Head]);
	m_pMonsterUI->Ready_GameObject(NULL);

	m_pDamegeNumUI = static_cast<CDamegeNumUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_DamegeNumUI", pArg));
	m_pDamegeNumUI->Set_Target(this);
	m_pDamegeNumUI->Ready_GameObject(pArg);

	/////////////// �ൿƮ�� init

	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create(false);

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", m_tObjParam.fHp_Cur);
	pBlackBoard->Set_Value(L"MAXHP", m_tObjParam.fHp_Max);
	pBlackBoard->Set_Value(L"HPRatio", 100);
	pBlackBoard->Set_Value(L"Show", true);
	// Monster_Speed -> delta ���ؼ� ����������, Monster_Dir

	//CBT_Selector* Start_Sel = Node_Selector("�ൿ ����"); // ��
	CBT_Sequence* Start_Sel = Node_Sequence("�ൿ ����"); // �׽�Ʈ
	CBT_UpdateGageRatio* UpdateHPRatioService = Node_UpdateGageRatio("ü�� ����", L"HPRatio", L"MAXHP", L"HP", 1, 0.01, 0, CBT_Service_Node::Infinite);

	pBehaviorTree->Set_Child(Start_Sel);

	Start_Sel->Add_Service(UpdateHPRatioService);

	//CBT_CompareValue* Check_ShowValue = Node_BOOL_A_Equal_Value("�ÿ�ȸ ���� üũ", L"Show", true);
	//Check_ShowValue->Set_Child(Start_Show());
	//Start_Sel->Add_Child(Check_ShowValue);
	Start_Sel->Add_Child(Start_Game());

	//Start_Sel->Add_Child(ShortDelay_Sting());

	//CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.2);
	//Start_Sel->Add_Child(Rotation0);

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
	if (false == m_bEnable)
		return NO_EVENT;

	Push_Collider();

	CGameObject::Update_GameObject(TimeDelta);

	// �׾��� ���
	if (m_bIsDead)
		return DEAD_OBJ;

	// MonsterHP UI
	m_pMonsterUI->Update_GameObject(TimeDelta);
	m_pDamegeNumUI->Update_GameObject(TimeDelta);

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

	Check_PhyCollider();

	return _int();
}

_int CSwordShieldGenji::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	m_pSword->Late_Update_GameObject(TimeDelta);
	m_pShield->Late_Update_GameObject(TimeDelta);

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
			m_pShaderCom->Begin_Pass(m_iPass);

			if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE_MAP))))
				return E_FAIL;

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

CBT_Composite_Node * CSwordShieldGenji::Upper_Slash()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");
	CBT_Sequence* MainSeq = Node_Sequence("���۽�����");
	CBT_Play_Ani* Show_Ani25 = Node_Ani("���۽�����", 25, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.25, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 3.f, 0.167, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani25);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.417, 1, 0.116, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::LongDelay_Sting()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("�� ������ ���");
	CBT_Sequence* MainSeq = Node_Sequence("�� ������ ���");
	CBT_Play_Ani* Show_Ani24 = Node_Ani("��� ���", 24, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.25, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 0.7f, 0.817, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani24);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.133, 1, 0.117, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Shield_Attack()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����ġ��");
	CBT_Sequence* MainSeq = Node_Sequence("����ġ��");
	CBT_Play_Ani* Show_Ani23 = Node_Ani("����ġ��", 23, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.233, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 1, 0.717, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani23);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pShield->Get_pTarget_Param(), CBT_UpdateParam::Collider, 0.933, 1, 0.134, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Turning_Cut()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("�ѹ��� ���鼭 ����");
	CBT_Sequence* MainSeq = Node_Sequence("�ѹ��� ���鼭 ����");
	CBT_Play_Ani* Show_Ani22 = Node_Ani("�ѹ��� ���鼭 ����", 22, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.433, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 2, 1.317, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani22);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.183, 1, 0.134, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::ShortDelay_Sting()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("ª�� ������ ���");
	CBT_Sequence* MainSeq = Node_Sequence("ª�� ������ ���");
	CBT_Play_Ani* Show_Ani21 = Node_Ani("���", 21, 0.95f);
	CBT_Play_Ani* Show_Ani42 = Node_Ani("�⺻", 42, 0.1f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 0.2, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 1, 0.663, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani21);
	MainSeq->Add_Child(Show_Ani42);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", m_pSword->Get_pTarget_Param(), CBT_UpdateParam::Collider, 1.05, 1, 0.133, 0);
	Root_Parallel->Add_Service(pHitCol);

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
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("���е��");
	CBT_Sequence* MainSeq = Node_Sequence("���е��");
	CBT_Play_Ani* Show_Ani3 = Node_Ani("����", 3, 0.8f);
	CBT_Play_Ani* Show_Ani4 = Node_Ani("����", 4, 0.95f);
	CBT_Wait* Wait0 = Node_Wait("���� ���", dGuardTime, 0);
	CBT_Play_Ani* Show_Ani5 = Node_Ani("��", 5, 0.8f);

	CBT_ChaseDir* RotationDir0 = Node_ChaseDir("�̵�", L"Player_Pos", dGuardTime + 1, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani3);
	MainSeq->Add_Child(Show_Ani4);
	MainSeq->Add_Child(Wait0);
	MainSeq->Add_Child(Show_Ani5);

	Root_Parallel->Set_Sub_Child(RotationDir0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Start_Game()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� ����");

	Root_Seq->Add_Child(Dist_Attack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::Dist_Attack()
{
	CBT_Selector* Root_Sel = Node_Selector("�Ÿ� �Ǵ�");

	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 3);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(MoveAround_NearAttack());

	Root_Sel->Add_Child(Chase());

	return Root_Sel;
}

CBT_Composite_Node * CSwordShieldGenji::Guard_LeftMoveAround()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("���� ���� ����");

	CBT_MoveAround*	MoveAround0 = Node_MoveAround("�������� �̵�", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 0.7f, 2, 0.4);

	CBT_Play_Ani* Show_Ani = Node_Ani("�������� �̵�", 8, 0.95f);

	Root_Parallel->Set_Main_Child(MoveAround0);
	Root_Parallel->Set_Sub_Child(Show_Ani);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Guard_RightMoveAround()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("������ ���� ����");

	CBT_MoveAround*	MoveAround0 = Node_MoveAround("���������� �̵�", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", -0.7f, 2, 0.4);

	CBT_Play_Ani* Show_Ani = Node_Ani("���������� �̵�", 7, 0.95f);

	Root_Parallel->Set_Main_Child(MoveAround0);
	Root_Parallel->Set_Sub_Child(Show_Ani);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::RotationAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� ���� ���� ����");
	CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.1);

	Root_Seq->Add_Child(Rotation0);
	Root_Seq->Add_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::Chase_Guard_NearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("����_����_��������");

	Root_Seq->Add_Child(Chase());
	Root_Seq->Add_Child(Guard(2));
	Root_Seq->Add_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::MoveAround_NearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("�����̰� ����");
	CBT_RotationDir* RotationDir0 = Node_RotationDir("�÷��̾� �ٶ󺸱�", L"Player_Pos", 0.2);
	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 3.f);

	Root_Seq->Add_Child(Guard_MoveAround());
	Root_Seq->Add_Child(RotationDir0);
	Root_Seq->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CSwordShieldGenji::Guard_MoveAround()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("�����̱�");

	CBT_Sequence* MainSeq = Node_Sequence("����");
	MainSeq->Add_Child(Guard_LeftMoveAround());
	MainSeq->Add_Child(Guard_RightMoveAround());

	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("�÷��̾� ��������", L"Player_Pos", 5, 0);

	Root_Parallel->Set_Main_Child(MainSeq);
	Root_Parallel->Set_Sub_Child(ChaseDir0);

	return Root_Parallel;
}

CBT_Composite_Node * CSwordShieldGenji::Chase()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_MoveDirectly* pChase = Node_MoveDirectly_Chase("����", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 3.f, 3.f);

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
	// 1. �÷��̾� ��ǥ ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos());
	// 2. ü�� ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"HP", m_tObjParam.fHp_Cur);

	//// 1. ���� ���� ���� ����
	//_v3 vBackDir = -(*(_v3*)&m_pTransformCom->Get_WorldMat().m[2]);
	//m_pAIControllerCom->Set_Value_Of_BloackBoard(L"BackDir", vBackDir);


	return E_NOTIMPL;
}

HRESULT CSwordShieldGenji::Update_NF()
{
	// �÷��̾� �߰� �� ���� ��
	if (false == m_bFindPlayer)
	{
		// �÷��̾� ��ǥ ����.
		_v3 vPlayer_Pos = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos();

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
			if (Is_InFov(m_fFov, vPlayer_Pos))
			{
				// �÷��̾� �߰�
				m_bFindPlayer = true;
			}
			else
			{
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
		case Client::CSwordShieldGenji::Sit1:
			m_pMeshCom->SetUp_Animation(Ani_StandUp1);
			break;
		case Client::CSwordShieldGenji::Sit2:
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

HRESULT CSwordShieldGenji::Update_Collider()
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

void CSwordShieldGenji::Skill_Movement(_float _fspeed, _v3 _vDir)
{
	_v3 tmpLook;
	_float fSpeed = _fspeed;

	tmpLook = _vDir;
	D3DXVec3Normalize(&tmpLook, &tmpLook);

	// �׺���̼� �����ϸ� 
	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &tmpLook, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

}

void CSwordShieldGenji::Decre_Skill_Movement(_float _fMutiply)
{
	m_fSkillMoveSpeed_Cur -= (0.3f - m_fSkillMoveAccel_Cur * m_fSkillMoveAccel_Cur * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60")) * _fMutiply;
	m_fSkillMoveAccel_Cur += g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

	if (m_fSkillMoveSpeed_Cur < 0.f)
	{
		m_fSkillMoveAccel_Cur = 0.5f;
		m_fSkillMoveSpeed_Cur = 0.f;
	}
}

_bool CSwordShieldGenji::Is_InFov(_float fDegreeOfFov, _v3 vTargetPos)
{
	_v3 vThisLook = *(_v3*)(&m_pTransformCom->Get_WorldMat().m[2]);
	vThisLook.y = 0.f;
	D3DXVec3Normalize(&vThisLook, &vThisLook);

	_v3 FromThisToTarget = vTargetPos - m_pTransformCom->Get_Pos();
	FromThisToTarget.y = 0.f;
	D3DXVec3Normalize(&FromThisToTarget, &FromThisToTarget);


	_float fDot_Temp = D3DXVec3Dot(&vThisLook, &FromThisToTarget);
	_float fRadian = acosf(fDot_Temp);

	//cout << "�þ߰� : " << D3DXToDegree(fRadian) << endl;

	if (D3DXToDegree(fRadian) < fDegreeOfFov * 0.5f)
		return true;

	return false;
}

void CSwordShieldGenji::Check_PhyCollider()
{
	if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		m_pMeshCom->Reset_OldIndx();	//�ִ� �ε��� �ʱ�ȭ

		//m_tObjParam.fHp_Cur -= 2.99f;	// ü�� ���Ƿ� ��� ����.

		m_bAIController = false;
		cout << "���� �ε��� ^^" << endl;
		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		m_bFight = true;	// �ο� ����

		m_fSkillMoveSpeed_Cur = 4.f;
		m_fSkillMoveAccel_Cur = 0.f;
		m_fSkillMoveMultiply = 0.5f;

		// ������ �÷��̾��� ���� �޾ƿͼ� �� �������� ��.
		m_vPushDir_forHitting = (*(_v3*)&TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_WorldMat().m[2]);

		m_pAIControllerCom->Reset_BT();

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			m_pMeshCom->SetUp_Animation(Ani_Dmg01_FL);	//���⿡ ���� ��� �������.
		}
		else
		{
			m_pMeshCom->SetUp_Animation(Ani_Death);	// ����ó�� ����
			Start_Dissolve(0.5f, false, true);
			m_pShield->Start_Dissolve();
			m_pSword->Start_Dissolve();
			//g_pManagement->Create_Spawn_Effect(m_pTransformCom->Get_Pos());
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

		// �и� ó��
		if (m_tObjParam.bIsHit == true)
		{
			Decre_Skill_Movement(m_fSkillMoveMultiply);
			Skill_Movement(m_fSkillMoveSpeed_Cur, m_vPushDir_forHitting);
			//cout << "�и��� ��" << endl;
		}
	}

}

void CSwordShieldGenji::Push_Collider()
{
	list<CGameObject*> tmpList = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE);

	for (auto& iter : tmpList)
	{
		CCollider* pCollider = TARGET_TO_COL(iter);

		// ���� �ӵ��� ���Ƿ� �־��µ� ���ؼ� �־������ - �Ϸ�
		if (m_pCollider->Check_Sphere(pCollider, m_pAIControllerCom->Get_V3Value(L"Monster_Dir"), m_pAIControllerCom->Get_FloatValue(L"Monster_Speed")))
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

HRESULT CSwordShieldGenji::Draw_Collider()
{
	for (auto& iter : m_vecPhysicCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	return S_OK;
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

	// for.Com_NavMesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	m_pCollider->Set_Radius(_v3{ 0.5f, 0.5f, 0.5f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y , 0.f });

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
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShaderCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CSwordShieldGenji::Ready_Weapon()
{
	// ������ ����
	m_pSword = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pSword->Change_WeaponData(CWeapon::WPN_SSword_Normal);

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("RightHandAttach");
	m_pSword->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pSword->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());

	// �޼� ����
	m_pShield = static_cast<CWeapon*>(g_pManagement->Clone_GameObject_Return(L"GameObject_Weapon", NULL));
	m_pShield->Change_WeaponData(CWeapon::WPN_Hammer_Normal);

	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("LeftHandAttach");
	m_pShield->Set_AttachBoneMartix(&pFamre->CombinedTransformationMatrix);
	m_pShield->Set_ParentMatrix(&m_pTransformCom->Get_WorldMat());


	return S_OK;
}

HRESULT CSwordShieldGenji::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

HRESULT CSwordShieldGenji::Ready_Collider()
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

HRESULT CSwordShieldGenji::Ready_NF(void * pArg)
{
	INFO eTemp = *(INFO*)pArg;

	m_eNF_Ani = eTemp.eNF_Ani;
	m_fFov = eTemp.fFov;
	m_fMaxLength = eTemp.fMaxLength;
	m_fMinLength = eTemp.fMinLength;

	return S_OK;
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
	Safe_Release(m_pMonsterUI);
	Safe_Release(m_pDamegeNumUI);

	Safe_Release(m_pShield);
	Safe_Release(m_pSword);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pCollider);

	CGameObject::Free();
}
