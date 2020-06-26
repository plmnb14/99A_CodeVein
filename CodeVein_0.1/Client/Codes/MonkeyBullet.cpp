#include "stdafx.h"
#include "..\Headers\MonkeyBullet.h"

CMonkeyBullet::CMonkeyBullet(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CMonster_Bullet(pGraphic_Device)
{
}

CMonkeyBullet::CMonkeyBullet(const CMonkeyBullet & rhs)
	: CMonster_Bullet(rhs)
{
}

HRESULT CMonkeyBullet::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CMonkeyBullet::Ready_GameObject(void * pArg)
{
	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		Ready_Collider();

		return S_OK;
	}

	MONSTER_BULLET_STATUS info = *(MONSTER_BULLET_STATUS*)pArg;

	m_eBulletType = info.eBulletType;
	m_vDir = info.vDir;
	m_fSpeed = info.fSpeed;
	m_dLifeTime = info.dLifeTime;

	m_pTransformCom->Set_Pos(info.vCreatePos);
	m_pTransformCom->Set_Scale(V3_ONE);

	// Calc Angle
	_v3	vRight = *D3DXVec3Cross(&vRight, &_v3(0.f, 1.f, 0.f), &m_vDir);
	V3_NORMAL_SELF(&vRight);
	_float	fDot = acosf(D3DXVec3Dot(&_v3{ 0,0,1 }, &m_vDir));
	if (vRight.z > 0)
		fDot *= -1.f;

	m_pTransformCom->Set_Angle(_v3(0.f, fDot, 0.f));

	m_tObjParam.bCanAttack = true;
	m_tObjParam.fDamage = 270.f * pow(1.5f, g_eStageIdx_Cur - 1);

	m_dCurTime = 0;
	m_bDead = false;
	m_bEffect = true;

	m_pEffect = CParticleMgr::Get_Instance()->Create_EffectReturn(L"Monkey_Knife");
	m_pEffect->Set_Desc(V3_NULL, nullptr);
	m_pEffect->Set_ParentObject(this);
	m_pEffect->Reset_Init();

	m_pTrailEffect = g_pManagement->Create_Trail();
	m_pTrailEffect->Set_TrailIdx(5); // Red Tail

	return S_OK;
}

_int CMonkeyBullet::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;

	Check_CollisionEvent();
	Update_Trails();

	m_pTransformCom->Add_Pos(m_fSpeed * (_float)TimeDelta, m_vDir);

	m_dCurTime += TimeDelta;

	if (m_dCurTime > m_dLifeTime)
	{
		m_pEffect->Set_Dead();
		m_pTrailEffect->Set_Dead();

		m_bDead = true;
	}
	else
	{
		if (m_bEffect)
			m_bEffect = false;
	}

	return NO_EVENT;
}

_int CMonkeyBullet::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NO_EVENT;
}

HRESULT CMonkeyBullet::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	Update_Collider();
	Render_Collider();

	return S_OK;
}

HRESULT CMonkeyBullet::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonkeyBullet::Ready_Collider()
{
	m_vecAttackCol.reserve(1);

	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 0.4f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

CMonkeyBullet * CMonkeyBullet::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMonkeyBullet* pInstance = new CMonkeyBullet(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMonkeyBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonkeyBullet::Clone_GameObject(void * pArg)
{
	CMonkeyBullet* pInstance = new CMonkeyBullet(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMonkeyBullet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonkeyBullet::Free()
{
	IF_NOT_NULL(m_pEffect)
		m_pEffect->Set_Dead();

	IF_NOT_NULL(m_pTrailEffect)
		m_pTrailEffect->Set_Dead();

	CMonster_Bullet::Free();

	return;
}
