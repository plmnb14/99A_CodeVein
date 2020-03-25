#include "stdafx.h"
#include "..\Headers\Dummy_Target.h"

CDummy_Target::CDummy_Target(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CDummy_Target::CDummy_Target(const CDummy_Target & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummy_Target::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CDummy_Target::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransform->Set_Pos(_v3(0.f, -1.3f, 0.f));
	m_pTransform->Set_Scale(_v3(1.f, 1.f, 1.f));

	return NOERROR;
}

_int CDummy_Target::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	for(auto& iter :m_vecPhysicCollider)
		iter->Update(m_pTransform->Get_WorldMat());

	if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return _int();
}

_int CDummy_Target::Late_Update_GameObject(_double TimeDelta)
{
	m_dTimeDelta = TimeDelta;

	m_pDynamic_Mesh->SetUp_Animation_Lower(0);
	m_pDynamic_Mesh->SetUp_Animation_Upper(0);
	m_pDynamic_Mesh->SetUp_Animation_RightArm(0);

	return _int();
}

HRESULT CDummy_Target::Render_GameObject()
{
	if (nullptr == m_pRenderer ||
		nullptr == m_pDynamic_Mesh)
		return E_FAIL;

	m_pDynamic_Mesh->Play_Animation_Lower(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * 2.f);
	m_pDynamic_Mesh->Play_Animation_Upper(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * 2.f);
	m_pDynamic_Mesh->Play_Animation_RightArm(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * 2.f, false);

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();

	_uint iNumMeshContainer = _uint(m_pDynamic_Mesh->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pDynamic_Mesh->Get_NumMaterials(i);

		m_pDynamic_Mesh->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			m_pShader->Begin_Pass(0);

			if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pDynamic_Mesh->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE))))
				return E_FAIL;

			m_pShader->Commit_Changes();

			m_pDynamic_Mesh->Render_Mesh(i, j);

			m_pShader->End_Pass();
		}
	}

	m_pShader->End_Shader();

	g_pManagement->Gizmo_Draw_Capsule(m_vecPhysicCollider[0]->Get_CenterPos(), m_vecPhysicCollider[0]->Get_Radius());

	return NOERROR;
}

HRESULT CDummy_Target::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Player", L"Mesh_Dynamic", (CComponent**)&m_pDynamic_Mesh)))
		return E_FAIL;

	m_vecPhysicCollider.reserve(16);

	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	pCollider->Set_Radius(_v3{ 0.4f, 0.5f, 0.4f });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_CAPSULE);
	pCollider->Set_CapsuleLength(1.8f);
	pCollider->Set_CenterPos(m_pTransform->Get_Pos() + _v3{ 0.f , pCollider->Get_Radius().y , 0.f });

	m_vecPhysicCollider.push_back(pCollider);

	m_pDynamic_Mesh->SetUp_Animation_Lower(0);
	m_pDynamic_Mesh->SetUp_Animation_Upper(0);
	m_pDynamic_Mesh->SetUp_Animation_RightArm(0);

	return NOERROR;
}

HRESULT CDummy_Target::SetUp_ConstantTable()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat		ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;

	Safe_Release(pManagement);

	return NOERROR;
}

CDummy_Target * CDummy_Target::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDummy_Target* pInstance = new CDummy_Target(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CDummy_Target");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDummy_Target::Clone_GameObject(void * pArg)
{
	CDummy_Target* pInstance = new CDummy_Target(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CDummy_Target");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy_Target::Free()
{
	for (auto& iter : m_vecPhysicCollider)
		Safe_Release(iter);

	m_vecPhysicCollider.clear();
	m_vecPhysicCollider.shrink_to_fit();

	Safe_Release(m_pTransform);
	Safe_Release(m_pDynamic_Mesh);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);

	CGameObject::Free();
}
