#include "stdafx.h"
#include "..\Headers\Weapon.h"
#include "Management.h"

CWeapon::CWeapon(_Device pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	ZeroMemory(m_tWeaponParam, sizeof(WPN_PARAM) * WPN_DATA_End);
}

CWeapon::CWeapon(const CWeapon & rhs)
	: CGameObject(rhs)
{
	ZeroMemory(m_tWeaponParam, sizeof(WPN_PARAM) * WPN_DATA_End);
}

HRESULT CWeapon::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CWeapon::Ready_GameObject(void * pArg)
{
	if (FAILED(SetUp_WeaponData()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	SetUp_Default();

	m_pTrailEffect = static_cast<Engine::CTrail_VFX*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SwordTrail", nullptr));
	m_pTrailEffect->Set_TrailIdx(0);

	m_pDistortionEffect = static_cast<Engine::CTrail_VFX*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SwordTrail", nullptr));
	m_pDistortionEffect->Set_TrailIdx(3);
	m_pDistortionEffect->Set_TrailType(Engine::CTrail_VFX::Trail_Distortion);

	m_pStaticTrailEffect = static_cast<Engine::CTrail_VFX*>(g_pManagement->Clone_GameObject_Return(L"GameObject_SwordTrail", nullptr));
	m_pStaticTrailEffect->Set_TrailIdx(1);

	return NOERROR;
}

_int CWeapon::Update_GameObject(_double TimeDelta)
{
	if (WEAPON_None == m_eWeaponType)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	Cacl_AttachBoneTransform();

	OnCollisionEnter();

	Update_Trails(TimeDelta);

	return NO_EVENT;
}

_int CWeapon::Late_Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pRenderer ||
		nullptr == m_pMesh_Static)
		return E_FAIL;

	if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return _int();
}

HRESULT CWeapon::Render_GameObject()
{
	if (nullptr == m_pShader ||
		nullptr == m_pMesh_Static)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();


	_uint iNumSubSet = (_uint)m_pMesh_Static->Get_NumMaterials();

	for (_uint i = 0; i < iNumSubSet; ++i)
	{
		m_pShader->Begin_Pass(m_iPass);

		if (FAILED(m_pShader->Set_Texture("g_DiffuseTexture", m_pMesh_Static->Get_Texture(i, MESHTEXTURE::TYPE_DIFFUSE))))
			return E_FAIL;

		if (FAILED(m_pShader->Set_Texture("g_NormalTexture", m_pMesh_Static->Get_Texture(i, MESHTEXTURE::TYPE_NORMAL))))
			return E_FAIL;

		//if (FAILED(m_pShader->Set_Texture("g_SpecularTexture", m_pMesh_Static->Get_Texture(i, MESHTEXTURE::TYPE_SPECULAR))))
		//	return E_FAIL;

		m_pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		m_pShader->End_Pass();
	}

	m_pShader->End_Shader();

	Draw_Collider();

	return NOERROR;
}

void CWeapon::OnCollisionEnter()
{
	Update_Collider();

	// =============================================================================================
	// 충돌
	// =============================================================================================

	if ( m_bPlayerFriendly)
	{
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE));
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Boss", SCENE_STAGE));
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_MonsterProjectile", SCENE_STAGE));
	}
	else
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_STAGE));


	// =============================================================================================
}

