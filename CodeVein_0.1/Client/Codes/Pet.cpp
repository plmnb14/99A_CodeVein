#include "stdafx.h"
#include "..\Headers\Pet.h"

CPet::CPet(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CPet::CPet(const CPet & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPet::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CPet::Ready_GameObject(void * pArg)
{
	return S_OK;
}

_int CPet::Update_GameObject(_double TimeDelta)
{
	return _int();
}

_int CPet::Late_Update_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CPet::Render_GameObject()
{
	return S_OK;
}

HRESULT CPet::Render_GameObject_SetPass(CShader * pShader, _int iPass)
{
	return S_OK;
}

void CPet::Function_FBLR()
{
}

void CPet::Function_RotateBody()
{
}

void CPet::Function_MoveAround(_float _fSpeed, _v3 _vDir)
{
}

void CPet::Function_CoolDown()
{
}

void CPet::Function_Movement(_float _fspeed, _v3 _vDir)
{
}

void CPet::Function_DecreMoveMent(_float _fMutiply)
{
}

void CPet::Function_ResetAfterAtk()
{
}

CGameObject * CPet::Clone_GameObject(void * pArg)
{
	return nullptr;
}

void CPet::Free()
{
}
