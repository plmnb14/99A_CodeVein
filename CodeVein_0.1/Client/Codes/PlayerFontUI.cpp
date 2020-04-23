#include "stdafx.h"
#include "..\Headers\PlayerFontUI.h"
#include "FontNumUI.h"

CPlayerFontUI::CPlayerFontUI(_Device pDevice)
	: CUI(pDevice)
{
}

CPlayerFontUI::CPlayerFontUI(const CPlayerFontUI & rhs)
	: CUI(rhs)
{
}


HRESULT CPlayerFontUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CPlayerFontUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	
	CFontNumUI* pFont = nullptr;

	// 5자리까지 생성
	LOOP(5)
	{
		pFont = static_cast<CFontNumUI*>(g_pManagement->Clone_GameObject_Return(L"GameObject_FontNumUI", nullptr));
		pFont->Set_UI_Pos(m_fPosX, m_fPosY);
		pFont->Set_UI_Size(m_fSizeX, m_fSizeY);
		pFont->Set_ViewZ(m_fViewZ);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pFont, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
		m_vecFontNum.push_back(pFont);
	}
	
	return NOERROR;
}

_int CPlayerFontUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);
	if (0 >= m_iNumber)
		m_iNumber = 0;
	
	MyNumberFont();
	
	return 0;
}

HRESULT CPlayerFontUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_Number", L"Com_Texture", (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBufferCom)))
		return E_FAIL;

	return NOERROR;
}

_ulong CPlayerFontUI::Calc_Digits(_ulong dwNumber)
{
	_ulong dwDigits = 0;

	if (0 == (dwNumber / 10))
		dwDigits = 1;
	else if (0 < (dwNumber / 10) && 10 > (dwNumber / 10))
		dwDigits = 2;
	else if (10 <= (dwNumber / 10) && 100 > (dwNumber / 10))
		dwDigits = 3;
	else if (100 <= (dwNumber / 10) && 1000 > (dwNumber / 10))
		dwDigits = 4;
	else if (1000 <= (dwNumber / 10) && 10000 > (dwNumber / 10))
		dwDigits = 5;
	else
		return 0;

	return dwDigits;
}

void CPlayerFontUI::MyNumberFont()
{
	for (auto& iter : m_vecFontNum)
		iter->Set_UI_Index(0);

	_ulong dwDigits = Calc_Digits(m_iNumber);

	if (dwDigits > 5)
		return;

	_ulong iIdx = 0;

	
	while (m_iNumber != 0)
	{
		m_vecFontNum[iIdx]->Set_UI_Index(m_iNumber % 10);
		m_iNumber /= 10;
			++iIdx;
	}

	reverse(m_vecFontNum.begin(), m_vecFontNum.end());

	for (_uint i = 0; i < m_vecFontNum.size(); ++i)
	{
		m_vecFontNum[i]->Set_UI_Pos(m_fPosX + m_fSizeX * i * 0.3f, m_fPosY);				
		m_vecFontNum[i]->Set_UI_Size(m_fSizeX, m_fSizeY);
		m_vecFontNum[i]->Set_ViewZ(m_fViewZ);
		m_vecFontNum[i]->Set_Active(m_bIsActive);
	}

	switch (dwDigits)
	{
	case 1:
	{
		LOOP(4)
			m_vecFontNum[i]->Set_UI_Index(12);
	}
		break;
	case 2:
	{
		LOOP(3)
			m_vecFontNum[i]->Set_UI_Index(12);
	}
		break;
	case 3:
	{
		LOOP(2)
			m_vecFontNum[i]->Set_UI_Index(12);
	}
		break;
	case 4:
	{		
		m_vecFontNum[0]->Set_UI_Index(12);
	}
		break;
	case 5:
		break;
	}
}

CPlayerFontUI * CPlayerFontUI::Create(_Device pGraphic_Device)
{
	CPlayerFontUI* pInstance = new CPlayerFontUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CPlayerFontUI::Clone_GameObject(void * pArg)
{
	CPlayerFontUI* pInstance = new CPlayerFontUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CPlayerFontUI::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}

