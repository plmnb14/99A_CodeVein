#include "stdafx.h"
#include "..\Headers\CustomSliderBar.h"
#include "Player.h"
#include "Costume_Hair.h"
#include "Costume_Head.h"

CCustomSliderBar::CCustomSliderBar(_Device pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CCustomSliderBar::CCustomSliderBar(const CCustomSliderBar & rhs)
	: CUI(rhs)
{
}

void CCustomSliderBar::Set_UI_Pos(_float fX, _float fY)
{
	m_fControlBarPosX = fX;
	m_fControlBarPosY = fY;
	m_fBackBarPosX = fX;
	m_fBackBarPosY = fY;
}

HRESULT CCustomSliderBar::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CCustomSliderBar::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.8f;
	m_fPosY = 470.f;
	m_fSizeX = 237.f * 0.7f;
	m_fSizeY = 44.f  * 0.7f;

	m_fMinLen = WINCX * 0.66f;
	m_fMaxLen = WINCX * 0.94f;

	m_fControlBarPosX = m_fPosX;
	m_fControlBarPosY = m_fPosY;
	m_fBackBarPosX = m_fPosX;
	m_fBackBarPosY = m_fPosY;

	m_fViewZ = 0.1f;
	m_bIsActive = false;
	if (FAILED(SetUp_CursorEffect()))
		return E_FAIL;
	
	return NOERROR;
}

_int CCustomSliderBar::Update_GameObject(_double TimeDelta)
{
	if (!m_bIsActive)
		return S_OK;

	CUI::Update_GameObject(TimeDelta);

	Late_Init();
	Coll_Mouse();
	Check_Drag();
	Check_Value();

	m_pRendererCom->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);


	return NO_EVENT;
}

_int CCustomSliderBar::Late_Update_GameObject(_double TimeDelta)
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

HRESULT CCustomSliderBar::Render_GameObject()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pBufferCom)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

	_int iIndex = 0;
	LOOP(2)
	{
		if (0 == i)
		{
			iIndex = 10;
			m_fSizeX = 512.f;
			m_fSizeY = 32.f;
			m_fPosX = m_fBackBarPosX;
			m_fPosY = m_fBackBarPosY;
			Late_Update_GameObject(0.f);

			if (FAILED(SetUp_ConstantTable(iIndex)))
				return E_FAIL;
		}
		else if (1 == i)
		{
			iIndex = 11;
			m_fSizeX = 48.f;
			m_fSizeY = 48.f;
			m_fPosX = m_fControlBarPosX;
			m_fPosY = m_fControlBarPosY;
			Late_Update_GameObject(0.f);

			if (FAILED(SetUp_ConstantTable(iIndex)))
				return E_FAIL;
		}

		m_pShaderCom->Begin_Shader();
		m_pShaderCom->Begin_Pass(1);
		m_pBufferCom->Render_VIBuffer();
		m_pShaderCom->End_Pass();
		m_pShaderCom->End_Shader();
	}
	
	return NOERROR;
}

HRESULT CCustomSliderBar::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"DefaultTex_Custom_UI", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	

	return NOERROR;
}

HRESULT CCustomSliderBar::SetUp_ConstantTable(_uint iIndex)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &m_matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->SetUp_OnShader("g_DiffuseTexture", m_pShaderCom, iIndex)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CCustomSliderBar::SetUp_CursorEffect()
{

	return NOERROR;
}

void CCustomSliderBar::Late_Init()
{
	if (m_bLateInit)
		return;

	m_bLateInit = true;
	m_pPlayer = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

	switch (m_eType)
	{
	case Client::CCustomSliderBar::TYPE_HAIR_HUE:
		m_fControlBarPosX = m_fMaxLen;
		break;
	case Client::CCustomSliderBar::TYPE_HAIR_CONTRAST:
		m_fControlBarPosX = m_fMinLen;
		break;
	case Client::CCustomSliderBar::TYPE_HAIR_BRIGHTNESS:
		m_fControlBarPosX = m_fMinLen;
		break;
	case Client::CCustomSliderBar::TYPE_HAIR_SATURATION:
		m_fControlBarPosX = m_fMaxLen;
		break;
	}
}

void CCustomSliderBar::Check_Drag()
{
	if (m_bIsColl && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
	{
		m_bDragStart = true;

		g_pSoundManager->Stop_Sound(CSoundManager::UI_SFX_01);
		g_pSoundManager->Play_Sound(L"UI_CommonClick.wav", CSoundManager::UI_SFX_01, CSoundManager::Effect_Sound);
	}

	if (g_pInput_Device->Get_DIMouseState_Up(CInput_Device::DIM_LB))
		m_bDragStart = false;

	if (m_bDragStart)
	{
		m_fControlBarPosX = _float(g_pInput_Device->Get_MousePos().x);

		if (m_fControlBarPosX < m_fMinLen)
			m_fControlBarPosX = m_fMinLen;
		if (m_fControlBarPosX > m_fMaxLen)
			m_fControlBarPosX = m_fMaxLen;

		_float fSliderLen = m_fMaxLen - m_fMinLen;
		_float fMyLen = m_fMaxLen - m_fControlBarPosX;
		m_fValue = 1.f - (fMyLen / fSliderLen);
	}
}

void CCustomSliderBar::Check_Value()
{
	CCostume_Hair* pHair = m_pPlayer->Get_Costume_Hair();
	_v4 vColor = pHair->Get_Color();
	switch (m_eType)
	{
	case Client::CCustomSliderBar::TYPE_HAIR_HUE:
		vColor.x = m_fValue * 360.f;
		break;
	case Client::CCustomSliderBar::TYPE_HAIR_CONTRAST:
		vColor.y = m_fValue;
		break;
	case Client::CCustomSliderBar::TYPE_HAIR_BRIGHTNESS:
		vColor.z = m_fValue;
		break;
	case Client::CCustomSliderBar::TYPE_HAIR_SATURATION:
		vColor.w = m_fValue;
		break;
	}

	pHair->Set_Color(vColor);
}


_bool CCustomSliderBar::Coll_Mouse()
{
	POINT pt = {};

	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	_float fMouseX = static_cast<float>(pt.x);
	_float fMouseY = static_cast<float>(pt.y);

	if (fMouseX < (m_fPosX - m_fSizeX * 0.5f) ||
		fMouseX >(m_fPosX + m_fSizeX * 0.5f) ||
		fMouseY < (m_fPosY - m_fSizeY * 0.5f) ||
		fMouseY >(m_fPosY + m_fSizeY * 0.5f))
	{
		m_bIsColl = false;
		return false;
	}

	m_bIsColl = true;
	return true;
}

CCustomSliderBar * CCustomSliderBar::Create(_Device pGraphic_Device)
{
	CCustomSliderBar* pInstance = new CCustomSliderBar(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("CCustomSliderBar Creating Fail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCustomSliderBar::Clone_GameObject(void * pArg)
{
	CCustomSliderBar* pInstance = new CCustomSliderBar(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned LogoBtn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomSliderBar::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}
