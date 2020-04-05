#include "stdafx.h"
#include "..\Headers\Total_Inven.h"


CTotal_Inven::CTotal_Inven(_Device pDevice)
	: CUI(pDevice)
{
}

CTotal_Inven::CTotal_Inven(const CTotal_Inven & rhs)
	: CUI(rhs)
{
}

HRESULT CTotal_Inven::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();
	return NOERROR;
}

HRESULT CTotal_Inven::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	m_fPosX = WINCX * 0.5f;
	m_fPosY = WINCY * 0.5f;
	m_fSizeX = 700.f;
	m_fSizeY = WINCY;
	m_fViewZ = 0.5f;
	m_bIsActive = false;

	SetUp_Default();

	return NOERROR;
}

_int CTotal_Inven::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CTotal_Inven::Late_Update_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CTotal_Inven::Render_GameObject()
{
	return E_NOTIMPL;
}

HRESULT CTotal_Inven::Add_Component()
{
	return E_NOTIMPL;
}

HRESULT CTotal_Inven::SetUp_ConstantTable()
{
	return E_NOTIMPL;
}

void CTotal_Inven::SetUp_Default()
{
}

void CTotal_Inven::Click_Inven()
{
}

CTotal_Inven * CTotal_Inven::Create(_Device pGraphic_Device)
{
	return nullptr;
}

CGameObject * CTotal_Inven::Clone_GameObject(void * pArg)
{
	return nullptr;
}

void CTotal_Inven::Free()
{
}
