#include "stdafx.h"
#include "..\Headers\IceBarrier.h"
#include "ParticleMgr.h"
#include "Effect.h"


CIceBarrier::CIceBarrier(LPDIRECT3DDEVICE9 pGraphic_device)
	: CMonster(pGraphic_device)
{
}

CIceBarrier::CIceBarrier(const CIceBarrier & rhs)
	: CMonster(rhs)
{
}

HRESULT CIceBarrier::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CIceBarrier::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	Ready_Collider();

	CBT_CreateBuff::BUFF_INFO temp = *(CBT_CreateBuff::BUFF_INFO*)(pArg);

	m_pTarget_Transform = temp.pTransform;
	Safe_AddRef(m_pTarget_Transform);

	m_pTarget_AIController = temp.pAIController;
	Safe_AddRef(m_pTarget_AIController);


	m_dLifeTime = temp.dLifeTime;

	m_tObjParam.bCanHit = true;
	m_tObjParam.fHp_Cur = 1.f;
	m_tObjParam.fHp_Max = m_tObjParam.fHp_Cur;

	m_pTransformCom->Set_Scale(_v3(1.f, 1.f, 1.f));

	return NOERROR;
}

_int CIceBarrier::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);


	if (m_bDead)
	{
		

		return DEAD_OBJ;
	}

	m_pTransformCom->Set_Pos(m_pTarget_Transform->Get_Pos() + _v3(0.f, 1.f, 0.f));

	m_dCurTime += TimeDelta;
	
	// 방어막은 일정시간 지나면 알아서 꺼짐
	if (m_dCurTime > m_dLifeTime)
	{
		m_bBarrierOff = true;	// 방어막 꺼짐 시작
	}
	else
	{
		m_fEffectOffset += _float(TimeDelta);
		// 일정시간마다 방어막 이펙트 발생
		if (m_fEffectOffset > 0.1f)
		{
			m_fEffectOffset = 0.f;
			CParticleMgr::Get_Instance()->Create_Effect(L"IceGirl_Buff_Aura", m_pTransformCom->Get_Pos(), nullptr);
		}
	}


	//  방어막 끝나고 0.5초동안 몬스터의 hit판정 막음.
	if (true == m_bBarrierOff)
	{
		if (!m_vecPhysicCol.empty())
		{
			for (auto& iter : m_vecPhysicCol)
			{
				Safe_Release(iter);
			}
			m_vecPhysicCol.clear();
		}

		m_dTime_BarrierOff += TimeDelta;

		if (m_dTime_BarrierOff > 0.5f)
		{
			m_pTarget_AIController->Set_Value_Of_BlackBoard(L"Ice_Barrier_On", false);
			m_bDead = true;
		}
	}
	else
		Check_PhyCollider();



	return NOERROR;
}

_int CIceBarrier::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRendererCom)
		return E_FAIL;

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CIceBarrier::Render_GameObject()
{
	Update_Collider();
	Draw_Collider();

	return NOERROR;
}


HRESULT CIceBarrier::Update_Collider()
{
	_ulong matrixIdx = 0;

	for (auto& iter : m_vecPhysicCol)
	{
		_mat tmpMat;
		tmpMat = m_pTransformCom->Get_WorldMat();

		_v3 ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);

		iter->Update(ColPos);

		++matrixIdx;
	}

	return NOERROR;
}

void CIceBarrier::Check_PhyCollider()
{
	if (false == m_tObjParam.bCanHit && m_tObjParam.bIsHit == false)
	{

		m_tObjParam.bIsHit = true;
		m_tObjParam.bCanHit = true;

		m_bBarrierOff = true;
		cout << "CIceBarrier - Check_PhyCollider: 보호막 깨짐" << endl;
	}
}

HRESULT CIceBarrier::Add_Component()
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

HRESULT CIceBarrier::SetUp_ConstantTable()
{
	return NOERROR;
}

HRESULT CIceBarrier::Ready_Collider()
{
	m_vecPhysicCol.reserve(2);

	//경계체크용
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = 1.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	// 총알 중앙
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	fRadius = 1.5f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_pTransformCom->Get_WorldMat().m[3][0], m_pTransformCom->Get_WorldMat().m[3][1], m_pTransformCom->Get_WorldMat().m[3][2]));
	pCollider->Set_Enabled(false);

	m_vecPhysicCol.push_back(pCollider);

	return NOERROR;
}

CIceBarrier * CIceBarrier::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CIceBarrier* pInstance = new CIceBarrier(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CIceBarrier");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CIceBarrier::Clone_GameObject(void * pArg)
{
	CIceBarrier* pInstance = new CIceBarrier(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CIceBarrier");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIceBarrier::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pCollider);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pTarget_Transform);
	Safe_Release(m_pTarget_AIController);

	CGameObject::Free();
}
