#include "stdafx.h"
#include "..\Headers\Effect_Parent.h"
#include "ParticleMgr.h"

CEffect_Parent::CEffect_Parent(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CEffect_Parent::CEffect_Parent(const CEffect_Parent & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_Parent::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CEffect_Parent::Ready_GameObject(void* pArg)
{
	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		return S_OK;
	}

	m_fRotSpeed = 1.f;
	m_dLifeTime = 5.f;
	m_fAccel = 1.f;
	m_fJumpPower = 5.f;

	m_dCurTime = 0.f;
	m_bDead = false;

	EFF_INFO tInfo = *(EFF_INFO*)pArg;
	m_fCreatePosY = tInfo.vCreatePos.y;
	m_vPos = tInfo.vCreatePos;
	m_vPos += _v3(0.f, 1.8f, 0.f);
	m_pTransformCom->Set_Pos(m_vPos);
	m_pTransformCom->Set_Scale(_v3(0.8f, 0.8f, 0.8f));
	m_vDir = tInfo.vDirection;
	m_fDelay = tInfo.fDelay;
	m_fSpeed = tInfo.fSpeed;
	lstrcpy(m_szName, tInfo.szBuff);

	// Calc Angle
	D3DXVec3Normalize(&m_vDir, &m_vDir);
	_v3	vRight = *D3DXVec3Cross(&vRight, &_v3(0.f, 1.f, 0.f), &m_vDir);
	V3_NORMAL_SELF(&vRight);
	_float	fDot = acosf(D3DXVec3Dot(&_v3{ 0,0,1 }, &m_vDir));
	if (vRight.z > 0)
		fDot *= -1.f;

	m_pTransformCom->Set_Angle(_v3(0, fDot, 0));

	m_fDelay += _float(CCalculater::Random_Num_Double(0.0, 0.05));

	m_pBulletBody = CParticleMgr::Get_Instance()->Create_EffectReturn(m_szName);
	m_pBulletBody->Set_Desc(_v3(0, 0, 0), nullptr);
	m_pBulletBody->Set_ParentObject(this);
	m_pBulletBody->Reset_Init();
	//g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody, SCENE_STAGE, L"Layer_Effect", nullptr);

	return NOERROR;
}

_int CEffect_Parent::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;
	
	Check_Delay(TimeDelta);
	if (m_fDelay > 0.f)
		return NO_EVENT;

	Check_Move(TimeDelta);

	m_dCurTime += TimeDelta;
		
	// 시간 초과
	if (m_dCurTime > m_dLifeTime)
	{
		//죽음 이펙트
		m_pBulletBody->Set_Dead();
		m_bDead = true;
	}
	// 진행중
	else
	{
		if (m_bEffect)
		{
			m_bEffect = false;
		}

		m_fEffectOffset += _float(TimeDelta);
		if (m_fEffectOffset > 0.1f)
		{
			m_fEffectOffset = 0.f;
		}
	}

	return NOERROR;
}

_int CEffect_Parent::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_EFFECT, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect_Parent::Render_GameObject()
{
	return NOERROR;
}

void CEffect_Parent::Check_Move(_double TimeDelta)
{
	_v3 vMove = m_vDir * m_fSpeed * _float(TimeDelta);
	m_pTransformCom->Add_Pos(vMove);
}

void CEffect_Parent::Check_Delay(_double TimeDelta)
{
	m_fDelay -= _float(TimeDelta);
}

HRESULT CEffect_Parent::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect_Parent::SetUp_ConstantTable()
{
	return NOERROR;
}

CEffect_Parent * CEffect_Parent::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_Parent* pInstance = new CEffect_Parent(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CEffect_Parent");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_Parent::Clone_GameObject(void * pArg)
{
	CEffect_Parent* pInstance = new CEffect_Parent(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CEffect_Parent");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Parent::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
