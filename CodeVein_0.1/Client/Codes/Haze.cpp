#include "stdafx.h"
#include "..\Headers\Haze.h"
#include "ParticleMgr.h"

CHaze::CHaze(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CHaze::CHaze(const CHaze & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHaze::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CHaze::Ready_GameObject(void* pArg)
{
	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		return S_OK;
	}

	m_fSpeed = 7.f;
	m_dLifeTime = 10.f;
	m_fCurveAccel = 1.f;
	m_fCurvePower = 15.f;

	HAZE_INFO tInfo = *(HAZE_INFO*)pArg;
	m_pTransformCom->Set_Pos(tInfo.vCreatePos + _v3(0.f, 1.3f, 0.f));
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	m_fDelay = tInfo.fDelay;
	m_fHazeValue = tInfo.fHazeValue;

	m_pBulletBody = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"Haze_Body", nullptr));
	m_pBulletBody->Set_Desc(_v3(0, 0, 0), m_pTransformCom);
	m_pBulletBody->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody, SCENE_STAGE, L"Layer_Effect", nullptr);

	m_pTrailEffect = g_pManagement->Create_Trail();
	m_pTrailEffect->Set_TrailIdx(4);

	_mat matRotX, matRotY, matRotZ;
	m_vCurveRotDir = _v3(1.f, 1.f, 1.f);
	D3DXMatrixRotationX(&matRotX, D3DXToRadian(CCalculater::Random_Num(0, 180)));
	D3DXMatrixRotationY(&matRotY, D3DXToRadian(CCalculater::Random_Num(0, 360)));
	D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(CCalculater::Random_Num(0, 180)));
	D3DXVec3TransformNormal(&m_vCurveRotDir, &m_vCurveRotDir, &matRotX);
	D3DXVec3TransformNormal(&m_vCurveRotDir, &m_vCurveRotDir, &matRotY);
	D3DXVec3TransformNormal(&m_vCurveRotDir, &m_vCurveRotDir, &matRotZ);
	D3DXVec3Normalize(&m_vCurveRotDir, &m_vCurveRotDir);

	m_pTargetTrans = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

	return NOERROR;
}

_int CHaze::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	if (m_bDead)
		return DEAD_OBJ;
	
	Check_Delay(TimeDelta);
	if (m_fDelay > 0.f)
		return NO_EVENT;

	Update_Trails(TimeDelta);
	Check_Move(TimeDelta);

	m_dCurTime += TimeDelta;

	// 시간 초과
	if (m_dCurTime > m_dLifeTime)
	{
		//죽음 이펙트
		m_pBulletBody->Set_Dead();
		m_pTrailEffect->Set_Dead();

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
			CParticleMgr::Get_Instance()->Create_Effect(L"Haze_FlashParticle", m_pTransformCom->Get_Pos());
		}
	}

	return NOERROR;
}

_int CHaze::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CHaze::Render_GameObject()
{
	return NOERROR;
}

void CHaze::Check_Move(_double TimeDelta)
{
	_v3 vDir = (m_pTargetTrans->Get_Pos() + _v3(0.f, 1.3f, 0.f)) - m_pTransformCom->Get_Pos();
	if (D3DXVec3Length(&vDir) < 0.5f)
	{
		m_dCurTime += 1000.f;
		return;
	}

	D3DXVec3Normalize(&vDir, &vDir);
	_v3 vMove = vDir * m_fSpeed * _float(TimeDelta);

	m_fCurveAccel -= _float(TimeDelta);
	_float fCurveValue = (m_fCurvePower * m_fCurveAccel + GRAVITY * m_fCurveAccel * m_fCurveAccel * 0.5f) * _float(TimeDelta);

	_v3 vCurveDir = *D3DXVec3Cross(&vCurveDir, &m_vCurveRotDir, &vDir);

	vMove += fCurveValue * vCurveDir * m_fSpeed * _float(TimeDelta);

	m_pTransformCom->Add_Pos(vMove);
}

void CHaze::Check_Delay(_double TimeDelta)
{
	m_fDelay -= _float(TimeDelta);
}

void CHaze::Update_Trails(_double TimeDelta)
{
	_mat matWorld = m_pTransformCom->Get_WorldMat();
	_v3 vBegin, vDir;

	memcpy(vBegin, &m_pTransformCom->Get_WorldMat()._41, sizeof(_v3));
	memcpy(vDir, &m_pTransformCom->Get_WorldMat()._21, sizeof(_v3));
	//m_vDir

	if (m_pTrailEffect)
	{
		m_pTrailEffect->Set_ParentTransform(&matWorld);
		m_pTrailEffect->Ready_Info(vBegin + vDir * -0.05f, vBegin + vDir * 0.05f);
		// m_pTrailEffect->Update_GameObject(TimeDelta);
	}
}

HRESULT CHaze::Add_Component()
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

HRESULT CHaze::SetUp_ConstantTable()
{
	return NOERROR;
}

CHaze * CHaze::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CHaze* pInstance = new CHaze(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CHaze");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHaze::Clone_GameObject(void * pArg)
{
	CHaze* pInstance = new CHaze(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CHaze");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHaze::Free()
{
	//Safe_Release(m_pTrailEffect);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
