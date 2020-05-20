#include "stdafx.h"
#include "..\Headers\MaterialSellCollectionUI.h"
#include "UI_Manager.h"


CMaterialSellCollectionUI::CMaterialSellCollectionUI(_Device pDevice)
	: CUI(pDevice)
{
}

CMaterialSellCollectionUI::CMaterialSellCollectionUI(const CMaterialSellCollectionUI & rhs)
	: CUI(rhs)
{
}

HRESULT CMaterialSellCollectionUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CMaterialSellCollectionUI::Ready_GameObject(void * pArg)
{
	CUI::Ready_GameObject(pArg);

	return NOERROR;
}

_int CMaterialSellCollectionUI::Update_GameObject(_double TimeDelta)
{

	return _int();
}

_int CMaterialSellCollectionUI::Late_Update_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CMaterialSellCollectionUI::Render_GameObject()
{
	return E_NOTIMPL;
}

HRESULT CMaterialSellCollectionUI::Add_Component()
{
	return E_NOTIMPL;
}

HRESULT CMaterialSellCollectionUI::SetUp_ConstantTable(_uint iIndex)
{
	return E_NOTIMPL;
}

void CMaterialSellCollectionUI::SetUp_SubUI_Active(_bool bIsActive)
{
}

void CMaterialSellCollectionUI::SetUp_SlotPos()
{
}

void CMaterialSellCollectionUI::Click_SubUI()
{
}

void CMaterialSellCollectionUI::SetUp_Default()
{
}

void CMaterialSellCollectionUI::Sell_MtrlItem(CMaterial_Slot * pSlot)
{
}

CMaterialSellCollectionUI * CMaterialSellCollectionUI::Create(_Device pGraphic_Device)
{
	return nullptr;
}

CGameObject * CMaterialSellCollectionUI::Clone_GameObject(void * pArg)
{
	return nullptr;
}

void CMaterialSellCollectionUI::Free()
{
}
