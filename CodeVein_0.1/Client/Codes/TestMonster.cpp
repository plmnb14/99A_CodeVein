#include "stdafx.h"
#include "..\Headers\TestMonster.h"

CTestMonster::CTestMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	D3DXMatrixIdentity(&m_matLastVP);
}

CTestMonster::CTestMonster(const CTestMonster & rhs)
	: CGameObject(rhs)
{
	D3DXMatrixIdentity(&m_matLastVP);
}

HRESULT CTestMonster::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CTestMonster::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(1.f, 0.f, 0.5f));
	m_pTransformCom->Set_Scale(_v3(0.1f, 0.1f, 0.1f));

	
	//CBlackBoard* pBlackBoard = CBlackBoard::Create();
	////pBlackBoard->Set_Value(L"Target_Pos", &_v3(-2.f, -2.f, -2.f), CBlackBoard::VALUE::VEC3);
	////pBlackBoard->Set_Value(L"A", &_bool(true), CBlackBoard::VALUE::BOOL);
	////_v3 temp = pBlackBoard->Get_V3Value(L"Target_Pos");

	//CBehaviorTree* pBehaviorTree = CBehaviorTree::Create();

	//CManagement*		pManagement = CManagement::Get_Instance();

	//CBT_Sequence* pSequence = static_cast<CBT_Sequence*>(pManagement->Clone_Node(L"Sequence", CBT_Node_Manager::COMPOSITE, &CBT_Sequence::INFO("Sequence1")));
	////CBT_Sequence* pSequence1 = static_cast<CBT_Sequence*>(pManagement->Clone_Node(L"Sequence", CBT_Node_Manager::COMPOSITE, &CBT_Sequence::INFO("Sequence2")));
	////CBT_Sequence* pSequence2 = static_cast<CBT_Sequence*>(pManagement->Clone_Node(L"Sequence", CBT_Node_Manager::COMPOSITE, nullptr));

	////CBT_Selector* pSelector = static_cast<CBT_Selector*>(pManagement->Clone_Node(L"Selector", CBT_Node_Manager::COMPOSITE, &CBT_Selector::INFO("Selecotr")));

	////CBT_Wait* pWait2 = static_cast<CBT_Wait*>(pManagement->Clone_Node(L"Wait", CBT_Node_Manager::TASK, &CBT_Wait::INFO("Wait2", 2, 0)));
	////CBT_Wait* CoolWait = static_cast<CBT_Wait*>(pManagement->Clone_Node(L"Wait", CBT_Node_Manager::TASK, &CBT_Wait::INFO("CoolWait",2)));
	////CBT_Wait* CoolWait22 = static_cast<CBT_Wait*>(pManagement->Clone_Node(L"Wait", CBT_Node_Manager::TASK, &CBT_Wait::INFO("Wait444", 4)));
	////CBT_Move* pMove = static_cast<CBT_Move*>(pManagement->Clone_Node(L"Move", CBT_Node_Manager::TASK, &CBT_Move::INFO(m_pTransformCom, 1, 5)));

	////static_cast<CBT_Compare*>(pManagement->Clone_Node(L"Play_Ani", CBT_Node_Manager::TASK, &CBT_Compare::INFO("Ani27", m_pMeshCom, 27, 0.6f)));

	//// �� -> ���׸� ��  ����  ����
	//CBT_Play_Ani* pAni27 = static_cast<CBT_Play_Ani*>(pManagement->Clone_Node(L"Play_Ani", CBT_Node_Manager::TASK, &CBT_Play_Ani::INFO("Ani27", m_pMeshCom, 27, 0.6f)));
	//CBT_Play_Ani* pAni26 = static_cast<CBT_Play_Ani*>(pManagement->Clone_Node(L"Play_Ani", CBT_Node_Manager::TASK, &CBT_Play_Ani::INFO("Ani26", m_pMeshCom, 26)));

	////���� ����

	//// 29 -> ����ź 


	//// 28 -> ���� ���� ���Ȱ�


	//// 25 -> �þ߰� �������� ���Ƽ� �������� ����


	//// 24 -> ������ ����


	//// 23 -> ���ݼ�


	//// 22 -> ������ ����


	//// 20 -> �ڵ��Ƽ� ����


	//// 19 -> ��ȸ�� �ѹ���  ����


	//// 18 -> Ʈ���ٹ̾�


	//// 17 -> �����ϸ鼭 5�� Ÿ���ϸ鼭 �Һ�


	////���� �����
	//// 15 -> ȥ�� ���� �� idle����

	//// 16 -> ������ ���� ��


	//// ����
	////14 -> �ڸ°� ����

	////13 -> �ո°� ����

	////12 -> �����۹� , ������ �� ���

	////11 -> ���ǵǸ�  2������ ���� ��

	////10 -> ȸ��

	////CBT_Simple_Parallel* pParallel = static_cast<CBT_Simple_Parallel*>(pManagement->Clone_Node(L"Simple_Parallel", CBT_Node_Manager::COMPOSITE, &CBT_Simple_Parallel::INFO("Parallel", CBT_Simple_Parallel::Mode::Immediate)));
	////CBT_Wait* pWait1 = static_cast<CBT_Wait*>(pManagement->Clone_Node(L"Wait", CBT_Node_Manager::TASK, &CBT_Wait::INFO("Wait1", 2, 1)));
	////CBT_Wait* pWait5 = static_cast<CBT_Wait*>(pManagement->Clone_Node(L"Wait", CBT_Node_Manager::TASK, &CBT_Wait::INFO("Wait3", 5)));
	////CBT_Wait* pWait4 = static_cast<CBT_Wait*>(pManagement->Clone_Node(L"Wait", CBT_Node_Manager::TASK, &CBT_Wait::INFO("Wait4", 4, 0)));

	////CBT_Cooldown* pCooldown1 = static_cast<CBT_Cooldown*>(pManagement->Clone_Node(L"Cooldown", CBT_Node_Manager::DECORATOR, &CBT_Cooldown::INFO("Colldown1", 1)));
	////CBT_Cooldown* pCooldown4 = static_cast<CBT_Cooldown*>(pManagement->Clone_Node(L"Cooldown", CBT_Node_Manager::DECORATOR, &CBT_Cooldown::INFO("Colldown4", 4)));

	////CBT_Loop* pLoop3 = static_cast<CBT_Loop*>(pManagement->Clone_Node(L"Loop", CBT_Node_Manager::DECORATOR, &CBT_Loop::INFO("Loop", 3)));
	////CBT_Loop* pLoop0 = static_cast<CBT_Loop*>(pManagement->Clone_Node(L"Loop", CBT_Node_Manager::DECORATOR, &CBT_Loop::INFO("Loop", 0)));

	////CBT_MoveDirectly* pMoveDirect = static_cast<CBT_MoveDirectly*>(pManagement->Clone_Node(L"MoveDirectly", CBT_Node_Manager::TASK, &CBT_MoveDirectly::INFO("MoveDirectly", m_pTransformCom, L"Target_Pos", 1, 3)));

	////CBT_ConeCheck* pConeCheck = static_cast<CBT_ConeCheck*>(pManagement->Clone_Node(L"ConeCheck", CBT_Node_Manager::DECORATOR, &CBT_ConeCheck::INFO("ConeCheck", L"Target_Pos", m_pTransformCom, 40, 5)));
	////CBT_Inverter* pInverter = static_cast<CBT_Inverter*>(pManagement->Clone_Node(L"Inverter", CBT_Node_Manager::DECORATOR, &CBT_Inverter::INFO("Inverter")));

	//if (FAILED(pBehaviorTree->Set_Child(pSequence))) return E_FAIL;
	////if (FAILED(pSelector->Add_Child(pSequence))) return E_FAIL;
	//if (FAILED(pSequence->Add_Child(pAni27))) return E_FAIL;
	//pSequence->Add_Child(pAni26);

	////Node_Parallel_Immediate(a, bbb::Immediate);


	////pCooldown4->Set_Child(pInverter);
	////pInverter->Set_Child(pWait1);
	////pConeCheck->Set_Child(pWait1);
	////pCooldown1->Set_Child(pLoop3);
	////pSelector->Add_Child(pWait4);
	////pLoop3->Set_Child(pWait1);
	////pCooldown4->Set_Child(pAni2);

	////if (FAILED(pBehaviorTree->Set_Child(pSequence))) return E_FAIL;
	////pSequence->Add_Child(pWait2);
	//////pSequence->Add_Child(pCooldown2);
	//////pCooldown2->Set_Child(CoolWait22);
	////pSequence->Add_Child(pSequence1);
	////pSequence->Add_Child(pWait3);
	////pSequence1->Add_Child(pParallel);
	////pParallel->Set_Main_Child(pWait4);
	////pParallel->Set_Sub_Child(pWait1);
	////pSequence1->Add_Child(pAni4);

	////pSequence->Add_Child(pParallel);
	////pParallel->Set_Main_Child(pSequence2);
	////pSequence2->Add_Child()
	////pParallel->Set_Sub_Child(pWait3);


	////pSequence->Add_Child(pParallel);
	////pParallel->Set_Main_Child(pWait2);
	////pParallel->Set_Sub_Child(pWait4);
	////m_pMeshCom->SetUp_Animation(0);



	//m_pAIControllerCom->Set_BeHaviorTree(pBehaviorTree);
	//m_pAIControllerCom->Set_BlackBoard(pBlackBoard);

	m_pMeshCom->SetUp_Animation(0);

	return NOERROR;
}

