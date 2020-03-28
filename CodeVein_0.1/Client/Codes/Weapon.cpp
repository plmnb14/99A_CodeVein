#include "stdafx.h"
#include "..\Headers\Weapon.h"

CWeapon::CWeapon(_Device pGraphic_Device)
	: CGameObject(pGraphic_Device)
{

}

CWeapon::CWeapon(const CWeapon & rhs)
	: CGameObject(rhs)
{

}

HRESULT CWeapon::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CWeapon::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	SetUp_Default();

	m_pTrailEffect = static_cast<Engine::CTrail_VFX*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SwordTrail", nullptr));
	m_pTrailEffect->Set_TrailIdx(0);

	m_pDistortionEffect = static_cast<Engine::CTrail_VFX*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SwordTrail", nullptr));
	m_pDistortionEffect->Set_TrailIdx(3);
	m_pDistortionEffect->Set_TrailType(Engine::CTrail_VFX::Trail_Distortion);

	m_pStaticTrailEffect = static_cast<Engine::CTrail_VFX*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SwordTrail", nullptr));
	m_pStaticTrailEffect->Set_TrailIdx(1);


	return NOERROR;
}

_int CWeapon::Update_GameObject(_double TimeDelta)
{
	if (WEAPON_None == m_eWeaponType)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	Cacl_AttachBoneTransform();

	UpdateTrails(TimeDelta);


	return NO_EVENT;
}

_int CWeapon::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRenderer ||
		nullptr == m_pMesh_Static)
		return E_FAIL;

	if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return _int();
}

HRESULT CWeapon::Render_GameObject()
{
	if (nullptr == m_pShader ||
		nullptr == m_pMesh_Static)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();


	_uint iNumSubSet = (_uint)m_pMesh_Static->Get_NumMaterials();

	for (_uint i = 0; i < iNumSubSet; ++i)
	{
		m_pShader->Begin_Pass(0);

		if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pMesh_Static->Get_Texture(i, MESHTEXTURE::TYPE_DIFFUSE))))
			return E_FAIL;

		m_pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		m_pShader->End_Pass();
	}


	m_pShader->End_Shader();

	g_pManagement->Gizmo_Draw_Capsule(m_pCollider->Get_CenterPos(), m_pCollider->Get_Radius());

	return NOERROR;
}

void CWeapon::UpdateTrails(_double TimeDelta)
{
	_mat matWorld = m_pTransform->Get_WorldMat();
	_v3 vBegin, vDir;

	memcpy(vBegin, &m_pTransform->Get_WorldMat()._41, sizeof(_v3));
	memcpy(vDir, &m_pTransform->Get_WorldMat()._31, sizeof(_v3));

	if (m_pTrailEffect)
	{
		//m_pTrailEffect->Set_ParentTransform(&matWorld);
		//m_pTrailEffect->Ready_Info(vBegin + vDir * 0.5f, vBegin + vDir * 1.4f);
		//m_pTrailEffect->Update_GameObject(TimeDelta);
	}

	if (m_pDistortionEffect)
	{
		m_pDistortionEffect->Set_ParentTransform(&matWorld);
		m_pDistortionEffect->Ready_Info(vBegin + vDir * 0.2f, vBegin + vDir * 1.5f);
		m_pDistortionEffect->Update_GameObject(TimeDelta);
	}

	if (m_pStaticTrailEffect)
	{
		m_pStaticTrailEffect->Set_ParentTransform(&matWorld);
		m_pStaticTrailEffect->Ready_Info(vBegin + vDir * 0.2f, vBegin + vDir * 1.6f);
		m_pStaticTrailEffect->Update_GameObject(TimeDelta);
	}
}


void CWeapon::Change_WeaponMesh(const _tchar* _MeshName)
{
	// 이름 비교해서 같으면 Return
	if (!lstrcmp(m_szName, _MeshName))
		return;

	// 다르다면 이름 갱신하고,
	lstrcpy(m_szName, _MeshName);
	 
	// 컴포넌트에 있는 매쉬 찾아서
	auto& iter = m_pmapComponents.find(L"Static_Mesh");

	// 둘 다 해제
	Safe_Release(m_pMesh_Static);
	Safe_Release(iter->second);

	// Release 한 컴포넌트에 새로이 Clone 받음.
	iter->second = m_pMesh_Static = static_cast<CMesh_Static*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _MeshName));

	Safe_AddRef(iter->second);
}

void CWeapon::Change_WeaponData(WEAPON_DATA _eWpnData)
{
	if (_eWpnData == m_eWeaponData)
		return;

	m_eWeaponData = _eWpnData;

	_tchar WeaponMeshName[MAX_STR] = L"";

	switch (_eWpnData)
	{
	case WPN_SSword_Normal:
	{
		lstrcpy(WeaponMeshName, L"Mesh_Sword");
		m_eWeaponType = WEAPON_Ssword;
		break;
	}
	case WPN_LSword_Normal:
	{
		lstrcpy(WeaponMeshName, L"Mesh_Sword");
		m_eWeaponType = WEAPON_Ssword;
		break;
	}
	case WPN_Hammer_Normal:
	{
		lstrcpy(WeaponMeshName, L"Mesh_Hammer");
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Hammer");
		m_eWeaponType = WEAPON_Hammer;
		break;
	}
	case WPN_Halverd_Normal:
	{
		lstrcpy(WeaponMeshName, L"Mesh_Sword");
		m_eWeaponType = WEAPON_Ssword;
		break;
	}
	case WPN_Gun_Normal:
	{
		lstrcpy(WeaponMeshName, L"Mesh_Sword");
		m_eWeaponType = WEAPON_Ssword;
		break;
	}
	}

	Change_WeaponMesh(WeaponMeshName);
}

HRESULT CWeapon::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Wpn_Sword", L"Static_Mesh", (CComponent**)&m_pMesh_Static)))
		return E_FAIL;


	lstrcpy(m_szName, L"Mesh_Wpn_Sword");


	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	lstrcpy(m_szName, L"Mesh_Wpn_Sword");

	m_pCollider->Set_Radius(_v3{ 0.2f, 0.5f, 0.2f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_CAPSULE);
	m_pCollider->Set_CapsuleLength(1.8f);
	m_pCollider->Set_CenterPos(m_pTransform->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y * 2.f , 0.f });

	return NOERROR;
}

HRESULT CWeapon::SetUp_Default()
{
	// Transform
	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);
	m_pTransform->Set_Angle(AXIS_X, D3DXToRadian(-90.f));
	return S_OK;
}

HRESULT CWeapon::SetUp_ConstantTable()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	
	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat		ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);


	if (FAILED(m_pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;

	return NOERROR;
}

void CWeapon::Cacl_AttachBoneTransform()
{
	m_pTransform->Calc_ParentMat(&(*m_pmatAttach * *m_pmatParent));
}

CWeapon * CWeapon::Create(_Device pGraphic_Device)
{
	CWeapon*	pInstance = new CWeapon(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CWeapon::Clone_GameObject(void * pArg)
{
	CWeapon*	pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	m_pmatAttach = nullptr;

	m_pmatParent = nullptr;

	Safe_Release(m_pTrailEffect);
	Safe_Release(m_pStaticTrailEffect);
	Safe_Release(m_pDistortionEffect);

	Safe_Release(m_pCollider);

	Safe_Release(m_pTransform);
	Safe_Release(m_pMesh_Static);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);

	CGameObject::Free();
}
