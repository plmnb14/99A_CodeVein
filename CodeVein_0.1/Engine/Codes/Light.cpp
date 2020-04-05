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
	pShader->Set_Value("g_LightVP_Close", &(m_matView * m_matProj), sizeof(_mat));

	pShader->Begin_Pass(iPassIndex); 

	pShader->Commit_Changes();

	m_pViewPortBuffer->Render_VIBuffer();

	pShader->End_Pass();

	return NOERROR;
}

void CLight::Update_Light()
{
	D3DXMatrixIdentity(&m_matView);
	_v3 vPos = m_vPos;

	//_v3 vDir, vRight, vUp, vLook, vLookAt;
	//
	//V3_NORMAL(&vLook, &vPos);
	//vLook *= -1.f;
	//CALC::V3_Cross_Normal(&vRight, &WORLD_UP, &vLook);
	//CALC::V3_Cross_Normal(&vUp, &vLook, &vRight);
	//
	//memcpy(&m_matView._11, &vRight, sizeof(_v3));
	//memcpy(&m_matView._21, &vUp, sizeof(_v3));
	//memcpy(&m_matView._31, &vLook, sizeof(_v3));
	//
	//m_matView._14 = -D3DXVec3Dot(&vRight, &vPos);
	//m_matView._24 = -D3DXVec3Dot(&vUp, &vPos);
	//m_matView._34 = -D3DXVec3Dot(&vLook, &vPos);
	//m_matView._44 = 1.f;
	//
	//D3DXMatrixTranspose(&m_matView, &m_matView);

	D3DXMatrixLookAtLH(&m_matView, &vPos, &_v3(-10, -10, -10), &WORLD_UP);

	// =================================================

	D3DXMatrixIdentity(&m_matProj);

	D3DXMatrixPerspectiveFovLH(&m_matProj, D3DXToRadian(60.f), WINCX / (float)WINCY, 0.1f, 1000.f);
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
