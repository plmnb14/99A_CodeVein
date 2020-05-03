#include "stdafx.h"
#include "..\Headers\Effect_BloodChunk.h"
#include "ParticleMgr.h"
#include "DecalEffect.h"

CEffect_BloodChunk::CEffect_BloodChunk(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CEffect_BloodChunk::CEffect_BloodChunk(const CEffect_BloodChunk & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffect_BloodChunk::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CEffect_BloodChunk::Ready_GameObject(void* pArg)
{
	if (nullptr == pArg)
	{
		if (FAILED(Add_Component()))
			return E_FAIL;

		return S_OK;
	}

	m_fSpeed = _float(CCalculater::Random_Num_Double(2.f, 4.f));
	m_dLifeTime = 10.f;
	m_fAccel = 1.f;
	m_fJumpPower = 6.f;

	EFF_INFO tInfo = *(EFF_INFO*)pArg;
	m_fCreatePosY = tInfo.vCreatePos.y;
	m_pTransformCom->Set_Pos(tInfo.vCreatePos);
	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));
	m_vDir = tInfo.vDirection;
	m_bIsWallDecal = tInfo.bWall;

	if (m_bIsWallDecal)
		m_fSpeed *= 2.5f;

	m_pBulletBody = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"Blood_Chunk_0", nullptr));
	m_pBulletBody->Set_Desc(_v3(0, 0, 0), m_pTransformCom);
	m_pBulletBody->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pBulletBody, SCENE_STAGE, L"Layer_Effect", nullptr);

	return NOERROR;
}

_int CEffect_BloodChunk::Update_GameObject(_double TimeDelta)
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

		_tchar szBuff[256] = L"";
		wsprintf(szBuff, L"Blood_Decal_%d", CCalculater::Random_Num(0, 6));
		if (m_bIsWallDecal)
		{
			CDecalEffect* pEff = static_cast<CDecalEffect*>(g_pManagement->Clone_GameObject_Return(szBuff, nullptr));
			pEff->Set_Desc(m_pTransformCom->Get_Pos(), nullptr);
			pEff->Reset_Init();
			pEff->Set_WallDecal(true);
			g_pManagement->Add_GameOject_ToLayer_NoClone(pEff, SCENE_STAGE, L"Layer_Effect", nullptr);
		}
		else
		{
			g_pManagement->Create_Effect_Decal(szBuff, m_pTransformCom->Get_Pos());
		}

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

_int CEffect_BloodChunk::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CEffect_BloodChunk::Render_GameObject()
{
	g_pManagement->Gizmo_Draw_Sphere(m_pCollider->Get_CenterPos(), m_pCollider->Get_Radius().x);

	return NOERROR;
}

void CEffect_BloodChunk::Check_Move(_double TimeDelta)
{
	_float fDownLength = m_fCreatePosY - m_pTransformCom->Get_Pos().y;
	if (fDownLength > (0.7f * m_fCreatePosY))
	{
		m_dCurTime += 1000.f;
		return;
	}

	if (m_bIsWallDecal &&
		fDownLength > (0.3f * m_fCreatePosY))
	{
		m_dCurTime += 1000.f;
		return;
	}

	if(!m_bIsWallDecal)
		D3DXVec3Normalize(&m_vDir, &m_vDir);
	_v3 vMove = m_vDir * m_fSpeed * _float(TimeDelta);
	

	m_fAccel += _float(TimeDelta);
	_float fY = (m_fJumpPower * m_fAccel + -GRAVITY * m_fAccel * m_fAccel * 0.5f) *  _float(TimeDelta);
	_v3 vPos = m_pTransformCom->Get_Pos();
	vPos += vMove;
	vPos.y += fY;
	m_pTransformCom->Set_Pos(vPos);
}

void CEffect_BloodChunk::Check_Delay(_double TimeDelta)
{
	m_fDelay -= _float(TimeDelta);
}

HRESULT CEffect_BloodChunk::Add_Component()
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

HRESULT CEffect_BloodChunk::SetUp_ConstantTable()
{
	return NOERROR;
}

CEffect_BloodChunk * CEffect_BloodChunk::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEffect_BloodChunk* pInstance = new CEffect_BloodChunk(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CEffect_BloodChunk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffect_BloodChunk::Clone_GameObject(void * pArg)
{
	CEffect_BloodChunk* pInstance = new CEffect_BloodChunk(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CEffect_BloodChunk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_BloodChunk::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();
}
