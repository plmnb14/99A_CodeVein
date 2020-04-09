#include "stdafx.h"
#include "..\Headers\PoisonButterfly.h"
#include "..\Headers\PoisonBullet.h"
#include "..\Headers\PoisonRotationBullet.h"

CPoisonButterfly::CPoisonButterfly(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPoisonButterfly::CPoisonButterfly(const CPoisonButterfly & rhs)
	: CGameObject(rhs)
{
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

	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 100.f;
	m_tObjParam.fDamage = 20.f;
	m_tObjParam.fDamage = 0.f;

	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));



	///////////////// �ൿƮ�� init

	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create();	//���ڿ� true �ָ� �ܼ�â�� ��������� ��, default = false

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	Update_Bone_Of_BlackBoard();

	pBlackBoard->Set_Value(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos());
	pBlackBoard->Set_Value(L"HP", m_tObjParam.fHp_Cur);
	pBlackBoard->Set_Value(L"MAXHP", m_tObjParam.fHp_Max);
	pBlackBoard->Set_Value(L"HPRatio", 100);
	//pBlackBoard->Set_Value(L"Show", true); // ��
	pBlackBoard->Set_Value(L"Show", false); // �׽�Ʈ
	pBlackBoard->Set_Value(L"Show_Near", true);

	
	CBT_Selector* Start_Sel = Node_Selector("�ൿ ����");
	//CBT_Sequence* Start_Sel = Node_Sequence("�ൿ ����");	//�׽�Ʈ
	CBT_UpdateGageRatio* UpdateHPRatioService = Node_UpdateGageRatio("ü�� ����", L"HPRatio", L"MAXHP", L"HP", 1, 0.01, 0, CBT_Service_Node::Infinite);

	pBehaviorTree->Set_Child(Start_Sel);

	Start_Sel->Add_Service(UpdateHPRatioService);

	//////////// �Ʒ��� �ּ��س��� 4���� �����ӿ��� �� ����, ���ʴ�� ������.

	CBT_CompareValue* Check_ShowValue = Node_BOOL_A_Equal_Value("�ÿ�ȸ ���� üũ", L"Show", true);
	Check_ShowValue->Set_Child(Start_Show());
	Start_Sel->Add_Child(Check_ShowValue);
	Start_Sel->Add_Child(Start_Game());

	////////////


	// ���� Ȯ�ο�,  �� ���� �Լ��� �Ʒ��� ������ �����

	//Start_Sel->Add_Child(Poison_Tornado_After_Charging());

	//CBT_RotationDir* Rotation0 = Node_RotationDir("����", L"Player_Pos", 0.2);
	//Start_Sel->Add_Child(Rotation0);


	//������Ű

	/////////////////////////////////////////////////////////////////////////////�Ϲ� ����
	/*
	�ں��� ����, ���� �ѹ��� ���Ƽ� �� Y OR N , �ٽ� ���� �������
	22 -> 23

	24   ���� ��
	25	������ ��

	�� -> ���׸�����
	27 -> 26   0.5���� �Ѿ�� ��

	*/
	/////////////////////////////////////////////////////////////////////////////Ư�� ����
	/*
	19  ���ݼ�
	20   ���� ���� �ѹ��� ������
	21   ���濡 ������ �� ����
	28  ���Ƽ� �� �ҿ뵹��
	29  ����ź

	*/
	/////////////////////////////////////////////////////////////////////////////�ܵ� ����
	/*
	17   ���� 5��
	18   Ʈ���ٹ̾� , Y OR N

	*/

	/////////////////////////////////////////////////////////////////////////////�Ϲ� ����
	/*
	0��    ������ �ȱ�
	1��		���� �ȱ�
	2		������ �ȱ�
	3		�ڷ� �ȱ�




	*/

	/////////////////////////////////////////////////////////////////////////////�Ϲ� ����
	/*
	4		������ 90��
	5		���� 90��
	6		�⺻

	7		�׷α� ����
	8		�׷α� ����
	9		�׷α� ��


	11		���� 2������ ����

	12		����  �����۹� ������  �߾��̴ʰ� �����

	//   ����ġ
	13		�ڿ���  ����
	14		�տ���  ����

	// �׻� 15�� �����ϴٰ� 16���� �Ǿ���Ѵ�.  �׸��� �������ʹ� �������� ����
	15		���� �� ���
	16		����
	*/












	/////////////////////////////////////////////////////////////////////////////�Ϲ� ����


	/////////////////////////////////////////////////////////////////////////////Ư�� ����
	//// 20 -> �ڵ��Ƽ� ����
	//CBT_Play_Ani* pAni20 = Node_Ani("�ڵ��Ƽ� ����", 20, 0.9f);
	//pSequence->Add_Child(pAni20);


	/////////////////////////////////////////////////////////////////////////////�ܵ� ����

	//����
	//// 19 -> ��ȸ�� �ѹ���  ����
	//CBT_Play_Ani* pAni19 = Node_Ani("��ȸ�� �ѹ��� ����", 19, 0.9f);
	//pSequence->Add_Child(pAni19);


	//���� ����
	// 28 -> ���� ���� ���Ȱ�
	//CBT_Play_Ani* pAni28 = Node_Ani("���� ���� ���Ȱ�", 28, 0.9f);
	//pSequence->Add_Child(pAni28);


	//����
	//// 17 -> �����ϸ鼭 5�� Ÿ�� �� �Һ�
	//CBT_Play_Ani* pAni17 = Node_Ani("�����ϸ鼭 5�� Ÿ�� �� �Һ�", 17, 0.9f);
	//pSequence->Add_Child(pAni17);


	//// 29 -> ����ź
	//CBT_Play_Ani* pAni29 = Node_Ani("5�߽��", 29, 0.9f);
	//pSequence->Add_Child(pAni29);


	//// 18 -> Ʈ���ٹ̾�
	//CBT_Play_Ani* pAni18 = Node_Ani("Ʈ���ٹ̾�", 18, 0.9f);
	//pSequence->Add_Child(pAni18);








	//// 21 -> ������� �� ����
	//CBT_Play_Ani* pAni21 = Node_Ani("������� �� ����", 21, 0.9f);
	//pSequence->Add_Child(pAni21);


	////���� �����
	//// 15 -> ȥ�� ���� �� idle����
	//CBT_Play_Ani* pAni15 = Node_Ani("ȥ�� ���� �� IDLE����", 15, 0.9f);
	//pSequence->Add_Child(pAni15);

	//// 16 -> ������ ���� ��
	//CBT_Play_Ani* pAni16 = Node_Ani("������ ���� ��", 16, 0.9f);
	//pSequence->Add_Child(pAni16);

	//// ����
	////13 -> �ڸ°� ����
	//CBT_Play_Ani* pAni13 = Node_Ani("�ڸ°� ����", 13, 0.9f);
	//pSequence->Add_Child(pAni13);

	////14 -> �ո°� ����
	//CBT_Play_Ani* pAni14 = Node_Ani("�ո°� ����", 14, 0.9f);
	//pSequence->Add_Child(pAni14);


	////12 -> �����۹� , ������ �� ���
	//CBT_Play_Ani* pAni12 = Node_Ani("���", 12, 0.9f);
	//pSequence->Add_Child(pAni12);

	////11 -> ���ǵǸ�  2������ ���� ��
	//CBT_Play_Ani* pAni11 = Node_Ani("���ǵǸ� 2������", 11, 0.9f);
	//pSequence->Add_Child(pAni11);


	///////////////////////////�ƿ�
	////10 -> ȸ��
	//CBT_Play_Ani* pAni10 = Node_Ani("ȸ��", 10, 0.9f);
	//pSequence->Add_Child(pAni10);



	m_pMeshCom->SetUp_Animation(Ani_Idle);


	return NOERROR;
}

