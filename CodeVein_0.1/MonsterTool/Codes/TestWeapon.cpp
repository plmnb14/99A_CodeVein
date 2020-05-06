#include "stdafx.h"
#include "TestWeapon.h"

CTestWeapon::CTestWeapon(_Device pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTestWeapon::CTestWeapon(LPDIRECT3DDEVICE9 pGraphic_Device, _tchar * szWeaponname)
	:CGameObject(pGraphic_Device)
{
	m_pszWeaponName = new _tchar[STR_128];
	lstrcpy(m_pszWeaponName, szWeaponname);
}

CTestWeapon::CTestWeapon(const CTestWeapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTestWeapon::Ready_GameObject_Prototype()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);
	m_pTransform->Set_Angle(AXIS_X, D3DXToRadian(-90.f));

	m_pManagement = CManagement::Get_Instance();
	IF_NULL_VALUE_RETURN(m_pManagement, E_FAIL);

	m_vecAttackCol.reserve(2);
	CCollider* pCollider = static_cast<CCollider*>(m_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	pCollider->Set_Radius(_v3{ m_fRadius, m_fRadius, m_fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(m_pTransform->Get_Pos() + _v3{ 0.f ,m_fRadius , 0.f });	// 콜라이더 중앙 위치 설정
	pCollider->Set_Enabled(false);
	m_vecAttackCol.push_back(pCollider);

	m_pmatAttach = nullptr;
	m_pmatParent = nullptr;

	return S_OK;
}

HRESULT CTestWeapon::Ready_GameObject(void * pArg)
{
	return S_OK;
}

_int CTestWeapon::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if(nullptr!= m_pmatAttach && nullptr != m_pmatParent)
		m_pTransform->Calc_ParentMat(&(*m_pmatAttach * *m_pmatParent));

	Change_Radius(m_fRadius);
	Change_Height(m_fHeight);

	return NO_EVENT;
}

_int CTestWeapon::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRenderer, E_FAIL);

	return _int();
}

HRESULT CTestWeapon::Render_GameObject()
{
	IF_NULL_VALUE_RETURN(m_pShader, E_FAIL);

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();


	_uint iNumSubSet = (_uint)m_pMesh->Get_NumMaterials();

	for (_uint i = 0; i < iNumSubSet; ++i)
	{
		//m_iPass = 0 == i ? 0 : 5;
		//
		//if (false == m_tmpEmissiveTest)
		//	m_iPass = 0;
		m_iPass = m_pMesh->Get_MaterialPass(i);

		m_pShader->Begin_Pass(m_iPass);

		m_pShader->Set_StaticTexture_Auto(m_pMesh, i);

		//if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pMesh_Static->Get_Texture(i, MESHTEXTURE::TYPE_DIFFUSE_MAP))))
		//	return E_FAIL;
		//
		//if (FAILED(m_pShader->Set_Texture("g_NormalTexture", m_pMesh_Static->Get_Texture(i, MESHTEXTURE::TYPE_NORMAL_MAP))))
		//	return E_FAIL;

		//if (FAILED(m_pShader->Set_Texture("g_EmissiveTexture", m_pMesh_Static->Get_Texture(i, MESHTEXTURE::TYPE_EMISSIVE_MAP))))
		//	return E_FAIL;

		m_pShader->Commit_Changes();

		m_pMesh->Render_Mesh(i);

		m_pShader->End_Pass();
	}

	m_pShader->End_Shader();

	for (auto& iter : m_vecAttackCol)
	{
		m_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	m_pManagement->Gizmo_ColliderEnable();

	return S_OK;
}

HRESULT CTestWeapon::Render_GameObject_SetPass(CShader * pShader, _int iPass)
{
	IF_NULL_VALUE_RETURN(pShader, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMesh, E_FAIL);

	_mat ViewMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	_mat ProjMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
		return E_FAIL;

	m_matLastWVP = m_pTransform->Get_WorldMat() * ViewMatrix * ProjMatrix;

	_ulong dwNumSubSet = m_pMesh->Get_NumMaterials();

	for (_ulong i = 0; i < dwNumSubSet; ++i)
	{
		pShader->Begin_Pass(iPass);

		pShader->Commit_Changes();

		m_pMesh->Render_Mesh(i);

		pShader->End_Pass();
	}

	return S_OK;
}

void CTestWeapon::Change_Radius(_float Radius)
{
	m_fRadius = Radius;

	for (auto& iter : m_vecAttackCol)
	{
		iter->Set_Radius(_v3(m_fRadius, m_fRadius, m_fRadius));
	}
}

void CTestWeapon::Change_Height(_float Height)
{
	m_fHeight = Height;

	_mat matWorld = m_pTransform->Get_WorldMat();
	_v3 vBegin, vDir;

	memcpy(vBegin, &matWorld._41, sizeof(_v3));
	memcpy(vDir, &matWorld._31, sizeof(_v3));

	for (auto& iter : m_vecAttackCol)
	{
		iter->Update(vBegin + vDir * m_fHeight);
	}
}

HRESULT CTestWeapon::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, m_pszWeaponName, L"Com_StaticMesh", (CComponent**)&m_pMesh)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestWeapon::SetUp_ConstantTable()
{
	IF_NULL_VALUE_RETURN(m_pShader, E_FAIL);

	CManagement* pManagement = CManagement::Get_Instance();
	IF_NULL_VALUE_RETURN(pManagement, E_FAIL);

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;

	return S_OK;
}

CTestWeapon * CTestWeapon::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _tchar* szWeaponName)
{
	CTestWeapon* pInstance = new CTestWeapon(pGraphic_Device, szWeaponName);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CTestMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTestWeapon::Clone_GameObject(void * pArg)
{
	CTestWeapon* pInstance = new CTestWeapon(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CTestMonster");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestWeapon::Free()
{
	Safe_Delete_Array(m_pszWeaponName);
	Safe_Release(m_pTransform);
	Safe_Release(m_pMesh);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);

	CGameObject::Free();

	return;
}
