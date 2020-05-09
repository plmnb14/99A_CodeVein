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

HRESULT CPet::Render_GameObject_SetPass(CShader * pShader, _int iPass)
{
	return S_OK;
}

void CPet::Check_Navi()
{
	_tchar szNavData[STR_128] = L"";

	lstrcpy(szNavData, (
		g_eSceneID_Cur == 5 ? L"Navmesh_Training.dat" :
		g_eSceneID_Cur == 6 ? L"Navmesh_Stage_01.dat" :
		g_eSceneID_Cur == 7 ? L"Navmesh_Stage_02.dat" :
		g_eSceneID_Cur == 8 ? L"Navmesh_Stage_03.dat" : L"Navmesh_Stage_04.dat"));

	//player ���������� 3.f��ŭ ��ȯ�ɰ���
	m_pTransform->Set_Pos(TARGET_TO_TRANS(m_pPlayer)->Get_Axis(AXIS_X) * 1.f);

	m_pNavMesh->Set_Index(-1);
	m_pNavMesh->Ready_NaviMesh(m_pGraphic_Dev, szNavData);
	m_pNavMesh->Check_OnNavMesh(m_pTransform->Get_Pos());


	return;
}

void CPet::Check_CollisionEvent()
{
	Check_CollisionPush();
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE));
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));

	return;
}

void CPet::Check_CollisionPush()
{
	list<CGameObject*> tmpList[4];

	tmpList[0] = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);
	tmpList[1] = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
	tmpList[2] = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);
	tmpList[3] = g_pManagement->Get_GameObjectList(L"Layer_Pet", SCENE_STAGE);

	for (auto& list_iter : tmpList)
	{
		for (auto& Obj_iter : list_iter)
		{
			CCollider* pCollider = TARGET_TO_COL(Obj_iter);

			if (m_pCollider->Check_Sphere(pCollider, m_pTransform->Get_Axis(AXIS_Z), m_fSkillMoveSpeed_Cur * DELTA_60))
			{
				CTransform* pTrans = TARGET_TO_TRANS(Obj_iter);
				CNavMesh*   pNav = TARGET_TO_NAV(Obj_iter);

				_v3 vDir = m_pTransform->Get_Pos() - pTrans->Get_Pos();
				V3_NORMAL_SELF(&vDir);

				vDir.y = 0;

				pTrans->Set_Pos(pNav->Move_OnNaviMesh(NULL, &pTrans->Get_Pos(), &vDir, m_pCollider->Get_Length().x));
			}
		}
	}

	return;
}

void CPet::Check_CollisionHit(list<CGameObject*> plistGameObject)
{
	if (false == m_tObjParam.bIsAttack)
		return;

	_bool bFirst = true;

	for (auto& iter : plistGameObject)
	{
		if (false == iter->Get_Target_CanHit())
			continue;

		for (auto& vecIter : m_vecAttackCol)
		{
			if (false == vecIter->Get_Enabled())
				continue;

			bFirst = true;

			for (auto& vecCol : iter->Get_PhysicColVector())
			{
				if (vecIter->Check_Sphere(vecCol))
				{
					if (bFirst)
					{
						bFirst = false;
						continue;
					}

					if (false == iter->Get_Target_IsDodge())
					{
						iter->Set_Target_CanHit(false);
						iter->Add_Target_Hp(m_tObjParam.fDamage);

						if (iter->Get_Target_IsHit())
						{
							iter->Set_HitAgain(true);
						}
					}

					vecIter->Set_Enabled(false);

					g_pManagement->Create_Hit_Effect(vecIter, vecCol, TARGET_TO_TRANS(iter));

					break;
				}
				else
				{
					if (bFirst)
						break;
				}
			}
		}
	}

	return;
}

void CPet::Function_FBLR(CGameObject* _pGameObject)
{
	_float fAngle = D3DXToDegree(m_pTransform->Chase_Target_Angle(&TARGET_TO_TRANS(_pGameObject)->Get_Pos()));

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
	_float fTargetAngle = m_pTransform->Chase_Target_Angle(&TARGET_TO_TRANS(_pGameObject)->Get_Pos());

	_float fYAngle = m_pTransform->Get_Angle().y;

	_v3 vTargetDir = m_pTransform->Get_Axis(AXIS_Z);
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

	m_pTransform->Set_Angle(AXIS_Y, fYAngle);

	return;
}

