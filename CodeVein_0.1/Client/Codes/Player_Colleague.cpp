#include "stdafx.h"
#include "..\Headers\Player_Colleague.h"
#include "Player.h"
#include "Weapon.h"


CPlayer_Colleague::CPlayer_Colleague(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CPlayer_Colleague::CPlayer_Colleague(const CPlayer_Colleague & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer_Colleague::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Colleague::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	SetUp_Default();

	//Ready_BoneMatrix();
	//Ready_Collider();

	return S_OK;
}

_int CPlayer_Colleague::Update_GameObject(_double TimeDelta)
{
	CGameObject::Late_Update_GameObject(TimeDelta);
	CGameObject::Update_GameObject(TimeDelta);

	Check_Do_List();
	Set_AniEvent();

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	//cout << "���� ��ġ: " << m_pTransformCom->Get_Pos().x << ", " << m_pTransformCom->Get_Pos().y << ", " << m_pTransformCom->Get_Pos().z << endl;
	//cout << "MoveType: " << m_eMovetype << ": Att�� 2" << endl;

	return S_OK;
}

_int CPlayer_Colleague::Late_Update_GameObject(_double TimeDelta)
{

	return S_OK;
}

HRESULT CPlayer_Colleague::LateInit_GameObject()
{
	return S_OK;
}

HRESULT CPlayer_Colleague::Render_GameObject()
{
	if (nullptr == m_pShaderCom || nullptr == m_pStaticMesh)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShaderCom->Begin_Shader();

	_ulong dwNumSubset = m_pStaticMesh->Get_NumMaterials();

	for (_ulong i = 0; i < dwNumSubset; ++i)
	{
		m_pShaderCom->Begin_Pass(0);
		m_pShaderCom->Set_StaticTexture_Auto(m_pStaticMesh, i);

		m_pShaderCom->Commit_Changes();

		m_pStaticMesh->Render_Mesh(i);

		m_pShaderCom->End_Pass();
	}

	m_pShaderCom->End_Shader();

	return S_OK;
}

HRESULT CPlayer_Colleague::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_DefaultBox", L"Com_StaticMesh", (CComponent**)&m_pStaticMesh)))
		return E_FAIL;

	//// for.Com_Mesh
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Jack", L"Com_DynamicMesh", (CComponent**)&m_pDynamicMesh)))
	//	return E_FAIL;

	// for.Com_NavMesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	// �ݶ��̴� ����
	m_pCollider->Set_Radius(_v3(0.35f, 0.35f, 0.35f));
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_Dynamic(true);		// �����̴���
	m_pCollider->Set_CenterPos(m_pTransformCom->Get_Pos());

	return S_OK;
}


HRESULT CPlayer_Colleague::SetUp_ConstantTable()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Value("g_matWorld", &m_pTransformCom->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matView", &g_pManagement->Get_Transform(D3DTS_VIEW), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_matProj", &g_pManagement->Get_Transform(D3DTS_PROJECTION), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShaderCom)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Colleague::Ready_BoneMatrix()
{
	// 0�� ����, 1�� �÷��̾�
	/*LPCSTR TempChar = "Head";

	D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(TempChar, 0);

	m_matBone[Bone_Head] = &pFrame->CombinedTransformationMatrix;

	TempChar = "Spine";

	pFrame = (D3DXFRAME_DERIVED*)m_pDynamicMesh->Get_BonInfo(TempChar, 0);

	m_matBone[Bone_Range] = &pFrame->CombinedTransformationMatrix;
	m_matBone[Bone_Body] = &pFrame->CombinedTransformationMatrix;*/

	return S_OK;
}

HRESULT CPlayer_Colleague::Ready_Collider()
{
	//m_vecPhysicCol;		// �浹 üũ�� �ݶ��̴� ����
	//m_vecAttackCol;		// ���ݿ� �ݶ��̴� ����
	m_vecPhysicCol.reserve(3);
	m_vecAttackCol.reserve(1);

	// ��� üũ�� Collider - ���� ���� �ִ���? -> ������ �ٷ� �����ϰ� �ָ������� �� �� ���ٰ� ���� �����ٴ��� �ϴ°�
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));
	IF_NULL_VALUE_RETURN(pCollider, E_FAIL);

	_float fRadius = 1.f;

	pCollider->Set_Radius(_v3(fRadius, fRadius, fRadius));
	pCollider->Set_Dynamic(true);
	pCollider->Set_CenterPos(_v3(m_matBone[Bone_Range]->_41, m_matBone[Bone_Range]->_42, m_matBone[Bone_Range]->_43));

	return S_OK;
}

