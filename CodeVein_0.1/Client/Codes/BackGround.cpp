#include "stdafx.h"
#include "..\Headers\BackGround.h"

CBackGround::CBackGround(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{

}

CBackGround::CBackGround(const CBackGround & rhs)
	: CGameObject(rhs)
{

}

HRESULT CBackGround::Ready_GameObject_Prototype()
{
	// ���� ��, ���� �ɸ� �� �ִ� ���۾�\���� �����Ѵ�.
	return NOERROR;
}

HRESULT CBackGround::Ready_GameObject(void * pArg)
{
	// �����ؼ� ���� �� ��, �߰������� �ʿ��� �����͵��� �����ϳ�.
	if (FAILED(Add_Component()))
		return E_FAIL;

	return NOERROR;
}

_int CBackGround::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CBackGround::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;


	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_PRIORITY, this)))
		return E_FAIL;

	return _int();
}

HRESULT CBackGround::Render_GameObject()
{
	if (nullptr == m_pShaderCom || 
		nullptr == m_pBufferCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	m_pShaderCom->Begin_Pass(0);
	
	// ���۸� �������Ѵ�.
	// (�ε�������(012023)�� �����ϰ��ִ� �ε����� ���� ������ �׸���.)
	// �ﰢ�� �ΰ��� �׸���.������ �ﰢ������ ��������, ������ ������ ���ؽ� ���̴��� ���ڷ� ������.
	m_pBufferCom->Render_VIBuffer();

	m_pShaderCom->End_Pass();

	m_pShaderCom->End_Shader();

	return NOERROR;
}

HRESULT CBackGround::Add_Component()
{
	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Texture_Default", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Default", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;
	
	
	return NOERROR;
}

HRESULT CBackGround::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	_mat			TmpMatrix;
	D3DXMatrixIdentity(&TmpMatrix);

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &TmpMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &TmpMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &TmpMatrix, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, 0)))
		return E_FAIL;

	return NOERROR;
}

CBackGround * CBackGround::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBackGround*	pInstance = new CBackGround(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CBackGround::Clone_GameObject(void * pArg)
{
	CBackGround*	pInstance = new CBackGround(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackGround::Free()
{

	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
