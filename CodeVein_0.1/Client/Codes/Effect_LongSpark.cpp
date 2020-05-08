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

	m_fSpeed = _float(CCalculater::Random_Num_Double(7.0, 10.0));
	m_fRotSpeed = 1.f;
	m_dLifeTime = 5.f;
	m_fAccel = 1.f;
	m_fJumpPower = 5.f;

	m_dCurTime = 0.f;
	m_bDead = false;

	//_v3 vCreatePos = *(_v3*)pArg;
	EFF_INFO tInfo = *(EFF_INFO*)pArg;
	m_fCreatePosY = tInfo.vCreatePos.y;
	m_vPos = tInfo.vCreatePos;
	m_vPos += _v3(0.f, 1.8f, 0.f);
	m_pTransformCom->Set_Pos(tInfo.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(0.8f, 0.8f, 0.8f));

	_mat matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat matY, matZ;
	D3DXMatrixInverse(&matView, NULL, &matView);
	//_v3 vViewUp = _v3(matView._21, matView._22, matView._23);
	_v3 vViewLook = _v3(matView._31, matView._32, matView._33);
	//D3DXMatrixRotationAxis(&matY, &vViewUp,		D3DXToRadian(_float(CCalculater::Random_Num_Double(0.0, 10.0))));
	D3DXMatrixRotationAxis(&matZ, &vViewLook,	D3DXToRadian(_float(CCalculater::Random_Num_Double(0.0, 30.0))));

	//m_vDir = *D3DXVec3TransformNormal(&_v3(), &_v3(1, 1, 1), &(matZ));
	//m_vDir += tInfo.vDirection;

	//m_vRot.z = _float(CCalculater::Random_Num_Double(45.0, 50.0));
	
	//_v3	vRight = *D3DXVec3Cross(&vRight, &_v3(1, 0, 0), &vViewLook);
	//V3_NORMAL_SELF(&vRight);
	//_float	fDot = acosf(D3DXVec3Dot(&_v3{ 0,0,1 }, &m_vDir));
	if (CCalculater::Random_Num(0, 1) == 0)
		m_bLeft = true;

	m_fDelay = _float(CCalculater::Random_Num_Double(0.0, 0.1));

	_tchar szBuff[256] = L"";
	if(!m_bLeft)
		wsprintf(szBuff, L"Hit_LongSpark_R_%d", CCalculater::Random_Num(0, 2));
	else
		wsprintf(szBuff, L"Hit_LongSpark_L_%d", CCalculater::Random_Num(0, 2));

	if (CCalculater::Random_Num(0, 2) == 0)
	{
		wsprintf(szBuff, L"Hit_LongSpark_Distortion_%d", 0);
	}
	

	//m_pBulletBody = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(szBuff, nullptr));
	m_pBulletBody = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(szBuff, nullptr));
	m_pBulletBody->Set_Desc(_v3(0, 0, 0), nullptr);
	m_pBulletBody->Set_ParentObject(this);
	m_pBulletBody->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody, SCENE_STAGE, L"Layer_Effect", nullptr);

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
	m_fSpeed -= m_fSpeed * _float(TimeDelta) * 2.f;
	if (m_fSpeed < 0.25f)
		m_fSpeed = 0.25f;
	m_fRotSpeed += m_fRotSpeed * _float(TimeDelta) * 1.0f;
		
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
	_mat matView = g_pManagement->Get_Transform(D3DTS_VIEW);
	D3DXMatrixInverse(&matView, NULL, &matView);
	_v3 vViewRight = _v3(matView._11, matView._12, matView._13);
	_v3 vViewLook = _v3(matView._31, matView._32, matView._33);
	_float fRandAngle = _float(CCalculater::Random_Num_Double(0.0, 30.0));
	
	if (m_bLeft)
	{
		fRandAngle = -fRandAngle;
		vViewRight = -vViewRight;
	}
	
	_mat matZ;
	D3DXMatrixRotationAxis(&matZ, &vViewLook, D3DXToRadian(fRandAngle));
	_v3 vDir = *D3DXVec3TransformNormal(&_v3(), &vViewRight, &(matZ));
	_v3 vNormalizeDir;
	D3DXVec3Normalize(&vNormalizeDir, &(vDir));

	_v3 vMove = vNormalizeDir * m_fSpeed * _float(TimeDelta);

	//m_pTransformCom->Set_Angle(D3DXToRadian(m_vRot) * _float(TimeDelta));

	m_fAccel += _float(TimeDelta) * m_fSpeed * 0.5f;
	const _float _GRAVITY = 6.f;
	_float fY = (m_fJumpPower * m_fAccel + -_GRAVITY * m_fAccel * m_fAccel * 0.5f) *  _float(TimeDelta);
	m_vPos += vMove;
	m_vPos.y += fY;
	
	_float fRotValue = (-90.f);
	if (m_bLeft)
		fRotValue = -fRotValue;
	m_fAngleZ += fRotValue * m_fRotSpeed * _float(TimeDelta);
	_float fAngle = m_fAngleZ + fRandAngle;
	if (m_bLeft && fAngle > 90.f)fAngle = 90.f;
	if (!m_bLeft && fAngle < -90.f)fAngle = -90.f;
	D3DXMatrixRotationAxis(&matView, &vViewLook, D3DXToRadian(fAngle));
	
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