_int CPoisonButterfly::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	//cout << "M : ������Ʈ Ÿ?" << endl;
	Push_Collider();

	CGameObject::Update_GameObject(TimeDelta);

	// �׾��� ���
	if (m_bIsDead)
		return DEAD_OBJ;

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
	OnCollisionEnter();


	//// �÷��̾� ��ǥ ����.
	//_v3 vPlayer_Pos = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos();

	//// �÷��̾�� ������ �Ÿ�
	//_v3 vLengthTemp = vPlayer_Pos - m_pTransformCom->Get_Pos();
	//vLengthTemp.y = 0.f;
	//_float fLength = D3DXVec3Length(&vLengthTemp);

	//cout << fLength << endl;

	return NOERROR;
}

_int CPoisonButterfly::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	return NOERROR;
}

HRESULT CPoisonButterfly::Render_GameObject()
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

			if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE))))
				return E_FAIL;

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

CBT_Composite_Node * CPoisonButterfly::Left_Eat()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� ��");

	CBT_Play_Ani* Show_Ani27 = Node_Ani("���� ��", 27, 0.95f);//27
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 6, 0.3f);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ְ�", L"ButterFly_Distortion_Circle", L"Bone_Tail6", 1.75f, 1, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("��¦ ���� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�ְ�Ȱ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0.f, 60, 0.7, 0);

	Root_Seq->Add_Service(Effect0);
	Root_Seq->Add_Service(Effect1);
	Root_Seq->Add_Service(Effect2);
	Root_Seq->Add_Service(Effect3);
	Root_Seq->Add_Service(Effect4);

	Root_Seq->Add_Child(Show_Ani27);
	Root_Seq->Add_Child(Show_Ani6);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.5, 1, 0.25, 0);
	Root_Seq->Add_Service(pHitCol);

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Right_Eat()
{
	CBT_Sequence* Root_Seq = Node_Sequence("������ ��");

	CBT_Play_Ani* Show_Ani22 = Node_Ani("������ ��", 22, 0.95f);//22
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 6, 0.3f);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ְ�", L"ButterFly_Distortion_Circle", L"Bone_Tail6", 1.65f, 1, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("��¦ ���� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�ְ�Ȱ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0.f, 60, 0.7, 0);

	Root_Seq->Add_Service(Effect0);
	Root_Seq->Add_Service(Effect1);
	Root_Seq->Add_Service(Effect2);
	Root_Seq->Add_Service(Effect3);
	Root_Seq->Add_Service(Effect4);

	Root_Seq->Add_Child(Show_Ani22);
	Root_Seq->Add_Child(Show_Ani6);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.5, 1, 0.25, 0);
	Root_Seq->Add_Service(pHitCol);

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Eat_TurnEat()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("�� �ڵ��Ƽ� ����");

	CBT_Sequence* MainSeq = Node_Sequence("�� �ڵ��Ƽ� ��");
	CBT_Play_Ani* Show_Ani22 = Node_Ani("��ȸ�� �ѹ��� ����", 22, 0.6f);//22
	CBT_Play_Ani* Show_Ani20 = Node_Ani("�ڵ��Ƽ� ����", 20, 0.95f);//20
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 6, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 2.8, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 10, 0.2, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("�ְ�", L"ButterFly_Distortion_Circle", L"Bone_Tail6_Tongue2", 2.0, 1, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("���� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("��¦ ���� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0.f, 160, 0.7, 0);
	CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�ְ�Ȱ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0.f, 60, 0.7, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("�ְ�", L"ButterFly_Distortion_Circle", L"Bone_Tail6_Tongue2", 4.2, 1, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);
	Root_Parallel->Add_Service(Effect4);
	Root_Parallel->Add_Service(Effect5);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani22);
	MainSeq->Add_Child(Show_Ani20);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.5, 1, 0.25, 0);
	Root_Parallel->Add_Service(pHitCol);

	pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 2.616, 1, 0.45, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Poison_Tornado_After_Charging()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���Ƽ� �� �ҿ뵹��");

	CBT_Play_Ani* Show_Ani28 = Node_Ani("���Ƽ� �� �ҿ뵹��", 28, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 6, 0.3f);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("������ �� ó�� ����ũ 01", L"ButterFly_SoftSmoke_Ready_1", L"Self_Pos", 3.2, 1, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("������ �� ó�� ����ũ 02", L"ButterFly_SoftSmoke_Ready_2", L"Self_Pos", 3.2, 1, 0.01, 0);
	//CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("�� ���� �� �޽�����Ʈ", L"ButterFly_Distortion_Circle", L"Self_Pos", 0.3, 1, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("�� ���� �� ����ũ 01", L"ButterFly_SoftSmoke", L"Self_Pos"		, 0.3, 40, 1.2, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�� ���� �� ����ũ 02", L"ButterFly_SoftSmoke_Bottom", L"Self_Pos"	, 0.8, 40, 1.5, 0);
	//CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("������ �� ó�� ���� ����ũ", L"ButterFly_Smoke_Red_Once", L"Self_Pos"	, 0, 1, 0.01, 0);
	CBT_CreateEffect* Effect5 = Node_CreateEffect_Finite("�����ϴ� ���� ����ũ", L"ButterFly_Smoke_Red_Particle", L"Self_Pos"	,3, 15, 0.01, 0);
	//CBT_CreateEffect* Effect6 = Node_CreateParticle_Finite("�ٴڿ� �򸮴� ���� ����ũ", L"ButterFly_SoftSmoke_Floor", L"Self_Pos", 10, 0.8, 150, 0, 0);
	CBT_CreateEffect* Effect6 = Node_CreateEffect_Finite("�ٴڿ� �򸮴� ���� ����ũ", L"ButterFly_SoftSmoke_Floor", L"Self_Pos", 0.8, 60, 1.5, 0);
	CBT_CreateEffect* Effect7 = Node_CreateEffect_Finite("������ �� �ڵλ� ���׶�� ��ƼŬ", L"ButterFly_PointParticle_Plum", L"Self_Pos", 3.2, 20, 0.7, 0);
	CBT_CreateEffect* Effect8 = Node_CreateEffect_Finite("������ �� ȸ���� �޽�����Ʈ 01", L"ButterFly_RingLine", L"Self_Pos"	, 3, 1, 0.01, 0);
	CBT_CreateEffect* Effect9 = Node_CreateEffect_Finite("������ �� ȸ���� �޽�����Ʈ 02", L"ButterFly_RingLine", L"Self_Pos"	, 3.45, 1, 0.01, 0);
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

	CBT_CreateBullet* PoisonBullet0 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonTornado", L"Self_Pos", L"", 0, 2.5, 3.36, 1, 0, 0, CBT_Service_Node::Finite);
	Root_Seq->Add_Service(PoisonBullet0);


	Root_Seq->Add_Child(Show_Ani28);
	Root_Seq->Add_Child(Show_Ani6);

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::OneTurn_Poison()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� ���� �ѹ��� �� �߻�");

	CBT_Play_Ani* Show_Ani20 = Node_Ani("���� ���� �ѹ��� �� �߻�", 24, 0.95f);//24
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 6, 0.3f);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("��ü������ ���� ���׶�� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("��ü������ ���̴� ���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("��¦�̴� ����� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�Ȱ��� ���� ������ �ְ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 150, 0.01, 0);

	Root_Seq->Add_Service(Effect0);
	Root_Seq->Add_Service(Effect1);
	Root_Seq->Add_Service(Effect2);
	Root_Seq->Add_Service(Effect3);

	Root_Seq->Add_Child(Show_Ani20);
	Root_Seq->Add_Child(Show_Ani6);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.25, 1, 0.983, 0);
	Root_Seq->Add_Service(pHitCol);

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Eat_Turn()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("�� ����");
	CBT_Sequence* MainSeq = Node_Sequence("�� ����");
	CBT_Play_Ani* Show_Ani25 = Node_Ani("��", 27, 0.5f);//27, 0.5
	CBT_Play_Ani* Show_Ani23 = Node_Ani("����", 26, 0.95f);//26
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 6, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_Wait* Wait0 = Node_Wait("���", 3, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 10, 0.2, 0);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("��ü������ ���� ���׶�� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 300, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("��ü������ ���̴� ���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 300, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("��¦�̴� ����� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 300, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�Ȱ��� ���� ������ �ְ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 300, 0.01, 0);
	//CBT_CreateEffect* Effect4 = Node_CreateEffect_Finite("�ְ�", L"ButterFly_Distortion_Circle", L"Bone_Tail6_Tongue2", 0.5, 1, 0.01, 0);

	Root_Parallel->Add_Service(Effect0);
	Root_Parallel->Add_Service(Effect1);
	Root_Parallel->Add_Service(Effect2);
	Root_Parallel->Add_Service(Effect3);

	Root_Parallel->Set_Main_Child(MainSeq);
	MainSeq->Add_Child(Show_Ani25);
	MainSeq->Add_Child(Show_Ani23);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(Wait0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.3, 1, 0.5, 0);
	Root_Parallel->Add_Service(pHitCol);
	pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 3, 1, 0.3, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Rush()
{
	CBT_Simple_Parallel* RushParallel = Node_Parallel_Immediate("���� ����");
	CBT_Sequence* RushAniSeq = Node_Sequence("RushSequence");
	CBT_Play_Ani* Show_Ani17 = Node_Ani("5ȸ Ÿ���� ����", 17, 0.95f);
	CBT_Play_Ani* Show_Ani6_9 = Node_Ani("�⺻", 6, 0.3f);

	CBT_Sequence* MoveSequence = Node_Sequence("���� �̵�");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����1", L"Player_Pos", 1.5, 0);
	CBT_RotationDir* Rotation0 = Node_RotationDir("���� ����2", L"Player_Pos", 0.2);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("����", L"Monster_Speed", L"Monster_Dir", 13, 1, 0);
	CBT_Wait* RushWaitB = Node_Wait("RushWait1", 0.2, 0);

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

	RushParallel->Set_Sub_Child(MoveSequence);
	MoveSequence->Add_Child(ChaseDir0);
	MoveSequence->Add_Child(Rotation0);
	MoveSequence->Add_Child(Move0);
	MoveSequence->Add_Child(RushWaitB);

	RushParallel->Set_Main_Child(RushAniSeq);

	RushAniSeq->Add_Child(Show_Ani17);
	RushAniSeq->Add_Child(Show_Ani6_9);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.5, 1, 1.3, 0);
	RushParallel->Add_Service(pHitCol);

	return RushParallel;
}

CBT_Composite_Node * CPoisonButterfly::Fire_5Bullet()
{
	CBT_Sequence* Root_Seq = Node_Sequence("5��ź");

	CBT_Play_Ani* Show_Ani29 = Node_Ani("5��ź", 29, 0.95f);
	CBT_Play_Ani* Show_Ani6_0 = Node_Ani("�⺻", 6, 0.3f);

	CBT_CreateBullet* PoisonBullet0 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonBullet", L"Bone_Head", L"Self_PoisonDir0", 5, 5, 1.3, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet1 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonBullet", L"Bone_Head", L"Self_PoisonDir1", 5, 5, 1.3, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet2 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonBullet", L"Bone_Head", L"Self_PoisonDir2", 5, 5, 1.3, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet3 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonBullet", L"Bone_Head", L"Self_PoisonDir3", 5, 5, 1.3, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet4 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonBullet", L"Bone_Head", L"Self_PoisonDir4", 5, 5, 1.3, 1, 1, 0, CBT_Service_Node::Finite);

	Root_Seq->Add_Service(PoisonBullet0);
	Root_Seq->Add_Service(PoisonBullet1);
	Root_Seq->Add_Service(PoisonBullet2);
	Root_Seq->Add_Service(PoisonBullet3);
	Root_Seq->Add_Service(PoisonBullet4);

	Root_Seq->Add_Child(Show_Ani29);
	Root_Seq->Add_Child(Show_Ani6_0);

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Fire_ChaseBullet()
{
	CBT_Sequence* Root_Seq = Node_Sequence("�ݹ��� ���Ƽ� ����");

	CBT_Play_Ani* Show_Ani19 = Node_Ani("�ݹ��� ���Ƽ� ����", 23, 0.95f);//23
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 6, 0.3f);

	CBT_CreateBullet* PoisonBullet0 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonChaseBullet", L"Bone_Tail6", L"Self_PoisonDir2", 5, 5, 2.5, 1, 1, 0, CBT_Service_Node::Finite);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("��ü������ ���� ���׶�� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("��ü������ ���̴� ���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("��¦�̴� ����� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�Ȱ��� ���� ������ �ְ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 150, 0.01, 0);

	Root_Seq->Add_Service(PoisonBullet0);

	Root_Seq->Add_Service(Effect0);
	Root_Seq->Add_Service(Effect1);
	Root_Seq->Add_Service(Effect2);
	Root_Seq->Add_Service(Effect3);

	Root_Seq->Add_Child(Show_Ani19);
	Root_Seq->Add_Child(Show_Ani6);


	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Turn_4PoisonShot()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���鼭 ������� �� ������");

	CBT_Play_Ani* Show_Ani19 = Node_Ani("���鼭 ������� �� ������", 24, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 6, 0.3f);

	CBT_CreateBullet* PoisonBullet0 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonRotationBullet", L"Bone_Tail6", L"Self_Front", 1.5, 3, 1.53, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet1 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonRotationBullet", L"Bone_Tail6", L"Self_Right", 1.5, 3, 1.70, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet2 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonRotationBullet", L"Bone_Tail6", L"Self_Back", 1.5, 3, 1.87, 1, 1, 0, CBT_Service_Node::Finite);
	CBT_CreateBullet* PoisonBullet3 = Node_CreateBullet("�� �Ѿ�", L"Monster_PoisonRotationBullet", L"Bone_Tail6", L"Self_Left", 1.5, 3, 2.04, 1, 1, 0, CBT_Service_Node::Finite);

	CBT_CreateEffect* Effect0 = Node_CreateEffect_Finite("��ü������ ���� ���׶�� ��ƼŬ", L"ButterFly_PointParticle", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect1 = Node_CreateEffect_Finite("��ü������ ���̴� ���� ���Ȱ�", L"ButterFly_SoftSmoke_Mist", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect2 = Node_CreateEffect_Finite("��¦�̴� ����� ��", L"ButterFly_GlitterSand", L"Bone_Tail6", 0, 150, 0.01, 0);
	CBT_CreateEffect* Effect3 = Node_CreateEffect_Finite("�Ȱ��� ���� ������ �ְ�", L"ButterFly_Distortion_Smoke", L"Bone_Tail6", 0, 150, 0.01, 0);

	Root_Seq->Add_Service(Effect0);
	Root_Seq->Add_Service(Effect1);
	Root_Seq->Add_Service(Effect2);
	Root_Seq->Add_Service(Effect3);

	Root_Seq->Add_Service(PoisonBullet0);
	Root_Seq->Add_Service(PoisonBullet1);
	Root_Seq->Add_Service(PoisonBullet2);
	Root_Seq->Add_Service(PoisonBullet3);

	Root_Seq->Add_Child(Show_Ani19);
	Root_Seq->Add_Child(Show_Ani6);


	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::WhirlWind()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("������");

	CBT_Sequence* MainSeq = Node_Sequence("������");
	CBT_Play_Ani* Show_Ani18 = Node_Ani("���׸���", 18, 0.95f);
	CBT_Play_Ani* Show_Ani6 = Node_Ani("�⺻", 6, 0.3f);

	CBT_Sequence* SubSeq = Node_Sequence("�̵�");
	CBT_ChaseDir* ChaseDir0 = Node_ChaseDir("���� ����", L"Player_Pos", 1.2, 0);
	CBT_MoveDirectly* Move0 = Node_MoveDirectly_Rush("�̵�", L"Monster_Speed", L"Monster_Dir", 2, 1, 0);

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
	MainSeq->Add_Child(Show_Ani18);
	MainSeq->Add_Child(Show_Ani6);

	Root_Parallel->Set_Sub_Child(SubSeq);
	SubSeq->Add_Child(ChaseDir0);
	SubSeq->Add_Child(Move0);

	CBT_UpdateParam* pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1, 1, 1.7, 0);
	Root_Parallel->Add_Service(pHitCol);
	pHitCol = Node_UpdateParam("���� ��Ʈ On", &m_tObjParam, CBT_UpdateParam::Collider, 1.6, 1, 1.1, 0);
	Root_Parallel->Add_Service(pHitCol);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::Right_Eat_Left_Eat()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� ��, ������ ��");

	Root_Seq->Add_Child(Right_Eat());
	Root_Seq->Add_Child(Left_Eat());

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::NearAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("���� �ٰŸ� ����");

	Root_Sel->Add_Child(Left_Eat());
	Root_Sel->Add_Child(Right_Eat());
	Root_Sel->Add_Child(Eat_TurnEat());
	Root_Sel->Add_Child(Poison_Tornado_After_Charging());
	Root_Sel->Add_Child(OneTurn_Poison());
	Root_Sel->Add_Child(WhirlWind());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::FarAttack()
{
	CBT_Selector* Root_Sel = Node_Selector_Random("������� ���Ÿ� ����");

	Root_Sel->Add_Child(Rush());
	Root_Sel->Add_Child(Fire_5Bullet());
	Root_Sel->Add_Child(Fire_ChaseBullet());
	Root_Sel->Add_Child(Turn_4PoisonShot());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::Start_Game()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� ����");

	Root_Seq->Add_Child(Dist_Attack());

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Dist_Attack()
{
	CBT_Selector* Root_Sel = Node_Selector("�ٰŸ� ���Ÿ� ���� ����");

	CBT_DistCheck* Dist0 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 3);
	CBT_DistCheck* Dist1 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 6);
	CBT_DistCheck* Dist2 = Node_DistCheck("�Ÿ� üũ", L"Player_Pos", 8);

	Root_Sel->Add_Child(Dist0);
	Dist0->Set_Child(NearAttack_Dist3());

	Root_Sel->Add_Child(Dist1);
	Dist1->Set_Child(NearAttack_Dist6());

	Root_Sel->Add_Child(Dist2);
	Dist2->Set_Child(FarAttack());

	Root_Sel->Add_Child(Rush());

	return Root_Sel;
}

CBT_Composite_Node * CPoisonButterfly::ChaseAndNearAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� ���� �Ǵ� ����");
	CBT_MoveDirectly* Chase = Node_MoveDirectly_Chase("����", L"Monster_Dir", L"Player_Pos", L"Monster_Speed", 3.f, 2.f);

	Root_Seq->Add_Child(Chase);
	Root_Seq->Add_Child(NearAttack());

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::TurnAndFarAttack()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� ���Ÿ� ����");
	CBT_RotationDir* TurnDir = Node_RotationDir("Look ȸ��", L"Player_Pos", 0.2);

	Root_Seq->Add_Child(TurnDir);
	Root_Seq->Add_Child(FarAttack());

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Chase()
{
	CBT_Simple_Parallel* Root_Parallel = Node_Parallel_Immediate("����");

	CBT_MoveDirectly* pChase = Node_MoveDirectly_Chase("����", L"Player_Pos", L"Monster_Speed", L"Monster_Dir", 3.f, 2.5f);

	CBT_Play_Ani* Show_Ani139 = Node_Ani("����", 139, 1.f);

	Root_Parallel->Set_Main_Child(pChase);

	Root_Parallel->Set_Sub_Child(Show_Ani139);

	return Root_Parallel;
}

CBT_Composite_Node * CPoisonButterfly::NearAttack_Dist3()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� Dist3 ����");

	CBT_RotationDir* Rotation0 = Node_RotationDir("�÷��̾� �ٶ󺸱�", L"Player_Pos", 0.5);
	CBT_Selector* Attack_Sel = Node_Selector_Random("���� Dist3 ����");

	Root_Seq->Add_Child(Rotation0);

	Root_Seq->Add_Child(Attack_Sel);
	Attack_Sel->Add_Child(OneTurn_Poison());
	Attack_Sel->Add_Child(Poison_Tornado_After_Charging());
	Attack_Sel->Add_Child(WhirlWind());

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::NearAttack_Dist6()
{
	CBT_Sequence* Root_Seq = Node_Sequence("���� Dist6 ����");

	CBT_RotationDir* Rotation0 = Node_RotationDir("�÷��̾� �ٶ󺸱�", L"Player_Pos", 0.5);
	CBT_Selector* Attack_Sel = Node_Selector_Random("���� Dist6 ����");

	Root_Seq->Add_Child(Rotation0);

	Root_Seq->Add_Child(Attack_Sel);
	Attack_Sel->Add_Child(Left_Eat());
	Attack_Sel->Add_Child(Right_Eat());
	Attack_Sel->Add_Child(Eat_TurnEat());
	Attack_Sel->Add_Child(Eat_Turn());
	Attack_Sel->Add_Child(Right_Eat_Left_Eat());

	return Root_Seq;
}

CBT_Composite_Node * CPoisonButterfly::Start_Show()
{
	CBT_Selector* Root_Sel = Node_Selector("�ÿ�ȸ");

	CBT_CompareValue* Check_ShowValueN = Node_BOOL_A_Equal_Value("�ÿ�ȸ �ٰŸ� ���� üũ", L"Show_Near", true);
	CBT_CompareValue* Check_ShowValueF = Node_BOOL_A_Equal_Value("�ÿ�ȸ ���Ÿ� ���� üũ", L"Show_Far", true);

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
	CBT_Cooldown* Cool7 = Node_Cooldown("��7", 300);

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
	Root_Sel->Add_Child(Cool7);
	Cool7->Set_Child(Right_Eat_Left_Eat());

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

	CBT_SetValue* Show_ValueOff = Node_BOOL_SetValue("�ÿ�ȸ OFF", L"Show", false);

	Root_Sel->Add_Child(Cool0);
	Cool0->Set_Child(Rush());
	Root_Sel->Add_Child(Cool1);
	Cool1->Set_Child(Fire_5Bullet());
	Root_Sel->Add_Child(Cool2);
	Cool2->Set_Child(Fire_ChaseBullet());
	Root_Sel->Add_Child(Cool3);
	Cool3->Set_Child(Turn_4PoisonShot());

	Root_Sel->Add_Child(Show_ValueOff);

	return Root_Sel;
}

HRESULT CPoisonButterfly::Update_Bone_Of_BlackBoard()
{
	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Tail6");
	m_vTail6 = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Bone_Tail6", m_vTail6);

	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Tail4");
	m_vTail4 = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Bone_Tail4", m_vTail4);

	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Tail2");
	m_vTail2 = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Bone_Tail2", m_vTail2);

	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head");
	m_vHead = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Bone_Head", m_vHead);

	pFamre = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Tail6_Tongue2");
	m_vTail6_Tongue2 = *(_v3*)(&(pFamre->CombinedTransformationMatrix * m_pTransformCom->Get_WorldMat()).m[3]);
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Bone_Tail6_Tongue2", m_vTail6_Tongue2);
	
	return S_OK;
}

HRESULT CPoisonButterfly::Update_Value_Of_BB()
{
	// 1. �÷��̾� ��ǥ ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Player_Pos", TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE))->Get_Pos());
	// 2. ü�� ������Ʈ
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"HP", m_tObjParam.fHp_Cur);


	// 1. 5�� ����
	_v3 vSelfDir = *(_v3*)&m_pTransformCom->Get_WorldMat().m[2];
	_v3 vDirTemp0;

	_v3 vTempFrontDir = _v3(0.f, 0.f, 1.f);
	_float fRadian = D3DXVec3Dot(&vSelfDir, &vTempFrontDir);

	if(fRadian >= 0)
		D3DXVec3TransformNormal(&vDirTemp0, &vSelfDir, D3DXMatrixRotationX(&_mat(), D3DXToRadian(15)));
	else if (fRadian < 0)
		D3DXVec3TransformNormal(&vDirTemp0, &vSelfDir, D3DXMatrixRotationX(&_mat(), D3DXToRadian(-15)));

	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_PoisonDir0", *D3DXVec3TransformNormal(&_v3(), &vDirTemp0, D3DXMatrixRotationY(&_mat(), D3DXToRadian(25))));
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_PoisonDir1", *D3DXVec3TransformNormal(&_v3(), &vDirTemp0, D3DXMatrixRotationY(&_mat(), D3DXToRadian(12.5f))));
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_PoisonDir2", vDirTemp0);
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_PoisonDir3", *D3DXVec3TransformNormal(&_v3(), &vDirTemp0, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-12.5f))));
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_PoisonDir4", *D3DXVec3TransformNormal(&_v3(), &vDirTemp0, D3DXMatrixRotationY(&_mat(), D3DXToRadian(-25))));

	// 2. ���� ��ǥ
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_Pos", m_pTransformCom->Get_Pos());

	// 3. �����̼� �Ѿ� 4����
	_v3 vFrontDir = *D3DXVec3Normalize(&_v3(), (_v3*)m_pTransformCom->Get_WorldMat().m[2]);
	_v3 vRightDir = *D3DXVec3Normalize(&_v3(), (_v3*)m_pTransformCom->Get_WorldMat().m[0]);
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_Front", vFrontDir);
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_Back", -vFrontDir);
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_Right", vRightDir);
	m_pAIControllerCom->Set_Value_Of_BloackBoard(L"Self_Left", -vRightDir);

	return S_OK;
}

HRESULT CPoisonButterfly::Update_NF()
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

	return S_OK;
}

_bool CPoisonButterfly::Is_InFov(_float fDegreeOfFov, _v3 vTargetPos)
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

void CPoisonButterfly::Check_PhyCollider()
{
	// �浹ó��, bCanHit�� ���Ⱑ false�����ٰ���.
	if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{
		m_pMeshCom->Reset_OldIndx();	//�ִ� �ε��� �ʱ�ȭ

		m_bAIController = false;
		
		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		m_bFight = true;		// �ο� ����

		m_pAIControllerCom->Reset_BT();

		if (m_tObjParam.fHp_Cur > 0.f)
		{
			//++m_iHitCount;

			//if (m_iHitCount == 3)
			//{
			//	m_pMeshCom->SetUp_Animation(Ani_Dmg01_B);	//���⿡ ���� ��� �������.
			//	m_iHitCount = 0;
			//}
		}
		else
		{
			m_pMeshCom->SetUp_Animation(Ani_Death);	// ����ó�� ����
			Start_Dissolve(0.7f, false, true);
			g_pManagement->Create_Spawn_Effect(m_pTransformCom->Get_Pos());
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

		else if (m_pMeshCom->Is_Finish_Animation(0.5f))	// �̶����� ���浹 ����
		{
			m_tObjParam.bIsHit = false;
		}

	}
}

void CPoisonButterfly::Push_Collider()
{
	list<CGameObject*> tmpList = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE);

	for (auto& iter : tmpList)
	{
		CCollider* pCollider = TARGET_TO_COL(iter);

		//cout << m_pAIControllerCom->Get_FloatValue(L"Monster_Speed") << endl;

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

HRESULT CPoisonButterfly::Draw_Collider()
{
	for (auto& iter : m_vecPhysicCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	for (auto& iter : m_vecAttackCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	return S_OK;
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
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE));


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

					if (false == iter->Get_Target_Dodge())
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

HRESULT CPoisonButterfly::SetUp_ConstantTable()
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

HRESULT CPoisonButterfly::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);

	m_matBones[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine", 0);

	m_matBones[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBones[Bone_Body] = &pFrame->CombinedTransformationMatrix;

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
	m_vecAttackCol.reserve(10);

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
	Safe_Release(m_pCollider);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
