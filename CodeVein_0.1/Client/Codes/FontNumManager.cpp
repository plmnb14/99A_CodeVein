#include "stdafx.h"
#include "..\Headers\FontNumManager.h"
#include "FontNumUI.h"

CFontNumManager::CFontNumManager(_Device pDevice)
	: CUI(pDevice)
{
}

CFontNumManager::CFontNumManager(const CFontNumManager & rhs)
	: CUI(rhs)
{
}


HRESULT CFontNumManager::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CFontNumManager::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);
	

	return NOERROR;
}

_int CFontNumManager::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	Calc_NumberFont(m_iNumber);

	/*for (_uint i = 0; i < m_vecFontNum.size(); ++i)
	{
	m_vecFontNum[i]->Set_UI_Pos(m_fPosX + m_fSizeX * i, m_fPosY);
	m_vecFontNum[i]->Set_UI_Size(m_fSizeX, m_fSizeY);
	m_vecFontNum[i]->Set_ViewZ(m_fViewZ);
	}*/
	return _int();
}

_int CFontNumManager::Late_Update_GameObject(_double TimeDelta)
{
	
	return _int();
}

HRESULT CFontNumManager::Render_GameObject()
{
	/*for (_ulong i = 0; i < m_vecFontNum.size(); ++i)
	{
	m_vecFontNum[i]->Set_Dead();
	}
	m_vecFontNum.shrink_to_fit();*/
	return NOERROR;
}


HRESULT CFontNumManager::Add_Component()
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

_ulong CFontNumManager::Calc_Digits(_ulong dwNumber)
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

void CFontNumManager::Calc_NumberFont(_ulong dwFontNum/*, _float fPosX, _float fPosY, _float fSizeX, _float fSizeY, _float fViewZ*/)
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
		vecFontNumUI[i]->Set_UI_Pos(m_fPosX + m_fSizeX * i, m_fPosY);
		vecFontNumUI[i]->Set_UI_Size(m_fSizeX, m_fSizeY);
		vecFontNumUI[i]->Set_ViewZ(m_fViewZ);
	}

	/*_uint num = 0;
	for (auto& pNum : vecFontNumUI)
	{
	pNum->Set_UI_Pos(fPosX - fSizeX * num, fPosY);
	pNum->Set_UI_Size(fSizeX, fSizeY);
	pNum->Set_ViewZ(fViewZ);
	++num;
	}*/


	for (_ulong i = 0; i < vecFontNumUI.size(); ++i)
	{
		vecFontNumUI[i]->Set_Dead();
	}


	/*reverse(vecFontNumUI.begin(), vecFontNumUI.end());
	m_vecFontNum = vecFontNumUI;*/
}

CFontNumUI* CFontNumManager::Make_FontNum()
{
	CUI::UI_DESC* pDesc = nullptr;

	g_pManagement->Add_GameObject_ToLayer(L"GameObject_FontNumUI", SCENE_STAGE, L"Layer_FontNumUI", pDesc);

	CFontNumUI* pFontNumUI = static_cast<CFontNumUI*>(g_pManagement->Get_GameObjectBack(L"Layer_FontNumUI", SCENE_STAGE));

	return pFontNumUI;
}

CFontNumManager * CFontNumManager::Create(_Device pGraphic_Device)
{
	CFontNumManager* pInstance = new CFontNumManager(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CFontNumManager::Clone_GameObject(void * pArg)
{
	CFontNumManager* pInstance = new CFontNumManager(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CFontNumManager::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);

	CUI::Free();
}