HRESULT CPlayer_Colleague::Ready_Weapon()
{
	return S_OK;
}

void CPlayer_Colleague::Update_Collider()
{
}

void CPlayer_Colleague::Render_Collider()
{
}

void CPlayer_Colleague::Check_Do_List()
{
	// �÷��̾� ������ �� ���� ��, 4.f �̻� ������������ �÷��̾ ���� ������ ������.

	// �켱 ����
	// - �÷��̾ 30.f �ȿ� �ִ���
	// - ������Ȳ�� �ƴ϶�� ���� ����� ���� �� ������ ã�´�
	// - ���� ���� ���Ͱ� �ִ���
	// - ������ �÷��̾ �ִ���

	_v3 pPlayerPos = m_pTargetTransformCom->Get_Pos();
	_float MyLength = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransformCom->Get_Pos()));


	if (MyLength < 30.f)	// - �÷��̾ 30.f �ȿ� �ִ��� ���� üũ
	{
		//	// ���Ͱ� ��ó�� ���� �� ���� ������ �̵��Ͽ� �����Ѵ�.
		//	// ���̽��� ��� �÷��̾�� ���� Ƣ�����
		//	// ������ �⺻ ���ݸ�
		//	// �Ŀ� �Ÿ� ����ؼ� ��ų ��� ���ε� ���ؾ� ��
		if (false == m_bStart_Fighting)
		{
			// ���� ��Ȳ�� �ƴ϶�� ���� ����� ���͸� ã�´�

			m_List_pMonTarget[0] = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
			m_List_pMonTarget[1] = g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE);
			
			if (m_List_pMonTarget[0].empty())
				m_bMonExistence = true;
			else
				m_bMonExistence = false;

			if (false == m_bMonExistence)
			{
				_v3 vecMonpos = V3_NULL;

				for (auto& iter : m_List_pMonTarget[0])
					vecMonpos = TARGET_TO_TRANS(iter)->Get_Pos();

				m_fAll_Length = V3_LENGTH(&(m_pTransformCom->Get_Pos() - vecMonpos));

				if (m_fAll_Length < 15.f)
				{
					m_bStart_Fighting = true;
					
				}
			}
		}
		if (true == m_bStart_Fighting)
		{
			_float MonHP;
			for (auto& iter : m_List_pMonTarget[0])
				MonHP = iter->Get_Target_Hp();

			// ���⼭ ���� ����
			if (m_fAll_Length < 15.f)
			{
				m_eMovetype = CPlayer_Colleague::Coll_Attack;
				m_eColl_AttackMoment = CPlayer_Colleague::Att_Idle;
			}
				

			if (0 >= MonHP)
				m_bStart_Fighting = false;
		}

		// - ���� �� ���Ͱ� ������ �÷��̾ �ִ���
		if (MyLength > 3.f && false == m_bStart_Fighting)
		{
			// no
			// �÷��̾�� �󸶳� �Ÿ��� �������ִ°�?
			// 7.f ���ϸ� �Ȱ�, �̻��̸� �پ��
			if (MyLength > 3.f && MyLength < 7.f)
			{
				m_eMovetype = CPlayer_Colleague::Coll_Move;
				m_eColl_Movement = CPlayer_Colleague::Move_Walk;
			}
			if (MyLength > 7.f)
			{
				m_eMovetype = CPlayer_Colleague::Coll_Move;
				m_eColl_Movement = CPlayer_Colleague::Move_Run;
			}
		}
		if (MyLength < 3.f && false == m_bNear_byMonster)
		{
			// �� ������ �� ������ ���Ͱ� �ִ��� üũ, ���� �� Idle_Waiting
			// �÷��̾� HP�� ���⼭ ��� �غ� ���� �Ѱ�?
			// yes
			m_eMovetype = CPlayer_Colleague::Coll_Idle;
			m_eColl_IdleMoment = CPlayer_Colleague::Idle_Waiting;
		}
	}
	if (MyLength > 30.f)
	{
		// �÷��̾ ���� ���� ������ϱ� �÷��̾� ��ó�� ���� ��ȯ�ؾ� ��
		// ��ġ�� �÷��̾�� �� 3.f ���� ������ ������ ����������
		m_pTargetTransformCom->Set_Pos(_v3(m_pTargetTransformCom->Get_Pos().x - 3.f, m_pTargetTransformCom->Get_Pos().y, m_pTargetTransformCom->Get_Pos().z - 2.f));
	}

	// �⺻ ���� ����
	// Hit, Att, Dead �̸� return ��Ų��
	if (m_eMovetype == CPlayer_Colleague::Coll_Hit || m_eMovetype == CPlayer_Colleague::Coll_Attack
		|| m_eMovetype == CPlayer_Colleague::Coll_Dead)
		return;

	if (true == m_tObjParam.bIsHit || true == m_tObjParam.bIsAttack || true == m_tObjParam.bIsDodge)
		return;
}