void CPet::Function_MoveAround(CGameObject* _pGameObject, _float _fSpeed, _v3 _vDir)
{
	_float fTargetAngle = m_pTransform->Chase_Target_Angle(&TARGET_TO_TRANS(_pGameObject)->Get_Pos());

	_float fYAngle = m_pTransform->Get_Angle().y;

	_v3 vTargetDir = m_pTransform->Get_Axis(AXIS_Z);
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

	m_pTransform->Set_Angle(AXIS_Y, fYAngle);

	m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &_vDir, _fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

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
	m_pTransform->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransform->Get_Pos(), &_vDir, _fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

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
	_float fLenth = V3_LENGTH(&(TARGET_TO_TRANS(m_pPlayer)->Get_Pos() - m_pTransform->Get_Pos()));

	if (_fMidDist >= fLenth)
		m_fSkillMoveMultiply = (fLenth - m_fPersonalRange) / (_fMidDist- m_fPersonalRange);
	else
		m_fSkillMoveMultiply = 1.f;

	if (0.1f >= m_fSkillMoveMultiply)
		m_fSkillMoveMultiply = 0.f;

	m_fSkillMoveSpeed_Cur = 8.f * m_fSkillMoveMultiply;

	return;
}

void CPet::Function_Find_Target()
{
	auto& MonsterContainer = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);

	auto& BossContainer = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);

	auto& ItemContainer = g_pManagement->Get_GameObjectList(L"Layer_Item", SCENE_STAGE);

	_float	fOldLength = 99999.f;

	switch (m_eNowPetMode)
	{
	case PET_MODE_TYPE::PET_MODE_ATK:
		for (auto& Monster_iter : MonsterContainer)
		{
			if (true == Monster_iter->Get_Dead())
				continue;
			else if (false == Monster_iter->Get_Enable())
				continue;
			else if (nullptr == Monster_iter)
				continue;

			_float fLenth = V3_LENGTH(&(TARGET_TO_TRANS(Monster_iter)->Get_Pos() - m_pTransform->Get_Pos()));

			if (fLenth > m_fRecognitionRange)
				continue;

			if (fOldLength <= fLenth)
				continue;

			fOldLength = fLenth;
			m_pTarget = Monster_iter;
			m_eTarget = PET_TARGET_TYPE::PET_TARGET_MONSTER;
			Safe_AddRef(m_pTarget);
		}

		IF_NOT_NULL_RETURN(m_pTarget);

		for (auto& Boss_iter : BossContainer)
		{
			if (true == Boss_iter->Get_Dead())
				continue;
			else if (false == Boss_iter->Get_Enable())
				continue;
			else if (nullptr == Boss_iter)
				continue;

			_float fLenth = V3_LENGTH(&(TARGET_TO_TRANS(Boss_iter)->Get_Pos() - m_pTransform->Get_Pos()));

			if (fLenth > m_fRecognitionRange)
				continue;

			if (fOldLength <= fLenth)
				continue;

			fOldLength = fLenth;
			m_pTarget = Boss_iter;
			m_eTarget = PET_TARGET_TYPE::PET_TARGET_BOSS;
			Safe_AddRef(m_pTarget);
		}

		IF_NOT_NULL_RETURN(m_pTarget);

		for (auto& Item_iter : ItemContainer)
		{
			if (true == Item_iter->Get_Dead())
				continue;
			else if (false == Item_iter->Get_Enable())
				continue;
			else if (nullptr == Item_iter)
				continue;

			_float fLenth = V3_LENGTH(&(TARGET_TO_TRANS(Item_iter)->Get_Pos() - m_pTransform->Get_Pos()));

			if (fLenth > m_fRecognitionRange)
				continue;
			if (fOldLength <= fLenth)
				continue;

			fOldLength = fLenth;
			m_pTarget = Item_iter;
			m_eTarget = PET_TARGET_TYPE::PET_TARGET_ITEM;
			Safe_AddRef(m_pTarget);
		}

		IF_NOT_NULL_RETURN(m_pTarget);

		break;

	case PET_MODE_TYPE::PET_MODE_UTILL:
		for (auto& Item_iter : ItemContainer)
		{
			if (true == Item_iter->Get_Dead())
				continue;
			else if (false == Item_iter->Get_Enable())
				continue;
			else if (nullptr == Item_iter)
				continue;

			_float fLenth = V3_LENGTH(&(TARGET_TO_TRANS(Item_iter)->Get_Pos() - m_pTransform->Get_Pos()));

			if (fLenth > m_fRecognitionRange)
				continue;
			if (fOldLength <= fLenth)
				continue;

			fOldLength = fLenth;
			m_pTarget = Item_iter;
			m_eTarget = PET_TARGET_TYPE::PET_TARGET_ITEM;
			Safe_AddRef(m_pTarget);
		}

		IF_NOT_NULL_RETURN(m_pTarget);

		for (auto& Monster_iter : MonsterContainer)
		{
			if (true == Monster_iter->Get_Dead())
				continue;
			else if (false == Monster_iter->Get_Enable())
				continue;
			else if (nullptr == Monster_iter)
				continue;

			_float fLenth = V3_LENGTH(&(TARGET_TO_TRANS(Monster_iter)->Get_Pos() - m_pTransform->Get_Pos()));

			if (fLenth > m_fRecognitionRange)
				continue;

			if (fOldLength <= fLenth)
				continue;

			fOldLength = fLenth;
			m_pTarget = Monster_iter;
			m_eTarget = PET_TARGET_TYPE::PET_TARGET_MONSTER;
			Safe_AddRef(m_pTarget);
		}

		IF_NOT_NULL_RETURN(m_pTarget);

		for (auto& Boss_iter : BossContainer)
		{
			if (true == Boss_iter->Get_Dead())
				continue;
			else if (false == Boss_iter->Get_Enable())
				continue;
			else if (nullptr == Boss_iter)
				continue;

			_float fLenth = V3_LENGTH(&(TARGET_TO_TRANS(Boss_iter)->Get_Pos() - m_pTransform->Get_Pos()));

			if (fLenth > m_fRecognitionRange)
				continue;

			if (fOldLength <= fLenth)
				continue;

			fOldLength = fLenth;
			m_pTarget = Boss_iter;
			m_eTarget = PET_TARGET_TYPE::PET_TARGET_BOSS;
			Safe_AddRef(m_pTarget);
		}

		IF_NOT_NULL_RETURN(m_pTarget);
		break;
	}

	if (nullptr == m_pTarget)
	{
		m_pTarget = nullptr;
		m_eTarget = PET_TARGET_TYPE::PET_TARGET_NONE;
	}
	
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

void CPet::Play_Idle()
{
	return;
}

void CPet::Play_Move()
{
	return;
}

void CPet::Play_Hit()
{
	return;
}

void CPet::Play_CC()
{
	return;
}

void CPet::Play_Dead()
{
	return;
}

HRESULT CPet::Add_Component(void * pArg)
{
	return S_OK;
}

HRESULT CPet::SetUp_ConstantTable()
{
	return S_OK;
}

HRESULT CPet::Ready_Status(void * pArg)
{
	return S_OK;
}

HRESULT CPet::Ready_Weapon(void * pArg)
{
	return S_OK;
}

HRESULT CPet::Ready_Collider(void * pArg)
{
	return S_OK;
}

HRESULT CPet::Ready_BoneMatrix(void * pArg)
{
	return S_OK;
}

void CPet::Free()
{
	Safe_Release(m_pMonsterUI);

	IF_NOT_NULL(m_pTarget)
		Safe_Release(m_pTarget);

	IF_NOT_NULL(m_pPlayer)
		Safe_Release(m_pPlayer);

	Safe_Release(m_pCollider);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pTransform);
	Safe_Release(m_pMesh);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);

	CGameObject::Free();

	return;
}
