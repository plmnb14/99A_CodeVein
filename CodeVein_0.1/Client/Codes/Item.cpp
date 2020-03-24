#include "stdafx.h"
#include "..\Headers\Item.h"

CItem::CItem(_Device pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CItem::CItem(const CItem & rhs)
	: CGameObject(rhs)
{
}

HRESULT CItem::Ready_GameObject_Prototype()
{
	CGameObject::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CItem::Ready_GameObject(void * pArg)
{
	CGameObject::Ready_GameObject(pArg);

	

	return NOERROR;
}

_int CItem::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	
	return NO_EVENT;
}

_int CItem::Late_Update_GameObject(_double TimeDelta)
{
	

	return NO_EVENT;
}

HRESULT CItem::Render_GameObject()
{
	
	return NOERROR;
}



void CItem::Free()
{
	CGameObject::Free();
}
