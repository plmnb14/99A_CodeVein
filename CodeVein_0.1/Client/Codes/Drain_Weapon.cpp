#include "stdafx.h"
#include "..\Headers\Drain_Weapon.h"
#include "CameraMgr.h"

CDrain_Weapon::CDrain_Weapon(_Device pGraphic_Device)
	: CGameObject(pGraphic_Device)
{

}

CDrain_Weapon::CDrain_Weapon(const CDrain_Weapon & rhs)
	: CGameObject(rhs)
{

}

HRESULT CDrain_Weapon::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CDrain_Weapon::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	SetUp_Default();

	m_fAnimMultiply = 1.f;
	m_pMesh_Dynamic->SetUp_Animation(m_eAnimnum);

	Find_TailHeadBone();

	return NOERROR;
}

_int CDrain_Weapon::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	Cacl_AttachBoneTransform();

	OnCollisionEnter();

	Parameter_State();
	
	return NO_EVENT;
}

_int CDrain_Weapon::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRenderer ||
		nullptr == m_pMesh_Dynamic)
		return E_FAIL;

	if (m_bActive)
	{
		m_pMesh_Dynamic->SetUp_Animation(m_eAnimnum);

		if (!m_bDissolve)
		{
			if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
				return E_FAIL;
			if (FAILED(m_pRenderer->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
				return E_FAIL;
			if (FAILED(m_pRenderer->Add_RenderList(RENDER_SHADOWTARGET, this)))
				return E_FAIL;
		}

		else if (m_bDissolve)
			if (FAILED(m_pRenderer->Add_RenderList(RENDER_ALPHA, this)))
				return E_FAIL;
	}

	return NO_EVENT;
}

HRESULT CDrain_Weapon::Render_GameObject()
{
	if (nullptr == m_pShader ||
		nullptr == m_pMesh_Dynamic)
		return E_FAIL;

	m_pMesh_Dynamic->Play_Animation(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply);

	_mat		ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(m_pShader)))
		return E_FAIL;

	m_pShader->Begin_Shader();

	_uint iNumMeshContainer = _uint(m_pMesh_Dynamic->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMesh_Dynamic->Get_NumMaterials(i);

		// 메시를 뼈에 붙인다.
		m_pMesh_Dynamic->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			m_iPass = m_pMesh_Dynamic->Get_MaterialPass(i, j);

			if (m_bDissolve)
				m_iPass = 3;

			//cout << "현재 패스 : " << m_iPass << endl;

			m_pShader->Begin_Pass(m_iPass);

			m_pShader->Set_DynamicTexture_Auto(m_pMesh_Dynamic, i, j);

			m_pShader->Commit_Changes();

			m_pMesh_Dynamic->Render_Mesh(i, j);

			m_pShader->End_Pass();
		}
	}

	m_pShader->End_Shader();

	_v3 vTmpColPos;

	_mat matTmp = *m_matTailSword * m_pTransform->Get_WorldMat();

	memcpy(vTmpColPos, &matTmp._41, sizeof(_v3));

	m_pCollider->Update(vTmpColPos);

	Draw_Collider();

	return NOERROR;
}

HRESULT CDrain_Weapon::Render_GameObject_SetPass(CShader * pShader, _int iPass)
{
	if (nullptr == pShader ||
		nullptr == m_pMesh_Dynamic)
		return E_FAIL;

	_mat		ViewMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
		return E_FAIL;

	m_matLastWVP = m_pTransform->Get_WorldMat() * ViewMatrix * ProjMatrix;

	_uint iNumMeshContainer = _uint(m_pMesh_Dynamic->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMesh_Dynamic->Get_NumMaterials(i);

		// 메시를 뼈에 붙인다.
		//m_pMesh_Dynamic->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			m_iPass = m_pMesh_Dynamic->Get_MaterialPass(i, j);

			pShader->Begin_Pass(iPass);

			pShader->Set_DynamicTexture_Auto(m_pMesh_Dynamic, i, j);

			pShader->Commit_Changes();

			m_pMesh_Dynamic->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	return NOERROR;
}

HRESULT CDrain_Weapon::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	if (nullptr == pShader ||
		nullptr == m_pMesh_Dynamic)
		return E_FAIL;

	m_pMesh_Dynamic->Play_Animation(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply);

	if (FAILED(SetUp_ConstantTable(pShader)))
		return E_FAIL;

	_uint iNumMeshContainer = _uint(m_pMesh_Dynamic->Get_NumMeshContainer());

	for (_uint i = 0; i < _uint(iNumMeshContainer); ++i)
	{
		_uint iNumSubSet = (_uint)m_pMesh_Dynamic->Get_NumMaterials(i);

		// 메시를 뼈에 붙인다.
		m_pMesh_Dynamic->Update_SkinnedMesh(i);

		for (_uint j = 0; j < iNumSubSet; ++j)
		{
			m_iPass = m_pMesh_Dynamic->Get_MaterialPass(i, j);

			if (m_bDissolve)
				m_iPass = 3;

			//cout << "현재 패스 : " << m_iPass << endl;

			pShader->Begin_Pass(m_iPass);

			pShader->Set_DynamicTexture_Auto(m_pMesh_Dynamic, i, j);

			pShader->Commit_Changes();

			m_pMesh_Dynamic->Render_Mesh(i, j);

			pShader->End_Pass();
		}
	}

	_v3 vTmpColPos;

	_mat matTmp = *m_matTailSword * m_pTransform->Get_WorldMat();

	memcpy(vTmpColPos, &matTmp._41, sizeof(_v3));

	m_pCollider->Update(vTmpColPos);

	Draw_Collider();

	return NOERROR;
}