void CWeapon::OnCollisionEvent(list<CGameObject*> plistGameObject)
{
	// 공격 불가능이면 체크 안함
	if (false == m_tObjParam.bCanAttack)
		return;

	//cout << "uykiuyuh" << endl;

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

		// 내가 가진 Vec 콜라이더와 비교한다.
		for (auto& vecIter : m_vecAttackCol)
		{
			bFirst = true;

			// 피직콜라이더랑 비교
			for (auto& vecCol : iter->Get_PhysicColVector())
			{
				// 물체 전체를 대표할 콜라이더.
				if (vecIter->Check_Sphere(vecCol))
				{
					// 첫번째는 경계구 콜라이더니까 다음 콜라이더와 충돌처리 한다.
					if (bFirst)
					{
						bFirst = false;
						continue;
					}

					if (false == iter->Get_Target_Dodge())
					{
						iter->Set_Target_CanHit(false);

						if (iter->Get_Target_IsHit())
						{
							iter->Set_HitAgain(true);
						}

						if (false == iter->Get_Target_Dodge())
						{
							// 무기 공격력의 +-20%까지 랜덤범위
							_uint min = (_uint)(m_tObjParam.fDamage - (m_tObjParam.fDamage * 0.2f));
							_uint max = (_uint)(m_tObjParam.fDamage + (m_tObjParam.fDamage * 0.2f));

							iter->Add_Target_Hp((_float)-CALC::Random_Num(min , max) * m_fSkillPercent);
							g_pManagement->Create_Hit_Effect(vecIter, vecCol, TARGET_TO_TRANS(iter));

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
}

void CWeapon::Update_Trails(_double TimeDelta)
{
	// 트레일은 기본적으로 꺼져 있음
	if (false == m_bTrailEnable)
		return;

	_mat matWorld = m_pTransform->Get_WorldMat();
	_v3 vBegin, vDir;

	memcpy(vBegin, &m_pTransform->Get_WorldMat()._41, sizeof(_v3));
	memcpy(vDir, &m_pTransform->Get_WorldMat()._31, sizeof(_v3));

	if (m_pTrailEffect)
	{
		m_pTrailEffect->Set_ParentTransform(&matWorld);
		m_pTrailEffect->Ready_Info(vBegin + vDir * 0.5f, vBegin + vDir * 1.4f);
		m_pTrailEffect->Update_GameObject(TimeDelta);
	}

	if (m_pDistortionEffect)
	{
		m_pDistortionEffect->Set_ParentTransform(&matWorld);
		m_pDistortionEffect->Ready_Info(vBegin + vDir * 0.2f, vBegin + vDir * 1.5f);
		m_pDistortionEffect->Update_GameObject(TimeDelta);
	}

	if (m_pStaticTrailEffect)
	{
		m_pStaticTrailEffect->Set_ParentTransform(&matWorld);
		m_pStaticTrailEffect->Ready_Info(vBegin + vDir * 0.2f, vBegin + vDir * 1.6f);
		m_pStaticTrailEffect->Update_GameObject(TimeDelta);
	}
}

void CWeapon::Set_Enable_Trail(_bool _bEnable)
{
	m_bTrailEnable = _bEnable;

	if (_bEnable)
	{
		m_pStaticTrailEffect->Clear_TrailInfo();
		m_pTrailEffect->Clear_TrailInfo();
		m_pDistortionEffect->Clear_TrailInfo();
	}
}

void CWeapon::Set_Enable_Record(_bool _bRecord)
{
	m_bRecordCollision = _bRecord;

	if (false == m_bRecordCollision)
	{
		Clear_CollisionRecordList();
	}
}

void CWeapon::Set_SkillPercent(_float _fPercent)
{
	m_fSkillPercent = _fPercent;
}

void CWeapon::Change_WeaponMesh(const _tchar* _MeshName)
{
	// 이름 비교해서 같으면 Return
	if (!lstrcmp(m_szName, _MeshName))
		return;

	// 다르다면 이름 갱신하고,
	lstrcpy(m_szName, _MeshName);

	// 컴포넌트에 있는 매쉬 찾아서
	auto& iter = m_pmapComponents.find(L"Com_StaticMesh");

	// 둘 다 해제
	Safe_Release(m_pMesh_Static);
	Safe_Release(iter->second);

	// Release 한 컴포넌트에 새로이 Clone 받음.
	iter->second = m_pMesh_Static = static_cast<CMesh_Static*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _MeshName));
	Safe_AddRef(iter->second);

	// 콜라이더도 업데이트 해야함.
	_float fRadius = m_tWeaponParam[m_eWeaponData].fRadius;
	m_vecAttackCol[0]->Set_Radius(_v3(fRadius, fRadius, fRadius));
}

void CWeapon::Update_Collider()
{
	// 콜라이더 위치 갱신을 위한 함수
	// 추후 방식 변경 예정

	_mat matWorld = m_pTransform->Get_WorldMat();
	_v3 vBegin, vDir;

	memcpy(vBegin, &m_pTransform->Get_WorldMat()._41, sizeof(_v3));
	memcpy(vDir, &m_pTransform->Get_WorldMat()._31, sizeof(_v3));

	for (auto& iter : m_vecAttackCol)
	{
		iter->Update(vBegin + vDir * m_tWeaponParam[m_eWeaponData].fCol_Height);
	}
}

_bool CWeapon::Update_CollisionRecord(CGameObject* pObject)
{
	for (auto& iter : m_listCollisionRecord)
	{
		// 순회하다가 같으면
		if (iter == pObject)
			return false;
	}

	return true;
}

void CWeapon::Draw_Collider()
{
	// 콜라이더 그리기 위한 함수

	for (auto& iter : m_vecAttackCol)
	{
		g_pManagement->Gizmo_Draw_Sphere(iter->Get_CenterPos(), iter->Get_Radius().x);
	}
}

void CWeapon::Change_WeaponData(WEAPON_DATA _eWpnData)
{
	if (_eWpnData == m_eWeaponData)
		return;

	m_eWeaponData = _eWpnData;

	_tchar WeaponMeshName[MAX_STR] = L"";

	switch (_eWpnData)
	{
	case WPN_SSword_Normal:
	{
		lstrcpy(WeaponMeshName, L"Mesh_Sword");
		m_eWeaponType = WEAPON_Ssword;
		break;
	}
	case WPN_LSword_Normal:
	{
		lstrcpy(WeaponMeshName, L"Mesh_Sword");
		m_eWeaponType = WEAPON_Ssword;
		break;
	}
	case WPN_Hammer_Normal:
	{
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Hammer");
		m_eWeaponType = WEAPON_Hammer;
		break;
	}
	case WPN_Halverd_Normal:
	{
		lstrcpy(WeaponMeshName, L"Mesh_Sword");
		m_eWeaponType = WEAPON_Ssword;
		break;
	}
	case WPN_Gun_Normal:
	{
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Gun");
		m_eWeaponType = WEAPON_Gun;
		break;
	}
	}

	Change_WeaponMesh(WeaponMeshName);
}

void CWeapon::Clear_CollisionRecordList()
{
	for (auto& iter : m_listCollisionRecord)
	{
		iter = nullptr;
	}
}

HRESULT CWeapon::Add_Component()
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Wpn_Sword", L"Com_StaticMesh", (CComponent**)&m_pMesh_Static)))
		return E_FAIL;

	// 최초 무기 이름
	lstrcpy(m_szName, L"Mesh_Wpn_Sword");


	//==============================================================================================================
	m_vecAttackCol.reserve(16);
	//==============================================================================================================
	// 콜라이더 받아오고
	CCollider* pCollider = static_cast<CCollider*>(g_pManagement->Clone_Component(SCENE_STATIC, L"Collider"));

	_float fRadius = m_tWeaponParam[m_eWeaponData].fRadius;

	// 첫번째 콜라이더는 경계 체크용 콜라이더
	pCollider->Set_Radius(_v3{ fRadius, fRadius, fRadius });		// 반지름 설정, 원은 세 값이 모두 같아야하며, 사각형은 달라도됨.
	pCollider->Set_Dynamic(true);						// 동적인 콜라이더 인지
	pCollider->Set_Type(COL_SPHERE);					// 콜라이더 타입
	pCollider->Set_CenterPos(m_pTransform->Get_Pos() + _v3{ 0.f ,fRadius , 0.f });	// 콜라이더 중앙 위치 설정
	pCollider->Set_Enabled(false);						// 콜라이더 활성화 유무

														// 첫번째 들어가는 콜라이더는 경계 체크용 콜라이더
														// 하나만 있을 경우, 경계 체크용으로 쓰지 않고 실질적으로 충돌함
	m_vecAttackCol.push_back(pCollider);
	//==============================================================================================================

	return NOERROR;
}

HRESULT CWeapon::SetUp_Default()
{
	// Transform
	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);
	m_pTransform->Set_Angle(AXIS_X, D3DXToRadian(-90.f));
	return S_OK;
}

HRESULT CWeapon::SetUp_WeaponData()
{
	m_tWeaponParam[WPN_SSword_Normal].fDamage = 20.f;
	m_tWeaponParam[WPN_SSword_Normal].fRadius = 0.7f;
	m_tWeaponParam[WPN_SSword_Normal].fTrail_Min = 0.f;
	m_tWeaponParam[WPN_SSword_Normal].fTrail_Max = 1.f;
	m_tWeaponParam[WPN_SSword_Normal].fCol_Height = 1.f;

	m_tWeaponParam[WPN_Hammer_Normal].fDamage = 50.f;
	m_tWeaponParam[WPN_Hammer_Normal].fRadius = 0.75f;
	m_tWeaponParam[WPN_Hammer_Normal].fTrail_Min = 0.75f;
	m_tWeaponParam[WPN_Hammer_Normal].fTrail_Max = 1.5f;
	m_tWeaponParam[WPN_Hammer_Normal].fCol_Height = 1.3f;

	m_tWeaponParam[WPN_Gun_Normal].fDamage = 25.f;
	m_tWeaponParam[WPN_Gun_Normal].fRadius = 0.5f;
	m_tWeaponParam[WPN_Gun_Normal].fTrail_Min = 0.f;
	m_tWeaponParam[WPN_Gun_Normal].fTrail_Max = 1.f;
	m_tWeaponParam[WPN_Gun_Normal].fCol_Height = 1.2f;

	return S_OK;
}

HRESULT CWeapon::SetUp_ConstantTable()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	_mat		ViewMatrix = g_pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = g_pManagement->Get_Transform(D3DTS_PROJECTION);

	if (FAILED(m_pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShader)))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	return NOERROR;
}

void CWeapon::Cacl_AttachBoneTransform()
{
	m_pTransform->Calc_ParentMat(&(*m_pmatAttach * *m_pmatParent));
}

CWeapon * CWeapon::Create(_Device pGraphic_Device)
{
	CWeapon*	pInstance = new CWeapon(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CWeapon::Clone_GameObject(void * pArg)
{
	CWeapon*	pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	m_pmatAttach = nullptr;
	m_pmatParent = nullptr;

	Safe_Release(m_pTrailEffect);
	Safe_Release(m_pStaticTrailEffect);
	Safe_Release(m_pDistortionEffect);

	Safe_Release(m_pTransform);
	Safe_Release(m_pMesh_Static);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);

	for (auto& iter : m_vecAttackCol)
		Safe_Release(iter);

	m_vecAttackCol.shrink_to_fit();
	m_vecAttackCol.clear();

	for (auto& iter : m_listCollisionRecord)
		iter = nullptr;

	CGameObject::Free();
}
