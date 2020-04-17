#include "stdafx.h"
#include "..\Headers\SkillGauge.h"
#include "FontNumManager.h"

CSkillGauge::CSkillGauge(_Device pDevice)
	: CUI(pDevice)
{
}

CSkillGauge::CSkillGauge(const CSkillGauge & rhs)
	: CUI(rhs)
{
}

HRESULT CSkillGauge::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CSkillGauge::Ready_GameObject(void * pArg)
{
	return E_NOTIMPL;
}

_int CSkillGauge::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CSkillGauge::Late_Update_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CSkillGauge::Render_GameObject()
{
	return E_NOTIMPL;
}

HRESULT CSkillGauge::Add_Component()
{
	return E_NOTIMPL;
}

HRESULT CSkillGauge::SetUp_ConstantTable()
{
	return E_NOTIMPL;
}

CSkillGauge * CSkillGauge::Create(_Device pGraphic_Device)
{
	return nullptr;
}

CGameObject * CSkillGauge::Clone_GameObject(void * pArg)
{
	return nullptr;
}

void CSkillGauge::Free()
{
}
