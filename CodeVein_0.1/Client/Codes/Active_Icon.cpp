#include "stdafx.h"
#include "..\Headers\Active_Icon.h"


CActive_Icon::CActive_Icon(_Device pDevice)
	: CUI(pDevice)
{
}

CActive_Icon::CActive_Icon(const CActive_Icon & rhs)
	: CUI(rhs)
{
}

HRESULT CActive_Icon::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CActive_Icon::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	return NOERROR;
}

_int CActive_Icon::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	if (m_fCurTime >= m_fMaxTime)
		m_fCurTime = 0.f;
	if (m_fCurTime <= 0.f)
		m_fCurTime = 0.f;

	m_fCurTime += _float(TimeDelta);
	m_fPercentage = m_fCurTime / m_fMaxTime;

	switch (m_eSkillID)
	{
	case Dragon_Lunge:
		m_iIndex = 1;
		break;
	case Vanishing_Hollow:
		m_iIndex = 2;
		break;
	case Circulating_Pulse:
		m_iIndex = 3;
		break;
	case Triple_Annihilator:
		m_iIndex = 4;
		break;
	case Shadow_Assault:
		m_iIndex = 5;
		break;
	case Severing_Abyss:
		m_iIndex = 6;
		break;
	case Swallow_Cutter:
		m_iIndex = 7;
		break;
	case Tormenting_Blast:
		m_iIndex = 8;
		break;
	case Phantom_Assault:
		m_iIndex = 9;
		break;
	case Legion_Punisher:
		m_iIndex = 10;
		break;
	case SkillID_End:
		m_iIndex = 0;
		break;
	}
	return NO_EVENT;
}

_int CActive_Icon::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return NO_EVENT;
}

HRESULT CActive_Icon::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	_uint iTexNum = 0;
	switch (m_eState)
	{
	case ACTIVE_USE:
	{
		for (_uint i = 0; i < 2; ++i)
		{
			(i == 0) ? (iTexNum = 11) : (iTexNum = m_iIndex);
			if (FAILED(SetUp_ConstantTable(iTexNum)))
				return E_FAIL;

			m_pShaderCom->Begin_Shader();

			m_pShaderCom->Begin_Pass(1);

			m_pBufferCom->Render_VIBuffer();

			m_pShaderCom->End_Pass();

			m_pShaderCom->End_Shader();
		}
	}
		break;
	case ACTIVE_NO_USE:
	{
		if (FAILED(SetUp_ConstantTable(11)))
			return E_FAIL;
		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(1);
		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();

		for (_uint i = 0; i < 2; ++i)
		{
			(i == 0) ? (iTexNum = m_iIndex) : (iTexNum = 14);
			if (FAILED(SetUp_ConstantTable(iTexNum)))
				return E_FAIL;

			m_pShaderCom->Begin_Shader();
			m_pShaderCom->Begin_Pass(1);
			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
			m_pShaderCom->End_Shader();
		}
	}
		break;
	case ACTIVE_COOL:
	{
		if (FAILED(SetUp_ConstantTable(11)))
			return E_FAIL;
		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(1);
		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();

		for (_uint i = 0; i < 2; ++i)
		{
			(i == 0) ? (iTexNum = m_iIndex) : (iTexNum = 15);
			if (FAILED(SetUp_ConstantTable(iTexNum)))
				return E_FAIL;
			m_pShaderCom->Begin_Shader();
			m_pShaderCom->Begin_Pass(1);
			m_pBufferCom->Render_VIBuffer();
			m_pShaderCom->End_Pass();
			m_pShaderCom->End_Shader();
		}

		if (FAILED(SetUp_ConstantTable(13)))
			return E_FAIL;
		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(7); // 흰색 프레임 UV값 변경
		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();
	}
		break;
	case ACTIVE_BUFF:
	{
		if (FAILED(SetUp_ConstantTable(14)))
			return E_FAIL;
		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(1);
		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();

		if (FAILED(SetUp_ConstantTable(12)))
			return E_FAIL;
		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(7); // 붉은색 프레임 UV값 변경
		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();

		if (FAILED(SetUp_ConstantTable(m_iIndex)))
			return E_FAIL;
		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(1);
		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();
	}
		break;
	}
	
	return NOERROR;
}

HRESULT CActive_Icon::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_ActiveUI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CActive_Icon::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (iIndex == 15)
	{
		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
			return E_FAIL;
		
		if (FAILED(m_pShaderCom->Set_Value("g_fPosX", &m_fPosX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_fSizeX", &m_fSizeX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fPercentage, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
			return E_FAIL;
	}
	else if (iIndex == 14)
	{
		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_fPosX", &m_fPosX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_fSizeX", &m_fSizeX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_fPercentage", &m_fPercentage, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
			return E_FAIL;
		if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
			return E_FAIL;
	}
	
	
	return NOERROR;
}

CActive_Icon * CActive_Icon::Create(_Device pGraphic_Device)
{
	CActive_Icon* pInstance = new CActive_Icon(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CActive_Icon::Clone_GameObject(void * pArg)
{
	CActive_Icon* pInstance = new CActive_Icon(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CActive_Icon::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);

	CUI::Free();
}
