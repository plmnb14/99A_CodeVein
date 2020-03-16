#include "..\Headers\Light.h"

CLight::CLight(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Device);
}

HRESULT CLight::Ready_Light(D3DLIGHT9 LightDesc)
{
	D3DVIEWPORT9		ViewPort;

	m_pGraphic_Device->GetViewport(&ViewPort);

	m_pViewPortBuffer = CBuffer_ViewPort::Create(m_pGraphic_Device, 0.0f - 0.5f, 0.0f - 0.5f, (_float)ViewPort.Width, (_float)ViewPort.Height);
	if (nullptr == m_pViewPortBuffer)
		return E_FAIL;

	m_LightDesc = LightDesc;

	return NOERROR;
}

HRESULT CLight::Render_Light(CShader* pShader)
{
	if (nullptr == m_pViewPortBuffer)
		return E_FAIL;

	_uint			iPassIndex = 0;

	if (D3DLIGHT_DIRECTIONAL == m_LightDesc.Type)
	{
		iPassIndex = 0;

		pShader->Set_Value("g_vLightDir", &_v4(m_LightDesc.Direction, 0.f), sizeof(_v4));
	}

	else if (D3DLIGHT_POINT == m_LightDesc.Type)
	{
		iPassIndex = 1;

		pShader->Set_Value("g_vLightPos", &_v4(m_LightDesc.Position, 1.f), sizeof(_v4));
		pShader->Set_Value("g_fRange", &m_LightDesc.Range, sizeof(_float));
	}
	
	pShader->Set_Value("g_vLightDiffuse", (_v4*)&m_LightDesc.Diffuse, sizeof(_v4));
	pShader->Set_Value("g_vLightAmbient", (_v4*)&m_LightDesc.Ambient, sizeof(_v4));	
	pShader->Set_Value("g_vLightSpecular", (_v4*)&m_LightDesc.Specular, sizeof(_v4));

	pShader->Begin_Pass(iPassIndex); 

	pShader->Commit_Changes();

	m_pViewPortBuffer->Render_VIBuffer();

	pShader->End_Pass();

	return NOERROR;
}

CLight * CLight::Create(LPDIRECT3DDEVICE9 pGraphic_Device, D3DLIGHT9 LightDesc)
{
	CLight*	pInstance = new CLight(pGraphic_Device);

	if (FAILED(pInstance->Ready_Light(LightDesc)))
	{
		MSG_BOX("Failed To Creating CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pViewPortBuffer);
	Safe_Release(m_pGraphic_Device);
}
