#include "..\Headers\RenderObject.h"
#include "Management.h"

CRenderObject::CRenderObject(_Device _pGraphicDev)
	:CGameObject(_pGraphicDev),
	m_eGroup(RENDER_NONALPHA),
	m_iIndex(0)
{
}

CRenderObject::CRenderObject(const CRenderObject& rhs)
	: CGameObject(rhs.m_pGraphic_Dev)
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
	if (false == m_pOptimization->Check_InFrustumforObject(&m_pTransform->Get_Pos(), 20.f))
		return NO_EVENT;

	CGameObject::LateInit_GameObject();
	CGameObject::Update_GameObject(_TimeDelta);

	if (true == m_bOnTool)
	{
		Update_Collider();
	}

	else if (false == m_bOnTool)
	{
		m_pRenderer->Add_RenderList(RENDER_NONALPHA, this);
		m_pRenderer->Add_RenderList(RENDER_MOTIONBLURTARGET, this);
		//m_pRenderer->Add_RenderList(RENDER_SHADOWTARGET, this);
	}

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
		m_iPass = m_pMesh_Static->Get_MaterialPass(i);

		m_pShader->Begin_Pass(m_iPass);

		m_pShader->Set_StaticTexture_Auto(m_pMesh_Static, i);

		m_pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		m_pShader->End_Pass();
	}

	m_pShader->End_Shader();


	CGizmo::Get_Instance()->Draw_AABB(m_pCollider->Get_GizmoPos(), m_pCollider->Get_CenterPos(), m_pTransform->Get_Size());

	if (m_bIsSelected)
		CGizmo::Get_Instance()->Draw_XYZ(m_pTransform->Get_Pos(), m_pTransform->Get_Axis(AXIS_Z), m_pTransform->Get_Axis(AXIS_X));

	return S_OK;
}

HRESULT CRenderObject::Render_GameObject_SetPass(CShader* pShader, _int iPass)
{
	if (nullptr == pShader ||
		nullptr == m_pMesh_Static)
		return E_FAIL;

	_mat		ViewMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
		return E_FAIL;

	m_matLastWVP = m_pTransform->Get_WorldMat() * ViewMatrix * ProjMatrix;

	_bool bMotionBlur = true;
	if (FAILED(pShader->Set_Bool("g_bMotionBlur", bMotionBlur)))
		return E_FAIL;
	_bool bDecalTarget = true;
	if (FAILED(pShader->Set_Bool("g_bDecalTarget", bDecalTarget)))
		return E_FAIL;

	_float fBloomPower = 0.5f;
	if (FAILED(pShader->Set_Value("g_fBloomPower", &fBloomPower, sizeof(_float))))
		return E_FAIL;

	_ulong dwNumSubSet = m_pMesh_Static->Get_NumMaterials();

	for (_ulong i = 0; i < dwNumSubSet; ++i)
	{
		pShader->Begin_Pass(iPass);

		pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		pShader->End_Pass();
	}

	return NOERROR;
}

void CRenderObject::Update_Collider()
{
	IF_NOT_NULL(m_pCollider)
	{
		m_pCollider->Set_Radius(V3_ONE);
		//m_pCollider->SetUp_Radius_Auto(V3_ONE, m_pMesh_Static->Get_VtxMin(), m_pMesh_Static->Get_VtxMax());
		m_pCollider->Update(m_pTransform->Get_Pos() + _v3{ 0.f, m_pCollider->Get_Radius().y ,0.f });
	}
}

HRESULT CRenderObject::Add_Essentional()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_DefaultBox", L"Com_StaticMesh", (CComponent**)&m_pMesh_Static)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	// for.Com_Optimization
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Optimization", L"Com_Opimaization", (CComponent**)& m_pOptimization)))
		return E_FAIL;

	lstrcpy(m_szName, L"Mesh_DefaultBox");

	return S_OK;
}

HRESULT CRenderObject::Add_Essentional_Copy()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, m_szName, L"Com_StaticMesh", (CComponent**)&m_pMesh_Static)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	// for.Com_Optimization
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Optimization", L"Com_Opimaization", (CComponent**)& m_pOptimization)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderObject::Default_Setting()
{
	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);

	return S_OK;
}

void CRenderObject::Change_Mesh(const _tchar* _MeshName)
{
	// 이름 비교해서 같으면 Return
	if (!lstrcmp(m_szName, _MeshName))
		return;

	// 다르다면 이름 갱신하고,
	lstrcpy(m_szName, _MeshName);

	// 컴포넌트에 있는 매쉬 찾아서
	auto& iter = m_pmapComponents.find(L"Com_StaticMesh");

	// 둘 다 해제
	Safe_Release(m_pMesh_Static);
	Safe_Release(iter->second);

	// Release 한 컴포넌트에 새로이 Clone 받음.
	iter->second = m_pMesh_Static = static_cast<CMesh_Static*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, m_szName));
	Safe_AddRef(iter->second);

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
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollider);
	Safe_Release(m_pMesh_Static);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pOptimization);

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

	float fRimValue = 0.f;
	m_pShader->Set_Value("g_fRimAlpha", &fRimValue, sizeof(_float));
}
