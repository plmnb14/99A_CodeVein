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

HRESULT CMonster::Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur)
{
	return S_OK;
}

void CMonster::Set_Target_To_Player()
{
	lstrcpy(m_pLayerTag_Of_Target, L"Layer_Player");
}

void CMonster::Set_Target_To_Colleague()
{
	lstrcpy(m_pLayerTag_Of_Target, L"Layer_Colleague");
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

	//cout << "�þ߰� : " << D3DXToDegree(fRadian) << endl;

	if (D3DXToDegree(fRadian) < fDegreeOfFov * 0.5f)
		return true;

	return false;
}

void CMonster::Set_Target_Auto(_bool Ransdom_Aggro)
{
	CGameObject* pColleaue = nullptr;
	CGameObject* pPlayer = nullptr;

	auto& ColleagueContainer = g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE);
	auto& PlayerContainer = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);

	if (!ColleagueContainer.empty())
	{
		// ������ ���� �ִ�,  ������ ����.
		pColleaue = ColleagueContainer.front();

		// ���ᰡ ��Ȱ��ȭ
		if (true == pColleaue->Get_Dead() ||
			false == pColleaue->Get_Enable())
		{
			// �ٽ� �ǵ���.
			pColleaue = nullptr;
		}
	}

	if (!PlayerContainer.empty())
	{
		// ������ �÷��̾� �ִ�,  ������ ����.
		pPlayer = PlayerContainer.front();

		// �÷��̾ ��Ȱ��ȭ
		if (true == pPlayer->Get_Dead() ||
			false == pPlayer->Get_Enable())
		{
			// �ٽ� �ǵ���.
			pPlayer = nullptr;
		}
	}

	// ���� ��׷� ����
	if (true == Ransdom_Aggro)
	{
		m_fAggroTime += g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

		// ��׷� �ð��� ����Ǹ� �ٽ� ����
		if (m_fAggroTime > m_fMaxAggroTime + m_fOffsetAggroTime)
		{
			// �ִϰ� ���� �� ��׷� ��
			if (m_pMeshCom->Is_Finish_Animation(0.9f))
			{
				// �÷��̾����� ��׷ο��°�
				if (0 == lstrcmp(m_pLayerTag_Of_Target, L"Layer_Player"))
				{
					// ����� ��׷� ��ü
					if (nullptr != pColleaue)
						Set_Target_To_Colleague();

				}
				else
				{
					// �÷��̾�� ��׷� ��ü
					if (nullptr != pPlayer)
						Set_Target_To_Player();
				}

				// ��׷� �ð�, ���� Offset �ʱ�ȭ
				m_fAggroTime = 0.f;
				m_fOffsetAggroTime = _float(CALC::Random_Num_Double(-2, 2));
			}
		}

	}
	// �Ÿ� üũ
	else
	{
		// �ִϰ� ���� �� ��׷� ��
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			// �� �� ��������� �Ÿ���
			if (pPlayer && pColleaue)
			{
				_float fLengthToPlayer = V3_LENGTH(&(TARGET_TO_TRANS(pPlayer)->Get_Pos() - m_pTransformCom->Get_Pos()));
				_float fLengthTopColleaue = V3_LENGTH(&(TARGET_TO_TRANS(pColleaue)->Get_Pos() - m_pTransformCom->Get_Pos()));

				if (fLengthTopColleaue > fLengthToPlayer)
					Set_Target_To_Colleague();
				else
					Set_Target_To_Player();
			}
			else if (nullptr == pColleaue)
				Set_Target_To_Player();
			else
				Set_Target_To_Colleague();
		}
	}



}

HRESULT CMonster::Draw_Collider()
{
	for (auto& iter : m_vecPhysicCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return S_OK;
}

void CMonster::Check_CollisionEvent()
{
	Check_CollisionPush();
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_MORTAL));

	return;
}