void CDrain_Weapon::Set_Active(_bool _bActiveDrain)
{
	m_bActive = _bActiveDrain;

	if (false == m_bActive)
	{
		m_bOffDissolve = false;

		m_pMesh_Dynamic->SetUp_Animation(0);
		m_pMesh_Dynamic->Play_Animation(g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60") * m_fAnimMultiply);
	}

	else if (true == m_bActive)
	{
		LOOP(4)
			m_bEventTrigger[i] = false;

		if (Drain_Parry == m_eAnimnum)
		{
			Start_Dissolve(1.5f, true);
		}

		else if (Drain_Charge_Start == m_eAnimnum)
		{
			Start_Dissolve(2.f, true);
		}
		// 활성화 되면, 디졸브 효과
	}
}

void CDrain_Weapon::Set_ActiveCollider(_bool _bActiveCollider)
{
	m_pCollider->Set_Enabled(_bActiveCollider);
}

void CDrain_Weapon::Set_ResetOldAnimIdx()
{
	m_pMesh_Dynamic->Reset_OldIndx();
}

void CDrain_Weapon::Set_AnimIdx(_ulong _eAnimState)
{
	switch (_eAnimState)
	{
		// Parry
	case 375:
	{
		m_eAnimnum = Drain_Parry;
		break;
	}

	// Suck_Charge_Start
	case 376:
	{
		m_eAnimnum = Drain_Charge_Start;
		break;
	}

	// Suck_Charge_End
	case 377:
	{
		m_eAnimnum = Drain_Charge_End;
		break;
	}

	// Suck_Combo
	case 378:
	{
		m_eAnimnum = Drain_ComboSuck;
		break;
	}

	// Suck_Ground
	case 379:
	{
		m_eAnimnum = Drain_GroundSuck;
		break;
	}

	// Suck_Special_01
	case 380:
	{
		m_eAnimnum = Drain_SpecialSuck_01;
		break;
	}

	// Suck_Special_02
	case 381:
	{
		m_eAnimnum = Drain_SpecialSuck_02;
		break;
	}

	}
}

void CDrain_Weapon::Set_Enable_Record(_bool _bOnRecord)
{
	m_bRecordCollision = _bOnRecord;

	if (false == m_bRecordCollision)
	{
		Clear_CollisionRecordList();
	}
}

void CDrain_Weapon::Find_TailHeadBone()
{
	LPCSTR tmpChar = "GCTailLeftBladeA";

	D3DXFRAME_DERIVED*	pFamre = (D3DXFRAME_DERIVED*)m_pMesh_Dynamic->Get_BonInfo(tmpChar, 0);

	m_matTailSword = &pFamre->CombinedTransformationMatrix;

	tmpChar = "GCTailSword";

	pFamre = (D3DXFRAME_DERIVED*)m_pMesh_Dynamic->Get_BonInfo(tmpChar, 0);

	m_matTailHead = &pFamre->CombinedTransformationMatrix;
}

void CDrain_Weapon::OnCollisionEnter()
{
	if (false == m_tObjParam.bCanAttack)
		return;

	OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));
}

