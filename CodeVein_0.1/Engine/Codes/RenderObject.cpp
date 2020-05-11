#include "..\Headers\RenderObject.h"
#include "Management.h"

CRenderObject::CRenderObject(_Device _pGraphicDev)
	:CGameObject(_pGraphicDev)
{
}

CRenderObject::CRenderObject(const CRenderObject& rhs)
	: CGameObject(rhs)
{

}

CRenderObject::CRenderObject(const CRenderObject & rhs, _bool _OnTool)
	:CGameObject(rhs)
	, m_eGroup(rhs.m_eGroup)
	, m_iIndex(rhs.m_iIndex)
{
	memcpy(m_szName, rhs.m_szName, sizeof(_tchar[STR_128]));

	Add_Essentional_Copy();
	Default_Setting();

	m_pTransform->Set_Info(rhs.m_pTransform->Get_Info());
}


CRenderObject::~CRenderObject()
{
}

_int CRenderObject::Update_GameObject(_double _TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (false == m_pOptimization->Check_InFrustumforObject(&m_pTransform->Get_Pos(), 10.f))
		return NO_EVENT;

	if(nullptr == m_pMesh_Static)
		return NO_EVENT;

	CGameObject::LateInit_GameObject();
	CGameObject::Update_GameObject(_TimeDelta);

	Update_Collider();

	return S_OK;
}

_int CRenderObject::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (true == m_bOnTool)
	{
		Update_Collider();
	}

	else if (false == m_bOnTool)
	{
		m_pRenderer->Add_RenderList(RENDER_SHADOWTARGET, this);

		if (false == m_pOptimization->Check_InFrustumforObject(&m_pTransform->Get_Pos(), 10.f))
			return NO_EVENT;

		m_pRenderer->Add_RenderList(RENDER_NONALPHA, this);
		m_pRenderer->Add_RenderList(RENDER_MOTIONBLURTARGET, this);
	}

	return _int();
}

HRESULT CRenderObject::Render_GameObject()
{
	if (false == m_bEnable)
		return NO_EVENT;

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

HRESULT CRenderObject::Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur)
{
	if (false == m_bEnable)
		return NO_EVENT;

	if (nullptr == pShader ||
		nullptr == m_pMesh_Static)
		return E_FAIL;
	//============================================================================================
	// 공통 변수
	//============================================================================================

	_mat	ViewMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	_mat	ProjMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);
	_mat	WorldMatrix = m_pTransform->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &WorldMatrix, sizeof(_mat))))
		return E_FAIL;

	//============================================================================================
	// 모션 블러 상수
	//============================================================================================
	if (_bIsForMotionBlur)
	{
		if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
			return E_FAIL;

		m_matLastWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		_bool bMotionBlur = true;
		if (FAILED(pShader->Set_Bool("g_bMotionBlur", bMotionBlur)))
			return E_FAIL;
		_bool bDecalTarget = true;
		if (FAILED(pShader->Set_Bool("g_bDecalTarget", bDecalTarget)))
			return E_FAIL;
		_float fBloomPower = 10.f;
		if (FAILED(pShader->Set_Value("g_fBloomPower", &fBloomPower, sizeof(_float))))
			return E_FAIL;
	}

	//============================================================================================
	// 기타 상수
	//============================================================================================
	else
	{
		_mat matWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		if (FAILED(pShader->Set_Value("g_matWVP", &matWVP, sizeof(_mat))))
			return E_FAIL;
	}



	//============================================================================================
	// 쉐이더 시작
	//============================================================================================

	_ulong dwNumSubSet = m_pMesh_Static->Get_NumMaterials();

	for (_ulong i = 0; i < dwNumSubSet; ++i)
	{
		pShader->Begin_Pass(iPass);

		pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		pShader->End_Pass();
	}

	//============================================================================================

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

CRenderObject * CRenderObject::Create_For_Tool(_Device _pGraphicDev)
{
	CRenderObject* pInstance = new CRenderObject(_pGraphicDev);

	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRenderObject * CRenderObject::CreateClone_For_Tool(CRenderObject * _pCopy, _bool _OnTool)
{
	return new CRenderObject(*_pCopy , _OnTool);
}

CRenderObject * CRenderObject::Create(_Device _pGraphicDev)
{
	CRenderObject* pInstance = new CRenderObject(_pGraphicDev);

	if (FAILED(pInstance->Initialize_For_Protoype()))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
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
	CRenderObject* pInstance = new CRenderObject(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CRenderObject::Initialize_For_Protoype()
{
	return S_OK;
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

	//=============================================================================================
	// 쉐이더 재질정보 수치 입력
	//=============================================================================================
	_float	fEmissivePower = 5.f;	// 이미시브 : 높을 수록, 자체 발광이 강해짐.
	_float	fSpecularPower = 0.5f;	// 메탈니스 : 높을 수록, 정반사가 강해짐.
	_float	fRoughnessPower = 1.f;	// 러프니스 : 높을 수록, 빛 산란이 적어짐(빛이 응집됨).
	_float	fMinSpecular = 0.1f;	// 최소 빛	: 최소 단위의 빛을 더해줌.
	_float	fID_R = 1.0f;	// ID_R : R채널 ID 값 , 1이 최대
	_float	fID_G = 0.5f;	// ID_G : G채널 ID 값 , 1이 최대
	_float	fID_B = 0.1f;	// ID_B	: B채널 ID 값 , 1이 최대
	_float	fRimAlpha = 0.0f;	// ID_B	: B채널 ID 값 , 1이 최대

	if (!lstrcmp(m_szName, L"Mesh_Home_Floor_One"))
	{
		fSpecularPower = 0.1f;
		fMinSpecular = 0.f;
		fRoughnessPower = 0.85f;
	}

	if (FAILED(m_pShader->Set_Value("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return;
	if (FAILED(m_pShader->Set_Value("g_fSpecularPower", &fSpecularPower, sizeof(_float))))
		return;
	if (FAILED(m_pShader->Set_Value("g_fRoughnessPower", &fRoughnessPower, sizeof(_float))))
		return;
	if (FAILED(m_pShader->Set_Value("g_fMinSpecular", &fMinSpecular, sizeof(_float))))
		return;
	if (FAILED(m_pShader->Set_Value("g_fID_R_Power", &fID_R, sizeof(_float))))
		return;
	if (FAILED(m_pShader->Set_Value("g_fID_G_Power", &fID_G, sizeof(_float))))
		return;
	if (FAILED(m_pShader->Set_Value("g_fID_B_Power", &fID_B, sizeof(_float))))
		return;
	if (FAILED(m_pShader->Set_Value("g_fRimAlpha", &fRimAlpha, sizeof(_float))))
		return;
	//=============================================================================================
}

HRESULT CRenderObject::Ready_GameObject(void * pAvg)
{
	OBJ_INFO Info = *(OBJ_INFO*)(pAvg);

	Add_Components(Info.szMeshName);

	m_pTransform->Set_Pos(Info.vPos);
	m_pTransform->Set_Angle(Info.vAngle);
	m_pTransform->Set_Scale(Info.vScale);

	return S_OK;
}

HRESULT CRenderObject::Add_Components(_tchar * szMeshName)
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, szMeshName, L"Com_StaticMesh", (CComponent**)&m_pMesh_Static)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	// for.Com_Optimization
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Optimization", L"Com_Opimaization", (CComponent**)& m_pOptimization)))
		return E_FAIL;

	lstrcpy(m_szName, szMeshName);

	return S_OK;
}
