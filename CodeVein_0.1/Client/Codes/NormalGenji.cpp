#include "stdafx.h"
#include "..\Headers\NormalGenji.h"

CNormalGenji::CNormalGenji(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CNormalGenji::CNormalGenji(const CNormalGenji & rhs)
	: CGameObject(rhs)
{
}

HRESULT CNormalGenji::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CNormalGenji::Ready_GameObject(void * pArg)
{

	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(1.f, 1.f, 1.f));
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));


	CBlackBoard* pBlackBoard = CBlackBoard::Create();
	//pBlackBoard->Set_Value(L"Target_Pos", &_v3(-2.f, -2.f, -2.f), CBlackBoard::VALUE::VEC3);
	//pBlackBoard->Set_Value(L"A", &_bool(true), CBlackBoard::VALUE::BOOL);
	//_v3 temp = pBlackBoard->Get_V3Value(L"Target_Pos");

	CBehaviorTree* pBehaviorTree = CBehaviorTree::Create();

	CManagement* pManagement = CManagement::Get_Instance();


	CBT_Sequence* pSequence = Node_Sequence("Sequence");
	if (FAILED(pBehaviorTree->Set_Child(pSequence))) return E_FAIL;

	//CBT_Sequence* pSequence1 = static_cast<CBT_Sequence*>(pManagement->Clone_Node(L"Sequence", CBT_Node_Manager::COMPOSITE, &CBT_Sequence::INFO("Sequence2")));
	//CBT_Sequence* pSequence2 = static_cast<CBT_Sequence*>(pManagement->Clone_Node(L"Sequence", CBT_Node_Manager::COMPOSITE, nullptr));

	//CBT_Selector* pSelector = static_cast<CBT_Selector*>(pManagement->Clone_Node(L"Selector", CBT_Node_Manager::COMPOSITE, &CBT_Selector::INFO("Selecotr")));

	//CBT_Wait* pWait2 = static_cast<CBT_Wait*>(pManagement->Clone_Node(L"Wait", CBT_Node_Manager::TASK, &CBT_Wait::INFO("Wait2", 2, 0)));
	//CBT_Wait* CoolWait = static_cast<CBT_Wait*>(pManagement->Clone_Node(L"Wait", CBT_Node_Manager::TASK, &CBT_Wait::INFO("CoolWait",2)));
	//CBT_Wait* CoolWait22 = static_cast<CBT_Wait*>(pManagement->Clone_Node(L"Wait", CBT_Node_Manager::TASK, &CBT_Wait::INFO("Wait444", 4)));
	//CBT_Move* pMove = static_cast<CBT_Move*>(pManagement->Clone_Node(L"Move", CBT_Node_Manager::TASK, &CBT_Move::INFO(m_pTransformCom, 1, 5)));

	//���� ����

	//// 29 -> ����ź
	//CBT_Play_Ani* pAni29 = Node_Ani("5�߽��", 29, 0.9f);
	//pSequence->Add_Child(pAni29);

	//���� ������
	CBT_Play_Ani* pAni0 = Node_Ani("���� ������ ", 0, 0.9f);
	pSequence->Add_Child(pAni0);

	//���� Ǯ��
	CBT_Play_Ani* pAni1 = Node_Ani("���� Ǯ�� ", 1, 0.9f);
	pSequence->Add_Child(pAni1);

	//���� ƽƽ
	CBT_Play_Ani* pAni2 = Node_Ani("���� ƽƽ ", 2, 0.9f);
	pSequence->Add_Child(pAni2);

	//���� ����
	CBT_Play_Ani* pAni3 = Node_Ani("���� ���� ", 3, 0.9f);
	pSequence->Add_Child(pAni3);

	//���� ����
	CBT_Play_Ani* pAni4 = Node_Ani("���� ���� ", 4, 0.9f);
	pSequence->Add_Child(pAni4);

	//���� ��
	CBT_Play_Ani* pAni5 = Node_Ani("���� �� ", 5, 0.9f);
	pSequence->Add_Child(pAni5);

	//���� �޸���
	CBT_Play_Ani* pAni6 = Node_Ani("���� �޸��� ", 6, 0.9f);
	pSequence->Add_Child(pAni6);

	//���� ������ �ȱ�
	CBT_Play_Ani* pAni7 = Node_Ani("���� �ڷ� �ȱ� ", 7, 0.9f);
	pSequence->Add_Child(pAni7);

	//���� ���� �ȱ�
	CBT_Play_Ani* pAni8 = Node_Ani("���� ���� �ȱ� ", 8, 0.9f);
	pSequence->Add_Child(pAni8);

	//���� �տ����U �ȱ�
	CBT_Play_Ani* pAni9 = Node_Ani("���� �տ����U �ȱ� ", 9, 0.9f);
	pSequence->Add_Child(pAni9);

	//���� �տ��ʿ����� �ȱ�
	CBT_Play_Ani* pAni10 = Node_Ani("���� �տ��ʿ����� �ȱ� ", 10, 0.9f);
	pSequence->Add_Child(pAni10);

	//���� �տ��ʿ޹� �ȱ�
	CBT_Play_Ani* pAni11 = Node_Ani("���� �տ��ʿ޹� �ȱ� ", 11, 0.9f);
	pSequence->Add_Child(pAni11);

	//���� ������ �ȱ�
	CBT_Play_Ani* pAni12 = Node_Ani("���� ������ �ȱ� ", 12, 0.9f);
	pSequence->Add_Child(pAni12);

	//���� �ڿ����ʿ����� �ȱ�
	CBT_Play_Ani* pAni13 = Node_Ani("���� �ڿ����ʿ����� �ȱ� ", 13, 0.9f);
	pSequence->Add_Child(pAni13);

	//���� �� �����ʿ޹� �ȱ�
	CBT_Play_Ani* pAni14 = Node_Ani("���� �� �����ʿ޹� �ȱ� ", 14, 0.9f);
	pSequence->Add_Child(pAni14);

	//���� �� ���� �ȱ�
	CBT_Play_Ani* pAni15 = Node_Ani("���� �� ���� �ȱ� ", 15, 0.9f);
	pSequence->Add_Child(pAni15);

	//���� �� �ȱ�
	CBT_Play_Ani* pAni16 = Node_Ani("���� �� �ȱ� ", 16, 0.9f);
	pSequence->Add_Child(pAni16);

	//���� ������180��
	CBT_Play_Ani* pAni17 = Node_Ani("���� ������180�� ", 17, 0.9f);
	pSequence->Add_Child(pAni17);

	//���� ������ 90��
	CBT_Play_Ani* pAni18 = Node_Ani("���� ������ 90�� ", 18, 0.9f);
	pSequence->Add_Child(pAni18);

	//���� ���� 180��
	CBT_Play_Ani* pAni19 = Node_Ani("���� ���� 180�� ", 19, 0.9f);
	pSequence->Add_Child(pAni19);

	//���� ���� 90��
	CBT_Play_Ani* pAni20 = Node_Ani("���� ���� 90�� ", 20, 0.9f);
	pSequence->Add_Child(pAni20);

	//���� ������
	CBT_Play_Ani* pAni21 = Node_Ani("���� ������ ", 21, 0.9f);
	pSequence->Add_Child(pAni21);

	//���е�� �ֵ���ġ��
	CBT_Play_Ani* pAni22 = Node_Ani("���е�� �ֵ���ġ�� ", 22, 0.9f);
	pSequence->Add_Child(pAni22);

	//���з� �ķ�ġ��
	CBT_Play_Ani* pAni23 = Node_Ani("���з� �ķ�ġ�� ", 23, 0.9f);
	pSequence->Add_Child(pAni23);

	//���е�� ������
	CBT_Play_Ani* pAni24 = Node_Ani("���е�� ������ ", 24, 0.9f);
	pSequence->Add_Child(pAni24);

	//���з� ġ�� ����ġ��
	CBT_Play_Ani* pAni25 = Node_Ani("���з� ġ�� ����ġ�� ", 25, 0.9f);
	pSequence->Add_Child(pAni25);

	//�ָӴϿ��� ǥâ
	CBT_Play_Ani* pAni26 = Node_Ani("�ָӴϿ��� ǥâ ", 26, 0.9f);
	pSequence->Add_Child(pAni26);

	//��� ǥâ��������
	CBT_Play_Ani* pAni27 = Node_Ani("��� ǥâ �������� ", 27, 0.9f);
	pSequence->Add_Child(pAni27);

	//�缱�ķ�����
	CBT_Play_Ani* pAni28 = Node_Ani("�缱 �ķ����� ", 28, 0.9f);
	pSequence->Add_Child(pAni28);

	//ũ�� ����������
	CBT_Play_Ani* pAni29 = Node_Ani("ũ�� ������ ����", 29, 0.9f);
	pSequence->Add_Child(pAni29);

	//�״Ͻ� �佺 �ι�����
	CBT_Play_Ani* pAni30 = Node_Ani("�״Ͻ� �佺 �ι����� ", 30, 0.9f);
	pSequence->Add_Child(pAni30);

	//���� �������ô�ϸ鼭 ���
	CBT_Play_Ani* pAni31 = Node_Ani("���� �������ô �ϸ鼭 ��� ", 31, 0.9f);
	pSequence->Add_Child(pAni31);

	//�ɾƼ� ���ʿ����� ����
	CBT_Play_Ani* pAni32 = Node_Ani("�ɾƼ� ���ʿ��������� ", 32, 0.9f);
	pSequence->Add_Child(pAni32);

	//�齺���ϸ� ǥâ
	CBT_Play_Ani* pAni33 = Node_Ani("�齺���ϸ� ǥâ ", 33, 0.9f);
	pSequence->Add_Child(pAni33);

	//�޷��� ������ ����
	CBT_Play_Ani* pAni34 = Node_Ani("�޷��� ������ ���� ", 34, 0.9f);
	pSequence->Add_Child(pAni34);

	//�޷��� ������ ����
	CBT_Play_Ani* pAni35 = Node_Ani("�޷��� ������ ���� ", 35, 0.9f);
	pSequence->Add_Child(pAni35);

	//�޷��� ������ ��
	CBT_Play_Ani* pAni36 = Node_Ani("�޷��� ������ �� ", 36, 0.9f);
	pSequence->Add_Child(pAni36);

	//�״Ͻ� �佺 ��������
	CBT_Play_Ani* pAni37 = Node_Ani("�״Ͻ� �佺 �������� ", 37, 0.9f);
	pSequence->Add_Child(pAni37);

	//��û�Ÿ��� ũ�� ����
	CBT_Play_Ani* pAni38 = Node_Ani("��û�Ÿ��� ũ�� ���� ", 38, 0.9f);
	pSequence->Add_Child(pAni38);

	//�ճ�ġ����
	CBT_Play_Ani* pAni39 = Node_Ani("�ճ�ġ���� ", 39, 0.9f);
	pSequence->Add_Child(pAni39);

	//�յ����� ��ġ����
	CBT_Play_Ani* pAni40 = Node_Ani("�յ����� ��ġ���� ", 40, 0.9f);
	pSequence->Add_Child(pAni40);

	//�̿�� �ܹ��е���
	CBT_Play_Ani* pAni41 = Node_Ani("�̿�� �ܹ��е���", 41, 0.9f);
	pSequence->Add_Child(pAni41);

	//�� �⺻
	CBT_Play_Ani* pAni42 = Node_Ani("�� �⺻", 42, 0.9f);
	pSequence->Add_Child(pAni42);

	//���Ӹ��� ġ��
	CBT_Play_Ani* pAni43 = Node_Ani("���Ӹ��� ġ�� ", 43, 0.9f);
	pSequence->Add_Child(pAni43);

	//������
	CBT_Play_Ani* pAni44 = Node_Ani("������ ", 44, 0.9f);
	pSequence->Add_Child(pAni44);

	//�Ȳ�ġ ġ��
	CBT_Play_Ani* pAni45 = Node_Ani("�Ȳ�ġ ġ�� ", 45, 0.9f);
	pSequence->Add_Child(pAni45);

	//�Ѽ� �� ���
	CBT_Play_Ani* pAni46 = Node_Ani("�Ѽ� �� ��� ", 46, 0.9f);
	pSequence->Add_Child(pAni46);

	//�� ���ʺ��� ����
	CBT_Play_Ani* pAni47 = Node_Ani("�� ���ʺ��� ���� ", 47, 0.9f);
	pSequence->Add_Child(pAni47);

	//������� 2ȸ ���
	CBT_Play_Ani* pAni48 = Node_Ani("������� 2ȸ ��� ", 48, 0.9f);
	pSequence->Add_Child(pAni48);

	//�� ���
	CBT_Play_Ani* pAni49 = Node_Ani("�� ��� ", 49, 0.9f);
	pSequence->Add_Child(pAni49);

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	return NOERROR;
}

_int CNormalGenji::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	m_pAIControllerCom->Update_AIController(TimeDelta);

	return _int();
}

_int CNormalGenji::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	return _int();
}

HRESULT CNormalGenji::Render_GameObject()
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

HRESULT CNormalGenji::Add_Component(void* pArg)
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


	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_NormalGenji", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	// for.Com_AIController
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"AIController", L"Com_AIController", (CComponent**)&m_pAIControllerCom)))
		return E_FAIL;


	return NOERROR;
}

HRESULT CNormalGenji::SetUp_ConstantTable()
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

CNormalGenji * CNormalGenji::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CNormalGenji* pInstance = new CNormalGenji(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CNormalGenji");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNormalGenji::Clone_GameObject(void * pArg)
{
	// ��, ��, ����
	// 

	CNormalGenji* pInstance = new CNormalGenji(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CNormalGenji");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNormalGenji::Free()
{
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
