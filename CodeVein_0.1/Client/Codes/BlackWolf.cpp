#include "stdafx.h"
#include "..\Headers\BlackWolf.h"

CBlackWolf::CBlackWolf(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CBlackWolf::CBlackWolf(const CBlackWolf & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlackWolf::Ready_GameObject_Prototype()
{
	return NOERROR;
}

HRESULT CBlackWolf::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Pos(_v3(1.f, 0.f, 1.f));
	m_pTransformCom->Set_Scale(V3_ONE);

	Ready_BoneMatrix();
	Ready_Collider();

	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE);
	m_pTargetTransform = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE));

	m_eFirstCategory = MONSTER_ANITYPE::IDLE;

	m_tObjParam.fHp_Max = 75.f; //4~5�� ���, �⺻���ݷ� 20+-5���� �ǰ���
	m_tObjParam.fHp_Cur = m_tObjParam.fHp_Max;
	m_tObjParam.fDamage = 20.f;

	m_tObjParam.bCanHit = true; //�±� ����
	m_tObjParam.bIsHit = false;	//�±� ������ �ƴ�
	m_tObjParam.bCanAttack = true; //���� ����
	m_tObjParam.bIsAttack = false; //���� ������ �ƴ�
	m_tObjParam.bDodge = false; //ù ������ false��

	m_bInRecognitionRange = false; //���� ���� ����
	m_bInAtkRange = false; //���� ���� ����
	m_bCanChase = false; //�߰� ����
	m_bIsDodge = false; //ȸ�� ������ ����
	m_bCanCoolDown = false; //��Ÿ�� ����
	m_bIsCoolDown = false; //��Ÿ�� ������ ����

	m_bCanIdleRandom = true;
	m_bCanAtkRandom = true;

	m_fRecognitionRange = 10.f; //��������
	m_fAtkRange = 4.f; //���ݹ���
	m_fCoolDownMax = 1.5f; //��Ÿ�� �ƽ����� ������
	m_fCoolDownCur = 0.f; //��Ÿ�� �ð��� ����
	m_iRandom = 0; //���� ���� ����
	m_iDodgeCount = 0; //nȸ �ǰݽ� �ٷ� ȸ��

	m_fSpeedForCollisionPush = 2.f;

	return NOERROR;
}

_int CBlackWolf::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	Check_Hit();
	Check_Dist();
	Set_AniEvent();
	Function_CoolDown();

	m_pMeshCom->SetUp_Animation(m_eState);

	Enter_Collision();

	return NOERROR;
}

_int CBlackWolf::Late_Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	IF_NULL_VALUE_RETURN(m_pRendererCom, E_FAIL);

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;
	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
		return E_FAIL;
	//if (FAILED(m_pRendererCom->Add_RenderList(RENDER_SHADOWTARGET, this)))
	//	return E_FAIL;

	m_dTimeDelta = TimeDelta;

	return NOERROR;
}

HRESULT CBlackWolf::Render_GameObject()
{
	IF_NULL_VALUE_RETURN(m_pShaderCom, E_FAIL);
	IF_NULL_VALUE_RETURN(m_pMeshCom, E_FAIL);

	m_pMeshCom->Play_Animation(DELTA_60 * m_dAniPlayMul); // * alpha

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

		m_pMeshCom->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			if (MONSTER_ANITYPE::DEAD != m_eFirstCategory)
				m_iPass = m_pMeshCom->Get_MaterialPass(i, j);

			m_pShaderCom->Begin_Pass(m_iPass);

			m_pShaderCom->Set_DynamicTexture_Auto(m_pMeshCom, i, j);

			m_pShaderCom->Commit_Changes();

			m_pMeshCom->Render_Mesh(i, j);

			m_pShaderCom->End_Pass();
		}
	}

	m_pShaderCom->End_Shader();

	Update_Collider();
	Render_Collider();

	return NOERROR;
}