void CDrain_Weapon::OnCollisionEvent(list<CGameObject*> plistGameObject)
{
	_bool bFirst = true;
	//게임 오브젝트를 받아와서
	for (auto& iter : plistGameObject)
	{
		// 1 : 다 의 기술일 경우, 기록을 켜야 한대만 맞는다.
		if (m_bRecordCollision)
		{
			if (false == Update_CollisionRecord(iter))
				continue;
		}

		// 맞을 수 없다면 리턴
		if (false == iter->Get_Target_CanHit())
			continue;

		// 피직콜라이더랑 비교
		for (auto& vecCol : iter->Get_PhysicColVector())
			{
				// 물체 전체를 대표할 콜라이더.
				if (m_pCollider->Check_Sphere(vecCol))
				{
					// 첫번째는 경계구 콜라이더니까 다음 콜라이더와 충돌처리 한다.
					if (bFirst)
					{
						bFirst = false;
						continue;
					}

					if (false == iter->Get_Target_IsDodge())
					{
						if (false == m_bOnExecution)
						{
							iter->Set_Target_CanHit(false);

							if (iter->Get_Target_IsHit())
							{
								iter->Set_HitAgain(true);
							}
						}

						if (false == iter->Get_Target_IsDodge())
						{
							m_tObjParam.fDamage = 99999.f; // iter->Get_Target_Hp();

							// 무기 공격력의 +-20%까지 랜덤범위
							// 몬스터 HP바 확인을 위해 데미지 추가해놓음 - Chae
							_uint min = (_uint)(m_tObjParam.fDamage - (m_tObjParam.fDamage * 0.2f));
							_uint max = (_uint)(m_tObjParam.fDamage + (m_tObjParam.fDamage * 0.2f));

							iter->Add_Target_Hp(-(_float)CALC::Random_Num(min, max) * 1.f);
							g_pManagement->Create_Hit_Effect(m_pCollider, vecCol, TARGET_TO_TRANS(iter));

							g_pTimer_Manager->Set_MutiplyTime(L"Timer_Fps_60", 0.025f);
							g_pTimer_Manager->Set_MutiplyResetTime(L"Timer_Fps_60", 0.1f);
							SHAKE_CAM_lv2;

							if (m_bRecordCollision)
							{
								m_listCollisionRecord.push_back(iter);
							}
						}
					}

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

void CDrain_Weapon::Update_Trails(_double TimeDelta)
{
}

void CDrain_Weapon::Update_Collider()
{
	_v3 tmpPos;
	memcpy(tmpPos, &m_matTailSword->_41, sizeof(_v3));

	m_pCollider->Update(tmpPos);
}

void CDrain_Weapon::Draw_Collider()
{
	g_pManagement->Gizmo_Draw_Sphere(m_pCollider->Get_CenterPos(), m_pCollider->Get_Radius().x);
}

_bool CDrain_Weapon::Update_CollisionRecord(CGameObject * pObject)
{
	for (auto& iter : m_listCollisionRecord)
	{
		// 순회하다가 같으면
		if (iter == pObject)
			return false;
	}

	return true;
}

void CDrain_Weapon::Clear_CollisionRecordList()
{
	for (auto& iter : m_listCollisionRecord)
	{
		iter = nullptr;
	}
}

void CDrain_Weapon::Change_WeaponMesh(const _tchar* _MeshName)
{
	// 이름 비교해서 같으면 Return
	if (!lstrcmp(m_szName, _MeshName))
		return;

	// 다르다면 이름 갱신하고,
	lstrcpy(m_szName, _MeshName);

	// 컴포넌트에 있는 매쉬 찾아서
	auto& iter = m_pmapComponents.find(L"Com_DynamicMesh");

	// 둘 다 해제
	Safe_Release(m_pMesh_Dynamic);
	Safe_Release(iter->second);

	// Release 한 컴포넌트에 새로이 Clone 받음.
	iter->second = m_pMesh_Dynamic = static_cast<CMesh_Dynamic*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _MeshName));
	Safe_AddRef(iter->second);
}

HRESULT CDrain_Weapon::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Mesh", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_Mesh
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Drain_LongTail", L"Com_DynamicMesh", (CComponent**)&m_pMesh_Dynamic)))
		return E_FAIL;

	// Collider
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	// for.Com_BattleAgent
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"BattleAgent", L"Com_BattleAgent", (CComponent**)&m_pBattleAgent)))
		return E_FAIL;

	lstrcpy(m_szName, L"Mesh_Drain_LongTail");

	m_pCollider->Set_Radius(_v3{ 1.f, 1.f, 1.f });
	m_pCollider->Set_Dynamic(true);
	m_pCollider->Set_Type(COL_SPHERE);
	m_pCollider->Set_CenterPos(m_pTransform->Get_Pos() + _v3{ 0.f , m_pCollider->Get_Radius().y * 2.f , 0.f });

	return NOERROR;
}

