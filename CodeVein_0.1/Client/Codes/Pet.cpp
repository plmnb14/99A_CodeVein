#include "stdafx.h"
#include "..\Headers\Pet.h"

CPet::CPet(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CPet::CPet(const CPet & rhs)
	:CGameObject(rhs)
{
}

void CPet::Function_FBLR(CGameObject* _pGameObject)
{
	//수정
	_float fAngle = D3DXToDegree(m_pTransformCom->Chase_Target_Angle(&TARGET_TO_TRANS(_pGameObject)->Get_Pos()));

	if (0.f <= fAngle && 30.f > fAngle)
		m_eFBLR = FBLR::FRONT;
	else if (30.f <= fAngle && 90.f > fAngle)
		m_eFBLR = FBLR::FRONTRIGHT;
	else if (90.f <= fAngle && 150.f > fAngle)
		m_eFBLR = FBLR::BACKRIGHT;
	else if (150.f <= fAngle && 180.f > fAngle)
		m_eFBLR = FBLR::BACK;
	else if (-180.f <= fAngle && -150.f > fAngle)
		m_eFBLR = FBLR::BACK;
	else if (-150.f <= fAngle && -90.f > fAngle)
		m_eFBLR = FBLR::BACKLEFT;
	else if (-90.f <= fAngle && -30.f > fAngle)
		m_eFBLR = FBLR::FRONTLEFT;
	else if (-30.f <= fAngle && 0 > fAngle)
		m_eFBLR = FBLR::FRONT;

	return;
}

void CPet::Function_RotateBody(CGameObject* _pGameObject)
{
	_float fTargetAngle = m_pTransformCom->Chase_Target_Angle(&TARGET_TO_TRANS(_pGameObject)->Get_Pos());

	_float fYAngle = m_pTransformCom->Get_Angle().y;

	_v3 vTargetDir = m_pTransformCom->Get_Axis(AXIS_Z);
	V3_NORMAL_SELF(&vTargetDir);

	if (fTargetAngle > 0)
	{
		if (fYAngle < 0)
		{
			if (-D3DXToRadian(90.f) > fYAngle && -D3DXToRadian(180.f) < fYAngle)
			{
				fYAngle -= DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle <= -D3DXToRadian(180.f)) fYAngle = D3DXToRadian(180.f);
			}
			else fYAngle += DELTA_60 * D3DXToRadian(360.f);
		}
		else
		{
			if (fYAngle < fTargetAngle)
			{
				fYAngle += DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle >= fTargetAngle) fYAngle = fTargetAngle;
			}
			else
			{
				fYAngle -= DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle <= fTargetAngle) fYAngle = fTargetAngle;
			}
		}
	}
	else if (fTargetAngle < 0)
	{
		if (fYAngle > 0)
		{
			if (D3DXToRadian(90.f) < fYAngle && D3DXToRadian(180.f) > fYAngle)
			{
				fYAngle += DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle >= D3DXToRadian(180.f)) fYAngle = -D3DXToRadian(180.f);
			}
			else fYAngle -= DELTA_60 * D3DXToRadian(360.f);
		}
		else
		{
			if (fYAngle > fTargetAngle)
			{
				fYAngle -= DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle <= fTargetAngle) fYAngle = fTargetAngle;
			}
			else
			{
				fYAngle += DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle >= fTargetAngle) fYAngle = fTargetAngle;
			}
		}
	}

	m_pTransformCom->Set_Angle(AXIS_Y, fYAngle);

	return;
}

void CPet::Function_MoveAround(CGameObject* _pGameObject, _float _fSpeed, _v3 _vDir)
{
	_float fTargetAngle = m_pTransformCom->Chase_Target_Angle(&TARGET_TO_TRANS(_pGameObject)->Get_Pos());

	_float fYAngle = m_pTransformCom->Get_Angle().y;

	_v3 vTargetDir = m_pTransformCom->Get_Axis(AXIS_Z);
	V3_NORMAL_SELF(&vTargetDir);

	if (fTargetAngle > 0)
	{
		if (fYAngle < 0)
		{
			if (-D3DXToRadian(90.f) > fYAngle && -D3DXToRadian(180.f) < fYAngle)
			{
				fYAngle -= DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle <= -D3DXToRadian(180.f)) fYAngle = D3DXToRadian(180.f);
			}
			else fYAngle += DELTA_60 * D3DXToRadian(360.f);
		}
		else
		{
			if (fYAngle < fTargetAngle)
			{
				fYAngle += DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle >= fTargetAngle) fYAngle = fTargetAngle;
			}
			else
			{
				fYAngle -= DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle <= fTargetAngle) fYAngle = fTargetAngle;
			}
		}
	}
	else if (fTargetAngle < 0)
	{
		if (fYAngle > 0)
		{
			if (D3DXToRadian(90.f) < fYAngle && D3DXToRadian(180.f) > fYAngle)
			{
				fYAngle += DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle >= D3DXToRadian(180.f)) fYAngle = -D3DXToRadian(180.f);
			}
			else fYAngle -= DELTA_60 * D3DXToRadian(360.f);
		}
		else
		{
			if (fYAngle > fTargetAngle)
			{
				fYAngle -= DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle <= fTargetAngle) fYAngle = fTargetAngle;
			}
			else
			{
				fYAngle += DELTA_60 * D3DXToRadian(360.f);

				if (fYAngle >= fTargetAngle) fYAngle = fTargetAngle;
			}
		}
	}

	m_pTransformCom->Set_Angle(AXIS_Y, fYAngle);

	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &_vDir, _fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

	return;
}

