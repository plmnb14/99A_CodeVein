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

	Ready_BoneMatrix();
	Ready_Collider();

	return S_OK;
}

_int CPlayer_Colleague::Update_GameObject(_double TimeDelta)
{
	CGameObject::Late_Update_GameObject(TimeDelta);
	CGameObject::Update_GameObject(TimeDelta);

	//ColleagueMove_Walk();
	Check_Do_List();
	Set_AniEvent();

	if (FAILED(m_pRendererCom->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Test_Io", L"Com_StaticMesh", (CComponent**)&m_pStaticMesh)))
		return E_FAIL;

	// for.Com_NavMesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
		return E_FAIL;

	// for.Com_Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Com_Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

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
	return S_OK;
}

HRESULT CPlayer_Colleague::Ready_Weapon()
{
	return S_OK;
}

void CPlayer_Colleague::Check_Do_List()
{
	// �÷��̾� ������ �� ���� ��, 4.f �̻� ������������ �÷��̾ ���� ������ ������.
	//// 4.f ���ϸ� ���� ������ �ʰ� �׳� �´�
	// �ƴϴ� Idle ������ ���� ������ �ʴ´�.

	// �⺻ ���� ����
	// Hit, Att, Dead �̸� return ��Ų��
	if (m_eMovetype == CPlayer_Colleague::Coll_Hit || m_eMovetype == CPlayer_Colleague::Coll_Attack
		|| m_eMovetype == CPlayer_Colleague::Coll_Dead)
		return;

	if (true == m_tObjParam.bIsHit || true == m_tObjParam.bIsAttack || true == m_tObjParam.bIsDodge)
		return;

	// �÷��̾ �ֺ��� �ִ°�?
	// - �÷��̾ ����κ��� 3.f �Ÿ���ŭ �������ִ��� üũ�Ѵ�
	
	_v3 pPlayerPos = m_pTargetTransformCom->Get_Pos();

	_float MyLength = V3_LENGTH(&(m_pTransformCom->Get_Pos() - m_pTargetTransformCom->Get_Pos()));

	if (MyLength < 4.f)
	{
		// �� ������ �� ������ ���Ͱ� �ִ��� üũ, ���� �� Idle_Waiting
		// �÷��̾� HP�� ���⼭ ��� �غ� ���� �Ѱ�?
		// yes
		m_eMovetype = CPlayer_Colleague::Coll_Idle;
		m_eColl_IdleMoment = CPlayer_Colleague::Idle_Waiting;
	}
	else
	{
		// no
		// �÷��̾�� �󸶳� �Ÿ��� �������ִ°�?
		// 6.f ���ϸ� �Ȱ�, �̻��̸� �پ��
		if (MyLength > 4.f && MyLength < 7.f)
		{
			m_eMovetype = Coll_Move;
			m_eColl_Movement = Move_Walk;
		}
		if (MyLength > 7.f)
		{
			m_eMovetype = Coll_Move;
			m_eColl_Movement = Move_Run;
		}	
	}
}

void CPlayer_Colleague::Set_AniEvent()
{
	// enum�� ����� ���� ������ �������ִ� �Լ�

	switch (m_eMovetype)
	{
		case Client::CPlayer_Colleague::Coll_Idle:
		{
			Colleague_Idle();
			break;
		}
		case Client::CPlayer_Colleague::Coll_Move:
		{
			Colleague_Move();
			break;
		}
		case Client::CPlayer_Colleague::Coll_Attack:
		{
			break;
		}
		case Client::CPlayer_Colleague::Coll_Hit:
		{
			break;
		}
		case Client::CPlayer_Colleague::Coll_Dead:
		{
			Colleague_Dead();
			break;
		}
	}
}

HRESULT CPlayer_Colleague::SetUp_Default()
{
	m_pTarget = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE));

	// �÷��̾�� 10.f ������ ��ġ���� ���� ����
	//m_pTransformCom->Set_Pos(_v3(TARGET_TO_TRANS(m_pTarget)->Get_Pos().x - 1.f, TARGET_TO_TRANS(m_pTarget)->Get_Pos().y, TARGET_TO_TRANS(m_pTarget)->Get_Pos().z - 1.f));
	m_pTransformCom->Set_Scale(V3_ONE);
	m_pTargetTransformCom = TARGET_TO_TRANS(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_STAGE));


	m_tObjParam.fHp_Cur = 1000.f;
	m_tObjParam.fHp_Max = 1000.f;
	m_tObjParam.fDamage = 200.f;

	m_tObjParam.bCanHit = true;		// ���� �� �ִ���
	m_tObjParam.bCanDodge = true;	// ȸ�� ��������
	m_tObjParam.bCanAttack = true;	// ���� ��������
	m_tObjParam.bCanGuard = true;	// ���带 �� �� �ִ���


	return S_OK;
}

