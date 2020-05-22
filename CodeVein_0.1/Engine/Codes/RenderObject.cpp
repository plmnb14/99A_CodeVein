#include "..\Headers\RenderObject.h"
#include "Management.h"

CRenderObject::CRenderObject(_Device _pGraphicDev)
	:CGameObject(_pGraphicDev)
{
	ZeroMemory(&m_tPBRInfo, sizeof(PBR));
}

CRenderObject::CRenderObject(const CRenderObject& rhs)
	: CGameObject(rhs),
	m_tPBRInfo(rhs.m_tPBRInfo)
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

	if(nullptr == m_pMesh_Static)
		return NO_EVENT;

	if (nullptr == m_pOptimization)
		return NO_EVENT;

	_float fFrustumRadius = m_bAdvencedCull ? 30.f : 10.f;
	m_bInFrustum = m_pOptimization->Check_InFrustumforObject(&m_pTransform->Get_Pos(), fFrustumRadius);

	if (false == m_bUpdated)
	{
		// 스태틱은 한번만 배치되면 되서 계속 갱신할 필요없음
		//m_bUpdated = true;

		CGameObject::LateInit_GameObject();
		CGameObject::Update_GameObject(_TimeDelta);
	}

	//if (m_bOnTool)
	//	Update_Collider();

	return S_OK;
}

_int CRenderObject::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable ||
		nullptr == m_pOptimization ||
		nullptr == m_pRenderer)
		return NO_EVENT;

	m_pRenderer->Add_RenderList(RENDER_SHADOWTARGET, this);

	if (m_bInFrustum)
	{
		m_pRenderer->Add_RenderList(RENDER_NONALPHA, this);
		m_pRenderer->Add_RenderList(RENDER_MOTIONBLURTARGET, this);
	}

	return _int();
}

HRESULT CRenderObject::Render_GameObject()
{
	if (false == m_bEnable)
		return NO_EVENT;

	CManagement* pManagement = CManagement::Get_Instance();
	Safe_AddRef(pManagement);

	_mat matView, matProj;

	matView = pManagement->Get_Transform(D3DTS_VIEW);
	matProj = pManagement->Get_Transform(D3DTS_PROJECTION);

	Init_Shader(m_pShader);

	m_pShader->Set_Value("g_matView", matView, sizeof(_mat));
	m_pShader->Set_Value("g_matProj", matProj, sizeof(_mat));

	m_pShader->Begin_Shader();

	_ulong dwSubCnt = m_pMesh_Static->Get_NumMaterials();

	for (_ulong i = 0; i < dwSubCnt; ++i)
	{
		m_iPass = m_pMesh_Static->Get_MaterialPass(i);

		m_pShader->Begin_Pass(m_iPass);

		m_pShader->Set_StaticTexture_Auto(m_pMesh_Static, i);

		m_pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		m_pShader->End_Pass();
	}

	m_pShader->End_Shader();


	Safe_Release(pManagement);

	//CGizmo::Get_Instance()->Draw_AABB(m_pCollider->Get_GizmoPos(), m_pCollider->Get_CenterPos(), m_pTransform->Get_Size());
	//
	//if (m_bIsSelected)
	//	CGizmo::Get_Instance()->Draw_XYZ(m_pTransform->Get_Pos(), m_pTransform->Get_Axis(AXIS_Z), m_pTransform->Get_Axis(AXIS_X));

	return S_OK;
}

