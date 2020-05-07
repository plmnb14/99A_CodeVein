#include "stdafx.h"
#include "TestMonster.h"

CTestMonster::CTestMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CTestMonster::CTestMonster(LPDIRECT3DDEVICE9 pGraphic_Device, _tchar* szMeshname)
	: CGameObject(pGraphic_Device)
{
	m_pszMeshName = new _tchar[STR_128];
	lstrcpy(m_pszMeshName, szMeshname);
}

CTestMonster::CTestMonster(const CTestMonster & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTestMonster::Ready_GameObject_Prototype()
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransform->Set_Pos(_v3(0.f,0.f,0.f));
	m_pTransform->Set_Scale(_v3(1.f, 1.f, 1.f));
	m_pTransform->Set_Angle(_v3(0.f, D3DXToRadian(180.f), 0.f));

	return NOERROR;
}

HRESULT CTestMonster::Ready_GameObject(void * pArg)
{
	return NOERROR;
} 

_int CTestMonster::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bisPlayCombo)
	{
		Play_Combo();
	}

	return _int();
}

_int CTestMonster::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRenderer, E_FAIL);

	//if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
	//	return E_FAIL;

	m_dTimeDelta = TimeDelta;

	return NO_EVENT;
}

HRESULT CTestMonster::Render_GameObject()
{
	if (nullptr == m_pShader ||
		nullptr == m_pMesh)
		return E_FAIL;

	m_pMesh->Play_Animation(_float(m_dTimeDelta) * m_fAniPlayMul); // * alpha

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();

	_uint iNumMeshContainer = _uint(m_pMesh->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMesh->Get_NumMaterials(i);

		m_pMesh->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			m_pShader->Begin_Pass(0);

			if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pMesh->Get_MeshTexture(i, j, MESHTEXTURE::TYPE_DIFFUSE_MAP))))
				return E_FAIL;

			m_pShader->Commit_Changes();

			m_pMesh->Render_Mesh(i, j);

			m_pShader->End_Pass();
		}
	}

	m_pShader->End_Shader();

	return NOERROR;
}

void CTestMonster::Set_Combo(_uint _Idx, _float _Ratio)
{
	m_vectorIndexAniRatio.push_back(pair<_uint, _float>(_Idx, _Ratio));

	m_iMaxCount = (_int)m_vectorIndexAniRatio.size();

}

void CTestMonster::Reset_Combo()
{

	if (!m_vectorIndexAniRatio.empty())
		m_vectorIndexAniRatio.clear();
}

void CTestMonster::Play_Combo()
{

	if (m_iMaxCount <= m_iComboCount)
	{
		m_iComboCount = 0;
	}
	
	m_pMesh->SetUp_Animation(m_vectorIndexAniRatio[m_iComboCount].first);

	if (m_pMesh->Is_Finish_Animation(m_vectorIndexAniRatio[m_iComboCount].second))
	{
		++m_iComboCount;
	}
}

HRESULT CTestMonster::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Render", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, m_pszMeshName, L"Com_MeshDynamic", (CComponent**)&m_pMesh)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTestMonster::SetUp_ConstantTable()
{
	IF_NULL_VALUE_RETURN(m_pShader, E_FAIL);

	CManagement* pManagement = CManagement::Get_Instance();
	IF_NULL_VALUE_RETURN(pManagement, E_FAIL);

	Safe_AddRef(pManagement);

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;

	Safe_Release(pManagement);

	return NOERROR;
}

CTestMonster * CTestMonster::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _tchar* szMeshname)
{
	CTestMonster* pInstance = new CTestMonster(pGraphic_Device, szMeshname);

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
	Safe_Delete_Array(m_pszMeshName);
	Safe_Release(m_pTransform);
	Safe_Release(m_pMesh);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);

	CGameObject::Free();

	return;
}



