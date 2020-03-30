#include "stdafx.h"
#include "..\Headers\PoisonBullet.h"
#include "ParticleMgr.h"

CPoisonBullet::CPoisonBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPoisonBullet::CPoisonBullet(const CPoisonBullet & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPoisonBullet::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CPoisonBullet::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	BULLET_INFO temp = *(BULLET_INFO*)(pArg);

	m_pTransformCom->Set_Pos(temp.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_vDir = temp.vDir;
	m_fSpeed = temp.fSpeed;
	m_dLifeTime = temp.dLifeTime;

	lstrcpy(m_pEffect_Tag0, L"ButterFly_PointParticle");
	lstrcpy(m_pEffect_Tag1, L"ButterFly_SoftSmoke_Mist");
	lstrcpy(m_pEffect_Tag2, L"ButterFly_VenomShot");
	lstrcpy(m_pEffect_Tag3, L"ButterFly_VenomShot_SubSmoke");
	lstrcpy(m_pEffect_Tag4, L"ButterFly_PointParticle");

	return NOERROR;
}

_int CPoisonBullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta, m_vDir);

	m_dCurTime += TimeDelta;

	// 시간 초과
	if (m_dCurTime > m_dLifeTime)
	{
		//죽음 이펙트

		m_bDead = true;
	}
	// 진행중
	else
	{
		if (m_bEffect)
		{
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag0, _v3(), m_pTransformCom);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag1, _v3(), m_pTransformCom);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag2, _v3(), m_pTransformCom);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag3, _v3(), m_pTransformCom);
			CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag4, _v3(), m_pTransformCom);

			m_bEffect = false;
		}
	}


	return NOERROR;
}

_int CPoisonBullet::Late_Update_GameObject(_double TimeDelta)
{
	// 충돌처리

	return NOERROR;
}

HRESULT CPoisonBullet::Render_GameObject()
{
	
	return NOERROR;
}

HRESULT CPoisonBullet::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CPoisonBullet::SetUp_ConstantTable()
{
	return NOERROR;
}

CPoisonBullet * CPoisonBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPoisonBullet* pInstance = new CPoisonBullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CPoisonBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPoisonBullet::Clone_GameObject(void * pArg)
{
	CPoisonBullet* pInstance = new CPoisonBullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPoisonBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPoisonBullet::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);

	CGameObject::Free();
}
