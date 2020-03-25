#include "..\Headers\Effect.h"

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{

}

CEffect::CEffect(const CEffect & rhs)
	: CGameObject(rhs.m_pGraphic_Dev)
	, m_pInfo(rhs.m_pInfo)
{
	m_bClone = true;
}

void CEffect::Reset_Init()
{
	m_bIsDead = false;
	this->Setup_Info();
}

HRESULT CEffect::Ready_GameObject_Prototype()
{
	// 생성 시, 오래 걸릴 수 있는 작업들을 수행한다.
	return NOERROR;
}

HRESULT CEffect::Ready_GameObject(void* pArg)
{
	return NOERROR;
}

HRESULT CEffect::LateInit_GameObject()
{
	return S_OK;
}

_int CEffect::Update_GameObject(_double TimeDelta)
{
	return S_OK;
}

_int CEffect::Late_Update_GameObject(_double TimeDelta)
{
	return S_OK;
}


HRESULT CEffect::Render_GameObject()
{
	return NOERROR;
}

void CEffect::Setup_Info()
{
}


void CEffect::Free()
{
	if(!m_bClone) Safe_Delete(m_pInfo);
	
	CGameObject::Free();
}
