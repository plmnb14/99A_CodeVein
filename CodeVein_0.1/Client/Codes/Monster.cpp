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

HRESULT CMonster::Render_GameObject()
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

	//cout << "시야각 : " << D3DXToDegree(fRadian) << endl;

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
		// 무조건 동료 있다,  없을리 없다.
		pColleaue = ColleagueContainer.front();

		// 동료가 비활성화
		if (true == pColleaue->Get_Dead() ||
			false == pColleaue->Get_Enable())
		{
			// 다시 되돌림.
			pColleaue = nullptr;
		}
	}

	if (!PlayerContainer.empty())
	{
		// 무조건 플레이어 있다,  없을리 없다.
		pPlayer = PlayerContainer.front();

		// 플레이어가 비활성화
		if (true == pPlayer->Get_Dead() ||
			false == pPlayer->Get_Enable())
		{
			// 다시 되돌림.
			pPlayer = nullptr;
		}
	}

	// 랜덤 어그로 핑퐁
	if (true == Ransdom_Aggro)
	{
		m_fAggroTime += g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

		// 어그로 시간이 경과되면 다시 핑퐁
		if (m_fAggroTime > m_fMaxAggroTime + m_fOffsetAggroTime)
		{
			// 애니가 끝날 때 어그로 평가
			if (m_pMeshCom->Is_Finish_Animation(0.9f))
			{
				// 플레이어한테 어그로였는가
				if (0 == lstrcmp(m_pLayerTag_Of_Target, L"Layer_Player"))
				{
					// 동료로 어그로 교체
					if (nullptr != pColleaue)
						Set_Target_To_Colleague();

				}
				else
				{
					// 플레이어로 어그로 교체
					if (nullptr != pPlayer)
						Set_Target_To_Player();
				}

				// 어그로 시간, 랜덤 Offset 초기화
				m_fAggroTime = 0.f;
				m_fOffsetAggroTime = _float(CALC::Random_Num_Double(-3, 3));
			}
		}

	}
	// 거리 체크
	else
	{
		// 애니가 끝날 때 어그로 평가
		if (m_pMeshCom->Is_Finish_Animation(0.9f))
		{
			// 둘 다 살아있으면 거리비교
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

CGameObject * CMonster::Get_pTargetObject()
{
	// 우선  SCENE_MORTAL에서 찾아봄.
	CGameObject* pObject = g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_MORTAL);

	// 못 찾았으면 동료를 찾으면됨.
	if (nullptr == pObject)
		pObject = g_pManagement->Get_GameObjectBack(m_pLayerTag_Of_Target, SCENE_STAGE);

	return pObject;
}

HRESULT CMonster::Draw_Collider()
{
	for (auto& iter : m_vecPhysicCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	for (auto& iter : m_vecAttackCol)
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);

	return S_OK;
}

void CMonster::Check_DropItem(MONSTER_NAMETYPE eMonsterName)
{
	ITEM_NAMETYPE eItem_Name = ITEM_NAMETYPE::NAMETYPE_Queen_Steel;

	switch (eMonsterName)
	{
		// 일반 몹 드랍
	case M_Cocoon:
	case M_Hunter:
	case M_Monkey:
	case M_Wolf:
	case M_Yeti:
	case M_ThaiMan:
	case M_Urchin:
	{
		m_iRandom = CALC::Random_Num(0, 8);

		switch (m_iRandom)
		{
		case 0:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_Expend_Blood;
			break;
		case 1:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_Expend_Cheet;
			break;
		case 2:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_Expend_Hp;
			break;
		case 3:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_Expend_MaximumUp;
			break;
		case 4:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_Expend_Return;
			break;
		case 5:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_Expend_SuperArmor;
			break;
		case 6:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_Queen_Steel;
			break;
		case 7:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_Queen_Titanium;
			break;
		case 8:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_Queen_Tungsten;
			break;
		default:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_Expend_Blood;
			break;
		}
	}
	break;

	case M_GunGenji:
	{
		m_iRandom = CALC::Random_Num(0, 3);

		switch (m_iRandom)
		{
		case 0:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_Gun_Bayonet;
			break;
		case 1:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_Gun_DarkBayonet;
			break;
		case 2:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_Gun_Nakil;
			break;
		case 3:
			//eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_Gun_ReverTardor;
			break;
		}
	}
	break;

	case M_SwordGenji:
	{
		m_iRandom = CALC::Random_Num(0, 7);

		switch (m_iRandom)
		{
		case 0:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_LSword_Black;
			break;
		case 1:
			//eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_LSword_DarkTsubai;
			break;
		case 2:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_LSword_Nakil;
			break;
		case 3:
			//eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_LSword_Tsubai;
			break;
		case 4:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_SSword_DarkBrodeSword;
			break;
		case 5:
			//eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_SSword_Executioner;
			break;
		case 6:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_SSword_Nakil;
			break;
		case 7:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_SSword_Red;
			break;
		}
	}
	break;

	case M_SwordShieldGenji:
	{
		m_iRandom = CALC::Random_Num(0, 3);

		switch (m_iRandom)
		{
		case 0:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_Halberd_Black;
			break;
		case 1:
			//eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_Halberd_DarkBodish;
			break;
		case 2:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_Halberd_Nakil;
			break;
		case 3:
			//eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_Halberd_SolidPiercer;
			break;
		}
	}
	break;

	case M_YachaMan:
	{
		m_iRandom = CALC::Random_Num(0, 3);

		switch (m_iRandom)
		{
		case 0:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_Hammer_Black;
			break;
		case 1:
			//eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_Hammer_DarkHeavyAxe;
			break;
		case 2:
			//eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_Hammer_ImperseAnker;
			break;
		case 3:
			eItem_Name = ITEM_NAMETYPE::NAMETYPE_WpnAll_Hammer_Nakil;
			break;
		}
	}
	break;

	case M_DeerKing:
		eItem_Name = ITEM_NAMETYPE::NAMETYPE_Pet_DeerKing;
		break;

	case M_FireBoy:
		break;
	case M_IceGirl:
		break;
	case M_PoisonButterfly:
		eItem_Name = ITEM_NAMETYPE::NAMETYPE_Pet_PoisonButterfly;
		break;

	case M_QueensKnight:
		break;

	default:
		break;
	}

	CObjectPool_Manager::Get_Instance()->Create_Object(L"GameObject_DropItem", &CDropItem::ITEM_STATUS(ITEM_GRADE_TYPE::ITEM_GRADE_NORMAL, eItem_Name, m_pTransformCom->Get_Pos(), 120.f));

	return;
}

void CMonster::Check_CollisionEvent()
{
	Check_CollisionPush();
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL));
	Check_CollisionHit(g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE));

	return;
}

