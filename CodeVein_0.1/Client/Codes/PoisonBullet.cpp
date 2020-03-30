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

	lstrcpy(m_pEffect_Tag, L"ButterFly_SoftSmoke");

	return NOERROR;
}

_int CPoisonBullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta, m_vDir);

	m_dCurTime += TimeDelta;

	// �ð� �ʰ�
	if (m_dCurTime > m_dLifeTime)
	{
		//���� ����Ʈ

		m_bDead = true;
	}
	// ������
	else
	{
		CParticleMgr::Get_Instance()->Create_Effect(m_pEffect_Tag, m_pTransformCom->Get_Pos());
	}


	return NOERROR;
}

_int CPoisonBullet::Late_Update_GameObject(_double TimeDelta)
{
	// �浹ó��

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
