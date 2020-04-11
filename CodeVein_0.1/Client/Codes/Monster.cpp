#include "stdafx.h"
#include "..\Headers\Monster.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonster::LateInit_GameObject()
{
	return S_OK;
}

HRESULT CMonster::Render_GameObject_SetShader(CShader * pShader)
{
	return S_OK;
}

HRESULT CMonster::Render_GameObject_SetPass(CShader * pShader, _int iPass)
{
	return S_OK;
}

_bool CMonster::Is_InFov(_float fDegreeOfFov, CTransform* pSelfTransform, _v3 vTargetPos)
{
	_v3 vThisLook = *(_v3*)(&pSelfTransform->Get_WorldMat().m[2]);
	vThisLook.y = 0.f;
	D3DXVec3Normalize(&vThisLook, &vThisLook);

	_v3 FromThisToTarget = vTargetPos - pSelfTransform->Get_Pos();
	FromThisToTarget.y = 0.f;
	D3DXVec3Normalize(&FromThisToTarget, &FromThisToTarget);


	_float fDot_Temp = D3DXVec3Dot(&vThisLook, &FromThisToTarget);
	_float fRadian = acosf(fDot_Temp);

	//cout << "½Ã¾ß°¢ : " << D3DXToDegree(fRadian) << endl;

	if (D3DXToDegree(fRadian) < fDegreeOfFov * 0.5f)
		return true;

	return false;
}

HRESULT CMonster::Draw_Collider()
{
	for (auto& iter : m_vecPhysicCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return S_OK;
}

void CMonster::Free()
{
}