HRESULT CRenderObject::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	if (false == m_bEnable)
		return NO_EVENT;

	Init_Shader(pShader);

	_ulong dwSubsetCnt = m_pMesh_Static->Get_NumMaterials();

	for (_ulong i = 0; i < dwSubsetCnt; ++i)
	{
		m_iPass = m_pMesh_Static->Get_MaterialPass(i);

		pShader->Begin_Pass(m_iPass);

		pShader->Set_StaticTexture_Auto(m_pMesh_Static, i);

		pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		pShader->End_Pass();
	}

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

	_mat	WorldMatrix = m_pTransform->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &WorldMatrix, sizeof(_mat))))
		return E_FAIL;

	CManagement* pManagement = CManagement::Get_Instance();
	Safe_AddRef(pManagement);

	_mat matView, matProj;

	matView = pManagement->Get_Transform(D3DTS_VIEW);
	matProj = pManagement->Get_Transform(D3DTS_PROJECTION);

	//============================================================================================
	// 모션 블러 상수
	//============================================================================================
	if (_bIsForMotionBlur)
	{
		if (FAILED(pShader->Set_Value("g_matView", &matView, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matProj", &matProj, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
			return E_FAIL;

		m_matLastWVP = WorldMatrix * matView * matProj;

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
		_mat matWVP = WorldMatrix * matView * matProj;

		if (FAILED(pShader->Set_Value("g_matWVP", &matWVP, sizeof(_mat))))
			return E_FAIL;
	}



	//============================================================================================
	// 쉐이더 시작
	//============================================================================================

	_ulong dwSubCnt = m_pMesh_Static->Get_NumMaterials();

	for (_ulong i = 0; i < dwSubCnt; ++i)
	{
		pShader->Begin_Pass(iPass);

		pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		pShader->End_Pass();
	}

	//============================================================================================

	Safe_Release(pManagement);

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

	// 메쉬 개수 갱신
	m_dwSubsetCnt = m_pMesh_Static->Get_NumMaterials();

	return;
}

void CRenderObject::Set_RenderGroup(RENDERID _eGroup)
{
	m_eGroup = _eGroup;
}

void CRenderObject::SetUp_IndividualShaderValue()
{
	m_tPBRInfo.fEmissivePower = 5.f;
	m_tPBRInfo.fSpecularPower = 1.f;
	m_tPBRInfo.fRoughnessPower = 1.f;
	m_tPBRInfo.fMinSpecular = 0.1f;
	m_tPBRInfo.fID_R = 1.f;
	m_tPBRInfo.fID_G = 0.5f;
	m_tPBRInfo.fID_B = 0.1f;
	m_tPBRInfo.fRimPower = 0.f;
	m_tPBRInfo.fRimAlpha = 0.f;

	if (!lstrcmp(L"Mesh_Homefloorstone", m_szName))
	{
		m_tPBRInfo.fEmissivePower = 0.f;
		m_tPBRInfo.fSpecularPower = 0.0f;
		m_tPBRInfo.fRoughnessPower = 0.f;
		m_tPBRInfo.fMinSpecular = 0.f;
	}

	else if (!lstrcmp(L"Mesh_Home_Floor_One", m_szName))
	{
		m_tPBRInfo.fEmissivePower = 0.f;
		m_tPBRInfo.fSpecularPower = 0.0f;
		m_tPBRInfo.fRoughnessPower = 0.f;
		m_tPBRInfo.fMinSpecular = 0.f;
	}

	else if (!lstrcmp(L"Long_Longcarpet", m_szName))
	{
		m_tPBRInfo.fEmissivePower = 0.f;
		m_tPBRInfo.fSpecularPower = 0.0f;
		m_tPBRInfo.fRoughnessPower = 0.f;
		m_tPBRInfo.fMinSpecular = 0.f;
	}
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

void CRenderObject::Init_Shader(CShader* pShader)
{
	_mat		matWorld, matView, matProj;

	matWorld = m_pTransform->Get_WorldMat();

	pShader->Set_Value("g_matWorld", &matWorld, sizeof(_mat));

	//=============================================================================================
	// 쉐이더 재질정보 수치 입력
	//=============================================================================================
	if (FAILED(pShader->Set_Value("g_fEmissivePower", &m_tPBRInfo.fEmissivePower, sizeof(_float))))
		return;
	if (FAILED(pShader->Set_Value("g_fSpecularPower", &m_tPBRInfo.fSpecularPower, sizeof(_float))))
		return;
	if (FAILED(pShader->Set_Value("g_fRoughnessPower", &m_tPBRInfo.fRoughnessPower, sizeof(_float))))
		return;
	if (FAILED(pShader->Set_Value("g_fMinSpecular", &m_tPBRInfo.fMinSpecular, sizeof(_float))))
		return;
	if (FAILED(pShader->Set_Value("g_fID_R_Power", &m_tPBRInfo.fID_R, sizeof(_float))))
		return;
	if (FAILED(pShader->Set_Value("g_fID_G_Power", &m_tPBRInfo.fID_G, sizeof(_float))))
		return;
	if (FAILED(pShader->Set_Value("g_fID_B_Power", &m_tPBRInfo.fID_B, sizeof(_float))))
		return;
	if (FAILED(pShader->Set_Value("g_fRimAlpha", &m_tPBRInfo.fRimAlpha, sizeof(_float))))
		return;
	if (FAILED(pShader->Set_Value("g_fRimPower", &m_tPBRInfo.fRimPower, sizeof(_float))))
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

	if(nullptr != m_pMesh_Static)
		m_dwSubsetCnt = m_pMesh_Static->Get_NumMaterials();

	SetUp_IndividualShaderValue();

	Check_Stage_01();
	Check_Stage_02();
	Check_Stage_03();

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

void CRenderObject::Check_Stage_01()
{
	if (!lstrcmp(L"Mesh_Building_C", m_szName))
	{
		m_bAdvencedCull = true;
	}

	if (!lstrcmp(L"Mesh_Building_D", m_szName))
	{
		m_bAdvencedCull = true;
	}

	if (!lstrcmp(L"Mesh_Building_E", m_szName))
	{
		m_bAdvencedCull = true;
	}
}

void CRenderObject::Check_Stage_02()
{
	if (!lstrcmp(L"Mesh_SM_MERGED_SplineGaia88", m_szName))
	{
		m_bAdvencedCull = true;
	}
	if (!lstrcmp(L"SM_MERGED_SplineGaia199", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia319", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia436", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia445", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia449", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia471", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia473", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia475", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia582", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia586", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia611", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia618", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia667", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia726", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia784", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia791", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"SM_MERGED_SplineGaia891", m_szName))
		m_bAdvencedCull = true;
}

void CRenderObject::Check_Stage_03()
{
	// 맵 외벽
	if (!lstrcmp(L"Mesh_SM_TowerSenRL2Top_st07a1", m_szName))
	{
		m_bAdvencedCull = true;
	}
	if (!lstrcmp(L"Room_Stage3_1", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"Room_Stage3_2", m_szName))
		m_bAdvencedCull = true;
}

void CRenderObject::Check_Stage_04()
{
	if (!lstrcmp(L"DuomoFloorB", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"Cathedral_PillarL_Center_Round", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"Cathedral_TowerWall_Under_A", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"Cathedral_TowerWall_Under_B", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"Cathedral_TowerFloor_D", m_szName))
		m_bAdvencedCull = true;
	if (!lstrcmp(L"Cathedral_Floor_Center_Round", m_szName))
		m_bAdvencedCull = true;
}
