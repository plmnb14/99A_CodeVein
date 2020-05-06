#include "stdafx.h"
#include "..\Headers\Sky_Blur.h"

CSky_Blur::CSky_Blur(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{

}

CSky_Blur::CSky_Blur(const CSky_Blur & rhs)
	: CGameObject(rhs)
{

}

HRESULT CSky_Blur::Ready_GameObject_Prototype()
{
	// ���� ��, ���� �ɸ� �� �ִ� ���۾�\���� �����Ѵ�.
	return NOERROR;
}

HRESULT CSky_Blur::Ready_GameObject(void * pArg)
{
	// �����ؼ� ���� �� ��, �߰������� �ʿ��� �����͵��� �����ϳ�.
	if (FAILED(Add_Component()))
		return E_FAIL;
	
	m_pTransformCom->Set_Pos(_v3(0.f, -20.f, 0.f));
	m_pTransformCom->Set_Scale(_v3(0.3f, 0.3f, 0.3f));

	return NOERROR;
}

_int CSky_Blur::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);
	return _int();
}

_int CSky_Blur::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_FOG, this)))
		return E_FAIL;

	return _int();
}

HRESULT CSky_Blur::Render_GameObject()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials();

	for (_uint i = 0; i < iNumSubSet; ++i)
	{
		m_pShaderCom->Begin_Pass(0);

		if (FAILED(m_pShaderCom->Set_Texture("g_DiffuseTexture", m_pMeshCom->Get_Texture(i, MESHTEXTURE::TYPE_DIFFUSE_MAP))))
			return E_FAIL;

		m_pShaderCom->Commit_Changes();

		m_pMeshCom->Render_Mesh(i);

		m_pShaderCom->End_Pass();
	}

	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CSky_Blur::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Sky", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Sky_Blur_1", L"Static_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;
	
	return NOERROR;
}

HRESULT CSky_Blur::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;


	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;	

	_mat		ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;

	//if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom)))
	//	return E_FAIL;

	return NOERROR;
}

CSky_Blur * CSky_Blur::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSky_Blur*	pInstance = new CSky_Blur(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CSky_Blur::Clone_GameObject(void * pArg)
{
	CSky_Blur*	pInstance = new CSky_Blur(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSky_Blur::Free()
{
	//Safe_Release(m_pTextureCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pShaderCom);	
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
