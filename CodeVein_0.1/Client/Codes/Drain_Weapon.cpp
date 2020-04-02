#include "stdafx.h"
#include "Drain_Weapon.h"

CDrain_Weapon::CDrain_Weapon(_Device pGraphic_Device)
	: CGameObject(pGraphic_Device)
{

}

CDrain_Weapon::CDrain_Weapon(const CDrain_Weapon & rhs)
	: CGameObject(rhs)
{

}

HRESULT CDrain_Weapon::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CDrain_Weapon::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	SetUp_Default();

	m_fAnimMultiply = 1.f;
	m_pMesh_Dynamic->SetUp_Animation(m_eAnimnum);

	return NOERROR;
}

_int CDrain_Weapon::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	Cacl_AttachBoneTransform();

	return NO_EVENT;
}

_int CDrain_Weapon::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRenderer ||
		nullptr == m_pMesh_Dynamic)
		return E_FAIL;

	if (m_bActive)
	{
		m_pMesh_Dynamic->SetUp_Animation(m_eAnimnum);

		if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
			return E_FAIL;
	}

	return NO_EVENT;
}

HRESULT CDrain_Weapon::Render_GameObject()
{
	if (nullptr == m_pShader ||
		nullptr == m_pMesh_Dynamic)
		return E_FAIL;

	m_pMesh_Dynamic->Play_Animation(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply);

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();

	_uint iNumMeshContainer = _uint(m_pMesh_Dynamic->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMesh_Dynamic->Get_NumMaterials(i);

		// 메시를 뼈에 붙인다.
		m_pMesh_Dynamic->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			m_pShader->Begin_Pass(0);

			if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pMesh_Dynamic->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE))))
				return E_FAIL;

			if (FAILED(m_pShader->Set_Texture("g_NormalTexture", m_pMesh_Dynamic->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_NORMAL))))
				return E_FAIL;

			m_pShader->Commit_Changes();

			m_pMesh_Dynamic->Render_Mesh(i, j);

			m_pShader->End_Pass();
		}
	}

	m_pShader->End_Shader();

	g_pManagement->Gizmo_Draw_Capsule(m_pCollider->Get_CenterPos(), m_pCollider->Get_Radius());

	return NOERROR;
}

void CDrain_Weapon::Set_Active(_bool _bActiveDrain)
{
	m_bActive = _bActiveDrain;

	if (false == m_bActive)
	{
		m_pMesh_Dynamic->SetUp_Animation(0);
		m_pMesh_Dynamic->Play_Animation(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply);
	}
}

void CDrain_Weapon::Set_ActiveCollider(_bool _bActiveCollider)
{
	m_pCollider->Set_Enabled(_bActiveCollider);
}

void CDrain_Weapon::Set_ResetOldAnimIdx()
{
	m_pMesh_Dynamic->Reset_OldIndx();
}

void CDrain_Weapon::Set_AnimIdx(_ulong _eAnimState)
{
	switch (_eAnimState)
	{
		// Parry
	case 375:
	{
		m_eAnimnum = Drain_Parry;
		break;
	}

	// Suck_Charge_Start
	case 376:
	{
		m_eAnimnum = Drain_Charge_Start;
		break;
	}

	// Suck_Charge_End
	case 377:
	{
		m_eAnimnum = Drain_Charge_End;
		break;
	}

	// Suck_Combo
	case 378:
	{
		m_eAnimnum = Drain_ComboSuck;
		break;
	}

	// Suck_Ground
	case 379:
	{
		m_eAnimnum = Drain_GroundSuck;
		break;
	}

	// Suck_Special_01
	case 380:
	{
		m_eAnimnum = Drain_SpecialSuck_01;
		break;
	}

	// Suck_Special_02
	case 381:
	{
		m_eAnimnum = Drain_SpecialSuck_02;
		break;
	}

	}
}

void CDrain_Weapon::Change_WeaponMesh(const _tchar* _MeshName)
{
	// 이름 비교해서 같으면 Return
	if (!lstrcmp(m_szName, _MeshName))
		return;

	// 다르다면 이름 갱신하고,
	lstrcpy(m_szName, _MeshName);

	// 컴포넌트에 있는 매쉬 찾아서
	auto& iter = m_pmapComponents.find(L"Com_DynamicMesh");

	// 둘 다 해제
	Safe_Release(m_pMesh_Dynamic);
	Safe_Release(iter->second);

	// Release 한 컴포넌트에 새로이 Clone 받음.
	iter->second = m_pMesh_Dynamic = static_cast<CMesh_Dynamic*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _MeshName));
	Safe_AddRef(iter->second);
}

HRESULT CDrain_Weapon::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Drain_LongTail", L"Com_DynamicMesh", (CComponent**)&m_pMesh_Dynamic)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	lstrcpy(m_szName, L"Mesh_Drain_LongTail");

	m_pCollider->Set_Radius(_v3{ 0.2f, 0.5f, 0.2f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CapsuleLength(1.8f);
	m_pCollider->Set_CenterPos(m_pTransform->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y * 2.f , 0.f });

	return NOERROR;
}

HRESULT CDrain_Weapon::SetUp_Default()
{
	// Transform
	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);
	//m_pTransform->Set_Angle(AXIS_X, -D3DXToRadian(90.f));
	//m_pTransform->Set_Angle(AXIS_Z, D3DXToRadian(45.f));
	return S_OK;
}

HRESULT CDrain_Weapon::SetUp_ConstantTable()
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

void CDrain_Weapon::Cacl_AttachBoneTransform()
{
	_mat tmpMat;
	D3DXMatrixIdentity(&tmpMat);

	memcpy(&tmpMat._41, &(*m_pmatAttach)._41, sizeof(_v3));

	m_pTransform->Calc_ParentMat(&(tmpMat * *m_pmatParent));
}

CDrain_Weapon * CDrain_Weapon::Create(_Device pGraphic_Device)
{
	CDrain_Weapon*	pInstance = new CDrain_Weapon(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating Prototype DrainWeapon");
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CDrain_Weapon::Clone_GameObject(void * pArg)
{
	CDrain_Weapon*	pInstance = new CDrain_Weapon(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Creating Clone DrainWeapon");
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDrain_Weapon::Free()
{
	m_pmatAttach = nullptr;
	m_pmatParent = nullptr;

	Safe_Release(m_pCollider);
	Safe_Release(m_pTransform);
	Safe_Release(m_pMesh_Dynamic);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);

	CGameObject::Free();
}
