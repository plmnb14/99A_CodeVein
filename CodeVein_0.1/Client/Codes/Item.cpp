#include "stdafx.h"
#include "..\Headers\Item.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CItem::CItem(const CItem & rhs)
	:CGameObject(rhs)
{
}

HRESULT CItem::Render_GameObject_SetPass(CShader * pShader, _int iPass)
{
	return S_OK;
}

void CItem::Check_PosY()
{
	m_pTransform->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransform->Get_Pos()));

	return;
}

void CItem::Check_Dist()
{
	return;
}

void CItem::Update_Collider()
{
	return;
}

void CItem::Render_Collider()
{
	return;
}

void CItem::Check_CollisionHit()
{
	return;
}

void CItem::Check_Ypos()
{
	m_pTransform->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransform->Get_Pos()));

	return;
}

HRESULT CItem::Ready_Status(void * _pArg)
{
	return S_OK;
}

HRESULT CItem::Ready_Collider()
{
	return S_OK;
}

void CItem::Free()
{
	CGameObject::Free();

	return;
}
