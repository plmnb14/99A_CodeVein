#include "..\Headers\RenderObject.h"
#include "Management.h"

CRenderObject::CRenderObject(_Device _pGraphicDev)
	:CGameObject(_pGraphicDev),
	m_eGroup(RENDER_NONALPHA),
	m_iIndex(0)
{
}

CRenderObject::CRenderObject(const CRenderObject& rhs)
	: CGameObject(rhs)
	, m_eGroup(rhs.m_eGroup)
	, m_iIndex(rhs.m_iIndex)
{
	memcpy(m_szName, rhs.m_szName, sizeof(_tchar[MAX_STR]));

	Add_Essentional_Copy();
	Default_Setting();

	m_pTransform->Set_Info(rhs.m_pTransform->Get_Info());
}


CRenderObject::~CRenderObject()
{
}

_int CRenderObject::Update_GameObject(_double _TimeDelta)
{
	CGameObject::LateInit_GameObject();
	CGameObject::Update_GameObject(_TimeDelta);

	Update_Collider();

	m_pRenderer->Add_RenderList(RENDER_NONALPHA, this);

	return S_OK;
}

_int CRenderObject::Late_Update_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CRenderObject::Render_GameObject()
{
	Init_Shader();

	m_pShader->Begin_Shader();

	_ulong dwNumSubSet = m_pMesh_Static->Get_NumMaterials();

	for (_ulong i = 0; i < dwNumSubSet; ++i)
	{
		m_pShader->Begin_Pass(m_dwPassNum);

		if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pMesh_Static->Get_Texture(i, MESHTEXTURE::TYPE_DIFFUSE))))

		m_pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		m_pShader->End_Pass();
	}

	m_pShader->End_Shader();


	CGizmo::Get_Instance()->Draw_AABB(m_pGraphic_Dev, m_pCollider->Get_GizmoPos(), m_pCollider->Get_CenterPos(), m_pTransform->Get_Size());

	if (m_bIsSelected)
		CGizmo::Get_Instance()->Draw_XYZ(m_pGraphic_Dev, m_pTransform->Get_Pos(), m_pTransform->Get_Axis(AXIS_Z), m_pTransform->Get_Axis(AXIS_X));

	return S_OK;
}

void CRenderObject::Update_Collider()
{
	IF_NOT_NULL(m_pCollider)
		m_pCollider->Update(m_pTransform->Get_Pos() + _v3{ 0.f, m_pCollider->Get_Radius().y ,0.f });
}

HRESULT CRenderObject::Add_Essentional()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Player", L"Mesh", (CComponent**)&m_pMesh_Static)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderObject::Add_Essentional_Copy()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Player", L"Mesh", (CComponent**)&m_pMesh_Static)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderObject::Default_Setting()
{
	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);

	return E_NOTIMPL;
}

void CRenderObject::Change_Mesh(const _tchar* _MeshName)
{
	if (!lstrcmp(m_szName, _MeshName))
		return;

	lstrcpy(m_szName, _MeshName);

	Safe_Release(m_pMesh_Static);

	auto& iter = m_pmapComponents.find(L"Static_Mesh");

	m_pMesh_Static = static_cast<CMesh_Static*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, m_szName));

	return;
}

void CRenderObject::Set_RenderGroup(RENDERID _eGroup)
{
	m_eGroup = _eGroup;
}

CRenderObject * CRenderObject::Create(_Device _pGraphicDev)
{
	CRenderObject* pInstance = new CRenderObject(_pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRenderObject * CRenderObject::CreateClone(CRenderObject* _pCopy)
{
	return new CRenderObject(*_pCopy);
}

void CRenderObject::Free()
{
	CGameObject::Free();
}

CGameObject * CRenderObject::Clone_GameObject(void * pArg)
{
	return nullptr;
}

HRESULT CRenderObject::Initialize()
{
	Add_Essentional();
	Default_Setting();

	return S_OK;
}

HRESULT CRenderObject::LateInit_GameObject()
{
	return S_OK;
}

void CRenderObject::Init_Shader()
{
	_mat		matWorld, matView, matProj;

	matWorld = m_pTransform->Get_WorldMat();
	m_pGraphic_Dev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphic_Dev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pShader->Set_Value("g_matWorld", &matWorld, sizeof(_mat));
	m_pShader->Set_Value("g_matView", &matView, sizeof(_mat));
	m_pShader->Set_Value("g_matProj", &matProj, sizeof(_mat));
}