HRESULT CBlackWolf::Render_GameObject_SetPass(CShader * pShader, _int iPass)
{
	if (nullptr == pShader ||
		nullptr == m_pMeshCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);
	if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
		return E_FAIL;

	m_matLastWVP = m_pTransformCom->Get_WorldMat() * ViewMatrix * ProjMatrix;

	_uint iNumMeshContainer = _uint(m_pMeshCom->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMeshCom->Get_NumMaterials(i);

		m_pMeshCom->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			pShader->Begin_Pass(iPass);

			m_pMeshCom->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	return NOERROR;
}

void CBlackWolf::Update_Collider()
{
	for (auto& vector_iter : m_vecAttackCol)
	{
		_mat matTemp = *m_matBone[Bone_Head] * m_pTransformCom->Get_WorldMat();

		_v3 ColPos = _v3(matTemp._41, matTemp._42, matTemp._43);

		vector_iter->Update(ColPos);
	}

	_ulong matrixIdx = 0;

	for (auto& iter : m_vecPhysicCol)
	{
		_mat tmpMat = *m_matBone[matrixIdx] * m_pTransformCom->Get_WorldMat();

		_v3 ColPos = _v3(tmpMat._41, tmpMat._42, tmpMat._43);

		iter->Update(ColPos);

		++matrixIdx;
	}

	m_pCollider->Update(m_pTransformCom->Get_Pos() + _v3(0.f, m_pCollider->Get_Radius().y, 0.f));

	return;
}

void CBlackWolf::Render_Collider()
{
	for (auto& iter : m_vecAttackCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}

	for (auto& iter : m_vecPhysicCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}
}

void CBlackWolf::Enter_Collision()
{
	Check_CollisionPush();
	Check_CollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE));

	return;
}

void CBlackWolf::Check_CollisionPush()
{
	list<CGameObject*> tmpList[3];

	tmpList[0] = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE);
	tmpList[1] = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
	tmpList[2] = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);

	for (auto& list_iter : tmpList)
	{
		for (auto& Obj_iter : list_iter)
		{
			CCollider* pCollider = TARGET_TO_COL(Obj_iter);

			// ���� �ӵ��� ���Ƿ� �־��µ� ���ؼ� �־������ - �Ϸ�
			if (m_pCollider->Check_Sphere(pCollider, m_pTransformCom->Get_Axis(AXIS_Z), m_fSpeedForCollisionPush * DELTA_60))
			{
				CTransform* pTrans = TARGET_TO_TRANS(Obj_iter);
				CNavMesh*   pNav = TARGET_TO_NAV(Obj_iter);

				// ���� �����ְ�
				_v3 vDir = m_pTransformCom->Get_Pos() - pTrans->Get_Pos();
				V3_NORMAL_SELF(&vDir);

				// y�� �̵��� ��������
				vDir.y = 0;

				// �׺� �޽�Ÿ�� �� ����
				pTrans->Set_Pos(pNav->Move_OnNaviMesh(NULL, &pTrans->Get_Pos(), &vDir, m_pCollider->Get_Length().x));
			}
		}
	}

	return;
}

void CBlackWolf::Check_CollisionEvent(list<CGameObject*> plistGameObject)
{
	// ���� �Ұ����̸� üũ ����
	if (false == m_tObjParam.bIsAttack)
	{
		return;
	}

	_bool bFirst = true;
	//���� ������Ʈ�� �޾ƿͼ�
	for (auto& iter : plistGameObject)
	{
		// ���� �� ���ٸ� ����
		if (false == iter->Get_Target_CanHit())
			continue;

		// ���� ���� Vec �ݶ��̴��� ���Ѵ�.
		for (auto& vecIter : m_vecAttackCol)
		{
			if (false == vecIter->Get_Enabled())
				continue;

			bFirst = true;

			// �����ݶ��̴��� ��
			for (auto& vecCol : iter->Get_PhysicColVector())
			{
				// ��ü ��ü�� ��ǥ�� �ݶ��̴�.
				if (vecIter->Check_Sphere(vecCol))
				{
					// ù��°�� ��豸 �ݶ��̴��ϱ� ���� �ݶ��̴��� �浹ó�� �Ѵ�.
					if (bFirst)
					{
						bFirst = false;
						continue;
					}

					if (false == iter->Get_Target_Dodge())
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
					{
						break;
					}
				}
			}
		}
	}

	return;
}

