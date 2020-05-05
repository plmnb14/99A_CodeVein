#include "..\Headers\Target.h"

CTarget::CTarget(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CTarget::Ready_Render_Target(_uint iSizeX, _uint iSizeY, D3DFORMAT eFormat, D3DXCOLOR ClearColor)
{
	// 1. 텍스쳐를 생성하낟.
	if (FAILED(D3DXCreateTexture(m_pGraphic_Device, iSizeX, iSizeY, 1, D3DUSAGE_RENDERTARGET, eFormat, D3DPOOL_DEFAULT, &m_pTarget_Texture)))
		return E_FAIL;
	
	// 2. 생성한 텍스쳐로부터 렌더타겟의 정보를 얻어온다.
	if (FAILED(m_pTarget_Texture->GetSurfaceLevel(0, &m_pTarget_Surface)))
		return E_FAIL;	

	m_ClearColor = ClearColor;

	D3DVIEWPORT9		ViewPort;
	m_pGraphic_Device->GetViewport(&ViewPort);
	m_pGraphic_Device->CreateDepthStencilSurface(iSizeX, iSizeY, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pNewStencil, NULL);

	return NOERROR;
}

HRESULT CTarget::Origin_DepthBuffer()
{
	m_pGraphic_Device->GetDepthStencilSurface(&m_pOriginStencil);

	return S_OK;
}

HRESULT CTarget::Begin_DepthBuffer()
{
	m_pGraphic_Device->SetDepthStencilSurface(m_pNewStencil);

	return S_OK;
}

HRESULT CTarget::End_DepthBuffer()
{
	m_pGraphic_Device->SetDepthStencilSurface(m_pOriginStencil);

	Safe_Release(m_pOriginStencil);

	return S_OK;
}

HRESULT CTarget::Origin_ViewPort()
{
	m_pGraphic_Device->GetViewport(&m_tViewPort_Old);

	m_tViewPort_New = { 0, 0, 3840, 2160, m_tViewPort_Old.MinZ, m_tViewPort_Old.MaxZ };

	return S_OK;
}

HRESULT CTarget::Begin_ViewPort()
{
	m_pGraphic_Device->SetViewport(&m_tViewPort_New);

	return S_OK;
}

HRESULT CTarget::End_ViewPort()
{
	m_pGraphic_Device->SetViewport(&m_tViewPort_Old);

	m_tViewPort_Old = {};
	m_tViewPort_New = {};

	return S_OK;
}
HRESULT CTarget::SetUp_OnGraphicDev(_uint iIndex)
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pTarget_Surface)
		return E_FAIL;

	m_pGraphic_Device->GetRenderTarget(iIndex, &m_pOld_Surface);

	m_pGraphic_Device->SetRenderTarget(iIndex, m_pTarget_Surface);

	

	return NOERROR;
}

HRESULT CTarget::Release_OnGraphicDev(_uint iIndex)
{
	if (nullptr == m_pGraphic_Device ||
		nullptr == m_pTarget_Surface)
		return E_FAIL;

	m_pGraphic_Device->SetRenderTarget(iIndex, m_pOld_Surface);

	Safe_Release(m_pOld_Surface);



	return NOERROR;
}

HRESULT CTarget::Clear_Target()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	if (FAILED(SetUp_OnGraphicDev(0)))
		return E_FAIL;

	m_pGraphic_Device->Clear(0, nullptr, D3DCLEAR_TARGET, m_ClearColor, 1.f, 0);

	if (FAILED(Release_OnGraphicDev(0)))
		return E_FAIL;

	return NOERROR;
}

#ifdef _DEBUG

HRESULT CTarget::Ready_Debug_Buffer(_float fStartX, _float fStartY, _float fSizeX, _float fSizeY)
{
	m_pViewPortBuffer = CBuffer_ViewPort::Create(m_pGraphic_Device, fStartX, fStartY, fSizeX, fSizeY);

	if (nullptr == m_pViewPortBuffer)
		return E_FAIL;

	return NOERROR;
}

HRESULT CTarget::Render_Debug_Buffer()
{
	if (nullptr == m_pViewPortBuffer || 
		nullptr == m_pTarget_Texture)
		return E_FAIL;

	m_pGraphic_Device->SetTexture(0, m_pTarget_Texture);
	
	m_pViewPortBuffer->Render_VIBuffer();

	return NOERROR;
}

#endif

CTarget * CTarget::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _uint iSizeX, _uint iSizeY, D3DFORMAT eFormat, D3DXCOLOR ClearColor)
{
	CTarget*	pInstance = new CTarget(pGraphic_Device);

	if (FAILED(pInstance->Ready_Render_Target(iSizeX, iSizeY, eFormat, ClearColor)))
	{
		MSG_BOX("Failed To Creating CTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTarget::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pViewPortBuffer);
#endif
	Safe_Release(m_pNewStencil);
	Safe_Release(m_pOriginStencil);

	Safe_Release(m_pTarget_Surface);
	Safe_Release(m_pTarget_Texture);
	Safe_Release(m_pGraphic_Device);
}