_int CTestMonster::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	//m_pAIControllerCom->Update_AIController(TimeDelta);

	return _int();
}

_int CTestMonster::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	m_dTimeDelta = TimeDelta;

	return _int();
}

HRESULT CTestMonster::Render_GameObject()
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
			m_pShaderCom->Begin_Pass(2);

			if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE_MAP))))
				return E_FAIL;

			m_pShaderCom->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			m_pShaderCom->End_Pass();
		}
	}

	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CTestMonster::Add_Component()
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

HRESULT CTestMonster::SetUp_ConstantTable()
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

	_mat matInvVP = m_pTransformCom->Get_WorldMat() * ViewMatrix * ProjMatrix;
	D3DXMatrixInverse(&matInvVP, nullptr, &matInvVP);
	if (FAILED(m_pShaderCom->Set_Value("g_matInvVP", &matInvVP, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matLastVP", &m_matLastVP, sizeof(_mat))))
		return E_FAIL;
	m_matLastVP = m_pTransformCom->Get_WorldMat() * ViewMatrix * ProjMatrix;
	Safe_Release(pManagement);

	return NOERROR;
}

CTestMonster * CTestMonster::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTestMonster* pInstance = new CTestMonster(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CTestMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTestMonster::Clone_GameObject(void * pArg)
{
	CTestMonster* pInstance = new CTestMonster(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CTestMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestMonster::Free()
{
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