HRESULT CDrain_Weapon::SetUp_Default()
{
	// Transform
	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);
	return S_OK;
}

HRESULT CDrain_Weapon::SetUp_ConstantTable(CShader* pShader)
{
	if (nullptr == pShader)
		return E_FAIL;

	//=============================================================================================
	// 기본 메트릭스
	//=============================================================================================

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	//=============================================================================================
	// 디졸브용 상수
	//=============================================================================================
	if (FAILED(pShader->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 쉐이더 재질정보 수치 입력
	//=============================================================================================
	_float	fEmissivePower = 5.f;	// 이미시브 : 높을 수록, 자체 발광이 강해짐.
	_float	fSpecularPower = 1.f;	// 메탈니스 : 높을 수록, 정반사가 강해짐.
	_float	fRoughnessPower = 1.f;	// 러프니스 : 높을 수록, 빛 산란이 적어짐(빛이 응집됨).
	_float	fMinSpecular = 1.f;	// 최소 빛	: 최소 단위의 빛을 더해줌.
	_float	fID_R = 1.0f;	// ID_R : R채널 ID 값 , 1이 최대
	_float	fID_G = 0.5f;	// ID_G : G채널 ID 값 , 1이 최대
	_float	fID_B = 0.1f;	// ID_B	: B채널 ID 값 , 1이 최대

	if (FAILED(pShader->Set_Value("g_fEmissivePower", &fEmissivePower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fSpecularPower", &fSpecularPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fRoughnessPower", &fRoughnessPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fMinSpecular", &fMinSpecular, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fID_R_Power", &fID_R, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fID_G_Power", &fID_G, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fID_B_Power", &fID_B, sizeof(_float))))
		return E_FAIL;
	//=============================================================================================

	m_pBattleAgent->Update_RimParam_OnShader(pShader);

	return NOERROR;
}

HRESULT CDrain_Weapon::Parameter_State()
{
	if (false == m_bActive)
		return S_OK;

	switch (m_eAnimnum)
	{
	case Drain_Parry:
	{
		_double dAniTime = m_pMesh_Dynamic->Get_TrackInfo().Position;

		if (false == m_bOffDissolve)
		{
			m_bOffDissolve = true;
		}

		else if (m_bOffDissolve)
		{
			if (dAniTime < 1.0f)
			{
				//if (m_fFXAlpha <= 0.45f)
				if (m_fFXAlpha >= 0.9f)
				{
					if (m_bDissolve)
					{
						m_fFXAlpha = 0.f;
						m_bDissolve = false;
					}
				}
			}

			else if (dAniTime >= 3.15f)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					Start_Dissolve(0.01f);
				}
			}
		}

		break;
	}

	case Drain_Charge_Start:
	{

		break;
	}

	case Drain_Charge_End:
	{
		_double dAniTime = m_pMesh_Dynamic->Get_TrackInfo().Position;

		if (false == m_bOffDissolve)
		{
			m_bOffDissolve = true;
		}

		else if (m_bOffDissolve)
		{
			if (m_pMesh_Dynamic->Is_Finish_Animation(0.7f))
			{
				m_bOffDissolve = false;
				m_bActive = false;

				return S_OK;
			}

			else if (dAniTime >= 3.f)
			{
				if (false == m_bEventTrigger[0])
				{
					m_bEventTrigger[0] = true;

					Start_Dissolve(0.5f);
				}
			}
		}

		break;
	}

	default:
	{
		m_bOffDissolve = false;

		break;
	}
	}

	return S_OK;
}

void CDrain_Weapon::Cacl_AttachBoneTransform()
{
	_mat tmpMat;
	D3DXMatrixIdentity(&tmpMat);

	memcpy(&tmpMat._41, &(*m_pmatAttach)._41, sizeof(_v3));

	m_pTransform->Calc_ParentMat(&(tmpMat * *m_pmatParent));
}

CDrain_Weapon * CDrain_Weapon::Create(_Device pGraphic_Device)
{
	CDrain_Weapon*	pInstance = new CDrain_Weapon(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating Prototype DrainWeapon");
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CDrain_Weapon::Clone_GameObject(void * pArg)
{
	CDrain_Weapon*	pInstance = new CDrain_Weapon(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Creating Clone DrainWeapon");
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDrain_Weapon::Free()
{
	m_pmatAttach = nullptr;
	m_pmatParent = nullptr;

	Safe_Release(m_pCollider);
	Safe_Release(m_pTransform);
	Safe_Release(m_pMesh_Dynamic);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pBattleAgent);

	CGameObject::Free();
}
