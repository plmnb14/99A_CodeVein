#include "stdafx.h"
#include "..\Headers\Urchin.h"
#include "..\Headers\MonsterUI.h"

CUrchin::CUrchin(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CUrchin::CUrchin(const CUrchin & rhs)
	:CGameObject(rhs)
{
}

HRESULT CUrchin::Ready_GameObject_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CUrchin::Ready_GameObject(void * pArg)
{
	return E_NOTIMPL;
}

_int CUrchin::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CUrchin::Late_Update_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CUrchin::Render_GameObject()
{
	return E_NOTIMPL;
}

HRESULT CUrchin::Render_GameObject_SetPass(CShader * pShader, _int iPass)
{
	return E_NOTIMPL;
}

void CUrchin::Update_Collider()
{
}

void CUrchin::Render_Collider()
{
}

void CUrchin::Enter_Collision()
{
}

void CUrchin::Check_CollisionPush()
{
}

void CUrchin::Check_CollisionEvent(list<CGameObject*> plistGameObject)
{
}

void CUrchin::Function_RotateBody()
{
}

void CUrchin::Function_CoolDown()
{
}

void CUrchin::Function_Movement(_float _fspeed, _v3 _vDir)
{
}

void CUrchin::Function_DecreMoveMent(_float _fMutiply)
{
}

void CUrchin::Function_ResetAfterAtk()
{
}

void CUrchin::Check_PosY()
{
}

void CUrchin::Check_Hit()
{
}

void CUrchin::Check_FBLR()
{
}

void CUrchin::Check_Dist()
{
}

void CUrchin::Check_AniEvent()
{
}

void CUrchin::Play_Idle()
{
}

void CUrchin::Play_Shot()
{
}

void CUrchin::Play_Mist()
{
}

void CUrchin::Play_Hit()
{
}

void CUrchin::Play_Dead()
{
}

HRESULT CUrchin::Add_Component()
{
	return E_NOTIMPL;
}

HRESULT CUrchin::SetUp_ConstantTable()
{
	return E_NOTIMPL;
}

HRESULT CUrchin::Ready_Status(void * pArg)
{
	return E_NOTIMPL;
}

HRESULT CUrchin::Ready_Collider()
{
	return E_NOTIMPL;
}

HRESULT CUrchin::Ready_BoneMatrix()
{
	return E_NOTIMPL;
}

CUrchin * CUrchin::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	return nullptr;
}

CGameObject * CUrchin::Clone_GameObject(void * pArg)
{
	return nullptr;
}

void CUrchin::Free()
{
}