void CMonster::Check_CollisionPush()
{
	list<CGameObject*> tmpList[4];

	tmpList[0] = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);
	tmpList[1] = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
	tmpList[2] = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);
	tmpList[3] = g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_MORTAL);

	for (auto& list_iter : tmpList)
	{
		for (auto& Obj_iter : list_iter)
		{
			CCollider* pCollider = TARGET_TO_COL(Obj_iter);

			if (m_pCollider->Check_Sphere(pCollider, m_pTransformCom->Get_Axis(AXIS_Z), m_fSkillMoveSpeed_Cur * DELTA_60))
			{
				CTransform* pTrans = TARGET_TO_TRANS(Obj_iter);
				CNavMesh*   pNav = TARGET_TO_NAV(Obj_iter);

				_v3 vDir = m_pTransformCom->Get_Pos() - pTrans->Get_Pos();
				V3_NORMAL_SELF(&vDir);

				vDir.y = 0;

				pTrans->Set_Pos(pNav->Move_OnNaviMesh(NULL, &pTrans->Get_Pos(), &vDir, m_pCollider->Get_Length().x));
			}
		}
	}

	return;
}

void CMonster::Check_CollisionHit(list<CGameObject*> plistGameObject)
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

void CMonster::Function_FBLR(CGameObject * _pGameObject)
{
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

void CMonster::Function_RotateBody(CGameObject * _pGameObject)
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

void CMonster::Function_MoveAround(CGameObject * _pGameObject, _float _fSpeed, _v3 _vDir)
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

void CMonster::Function_CoolDown()
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

void CMonster::Function_Movement(_float _fspeed, _v3 _vDir)
{
	V3_NORMAL(&_vDir, &_vDir);
	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &_vDir, _fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

	return;
}

void CMonster::Function_DecreMoveMent(_float _fMutiply)
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

void CMonster::Function_Find_Target()
{
	if (nullptr != m_pTarget)
	{
		Safe_Release(m_pTarget);
		m_pTarget = nullptr;
	}

	_float	fOldLength = 99999.f;

	auto& ColleagueContainer = g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_MORTAL);

	auto& PlayerContainer = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);

	for (auto& Colleague_iter : ColleagueContainer)
	{
		if (true == Colleague_iter->Get_Dead())
			continue;
		else if (false == Colleague_iter->Get_Enable())
			continue;
		else if (nullptr == Colleague_iter)
			continue;

		_float fLenth = V3_LENGTH(&(TARGET_TO_TRANS(Colleague_iter)->Get_Pos() - m_pTransformCom->Get_Pos()));

		if (fLenth > m_fRecognitionRange)
			continue;

		if (fOldLength <= fLenth)
			continue;

		fOldLength = fLenth;
		m_pTarget = Colleague_iter;
		Safe_AddRef(m_pTarget);	
	}

	IF_NOT_NULL_RETURN(m_pTarget);

	for (auto& Player_iter : PlayerContainer)
	{
		if (true == Player_iter->Get_Dead())
			continue;
		else if (false == Player_iter->Get_Enable())
			continue;
		else if (nullptr == Player_iter)
			continue;

		_float fLenth = V3_LENGTH(&(TARGET_TO_TRANS(Player_iter)->Get_Pos() - m_pTransformCom->Get_Pos()));

		if (fLenth > m_fRecognitionRange)
			continue;

		if (fOldLength <= fLenth)
			continue;

		fOldLength = fLenth;
		m_pTarget = Player_iter;
		Safe_AddRef(m_pTarget);
	}

	IF_NOT_NULL_RETURN(m_pTarget);

	return;
}

void CMonster::Function_ResetAfterAtk()
{
	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsHit = false;

	m_tObjParam.bCanDodge = true;
	m_tObjParam.bIsDodge = false;

	m_tObjParam.bSuperArmor = false;
	m_tObjParam.bIsAttack = false;

	//200430 �߰�, �Լ� ���� �����ҿ���
	m_bCanCoolDown = false;
	m_bIsCoolDown = false;

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

void CMonster::Free()
{
	CGameObject::Free();

	return;
}