void CMonster::Check_CollisionPush()
{
	list<CGameObject*> tmpList[4];

	tmpList[0] = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);
	tmpList[1] = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
	tmpList[2] = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);
	tmpList[3] = g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE);

	for (auto& list_iter : tmpList)
	{
		if (list_iter.empty())
			continue;

		for (auto& Obj_iter : list_iter)
		{
			CCollider* pCollider = TARGET_TO_COL(Obj_iter);

			if (m_pColliderCom->Check_Sphere(pCollider, m_pTransformCom->Get_Axis(AXIS_Z), m_fSkillMoveSpeed_Cur * DELTA_60))
			{
				CTransform* pTrans = TARGET_TO_TRANS(Obj_iter);
				CNavMesh*   pNav = TARGET_TO_NAV(Obj_iter);

				_v3 vDir = m_pTransformCom->Get_Pos() - pTrans->Get_Pos();
				V3_NORMAL_SELF(&vDir);

				vDir.y = 0;

				pTrans->Set_Pos(pNav->Move_OnNaviMesh(m_pRigidCom, &pTrans->Get_Pos(), &vDir, m_pColliderCom->Get_Length().x));
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
						iter->Hit_Target(m_tObjParam.fDamage);

						if (iter->Get_Target_IsHit())
							iter->Set_HitAgain(true);
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

	m_pTransformCom->Set_Pos((m_pNavMeshCom->Move_OnNaviMesh(m_pRigidCom, &m_pTransformCom->Get_Pos(), &_vDir, _fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

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
	m_pTransformCom->Set_Pos((m_pNavMeshCom->Move_OnNaviMesh(m_pRigidCom, &m_pTransformCom->Get_Pos(), &_vDir, _fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

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
	_float	fOldLength = 99999.f;

	if (nullptr != m_pAggroTarget)
		m_pAggroTarget = nullptr;

	auto& ColleagueContainer = g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE);

	auto& PlayerContainer = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);

	for (auto& Colleague_iter : ColleagueContainer)
	{
		if (nullptr == Colleague_iter)
			continue;
		else if (true == Colleague_iter->Get_Dead())
			continue;
		else if (false == Colleague_iter->Get_Enable())
			continue;

		_float fLenth = V3_LENGTH(&(TARGET_TO_TRANS(Colleague_iter)->Get_Pos() - m_pTransformCom->Get_Pos()));

		if (fLenth > m_fRecognitionRange)
			continue;

		if (fOldLength <= fLenth)
			continue;

		fOldLength = fLenth;
		m_pAggroTarget = Colleague_iter;
	}

	IF_NOT_NULL_RETURN(m_pAggroTarget);

	for (auto& Player_iter : PlayerContainer)
	{
		if (nullptr == Player_iter)
			continue;
		else if (true == Player_iter->Get_Dead())
			continue;
		else if (false == Player_iter->Get_Enable())
			continue;

		_float fLenth = V3_LENGTH(&(TARGET_TO_TRANS(Player_iter)->Get_Pos() - m_pTransformCom->Get_Pos()));

		if (fLenth > m_fRecognitionRange)
			continue;

		if (fOldLength <= fLenth)
			continue;

		fOldLength = fLenth;
		m_pAggroTarget = Player_iter;
	}

	IF_NOT_NULL_RETURN(m_pAggroTarget);

	return;
}

void CMonster::Function_ResetAfterAtk()
{
	m_dAniPlayMul = 1;

	m_fCoolDownMax = 0.f;
	m_fCoolDownCur = 0.f;

	m_tObjParam.bCanHit = true;
	m_tObjParam.bIsHit = false;

	m_tObjParam.bCanDodge = true;
	m_tObjParam.bIsDodge = false;

	m_tObjParam.bSuperArmor = false;
	m_tObjParam.bIsAttack = false;

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

void CMonster::Give_Mana_To_Player(_byte _Mana)
{
	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	IF_NULL_RETURN(pPlayer);

	// 마나 더해줌.
	pPlayer->Add_Target_Mana(_Mana);
	
	// 현재 마나가 최대보다 더 많다면 최대치로 맞춰줌.
	if (pPlayer->Get_Target_Mana_Max() < pPlayer->Get_Target_Mana_Cur())
		pPlayer->Set_Target_Mana_Cur(pPlayer->Get_Target_Mana_Max());

	return;
}

void CMonster::Free()
{
	Safe_Release(m_pMonsterUI);

	IF_NOT_NULL(m_pWeapon)
		Safe_Release(m_pWeapon);

	Safe_Release(m_pRigidCom);
	Safe_Release(m_pAIControllerCom);
	Safe_Release(m_pOptimizationCom);
	Safe_Release(m_pBattleAgentCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavMeshCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	CGameObject::Free();

	return;
}