void CBlackWolf::Check_Hit()
{
	//�ǰ� ���� ���� ó�� �ؾ���
	if (MONSTER_ANITYPE::DEAD == m_eFirstCategory)
	{
		return;
	}
	else //���� ���� �ƴ�
	{
		if (false == m_tObjParam.bCanHit) //�ǰ�o
		{
			if (true == m_tObjParam.bIsHit) //�ǰ� ������o
			{
				if (true == m_tObjParam.bHitAgain) //�߰� �ǰ�o
				{
					m_eFirstCategory = MONSTER_ANITYPE::HIT;
					Check_FBLR();
					m_tObjParam.bHitAgain = false;
					m_pMeshCom->Reset_OldIndx();

					return;
				}
				else //�߰� �ǰ�x
				{
					return;
				}
			}
			else //�ǰ� ������x
			{
				if (0 >= m_tObjParam.fHp_Cur) //ü�¾���
				{
					//Down�� ��� Dead_Strong�� ����
					m_eFirstCategory = MONSTER_ANITYPE::DEAD;
					return;
				}
				else //ü�� ����
				{
					m_eFirstCategory = MONSTER_ANITYPE::HIT;
					Check_FBLR();
					return;
				}
			}
		}
	}

	return;
}

void CBlackWolf::Check_FBLR()
{
	//�ǰ� ��,�� üũ
	m_eSecondCategory_HIT = WOLF_HITTYPE::HIT_HIT_F;
	//m_eSecondCategory_HIT = WOLF_HITTYPE::HIT_HIT_B;

	return;
}

void CBlackWolf::Check_Dist()
{
	//�Ʒ� ���½� üũ ����
	if (MONSTER_ANITYPE::HIT == m_eFirstCategory ||
		MONSTER_ANITYPE::DOWN == m_eFirstCategory ||
		MONSTER_ANITYPE::DEAD == m_eFirstCategory)
		return;

	//ȸ����, ������, �´��� üũ ����
	if (true == m_bIsDodge ||
		true == m_tObjParam.bIsAttack ||
		true == m_tObjParam.bIsHit)
		return;

	_float fLenth = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransform->Get_Pos()));

	m_fRecognitionRange >= fLenth ? m_bInRecognitionRange = true : m_bInRecognitionRange = false;
	m_fAtkRange >= fLenth ? m_bInAtkRange = true : m_bInAtkRange = false;

	if (true == m_bInRecognitionRange) //����o
	{
		//�ϻ� �ִ� �۵���o
		if (WOLF_ANI::Sit_End == m_eState || WOLF_ANI::Sit == m_eState || WOLF_ANI::Eat_End == m_eState || WOLF_ANI::Eat == m_eState)
		{
			m_eFirstCategory = MONSTER_ANITYPE::IDLE;
			return;
		}
		else //�ϻ� �ִ� �۵���x
		{
			if (true == m_bInAtkRange) //���ݹ��� o
			{
				if (true == m_tObjParam.bCanAttack) //���� ����
				{
					if (true == m_bIsCoolDown) //��Ÿ����o
					{
						//���,����
						m_eFirstCategory = MONSTER_ANITYPE::IDLE;
						m_eSecondCategory_IDLE = WOLF_IDLETYPE::IDLE_IDLE;
						Function_RotateBody();
						return;
					}
					else //��Ÿ����x
					{
						m_bCanAtkRandom = true;
						m_eFirstCategory = MONSTER_ANITYPE::ATTACK;
						Function_RotateBody();
						return;
					}
				}
				else //���� �Ұ�
				{
					//���� ȸ���ϸ鼭 ����Ѵ�
					m_eFirstCategory = MONSTER_ANITYPE::IDLE; //ȸ�� ��� ���
					m_eSecondCategory_IDLE = WOLF_IDLETYPE::IDLE_IDLE;
					Function_RotateBody();
					return;
				}
			}
			else //���ݹ���x
			{
				m_bCanChase = true;
				m_eFirstCategory = MONSTER_ANITYPE::MOVE;
				m_eSecondCategory_MOVE = WOLF_MOVETYPE::MOVE_RUN;
				return;
			}
		}
	}
	else //����x
	{
		//�ϻ�
		m_bCanChase = false;
		m_eFirstCategory = MONSTER_ANITYPE::IDLE;
		if (true == m_bCanIdleRandom) //�ϻ� ����
		{
			switch (CALC::Random_Num(WOLF_IDLETYPE::IDLE_IDLE, WOLF_IDLETYPE::IDLE_SIT))
			{
			case WOLF_IDLETYPE::IDLE_IDLE:
				m_eSecondCategory_IDLE = WOLF_IDLETYPE::IDLE_IDLE;
				break;
			case WOLF_IDLETYPE::IDLE_EAT:
				m_eSecondCategory_IDLE = WOLF_IDLETYPE::IDLE_EAT;
				break;
			case WOLF_IDLETYPE::IDLE_SIT:
				m_eSecondCategory_IDLE = WOLF_IDLETYPE::IDLE_SIT;
				break;
			}
		}

		return;
	}

	return;
}

