#include "stdafx.h"
#include "..\Headers\BlackWolf.h"

CBlackWolf::CBlackWolf(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CBlackWolf::CBlackWolf(const CBlackWolf & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlackWolf::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CBlackWolf::Ready_GameObject(void * pArg)
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


	// �ȱ�
	CBT_Play_Ani* pAni0 = Node_Ani("�ȱ�", 0, 0.9f);
	pSequence->Add_Child(pAni0);

	// ������ 180�� ����
	CBT_Play_Ani* pAni1 = Node_Ani("������ 180�� ����", 1, 0.9f);
	pSequence->Add_Child(pAni1);

	// ������ 90�� ����
	CBT_Play_Ani* pAni2= Node_Ani("������ 90�� ����", 2, 0.9f);
	pSequence->Add_Child(pAni2);

	// ���� 180�� ����
	CBT_Play_Ani* pAni3 = Node_Ani("���� 90�� ����", 3, 0.9f);
	pSequence->Add_Child(pAni3);

	// ���� 90�� ����
	CBT_Play_Ani* pAni4 = Node_Ani("���� 180�� ����", 4, 0.9f);
	pSequence->Add_Child(pAni4);

	// ���� 2 
	CBT_Play_Ani* pAni5 = Node_Ani("����2", 5, 0.9f);
	pSequence->Add_Child(pAni5);

	// ���� 1 
	CBT_Play_Ani* pAni6 = Node_Ani("����1", 6, 0.9f);
	pSequence->Add_Child(pAni6);

	// ��ȭ �ȱ�
	CBT_Play_Ani* pAni7= Node_Ani("��ȭ �ȱ�", 7, 0.9f);
	pSequence->Add_Child(pAni7);

	// ��ȭ ���� �ɱ�2
	CBT_Play_Ani* pAni8 = Node_Ani("��ȭ ���� �ɱ�2", 8, 0.9f);
	pSequence->Add_Child(pAni8);

	// ��ȭ ���� �ɱ�1
	CBT_Play_Ani* pAni9 = Node_Ani("��ȭ ���� �ɱ�1", 9, 0.9f);
	pSequence->Add_Child(pAni9);

	// ��ȭ ���� �Ա� 2
	CBT_Play_Ani* pAni10 = Node_Ani("��ȭ ���� �ɱ�2", 10, 0.9f);
	pSequence->Add_Child(pAni10);

	// ��ȭ ���� �Ա� 1
	CBT_Play_Ani* pAni11 = Node_Ani("��ȭ ���� �Ա� 1", 11, 0.9f);
	pSequence->Add_Child(pAni11);

	// ��ȭ �ɱ� Idle
	CBT_Play_Ani* pAni12 = Node_Ani("��ȭ �ɱ� �⺻", 12, 1.0f);
	pSequence->Add_Child(pAni12);

	// ��ȭ Idle
	CBT_Play_Ani* pAni13 = Node_Ani("��ȭ �⺻", 13, 1.0f);
	pSequence->Add_Child(pAni13);

	// ��ȭ �Ա� Idle
	CBT_Play_Ani* pAni14 = Node_Ani("��ȭ �Ա� �⺻", 14, 1.0f);
	pSequence->Add_Child(pAni14);

	// Idle
	CBT_Play_Ani* pAni15 = Node_Ani("�⺻", 15, 1.0f);
	pSequence->Add_Child(pAni15);

	// ���ϰ� �Ѿ��� ����
	CBT_Play_Ani* pAni16 = Node_Ani("���ϰ� �Ѿ��� ���� ", 16, 0.9f);
	pSequence->Add_Child(pAni16);

	// ���ϰ� �Ѿ��� ����
	CBT_Play_Ani* pAni17 = Node_Ani("���ϰ� �Ѿ��� ���� ", 17, 0.9f);
	pSequence->Add_Child(pAni17);

	// ���ϰ� �Ѿ��� ��
	CBT_Play_Ani* pAni18 = Node_Ani("���ϰ� �Ѿ��� �� ", 18, 0.9f);
	pSequence->Add_Child(pAni18);

	// ���ϰ� �Ѿ��� ����
	CBT_Play_Ani* pAni19 = Node_Ani("���ϰ� �Ѿ��� ���� ", 19, 0.9f);
	pSequence->Add_Child(pAni19);

	// ���ϰ� �Ѿ��� ����
	CBT_Play_Ani* pAni20 = Node_Ani("���ϰ� �Ѿ��� ���� ", 20, 0.9f);
	pSequence->Add_Child(pAni20);

	// ���ϰ� �Ѿ��� ��
	CBT_Play_Ani* pAni21 = Node_Ani("���ϰ� �Ѿ��� �� ", 21, 0.9f);
	pSequence->Add_Child(pAni21);

	// ȸ��
	CBT_Play_Ani* pAni22 = Node_Ani("ȸ�� ", 22, 0.9f);
	pSequence->Add_Child(pAni22);

	// �߾� ����
	CBT_Play_Ani* pAni23 = Node_Ani("�߾����� ", 23, 0.9f);
	pSequence->Add_Child(pAni23);

	// ��Ʋ�Ÿ��ٰ� ����
	CBT_Play_Ani* pAni24 = Node_Ani("��Ʋ�Ÿ��ٰ� ���� ", 24, 0.9f);
	pSequence->Add_Child(pAni24);

	// �տ��� ����
	CBT_Play_Ani* pAni25 = Node_Ani("�տ��� ���� ", 25, 0.9f);
	pSequence->Add_Child(pAni25);

	// �ڿ��� ����
	CBT_Play_Ani* pAni26 = Node_Ani("�ڿ��� ���� ", 26, 0.9f);
	pSequence->Add_Child(pAni26);

	// �� �� �� ����
	CBT_Play_Ani* pAni27 = Node_Ani("�� �� �� ���� ", 27, 0.9f);
	pSequence->Add_Child(pAni27);

	// ������ 90�� ���� ����
	CBT_Play_Ani* pAni28 = Node_Ani("������ 90�� ���� ���� ", 28, 0.9f);
	pSequence->Add_Child(pAni28);

	// ���� 90�� ���� ����
	CBT_Play_Ani* pAni29 = Node_Ani("���� 90�� ���� ���� ", 29, 0.9f);
	pSequence->Add_Child(pAni29);

	// ���� ����
	CBT_Play_Ani* pAni30 = Node_Ani("���� ���� ", 30, 0.9f);
	pSequence->Add_Child(pAni30);

	m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	return NOERROR;
}

_int CBlackWolf::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	m_pAIControllerCom->Update_AIController(TimeDelta);

	return _int();
}

_int CBlackWolf::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	return _int();
}

HRESULT CBlackWolf::Render_GameObject()
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

HRESULT CBlackWolf::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_BlackWolf", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	// for.Com_AIController
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"AIController", L"Com_AIController", (CComponent**)&m_pAIControllerCom)))
		return E_FAIL;


	return NOERROR;
}

HRESULT CBlackWolf::SetUp_ConstantTable()
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

CBlackWolf * CBlackWolf::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBlackWolf* pInstance = new CBlackWolf(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CBlackWolf");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBlackWolf::Clone_GameObject(void * pArg)
{
	CBlackWolf* pInstance = new CBlackWolf(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CBlackWolf");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlackWolf::Free()
{
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