void CPet::Function_CoolDown()
{
	if (true == m_bCanCoolDown)
	{
		m_fCoolDownCur += DELTA_60;

		if (m_fCoolDownCur >= m_fCoolDownMax)
		{
			m_fCoolDownCur = 0.f;
			m_bCanCoolDown = false;
			m_bIsCoolDown = false;

			m_bIsMoveAround = false;

			m_tObjParam.bCanAttack = true;
		}
	}

	return;
}

void CPet::Function_Movement(_float _fspeed, _v3 _vDir)
{
	V3_NORMAL(&_vDir, &_vDir);
	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &_vDir, _fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

	return;
}

void CPet::Function_DecreMoveMent(_float _fMutiply)
{
	m_fSkillMoveSpeed_Cur -= (0.3f - m_fSkillMoveAccel_Cur * m_fSkillMoveAccel_Cur * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60")) * _fMutiply;
	m_fSkillMoveAccel_Cur += g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

	if (m_fSkillMoveSpeed_Cur < 0.f)
	{
		m_fSkillMoveAccel_Cur = 0.5f;
		m_fSkillMoveSpeed_Cur = 0.f;
	}

	return;
}

void CPet::Function_CalcMoveSpeed(_float _fMidDist)
{
	_float fLenth = V3_LENGTH(&(TARGET_TO_TRANS(m_pPlayer)->Get_Pos() - m_pTransformCom->Get_Pos()));

	if (_fMidDist >= fLenth)
		m_fSkillMoveMultiply = (fLenth - m_fPersonalRange) / (_fMidDist- m_fPersonalRange);
	else
		m_fSkillMoveMultiply = 1.f;

	m_fSkillMoveSpeed_Cur = 8.f * m_fSkillMoveMultiply;

	return;
}

void CPet::Function_Find_Target(_float _fDist)
{
	if (!g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE).empty())
	{
		auto& MonsterContainer = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);

		for (auto& Monster_iter : MonsterContainer)
		{
			if (false == Monster_iter->Get_Enable())
				continue;
			else if(true == Monster_iter->Get_Dead())
				continue;
			else if(nullptr == Monster_iter)
				continue;
			//몬스터가 존재한다면
			else
			{
				if (_fDist >= V3_LENGTH(&(TARGET_TO_TRANS(Monster_iter)->Get_Pos() - m_pTransformCom->Get_Pos())))
				{
					m_eTarget = TARGET_TYPE::TARGET_MONSTER;
					m_pTarget = Monster_iter;
				}
				else
					continue;
			}
		}
	}
	else if (!g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE).empty())
	{
		auto& BossContainer = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);

		for (auto& Boss_iter : BossContainer)
		{
			if (false == Boss_iter->Get_Enable())
				continue;
			else if (true == Boss_iter->Get_Dead())
				continue;
			else if (nullptr == Boss_iter)
				continue;
			else
			{
				if (_fDist >= V3_LENGTH(&(TARGET_TO_TRANS(Boss_iter)->Get_Pos() - m_pTransformCom->Get_Pos())))
				{
					m_eTarget = TARGET_TYPE::TARGET_BOSS;
					m_pTarget = Boss_iter;
				}
				else
					continue;
			}
		}
	}
	//else if (!g_pManagement->Get_GameObjectList(L"Layer_Item", SCENE_STAGE).empty())
	//{
	//	auto& ItemContainer = g_pManagement->Get_GameObjectList(L"Layer_Item", SCENE_STAGE);
	//	for (auto& Item_iter : ItemContainer)
	//	{
	//		if (false == Item_iter->Get_Enable())
	//			continue;
	//		else
	//		{
	//			if (_fDist >= V3_LENGTH(&(TARGET_TO_TRANS(Item_iter)->Get_Pos() - m_pTransformCom->Get_Pos())))
	//			{
	//				m_eTarget = TARGET_TYPE::TARGET_ITEM;
	//				m_pTarget = Item_iter;
	//			}
	//			else
	//				continue;
	//		}
	//	}
	//}
	//else
	//{
	//	m_eTarget = TARGET_TYPE::TARGET_NONE;
	//	m_pTarget = nullptr;
	//}

	return;
}

void CPet::Function_ResetAfterAtk()
{
	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsHit = false;

	m_tObjParam.bCanDodge = true;
	m_tObjParam.bIsDodge = false;

	m_tObjParam.bSuperArmor = false;
	m_tObjParam.bIsAttack = false;

	m_bCanIdle = true;
	m_bIsIdle = false;

	m_bCanMoveAround = true;
	m_bIsMoveAround = false;

	m_bCanChooseAtkType = true;
	m_bIsCombo = false;

	for (auto& vetor_iter : m_vecAttackCol)
		vetor_iter->Set_Enabled(false);

	IF_NOT_NULL(m_pWeapon)
		m_pWeapon->Set_Target_CanAttack(false);
	IF_NOT_NULL(m_pWeapon)
		m_pWeapon->Set_Enable_Trail(false);

	LOOP(30)
		m_bEventTrigger[i] = false;

	return;
}

void CPet::Free()
{
	CGameObject::Free();

	return;
}
