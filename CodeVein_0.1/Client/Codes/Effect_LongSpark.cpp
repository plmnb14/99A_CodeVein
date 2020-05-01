#include "stdafx.h"
#include "..\Headers\Effect_LongSpark.h"
#include "ParticleMgr.h"

CEffect_LongSpark::CEffect_LongSpark(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CEffect_LongSpark::CEffect_LongSpark(const CEffect_LongSpark & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_LongSpark::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CEffect_LongSpark::Ready_GameObject(void* pArg)
{
	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		return S_OK;
	}

	m_fSpeed = 6.f;
	m_fRotSpeed = 7.f;
	m_dLifeTime = 10.f;
	m_fAccel = 1.f;
	m_fJumpPower = 0.f;

	//_v3 vCreatePos = *(_v3*)pArg;
	EFF_INFO tInfo = *(EFF_INFO*)pArg;
	m_fCreatePosY = tInfo.vCreatePos.y;
	m_vPos = tInfo.vCreatePos;
	m_vPos += _v3(0.f, 0.5f, 0.f);
	m_pTransformCom->Set_Pos(tInfo.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(0.8f, 0.8f, 0.8f));
	m_vDir = tInfo.vDirection;

	//m_fDelay = tInfo.fDelay;

	m_pBulletBody = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"Hit_LongSpark_0", nullptr));
	m_pBulletBody->Set_Desc(_v3(0, 0, 0), nullptr);
	m_pBulletBody->Set_ParentObject(this);
	m_pBulletBody->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody, SCENE_STAGE, L"Layer_Effect", nullptr);

	_mat matRotX, matRotY, matRotZ;
	_v3 vDir = _v3(1.f, 1.f, 1.f);
	//D3DXMatrixRotationX(&matRotX, D3DXToRadian(CCalculater::Random_Num(0, 360)));
	//D3DXMatrixRotationY(&matRotY, D3DXToRadian(CCalculater::Random_Num(0, 360)));
	D3DXMatrixRotationZ(&matRotZ, D3DXToRadian(CCalculater::Random_Num(0, 30)));
	//D3DXVec3TransformNormal(&vDir, &vDir, &matRotX);
	//D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
	D3DXVec3TransformNormal(&vDir, &vDir, &matRotZ);

	m_vDir += vDir;

	D3DXVec3Normalize(&m_vDir, &m_vDir);
	_v3	vRight = *D3DXVec3Cross(&vRight, &_v3(0, 1, 0), &m_vDir);
	V3_NORMAL_SELF(&vRight);

	if (vRight.z > 0)
		m_fAngleZ = 90.f;
	else
	{
		m_fAngleZ = -90.f;
		m_bLeft = true;
	}

	return NOERROR;
}

_int CEffect_LongSpark::Update_GameObject(_double TimeDelta)
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

_int CEffect_LongSpark::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_EFFECT, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect_LongSpark::Render_GameObject()
{
	return NOERROR;
}

void CEffect_LongSpark::Check_Move(_double TimeDelta)
{
	_v3 vNormalizeDir;
	D3DXVec3Normalize(&vNormalizeDir, &m_vDir);
	_v3 vMove = vNormalizeDir * m_fSpeed * _float(TimeDelta);

	m_fAccel += _float(TimeDelta);
	_float fY = (m_fJumpPower * m_fAccel + -GRAVITY * m_fAccel * m_fAccel * 0.5f) *  _float(TimeDelta);
	m_vPos += vMove;
	m_vPos.y += fY;
	//m_pTransformCom->Set_Pos(vPos);
	
	_float fRotValue = 1.f;
	if (m_bLeft)
		fRotValue = -1.f;
	m_fAngleZ += fRotValue * m_fRotSpeed * _float(TimeDelta);

	_mat matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	_v3 vViewLook = _v3(matView._31, matView._32, matView._33);
	D3DXMatrixRotationAxis(&matView, &vViewLook, m_fAngleZ);

	memcpy(&matView._41, &m_vPos, sizeof(_v3));

	m_pTransformCom->Set_WorldMat(matView);
}

void CEffect_LongSpark::Check_Delay(_double TimeDelta)
{
	m_fDelay -= _float(TimeDelta);
}

HRESULT CEffect_LongSpark::Add_Component()
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

HRESULT CEffect_LongSpark::SetUp_ConstantTable()
{
	return NOERROR;
}

CEffect_LongSpark * CEffect_LongSpark::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_LongSpark* pInstance = new CEffect_LongSpark(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CEffect_LongSpark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_LongSpark::Clone_GameObject(void * pArg)
{
	CEffect_LongSpark* pInstance = new CEffect_LongSpark(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CEffect_LongSpark");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_LongSpark::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
