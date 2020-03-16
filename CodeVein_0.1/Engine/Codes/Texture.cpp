#include "..\Headers\Texture.h"
#include "Shader.h"

CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_vecTexture(rhs.m_vecTexture)	
{
	for (auto& pTexture : m_vecTexture)
		pTexture->AddRef();

	lstrcpy(m_szFileName, rhs.m_szFileName);
}

HRESULT CTexture::SetUp_OnGraphicDev(_uint iStage, _uint iIndex)
{
	if (nullptr == m_pGraphic_Dev ||
		m_vecTexture.size() <= iIndex)
		return E_FAIL;

	m_pGraphic_Dev->SetTexture(iStage, m_vecTexture[iIndex]);

	return NOERROR;
}

HRESULT CTexture::SetUp_OnShader(D3DXHANDLE hHandle, CShader * pShader, _uint iIndex)
{
	if (nullptr == pShader ||
		m_vecTexture.size() <= iIndex)
		return E_FAIL;

	return pShader->Set_Texture(hHandle, m_vecTexture[iIndex]);	
}

HRESULT CTexture::Ready_Component_Prototype(TEXTURETYPE eType, const _tchar * pFileName, const _uint & iCnt)
{
	lstrcpy(m_szFileName, pFileName);
	
	m_vecTexture.reserve(iCnt);

	for (size_t i = 0; i < iCnt; ++i)
	{
		_tchar			szFullPath[MAX_PATH] = L"";

		wsprintf(szFullPath, pFileName, i);

		IDirect3DBaseTexture9*			pTexture = nullptr;

		HRESULT			hr = 0;

		switch (eType)
		{
		case TYPE_GENERAL:
			hr = D3DXCreateTextureFromFile(m_pGraphic_Dev, szFullPath, (LPDIRECT3DTEXTURE9*)&pTexture);
			break;
		case TYPE_CUBE:
			hr = D3DXCreateCubeTextureFromFile(m_pGraphic_Dev, szFullPath, (LPDIRECT3DCUBETEXTURE9*)&pTexture);
			break;
		}

		if (FAILED(hr))
			return E_FAIL;

		m_vecTexture.push_back(pTexture);
	}

	return NOERROR;
}

HRESULT CTexture::Ready_Component(void * pArg)
{

	return NOERROR;
}


CTexture * CTexture::Create(LPDIRECT3DDEVICE9 pGraphic_Device, TEXTURETYPE eType, const _tchar * pFileName, const _uint & iCnt)
{
	CTexture*		pInstance = new CTexture(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype(eType, pFileName, iCnt)))
	{
		MSG_BOX("CTexture Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTexture::Clone_Component(void* pArg)
{
	CTexture*		pInstance = new CTexture(*this);

	if (FAILED(pInstance->Ready_Component(pArg)))
	{
		MSG_BOX("CTexture Cloned Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	for (auto& pTexture : m_vecTexture)
	{
		_ulong		dwRefCnt = 0;
		if (dwRefCnt = Safe_Release(pTexture))
		{
		
		}

	}
	m_vecTexture.clear();

	CComponent::Free();
}
