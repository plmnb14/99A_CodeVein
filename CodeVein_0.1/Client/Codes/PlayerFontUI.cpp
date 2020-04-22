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

	//Calc_NumberFont(m_iNumber);
	MyNumberFont();
	
	return _int();
}

_int CPlayerFontUI::Late_Update_GameObject(_double TimeDelta)
{
	
	return _int();
}

HRESULT CPlayerFontUI::Render_GameObject()
{
	
	return NOERROR;
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
		iter->Set_UI_Index(12);

	_ulong dwDigits = Calc_Digits(m_iNumber);

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
		if (dwDigits <= 3)
			m_vecFontNum[i]->Set_UI_Pos(m_fPosX + m_fSizeX * i * 0.3f, m_fPosY);
		else if (dwDigits == 4)
			m_vecFontNum[i]->Set_UI_Pos(m_fPosX + m_fSizeX * i * 0.3f - m_fSizeX * 0.3f, m_fPosY);
		else if (dwDigits == 5)
			m_vecFontNum[i]->Set_UI_Pos(m_fPosX + m_fSizeX * i * 0.3f - m_fSizeX * 0.3f, m_fPosY);
		else
			return;
		m_vecFontNum[i]->Set_UI_Size(m_fSizeX, m_fSizeY);
		m_vecFontNum[i]->Set_ViewZ(m_fViewZ);
	}
}

void CPlayerFontUI::Calc_NumberFont(_ulong dwFontNum)
{
	vector<CFontNumUI*> vecFontNumUI;
	_ulong dwNumDigits = Calc_Digits(dwFontNum);


	LOOP(_int(dwNumDigits))
	{
		vecFontNumUI.push_back(Make_FontNum());
	}

	_ulong iIdx = 0;
	while (dwFontNum != 0)
	{
		vecFontNumUI[iIdx]->Set_UI_Index(dwFontNum % 10);
		dwFontNum /= 10;

		++iIdx;
	}

	reverse(vecFontNumUI.begin(), vecFontNumUI.end());

	for (_uint i = 0; i < vecFontNumUI.size(); ++i)
	{
		if (dwNumDigits <= 3)
			vecFontNumUI[i]->Set_UI_Pos(m_fPosX + m_fSizeX * i * 0.3f, m_fPosY);
		else if (dwNumDigits == 4)
			vecFontNumUI[i]->Set_UI_Pos(m_fPosX + m_fSizeX * i * 0.3f - m_fSizeX * 0.3f, m_fPosY);
		else if (dwNumDigits == 5)
			vecFontNumUI[i]->Set_UI_Pos(m_fPosX + m_fSizeX * i * 0.3f - m_fSizeX * 0.3f, m_fPosY);
		else
			return;
		vecFontNumUI[i]->Set_UI_Size(m_fSizeX, m_fSizeY);
		vecFontNumUI[i]->Set_ViewZ(m_fViewZ);
	}

	for (_ulong i = 0; i < vecFontNumUI.size(); ++i)
	{
		vecFontNumUI[i]->Set_Dead();
	}
}

CFontNumUI* CPlayerFontUI::Make_FontNum()
{
	CUI::UI_DESC* pDesc = nullptr;

	g_pManagement->Add_GameObject_ToLayer(L"GameObject_FontNumUI", SCENE_STATIC, L"Layer_StaticUI", pDesc);

	CFontNumUI* pFontNumUI = static_cast<CFontNumUI*>(g_pManagement->Get_GameObjectBack(L"Layer_StaticUI", SCENE_STATIC));
	
	return pFontNumUI;
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