void CPlayer_Colleague::Set_AniEvent()
{
	// enum�� ����� ���� ������ �������ִ� �Լ�

	switch (m_eMovetype)
	{
		case CPlayer_Colleague::Coll_Idle:
		{
			switch (m_eColl_IdleMoment)
			{
			case CPlayer_Colleague::Idle_Waiting:
			{
				CollIdle_Waiting();
				// Idle_Waiting - �÷��̾ ���� �ȿ� �ְ� ������ ���͵� ���� ��
				break;
			}
			case CPlayer_Colleague::Idle_Guard:
			{
				// Idle_Guard - �÷��̾�� ���� ���� �� �����ϰ� ���� ���� ��
				break;
			}
			}
			break;
		}
		case CPlayer_Colleague::Coll_Move:
		{
			switch (m_eColl_Movement)
			{
			case Client::CPlayer_Colleague::Move_Walk:
			{
				// �÷��̾�� ���� �Ÿ� �̻����� ��������� �� �ȴ´�
				CollMove_Walk();
				break;
			}
			case CPlayer_Colleague::Move_Run:
			{
				// �÷��̾�� ���� �Ÿ� �̻� ������ ���� �� �پ �i�ư���
				CollMove_Run();
				break;
			}
			case CPlayer_Colleague::Move_Dodge:
			{
				// �̰� ȸ��
				break;
			}
			}
			break; 
		}
		case CPlayer_Colleague::Coll_Guard:
		{
			switch (m_eColl_GuardMoment)
			{
			case Client::CPlayer_Colleague::Guard_Idle:
			{
				break;
			}
			case Client::CPlayer_Colleague::Gurad_Walk:
			{
				break;
			}
			case Client::CPlayer_Colleague::Gurad_Hit:
			{
				break;
			}
			}
			break;
		}
		case CPlayer_Colleague::Coll_Attack:
		{
			switch (m_eColl_AttackMoment)
			{
			case CPlayer_Colleague::Att_waiting:
			{
				// ���� ��� ���� - ������ ����?
				break;
			}
			case CPlayer_Colleague::Att_Idle:
			{
				CollAtt_Idle();
				break;
			}
			case CPlayer_Colleague::Att_Skill:
			{

				break;
			}
			}
			break;
		}
		case CPlayer_Colleague::Coll_Hit:
		{
			Colleague_Hit();
			break;
		}
		case CPlayer_Colleague::Coll_Dead:
		{
			Colleague_Dead();
			break;
		}
	}
}

HRESULT CPlayer_Colleague::SetUp_Default()
{
	m_pTarget = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
	IF_NULL_VALUE_RETURN(m_pTarget, E_FAIL);

	// �÷��̾�� 10.f ������ ��ġ���� ���� ����
	//m_pTransformCom->Set_Pos(_v3(TARGET_TO_TRANS(m_pTarget)->Get_Pos().x - 1.f, TARGET_TO_TRANS(m_pTarget)->Get_Pos().y, TARGET_TO_TRANS(m_pTarget)->Get_Pos().z - 1.f));
	m_pTransformCom->Set_Scale(V3_ONE);
	m_pTargetTransformCom = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

	m_tObjParam.fHp_Cur = 1000.f;
	m_tObjParam.fHp_Max = 1000.f;
	m_tObjParam.fDamage = 200.f;

	m_tObjParam.bCanHit = true;		// ���� �� �ִ���
	m_tObjParam.bIsHit = false;		// �´� ��������
	m_tObjParam.bHitAgain = false;

	m_tObjParam.bCanAttack = true;	// ���� ��������
	m_tObjParam.bIsAttack = false;	// ���� ������

	m_tObjParam.bCanDodge = true;	// ȸ�� ��������
	m_tObjParam.bIsDodge = false;	// ȸ�� ������

	m_tObjParam.bCanGuard = true;	// ���� ����?
	m_tObjParam.bIsGuard = false;	// ���� ������

	return S_OK;
}

