#include "stdafx.h"
#include "..\Headers\PoisonButterfly.h"

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

	// �� -> ���׸� ��  ����  ����
	CBT_Play_Ani* pAni27 = Node_Ani("��", 27, 0.6f);
	CBT_Play_Ani* pAni26 = Node_Ani("���׸�����", 26, 0.9f);
	pSequence->Add_Child(pAni27);
	pSequence->Add_Child(pAni26);

	// 29 -> ����ź
	CBT_Play_Ani* pAni29 = Node_Ani("5�߽��", 29, 0.9f);
	pSequence->Add_Child(pAni29);

	// 28 -> ���� ���� ���Ȱ�
	CBT_Play_Ani* pAni28 = Node_Ani("���� ���� ���Ȱ�", 28, 0.9f);
	pSequence->Add_Child(pAni28);

	// 25 -> �þ߰� �������� ���Ƽ� �������� ����
	CBT_Play_Ani* pAni25 = Node_Ani("�������� ����", 25, 0.9f);
	pSequence->Add_Child(pAni25);

	// 24 -> ������ ����
	CBT_Play_Ani* pAni24 = Node_Ani("������ ����", 24, 0.9f);
	pSequence->Add_Child(pAni24);

	// 23 -> ���ݼ�
	CBT_Play_Ani* pAni23 = Node_Ani("���ݼ�", 23, 0.9f);
	pSequence->Add_Child(pAni23);

	// 22 -> ������ ����
	CBT_Play_Ani* pAni22 = Node_Ani("������ ����", 22, 0.9f);
	pSequence->Add_Child(pAni22);

	// 20 -> �ڵ��Ƽ� ����
	CBT_Play_Ani* pAni20 = Node_Ani("�ڵ��Ƽ� ����", 20, 0.9f);
	pSequence->Add_Child(pAni20);

	// 19 -> ��ȸ�� �ѹ���  ����
	CBT_Play_Ani* pAni19 = Node_Ani("��ȸ�� �ѹ��� ����", 19, 0.9f);
	pSequence->Add_Child(pAni19);
	
	// 18 -> Ʈ���ٹ̾�
	CBT_Play_Ani* pAni18 = Node_Ani("Ʈ���ٹ̾�", 18, 0.9f);
	pSequence->Add_Child(pAni18);

	// 17 -> �����ϸ鼭 5�� Ÿ�� �� �Һ�
	CBT_Play_Ani* pAni17 = Node_Ani("�����ϸ鼭 5�� Ÿ�� �� �Һ�", 17, 0.9f);
	pSequence->Add_Child(pAni17);

	//���� �����
	// 15 -> ȥ�� ���� �� idle����
	CBT_Play_Ani* pAni15 = Node_Ani("ȥ�� ���� �� IDLE����", 15, 0.9f);
	pSequence->Add_Child(pAni15);


	// 16 -> ������ ���� ��
	CBT_Play_Ani* pAni16 = Node_Ani("������ ���� ��", 16, 0.9f);
	pSequence->Add_Child(pAni16);

	// ����
	//13 -> �ڸ°� ����
	CBT_Play_Ani* pAni13 = Node_Ani("�ڸ°� ����", 13, 0.9f);
	pSequence->Add_Child(pAni13);

	//14 -> �ո°� ����
	CBT_Play_Ani* pAni14 = Node_Ani("�ո°� ����", 14, 0.9f);
	pSequence->Add_Child(pAni14);


	//12 -> �����۹� , ������ �� ���
	CBT_Play_Ani* pAni12 = Node_Ani("���", 12, 0.9f);
	pSequence->Add_Child(pAni12);

	//11 -> ���ǵǸ�  2������ ���� ��
	CBT_Play_Ani* pAni11 = Node_Ani("���ǵǸ� 2������", 11, 0.9f);
	pSequence->Add_Child(pAni11);

	//10 -> ȸ��
	CBT_Play_Ani* pAni10 = Node_Ani("ȸ��", 10, 0.9f);
	pSequence->Add_Child(pAni10);



	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	return NOERROR;
}

_int CPoisonButterfly::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	m_pAIControllerCom->Update_AIController(TimeDelta);

	return _int();
}

_int CPoisonButterfly::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	return _int();
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

	Safe_Release(pManagement);

	return NOERROR;
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
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