void CBlackWolf::Set_AniEvent()
{
	switch (m_eFirstCategory)
	{
	case MONSTER_ANITYPE::IDLE:
		if (true == m_bCanIdleRandom)
		{
			m_bCanIdleRandom = false;

			switch (m_eSecondCategory_IDLE)
			{
			case Client::CBlackWolf::IDLE_IDLE:
				m_eState = WOLF_ANI::Idle;
				break;
			case Client::CBlackWolf::IDLE_EAT:
				m_eState = WOLF_ANI::Eat;
				break;
			case Client::CBlackWolf::IDLE_SIT:
				m_eState = WOLF_ANI::Sit;
				break;
			}
		}
		else
		{
			switch (m_eSecondCategory_IDLE)
			{
			case Client::CBlackWolf::IDLE_IDLE:
				Play_Idle();
				break;
			case Client::CBlackWolf::IDLE_EAT:
				Play_Eat();
				break;
			case Client::CBlackWolf::IDLE_SIT:
				Play_Sit();
				break;
			}
		}
		break;

	case MONSTER_ANITYPE::MOVE:
		switch (m_eSecondCategory_MOVE)
		{
		case WOLF_MOVETYPE::MOVE_RUN:
			Play_Run();
			break;
		case WOLF_MOVETYPE::MOVE_WALK:
			Play_Walk();
			break;
		case WOLF_MOVETYPE::MOVE_DODGE:
			Play_Dodge();
			break;
		}
		break;

	case MONSTER_ANITYPE::ATTACK:
		if (true == m_bCanAtkRandom)
		{
			m_bCanAtkRandom = false; //���� ���� �Ұ�
			m_tObjParam.bCanAttack = false; //���ݺҰ�
			m_tObjParam.bIsAttack = true; //������

			m_iRandom = CALC::Random_Num(BiteLRL, Frisbee);

			switch (m_iRandom)
			{
			case WOLF_ANI::BiteLRL:
				m_eState = WOLF_ANI::BiteLRL;
				break;
			case WOLF_ANI::RDodgeAtk:
				m_eState = WOLF_ANI::RDodgeAtk;
				break;
			case WOLF_ANI::LDodgeAtk:
				m_eState = WOLF_ANI::LDodgeAtk;
				break;
			case WOLF_ANI::Frisbee:
				m_eState = WOLF_ANI::Frisbee;
				break;
			}

			return;
		}
		else
		{
			switch (m_eState)
			{
			case Client::CBlackWolf::BiteLRL:
				Play_Bite_LRL();
				break;
			case Client::CBlackWolf::RDodgeAtk:
				Play_RDodgeAtk();
				break;
			case Client::CBlackWolf::LDodgeAtk:
				Play_LDodgeAtk();
				break;
			case Client::CBlackWolf::Frisbee:
				Play_Frisbee();
				break;
			}
		}
		break;

	case MONSTER_ANITYPE::HIT:
		Play_Hit();
		//if (WOLF_HITTYPE::HIT_HIT_F == m_eSecondCategory_IDLE)
		//{
		//	Play_Hit_F();
		//	break;
		//}
		//if (WOLF_HITTYPE::HIT_HIT_B == m_eSecondCategory_IDLE)
		//{
		//	Play_Hit_B();
		//	break;
		//}
		break;

	case MONSTER_ANITYPE::DOWN:
		break;

	case MONSTER_ANITYPE::DEAD:
		Play_Dead();
		break;
	}

	return;
}

void CBlackWolf::Function_RotateBody()
{
	_float fTargetAngle = m_pTransformCom->Chase_Target_Angle(&m_pTargetTransform->Get_Pos());

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

void CBlackWolf::Function_CoolDown()
{
	if (true == m_bCanCoolDown) //���� ���� ����
	{
		m_fCoolDownCur += DELTA_60;

		if (m_fCoolDownCur >= m_fCoolDownMax) //�����ð��� ��Ÿ�Ӻ��� ũ�ٸ�
		{
			m_fCoolDownCur = 0.f;
			m_bCanCoolDown = false;
			m_bIsCoolDown = false;
			m_tObjParam.bCanAttack = true;
		}
	}

	return;
}

void CBlackWolf::Function_Movement(_float _fspeed, _v3 _vDir)
{
	V3_NORMAL(&_vDir, &_vDir);

	// �׺� ������
	//m_pTransformCom->Add_Pos(_fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"), _vDir);

	// �׺� ����
	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &_vDir, _fspeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));

	return;
}