void CPlayer_Colleague::Colleague_Movement(_float fSpeed, _v3 vDir)
{
	V3_NORMAL(&vDir, &vDir);

	m_pTransformCom->Set_Pos((m_pNavMesh->Move_OnNaviMesh(NULL, &m_pTransformCom->Get_Pos(), &vDir, fSpeed * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60"))));
	//m_pTransformCom->Set_Angle(TARGET_TO_TRANS(m_pTarget)->Chase_Target());
}

void CPlayer_Colleague::Colleague_Idle()
{
	// ���⼭ Idle ���¸� ������.
	switch (m_eColl_IdleMoment)
	{
	case Client::CPlayer_Colleague::Idle_Waiting:
	{
		// Idle_Waiting - �÷��̾ ���� �ȿ� �ְ� ������ ���͵� ���� ��
		break;
	}
	case Client::CPlayer_Colleague::Idle_Attwaiting:
	{
		// Idle_Attwaiting - �÷��̾�� ���� ���� ���� �ְ� ������ �غ� �� ��
		break;
	}
	case Client::CPlayer_Colleague::Idle_Guard:
	{
		// Idle_Guard - �÷��̾�� ���� ���� �� �����ϰ� ���� ���� ��
		break;
	}
	}
}

void CPlayer_Colleague::Colleague_Move()
{
	switch (m_eColl_Movement)
	{
	case Client::CPlayer_Colleague::Move_Walk:
	{
		// �÷��̾�� ���� �Ÿ� �̻����� ��������� �� �ȴ´�
		ColleagueMove_Walk();
		break;
	}
	case Client::CPlayer_Colleague::Move_Run:
	{
		// �÷��̾�� ���� �Ÿ� �̻� ������ ���� �� �پ �i�ư���
		ColleagueMove_Run();
		break;
	}
	case Client::CPlayer_Colleague::Move_Dodge:
	{
		// �̰� ȸ��
		break;
	}
	}
}

void CPlayer_Colleague::Colleague_Dead()
{
}

void CPlayer_Colleague::ColleagueMove_Walk()
{
	m_eColl_Movement = CPlayer_Colleague::Move_Walk;
	Funtion_RotateBody();
	Colleague_Movement(2.f, _v3(m_pTargetTransformCom->Get_Pos().x, 0.f, m_pTargetTransformCom->Get_Pos().z));
}

void CPlayer_Colleague::ColleagueMove_Run()
{
	// �� �� �ӵ������ϴ°ǵ� �Ͻ������� �̷��� �ΰ� ���߿� �� �����ؾ� ��
	m_eColl_Movement = CPlayer_Colleague::Move_Run;
	Funtion_RotateBody();
	Colleague_Movement(4.f, _v3(m_pTargetTransformCom->Get_Pos().x, 0.f, m_pTargetTransformCom->Get_Pos().z));
}

void CPlayer_Colleague::ColleagueMove_Dodge()
{
	// �����⳪ ���� �� �ִ� ��Ҹ� ���� �Լ�
}

void CPlayer_Colleague::Funtion_RotateBody()
{
	_float fTargetAngle = m_pTransformCom->Chase_Target_Angle(&m_pTargetTransformCom->Get_Pos());

	_float fYAngle = m_pTransformCom->Get_Angle().y;

	_v3 vTargetDir = m_pTransformCom->Get_Axis(AXIS_Z);
	V3_NORMAL_SELF(&vTargetDir);

	// �÷��̾��� ������ 0 ���� ũ��, �� ������ 0���� ���� ��
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

CPlayer_Colleague * CPlayer_Colleague::Create(_Device pGraphic_Device)
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
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTargetTransformCom);
	Safe_Release(m_pStaticMesh);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavMesh);

	CGameObject::Free();
}