void CPlayer_Colleague::Colleague_Movement(_float fSpeed, _v3 vDir)
{
	V3_NORMAL(&vDir, &vDir);

	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &vDir, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));
	m_pTransformCom->Set_Pos_Axis(m_pNavMesh->Axis_Y_OnNavMesh(m_pTargetTransformCom->Get_Pos()), AXIS_Y);
}

void CPlayer_Colleague::Colleague_Dead()
{
	// ���� �ִϸ��̼� ���
	
}

void CPlayer_Colleague::Colleague_Guard()
{
}

void CPlayer_Colleague::Colleague_Hit()
{
	// �¾Ҵ�!
}

void CPlayer_Colleague::CollMove_Walk()
{
	m_eColl_Movement = CPlayer_Colleague::Move_Walk;
	Funtion_RotateBody();
	Colleague_Movement(2.f, m_pTransformCom->Get_Axis(AXIS_Z));
}

void CPlayer_Colleague::CollMove_Run()
{
	// �� �� �ӵ������ϴ°ǵ� �Ͻ������� �̷��� �ΰ� ���߿� �� �����ؾ� ��
	m_eColl_Movement = CPlayer_Colleague::Move_Run;
	Funtion_RotateBody();
	Colleague_Movement(4.f, m_pTransformCom->Get_Axis(AXIS_Z));
}

void CPlayer_Colleague::CollMove_Dodge()
{
	// �����⳪ ���� �� �ִ� ��Ҹ� ���� �Լ�
}

void CPlayer_Colleague::CollIdle_Waiting()
{
	m_eColl_IdleMoment = CPlayer_Colleague::Idle_Waiting;
	Funtion_RotateBody();
	Colleague_Movement(0.f, m_pTransformCom->Get_Axis(AXIS_Z));
}

void CPlayer_Colleague::CollAtt_Idle()
{
	// �⺻ ���ݸ� �ϴ� �Լ�
	// ���� �⺻ ����
	// �⺻ 4Ÿ
	// ���Ͱ� ���� ���� ���̸� ������ ��� �����Ѵ�.
	// ������ �����ؾ� ��
	CTransform* pMonTrans = nullptr;
	CGameObject* pMonster = nullptr;

	for (auto& Moniter : m_List_pMonTarget[0])
		pMonster = Moniter;

	for (auto& iter : m_List_pMonTarget[0])
		pMonTrans = TARGET_TO_TRANS(iter);

	if (nullptr == pMonTrans)
		return;


	Funtion_RotateBody();

	_float fMonLenght = V3_LENGTH(&(m_pTransformCom->Get_Pos() - pMonTrans->Get_Pos()));

	// ������������ ���������� ���� ������ ����
	if (fMonLenght > 2.3f)
	{
		// ���������� ���� �i�ư���
		Colleague_Movement(4.f, pMonTrans->Get_Axis(AXIS_Z));
	}
		
	if (fMonLenght < 2.3f)
	{
		// ���� ���� �������� �����ٰ� ����
		if (true == pMonster->Get_Target_Param().bIsAttack)
		{
			// ���Ͱ� ���� �� ���带 �ø�
			m_eMovetype = CPlayer_Colleague::Coll_Guard;
		}
	}
}

void CPlayer_Colleague::Funtion_RotateBody()
{
	_float fTargetAngle = m_pTransformCom->Chase_Target_Angle(&m_pTargetTransformCom->Get_Pos());

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
}

CPlayer_Colleague* CPlayer_Colleague::Create(_Device pGraphic_Device)
{
	CPlayer_Colleague*	pInstance = new CPlayer_Colleague(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Colleague::Clone_GameObject(void* pArg)
{
	CPlayer_Colleague*	pInstance = new CPlayer_Colleague(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CPlayer_Colleague");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Colleague::Free()
{
	Safe_Release(m_pSword);
	Safe_Release(m_pCollider);

	// ���� Transfrom
	Safe_Release(m_pTransformCom);

	// �÷��̾� Ransform
	//Safe_Release(m_pTargetTransformCom);
	Safe_Release(m_pStaticMesh);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavMesh);

	CGameObject::Free();
}