void CBlackWolf::Function_DecreMoveMent(_float _fMutiply)
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

void CBlackWolf::Function_ResetAfterAtk()
{
	m_tObjParam.bCanHit = true; //�ǰ� ����
	m_tObjParam.bIsHit = false; //�ǰ��� x

	m_tObjParam.bDodge = false;  //ȸ�Ǿƴ�
	m_bIsDodge = false; //ȸ���� �ƴ�

	m_tObjParam.bIsAttack = false; //������x
	m_bCanAtkRandom = true;//���� ���� ����

	m_vecAttackCol[0]->Set_Enabled(false);

	LOOP(10)
		m_bEventTrigger[i] = false;

	return;
}

void CBlackWolf::Play_Idle()
{
	if (true == m_bInRecognitionRange) //����o
	{
		if (true == m_tObjParam.bCanAttack)
		{
			//����, ���� ����->���
			m_eState = WOLF_ANI::Idle;
			return;
		}
		else
		{
			//����, ���� �Ұ�->���
			m_eState = WOLF_ANI::Threat;
			return;
		}
	}
	else //����x
	{
		m_bCanIdleRandom = true;
		m_eState = WOLF_ANI::Idle;
		return;
	}

	return;
}

void CBlackWolf::Play_Eat()
{
	if (true == m_bInRecognitionRange)
	{
		if (WOLF_ANI::Eat == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation())
				m_eState = WOLF_ANI::Eat_End;

			return;
		}
		else if (WOLF_ANI::Eat_End == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation(0.98f))
			{
				m_bCanIdleRandom = true;
				m_eState = WOLF_ANI::Idle;
			}

			return;
		}
	}
	else
	{
		m_eState = WOLF_ANI::Eat;

		return;
	}

	return;
}

void CBlackWolf::Play_Sit()
{
	if (true == m_bInRecognitionRange)
	{
		if (WOLF_ANI::Sit == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation())
				m_eState = WOLF_ANI::Sit_End;

			return;
		}
		else if (WOLF_ANI::Sit_End == m_eState)
		{
			if (m_pMeshCom->Is_Finish_Animation(0.98f))
			{
				m_bCanIdleRandom = true;
				m_eState = WOLF_ANI::Idle;
			}

			return;
		}
	}
	else
	{
		m_eState = WOLF_ANI::Sit;

		return;
	}

	return;
}

void CBlackWolf::Play_Walk()
{
	m_eState = WOLF_ANI::Walk;

	Function_RotateBody();

	Function_Movement(2.f, m_pTransformCom->Get_Axis(AXIS_Z));

	Function_DecreMoveMent(0.1f);

	return;
}

void CBlackWolf::Play_Run()
{
	m_eState = WOLF_ANI::Run;

	Function_RotateBody();

	Function_Movement(4.f, m_pTransformCom->Get_Axis(AXIS_Z));

	Function_DecreMoveMent(0.1f);

	return;
}

void CBlackWolf::Play_Dodge()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	//ȸ�Ǵ� ������ �� �̻��ϴ� �ϴ� ����
	if (false == m_tObjParam.bDodge) //ȸ�Ǿƴ�?
	{
		m_eState = WOLF_ANI::Dodge;
		m_tObjParam.bDodge = true; //ȸ����
		m_bIsDodge = true; //ȸ����
	}
	else //ȸ����
	{
		if (m_pMeshCom->Is_Finish_Animation(0.98f))
		{
			m_eFirstCategory = MONSTER_ANITYPE::IDLE;
			m_bIsDodge = false;
			m_tObjParam.bDodge = true;
			Function_ResetAfterAtk();

			return;
		}
		else if (0.8f < AniTime && 2.f> AniTime)
		{
			if (m_bEventTrigger[0] == false)
			{
				m_bEventTrigger[0] = true;
				m_fSkillMoveSpeed_Cur = 4.f;
				m_fSkillMoveAccel_Cur = 0.f;
				m_fSkillMoveMultiply = 2.f;
			}

			Function_RotateBody(); //�ٶ󺸸鼭 ȸ���ϰ� ȸ���ϸ鼭�� �����ؾ���

			Function_Movement(m_fSkillMoveSpeed_Cur, -m_pTransformCom->Get_Axis(AXIS_Z));

			Function_DecreMoveMent(m_fSkillMoveMultiply);
		}
	}

	return;
}

