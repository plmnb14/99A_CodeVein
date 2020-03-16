#include "..\Headers\Shader.h"

CShader::CShader(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
	, m_pEffect(nullptr)
{
	
}

CShader::CShader(const CShader & rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
	, m_pErrorBuffer(rhs.m_pErrorBuffer)
{
	Safe_AddRef(m_pEffect);
	Safe_AddRef(m_pErrorBuffer);
}

HRESULT CShader::Set_Bool(D3DXHANDLE Handle, _bool Bool)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetBool(Handle, Bool);	
}

HRESULT CShader::Set_Value(D3DXHANDLE Handle, const void* pData, _uint iByte)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetValue(Handle, pData, iByte);	
}

HRESULT CShader::Set_Texture(D3DXHANDLE Handle, LPDIRECT3DBASETEXTURE9 pTexture)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	return m_pEffect->SetTexture(Handle, pTexture);
}

HRESULT CShader::Commit_Changes()
{
	if (nullptr == m_pEffect)
		return E_FAIL;
	
	return m_pEffect->CommitChanges();;
}

HRESULT CShader::Ready_Component_Prototype(const _tchar * pFilePath)
{
	if (FAILED(D3DXCreateEffectFromFile(m_pGraphic_Dev, pFilePath, nullptr, nullptr, 0, nullptr, &m_pEffect, &m_pErrorBuffer)))
	{
		// MessageBoxA(0, (char*)m_pErrorBuffer->GetBufferPointer(), "System Error", MB_OK);
		return E_FAIL;
	}

	return NOERROR;
}

HRESULT CShader::Ready_Component(void * pArg)
{
	

	return NOERROR;
}

HRESULT CShader::Begin_Shader()
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	_uint iNumPass = 0;

	m_pEffect->Begin(nullptr, 0);

	return NOERROR;
}

HRESULT CShader::Begin_Pass(_uint iPassIdx)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->BeginPass(iPassIdx);

	return NOERROR;
}

HRESULT CShader::End_Pass()
{
	if (nullptr == m_pEffect)
		return E_FAIL;	

	m_pEffect->EndPass();

	return NOERROR;
}

HRESULT CShader::End_Shader()
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->End();

	return NOERROR;
}

CShader * CShader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pFilePath)
{
	CShader*		pInstance = new CShader(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype(pFilePath)))
	{
		MSG_BOX("CShader Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CShader::Clone_Component(void * pArg)
{
	CShader*		pInstance = new CShader(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MSG_BOX("CTexture Cloned Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader::Free()
{
	Safe_Release(m_pErrorBuffer);
	Safe_Release(m_pEffect);

	CComponent::Free();
}