void CBlackWolf::Play_Bite_LRL()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack) //���ݰ���
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_bCanCoolDown = true;
			m_fCoolDownMax = 2.f;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			if (1.8f < AniTime && 2.2f> AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true); //�����ݶ��̴� �۵�
				}
				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (2.2f <= AniTime &&  2.8 >= AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
			else if (2.8f < AniTime && 3.8f > AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_fSkillMoveSpeed_Cur = 4.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (3.8f <= AniTime && 4.4f >= AniTime)
			{
				if (m_bEventTrigger[3] == false)
				{
					m_bEventTrigger[3] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
				Function_RotateBody();
			}
			else if (4.4f < AniTime && 4.9f > AniTime)
			{
				if (m_bEventTrigger[4] == false)
				{
					m_bEventTrigger[4] = true;
					m_fSkillMoveSpeed_Cur = 5.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (4.9f <= AniTime)
			{
				if (m_bEventTrigger[5] == false)
				{
					m_bEventTrigger[5] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
		}
	}

	return;
}

void CBlackWolf::Play_RDodgeAtk()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.5f;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			if (1.2f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}
				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (1.6f < AniTime && 2.8f > AniTime) //1.6~2.8��ġ �浹����
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (2.8f <= AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
		}
	}
}

void CBlackWolf::Play_LDodgeAtk()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.5f;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			if (1.2f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
				}
				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (AniTime > 1.6f && AniTime < 2.8f)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_fSkillMoveSpeed_Cur = 3.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true);
				}
				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (2.8f <= AniTime)
			{
				if (m_bEventTrigger[2] == false)
				{
					m_bEventTrigger[2] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
		}
	}

	return;
}

void CBlackWolf::Play_Frisbee()
{
	_double AniTime = m_pMeshCom->Get_TrackInfo().Position;

	if (true == m_tObjParam.bCanAttack)
	{
		m_tObjParam.bCanAttack = false;
		m_tObjParam.bIsAttack = true;
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_bCanCoolDown = true;
			m_fCoolDownMax = 1.3f;
			Function_ResetAfterAtk();

			return;
		}
		else
		{
			if (1.5f < AniTime && 2.8f > AniTime)
			{
				if (m_bEventTrigger[0] == false)
				{
					m_bEventTrigger[0] = true;
					m_fSkillMoveSpeed_Cur = 6.f;
					m_fSkillMoveAccel_Cur = 0.f;
					m_fSkillMoveMultiply = 0.1f;
					m_vecAttackCol[0]->Set_Enabled(true);
				}

				Function_Movement(m_fSkillMoveSpeed_Cur, m_pTransformCom->Get_Axis(AXIS_Z));
				Function_DecreMoveMent(m_fSkillMoveMultiply);
			}
			else if (2.8f <= AniTime)
			{
				if (m_bEventTrigger[1] == false)
				{
					m_bEventTrigger[1] = true;
					m_vecAttackCol[0]->Set_Enabled(false);
				}
			}
		}
	}

	return;
}

void CBlackWolf::Play_Hit()
{
	//���� �ǰ�
	if (false == m_tObjParam.bIsHit)
	{
		Function_ResetAfterAtk();
		m_tObjParam.bIsHit = true;
		m_eState = WOLF_ANI::Dmg_F;
		//if (WOLF_HITTYPE::HIT_HIT_F == m_eSecondCategory_HIT)
		//{
		//}
		//else
		//{
		//}
	}
	else
	{
		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_tObjParam.bCanHit = true;
			m_tObjParam.bIsHit = false;
			m_bCanCoolDown = true;

			m_fCoolDownMax = 0.5f;

			m_eFirstCategory = MONSTER_ANITYPE::IDLE;
		}
		else if (m_pMeshCom->Is_Finish_Animation(0.2f))
		{
			if (false == m_tObjParam.bCanHit) //�߰� �ǰ��� ���
			{
				m_tObjParam.bCanHit = true;
				Check_FBLR();
			}
		}
	}

	return;
}

void CBlackWolf::Play_Down_Strong()
{
	return;
}

void CBlackWolf::Play_Down_Weak()
{
	return;
}

void CBlackWolf::Play_Dead()
{
	if (false == m_bCanDead)
	{
		m_bCanDead = true;
		m_bCanDissolve = true;
		m_eState = WOLF_ANI::Dead;
	}
	else
	{
		if (true == m_bCanDissolve && m_pMeshCom->Is_Finish_Animation(0.35f))
		{
			Start_Dissolve(0.5f, false, true);
			m_bCanDissolve = false;
		}

		if (m_pMeshCom->Is_Finish_Animation(0.95f))
		{
			m_bEnable = false;
			m_dAniPlayMul = 0;
		}
	}

	return;
}

void CBlackWolf::Play_Dead_Strong()
{
	m_eState = WOLF_ANI::Dead_Strong;

	if (m_pMeshCom->Is_Finish_Animation(0.95f))
	{
		m_bIsDead = true;
		m_dAniPlayMul = 0.f;
	}

	return;
}

HRESULT CBlackWolf::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Wolf_Black", L"Com_Mesh", (CComponent**)&m_pMeshCom)))
		return E_FAIL;

	// for.Com_NavMesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	m_pCollider->Set_Radius(_v3{ 0.5f, 0.5f, 0.5f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y , 0.f });

	return NOERROR;
}

HRESULT CBlackWolf::SetUp_ConstantTable()
{
	IF_NULL_VALUE_RETURN(m_pShaderCom, E_FAIL);

	CManagement* pManagement = CManagement::Get_Instance();
	IF_NULL_VALUE_RETURN(pManagement, E_FAIL);

	Safe_AddRef(pManagement);

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShaderCom->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShaderCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	Safe_Release(pManagement);

	return NOERROR;
}

HRESULT CBlackWolf::Ready_Collider()
{
	m_vecPhysicCol.reserve(3); //2ĭ�� ���� �Ҵ�
	m_vecAttackCol.reserve(2); //2ĭ�� ���� �Ҵ�

	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	IF_NULL_VALUE_RETURN(pCollider, E_FAIL);

	_float fRadius = 1.2f;

	// ù��° �ݶ��̴��� ��� üũ�� �ݶ��̴�
	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Range]->_41, m_matBone[Bone_Range]->_42, m_matBone[Bone_Range]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//==============================================================================================================

	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	IF_NULL_VALUE_RETURN(pCollider, E_FAIL);

	fRadius = 0.5f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Body]->_41, m_matBone[Bone_Body]->_42, m_matBone[Bone_Body]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//==============================================================================================================

	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	IF_NULL_VALUE_RETURN(pCollider, E_FAIL);

	fRadius = 0.2f;

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Head]->_41, m_matBone[Bone_Head]->_42, m_matBone[Bone_Head]->_43));
	pCollider->Set_Enabled(true);

	m_vecPhysicCol.push_back(pCollider);

	//==============================================================================================================

	//���� �� �ֱ�
	pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	IF_NULL_VALUE_RETURN(pCollider, E_FAIL);

	fRadius = 0.5f; //ũ�� Ȯ��

	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });
	pCollider->Set_Dynamic(true);
	pCollider->Set_Type(COL_SPHERE);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Head]->_41, m_matBone[Bone_Head]->_42, m_matBone[Bone_Head]->_43));
	pCollider->Set_Enabled(true);

	m_vecAttackCol.push_back(pCollider);

	return S_OK;
}

HRESULT CBlackWolf::Ready_BoneMatrix()
{
	D3DXFRAME_DERIVED*	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Head", 0);
	IF_NULL_VALUE_RETURN(pFrame, E_FAIL);

	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	pFrame = (D3DXFRAME_DERIVED*)m_pMeshCom->Get_BonInfo("Spine1", 0);
	IF_NULL_VALUE_RETURN(pFrame, E_FAIL);

	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;

	return S_OK;
}

CBlackWolf* CBlackWolf::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CBlackWolf* pInstance = new CBlackWolf(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CBlackWolf");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlackWolf::Clone_GameObject(void * pArg)
{
	CBlackWolf* pInstance = new CBlackWolf(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CBlackWolf");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlackWolf::Free()
{
	Safe_Release(m_pCollider);
	Safe_Release(m_pNavMesh);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pMeshCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	for (auto& vecter_iter : m_vecPhysicCol)
	{
		Safe_Release(vecter_iter);
	}

	for (auto& vecter_iter : m_vecAttackCol)
	{
		Safe_Release(vecter_iter);
	}

	for (auto& iter : m_matBone)
	{
		iter = nullptr;
	}

	CGameObject::Free();
}
