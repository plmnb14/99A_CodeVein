#include "stdafx.h"
#include "..\Headers\Weapon.h"
#include "Management.h"
#include "CameraMgr.h"

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

WPN_PARAM CWeapon::Get_WeaponParam(WEAPON_DATA _eData)
{
	return m_tWeaponParam[_eData];
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

	m_pTrailEffect = g_pManagement->Create_Trail();
	m_pTrailEffect->Set_TrailIdx(0);
	
	m_pDistortionEffect = g_pManagement->Create_Trail();
	m_pDistortionEffect->Set_TrailIdx(3);
	m_pDistortionEffect->Set_TrailType(Engine::CTrail_VFX::Trail_Distortion);

	m_pStaticTrailEffect = g_pManagement->Create_Trail();
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

	_v3 vPos;
	memcpy(vPos, &m_pTransform->Get_WorldMat()._41, sizeof(_v3));

	if (m_pOptimization->Check_InFrustumforObject(&vPos, 2.f))
	{
		if (false == m_tObjParam.bInvisible)
		{
			if (!m_bDissolve)
			{
				if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pRenderer->Add_RenderList(RENDER_ALPHA, this)))
					return E_FAIL;
			}

			if (FAILED(m_pRenderer->Add_RenderList(RENDER_MOTIONBLURTARGET, this)))
				return E_FAIL;

			if (FAILED(m_pRenderer->Add_RenderList(RENDER_SHADOWTARGET, this)))
				return E_FAIL;
		}
	}

	return _int();
}

HRESULT CWeapon::Render_GameObject()
{
	if (nullptr == m_pShader ||
		nullptr == m_pMesh_Static)
		return E_FAIL;

	_mat matveiwView = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	_mat matPro = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);

	m_pShader->Set_Value("g_matView", &matveiwView, sizeof(_mat));
	m_pShader->Set_Value("g_matProj", &matPro, sizeof(_mat));

	if (FAILED(g_pDissolveTexture->SetUp_OnShader("g_FXTexture", m_pShader)))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(m_pShader)))
		return E_FAIL;

	m_pShader->Begin_Shader();

	_uint iNumSubSet = (_uint)m_pMesh_Static->Get_NumMaterials();

	for (_uint i = 0; i < iNumSubSet; ++i)
	{
		m_iPass = m_pMesh_Static->Get_MaterialPass(i);

		if (m_bDissolve)
			m_iPass = 3;

		m_pShader->Begin_Pass(m_iPass);

		m_pShader->Set_StaticTexture_Auto(m_pMesh_Static, i);

		m_pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		m_pShader->End_Pass();
	}

	m_pShader->End_Shader();

	Draw_Collider();

	return NOERROR;
}

HRESULT CWeapon::Render_GameObject_Instancing_SetPass(CShader * pShader)
{
	if (nullptr == pShader ||
		nullptr == m_pMesh_Static)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable(pShader)))
		return E_FAIL;

	_uint iNumSubSet = (_uint)m_pMesh_Static->Get_NumMaterials();

	for (_uint i = 0; i < iNumSubSet; ++i)
	{
		m_iPass = m_pMesh_Static->Get_MaterialPass(i);

		if (m_bDissolve)
			m_iPass = 3;

		pShader->Begin_Pass(m_iPass);

		pShader->Set_StaticTexture_Auto(m_pMesh_Static, i);

		pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		pShader->End_Pass();
	}

	//Draw_Collider();

	return NOERROR;
}

HRESULT CWeapon::Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur)
{
	if (nullptr == pShader ||
		nullptr == m_pMesh_Static)
		return E_FAIL;

	//============================================================================================
	// 공통 변수
	//============================================================================================

	_mat	ViewMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
	_mat	ProjMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);
	_mat	WorldMatrix = m_pTransform->Get_WorldMat();

	if (FAILED(pShader->Set_Value("g_matWorld", &WorldMatrix, sizeof(_mat))))
		return E_FAIL;

	//============================================================================================
	// 모션 블러 상수
	//============================================================================================
	if (_bIsForMotionBlur)
	{
		if (FAILED(pShader->Set_Value("g_matLastWVP", &m_matLastWVP, sizeof(_mat))))
			return E_FAIL;

		m_matLastWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		_bool bMotionBlur = true;
		if (FAILED(pShader->Set_Bool("g_bMotionBlur", bMotionBlur)))
			return E_FAIL;
		_bool bDecalTarget = false;
		if (FAILED(pShader->Set_Bool("g_bDecalTarget", bDecalTarget)))
			return E_FAIL;
		_float fBloomPower = 0.f;
		if (FAILED(pShader->Set_Value("g_fBloomPower", &fBloomPower, sizeof(_float))))
			return E_FAIL;
	}

	//============================================================================================
	// 기타 상수
	//============================================================================================
	else
	{
		_mat matWVP = WorldMatrix * ViewMatrix * ProjMatrix;

		if (FAILED(pShader->Set_Value("g_matWVP", &matWVP, sizeof(_mat))))
			return E_FAIL;
	}

	//============================================================================================
	// 쉐이더 시작
	//============================================================================================

	_ulong dwNumSubSet = m_pMesh_Static->Get_NumMaterials();

	for (_ulong i = 0; i < dwNumSubSet; ++i)
	{
		pShader->Begin_Pass(iPass);

		pShader->Commit_Changes();

		m_pMesh_Static->Render_Mesh(i);

		pShader->End_Pass();
	}

	//============================================================================================


	return NOERROR;
}

void CWeapon::OnCollisionEnter()
{
	if (false == m_tObjParam.bCanAttack)
		return;

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
	{
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL), true);
		OnCollisionEvent(g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE), true);
	}
		


	// =============================================================================================
}

void CWeapon::OnCollisionEvent(list<CGameObject*> plistGameObject, _bool _bIsPlayer)
{
	// 공격 불가능이면 체크 안함
	if (false == m_tObjParam.bCanAttack)
		return;

	_v3 vHitDir = V3_NULL;
	_bool bFirst = true;

	//게임 오브젝트를 받아와서
	for (auto& iter : plistGameObject)
	{
		if(false == iter->Get_Enable())
			continue;

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

					// 충돌하는 대상이 카운터중이면,
					if (nullptr != m_pTarget && true == iter->Get_Target_IsCounter())
					{
						// 카운터하는 대상의 전방에 내가 포함되나 본다.
						if (m_pBattleAgent->Check_TargetIsFrontOfMe(TARGET_TO_TRANS(iter), TARGET_TO_TRANS(m_pTarget)))
						{
							// 시간정지 & 화면흔들림
							g_pTimer_Manager->Set_MutiplyTime(L"Timer_Fps_60", 0.025f);
							g_pTimer_Manager->Set_MutiplyResetTime(L"Timer_Fps_60", 0.15f);
							CCameraMgr::Get_Instance()->MainCamera_Oscillatation_SetUp(2.f, 20.f, 0.5f, 0.9f, CCamera::CAM_OSC_TYPE::POS_OSC);
							//SHAKE_CAM_lv3;

							_v3 tmpPos = TARGET_TO_TRANS(iter)->Get_Pos() + TARGET_TO_TRANS(iter)->Get_Axis(AXIS_Z) + _v3(0.f, 2.f, 0.f);

							Create_PointLight(tmpPos);

							if (_bIsPlayer)
							{
								// 처형되게 함.
								iter->Set_Target_CanExicution(false);
								iter->Set_CounterTarget(m_pTarget);
							}

							// 포함되면, 나의 주인은 카운터 당한 상태이다.
							m_pTarget->Set_Target_CanRepel(false);
							// 무기 또한 공격불가
							m_tObjParam.bCanAttack = false;
						}
					}

					else if (false == iter->Get_Target_IsDodge())
					{
						iter->Set_Target_CanHit(false);

						if (iter->Get_Target_IsHit())
						{
							iter->Set_HitAgain(true);
						}

						if (false == iter->Get_Target_IsDodge())
						{
							CTransform* pIterTransform = TARGET_TO_TRANS(iter);
						
							// 무기의 공격력 받아옴
							m_tObjParam.fDamage = m_tWeaponParam[m_eWeaponData].fDamage;
						
							// 무기 공격력의 +-20%까지 랜덤범위
							//_uint min = (_uint)(m_tObjParam.fDamage - (m_tObjParam.fDamage * 0.2f) + m_tWeaponParam[m_eWeaponData].fPlusDamage);
							//_uint max = (_uint)(m_tObjParam.fDamage + (m_tObjParam.fDamage * 0.2f) + m_tWeaponParam[m_eWeaponData].fPlusDamage);
						
							//피격시 밀림처리..... ( 무기니까 부모의 위치값 )
							memcpy(vHitDir, &(m_pmatParent->_41), sizeof(_v3));
						
							// 때린놈의 위치 값
							iter->Set_Target_HitPos(vHitDir);
						
							// 때린놈으로 부터의 방향벡터
							V3_NORMAL(&m_tObjParam.vHitDir, &(pIterTransform->Get_Pos() - vHitDir));
							iter->Set_Target_HitDir(m_tObjParam.vHitDir);
						
							// 시간정지 & 화면흔들림
							g_pTimer_Manager->Set_MutiplyTime(L"Timer_Fps_60", 0.025f);
							g_pTimer_Manager->Set_MutiplyResetTime(L"Timer_Fps_60", 0.1f);
							CCameraMgr::Get_Instance()->MainCamera_Oscillatation_SetUp(2.f, 20.f, 0.5f, 0.6f, CCamera::CAM_OSC_TYPE::POS_OSC);
						
							// HP 감소
							//iter->Add_Target_Hp(-(_float)CALC::Random_Num(min , max) * m_fSkillPercent);
							
							// 플레이어의 공격
							if(m_bPlayerFriendly)
								iter->Hit_Target((m_tObjParam.fDamage + m_tWeaponParam[m_eWeaponData].fPlusDamage) * m_fSkillPercent);
							// 몬스터의 공격
							else
								iter->Hit_Target((m_tObjParam.fDamage + m_tWeaponParam[m_eWeaponData].fPlusDamage) * m_fSkillPercent * pow(1.5f, g_eStageIdx_Cur - 1));


							g_pManagement->Create_Hit_Effect(vecIter, vecCol, pIterTransform);
						
							_int iRandNum = CCalculater::Random_Num(0, 3);

							switch (iRandNum)
							{
							case 0:
							{
								g_pSoundManager->Stop_Sound(CSoundManager::Hit_SFX_01);
								g_pSoundManager->Play_Sound(L"IMPT_Hit_Short_01.wav", CSoundManager::Hit_SFX_01, CSoundManager::Effect_Sound);

								break;
							}
							case 1:
							{
								g_pSoundManager->Stop_Sound(CSoundManager::Hit_SFX_01);
								g_pSoundManager->Play_Sound(L"Slash_Sword_02_Short.wav", CSoundManager::Hit_SFX_01, CSoundManager::Effect_Sound);

								break;
							}
							case 2:
							{
								g_pSoundManager->Stop_Sound(CSoundManager::Hit_SFX_01);
								g_pSoundManager->Play_Sound(L"Slash_Sword_03.wav", CSoundManager::Hit_SFX_01, CSoundManager::Effect_Sound);

								break;
							}

							case 3:
							{
								g_pSoundManager->Stop_Sound(CSoundManager::Hit_SFX_01);
								g_pSoundManager->Play_Sound(L"Slash_Sword_04.wav", CSoundManager::Hit_SFX_01, CSoundManager::Effect_Sound);


								break;
							}
							}
							
							iRandNum = CCalculater::Random_Num(0, 3);

							switch (iRandNum)
							{
							case 0:
							{
								g_pSoundManager->Stop_Sound(CSoundManager::Hit_SFX_02);
								g_pSoundManager->Play_Sound(L"GORE_Gun_Gore_01.wav", CSoundManager::Hit_SFX_02, CSoundManager::Effect_Sound);

								break;
							}
							case 1:
							{
								g_pSoundManager->Stop_Sound(CSoundManager::Hit_SFX_02);
								g_pSoundManager->Play_Sound(L"GORE_Gun_Gore_02.wav", CSoundManager::Hit_SFX_02, CSoundManager::Effect_Sound);

								break;
							}
							case 2:
							{
								g_pSoundManager->Stop_Sound(CSoundManager::Hit_SFX_02);
								g_pSoundManager->Play_Sound(L"GORE_Gun_Gore_03.wav", CSoundManager::Hit_SFX_02, CSoundManager::Effect_Sound);

								break;
							}
							}

							iRandNum = CCalculater::Random_Num(0, 3);

							switch (iRandNum)
							{
							case 0:
							{
								g_pSoundManager->Stop_Sound(CSoundManager::Hit_SFX_03);
								g_pSoundManager->Play_Sound(L"SE_METAL_CLASH_018.wav", CSoundManager::Hit_SFX_03, CSoundManager::Effect_Sound);

								break;
							}
							case 1:
							{
								g_pSoundManager->Stop_Sound(CSoundManager::Hit_SFX_03);
								g_pSoundManager->Play_Sound(L"SE_METAL_CLASH_022.wav", CSoundManager::Hit_SFX_03, CSoundManager::Effect_Sound);

								break;
							}
							case 2:
							{
								g_pSoundManager->Stop_Sound(CSoundManager::Hit_SFX_03);
								g_pSoundManager->Play_Sound(L"SE_METAL_CLASH_024.wav", CSoundManager::Hit_SFX_03, CSoundManager::Effect_Sound);

								break;
							}

							case 3:
							{
								g_pSoundManager->Stop_Sound(CSoundManager::Hit_SFX_03);
								g_pSoundManager->Play_Sound(L"SE_METAL_CLASH_028.wav", CSoundManager::Hit_SFX_03, CSoundManager::Effect_Sound);


								break;
							}
							}

							Create_PointLight(vecIter->Get_CenterPos());
						
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
	_float fBeginValue, fEndValue;

	fBeginValue = m_tWeaponParam[m_eWeaponData].fTrail_Min;
	fEndValue = m_tWeaponParam[m_eWeaponData].fTrail_Max;

	memcpy(vBegin, &m_pTransform->Get_WorldMat()._41, sizeof(_v3));
	memcpy(vDir, &m_pTransform->Get_WorldMat()._31, sizeof(_v3));

	if (m_pTrailEffect)
	{
		m_pTrailEffect->Set_ParentTransform(&matWorld);
		m_pTrailEffect->Ready_Info(vBegin + vDir * fBeginValue, vBegin + vDir * fEndValue);
		m_pTrailEffect->Update_GameObject(TimeDelta);
	}

	if (m_pDistortionEffect && !m_bSingleTrail)
	{
		m_pDistortionEffect->Set_ParentTransform(&matWorld);
		m_pDistortionEffect->Ready_Info(vBegin + vDir * fBeginValue, vBegin + vDir * fEndValue);
		m_pDistortionEffect->Update_GameObject(TimeDelta);
	}

	if (m_pStaticTrailEffect)
	{
		m_pStaticTrailEffect->Set_ParentTransform(&matWorld);
		m_pStaticTrailEffect->Ready_Info(vBegin + vDir * fBeginValue, vBegin + vDir * fEndValue);
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

void CWeapon::Set_SkillMode(_bool _bSkill)
{
	m_bSingleTrail = _bSkill;

	if (!m_pTrailEffect)
		return;

	if (_bSkill)
		m_pTrailEffect->Set_TrailIdx(6);
	else
		m_pTrailEffect->Set_TrailIdx(0);
}

void CWeapon::Set_TrailIndex(_int iIdx, _bool bStaticTrail)
{
	//m_bSingleTrail = true;

	if (!m_pTrailEffect || !m_pStaticTrailEffect)
		return;

	if(!bStaticTrail)
		m_pTrailEffect->Set_TrailIdx(iIdx);
	else
		m_pStaticTrailEffect->Set_TrailIdx(iIdx);
}

void CWeapon::Set_TrailUseMask(_bool bUse, _int iIdx, _bool bStaticTrail)
{
	if (!m_pTrailEffect || !m_pStaticTrailEffect)
		return;

	if (!bStaticTrail)
		m_pTrailEffect->Set_UseMask(bUse, iIdx);
	else
		m_pStaticTrailEffect->Set_UseMask(bUse, iIdx);
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

	// Static 멤버변수는 처음에 Clone 할때 AddRef 해준다., 
	Safe_Release(m_pMesh_Static);
	Safe_Release(iter->second);

	// Release 한 컴포넌트에 새로이 Clone 받음.
	iter->second = m_pMesh_Static = static_cast<CMesh_Static*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, _MeshName));
	Safe_AddRef(iter->second);

	// 콜라이더도 업데이트 해야함.
	_float fRadius = m_tWeaponParam[m_eWeaponData].fRadius;
	m_vecAttackCol[0]->Set_Radius(_v3(fRadius, fRadius, fRadius));

	// 공격력도 업데이트
	m_tObjParam.fDamage = m_tWeaponParam->fDamage * 20.f;
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

	_tchar WeaponMeshName[STR_128] = L"";

	switch (_eWpnData)
	{
	case WEAPON_DATA::Wpn_SSword:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_SSword");
		m_eWeaponType = WEAPON_SSword;
		break;
	case WEAPON_DATA::Wpn_SSword_Black:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_SSword_Black");
		m_eWeaponType = WEAPON_SSword;
		break;
	case WEAPON_DATA::Wpn_SSword_Military:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_SSword_Military");
		m_eWeaponType = WEAPON_SSword;
		break;
	case WEAPON_DATA::Wpn_SSword_Slave:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_SSword_Slave");
		m_eWeaponType = WEAPON_SSword;
		break;

	case WEAPON_DATA::Wpn_LSword:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_LSword");
		m_eWeaponType = WEAPON_LSword;
		break;
	case WEAPON_DATA::Wpn_LSword_Black:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_LSword_Black");
		m_eWeaponType = WEAPON_LSword;
		break;
	case WEAPON_DATA::Wpn_LSword_Military:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_LSword_Military");
		m_eWeaponType = WEAPON_LSword;
		break;
	case WEAPON_DATA::Wpn_LSword_Slave:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_LSword_Slave");
		m_eWeaponType = WEAPON_LSword;
		break;

	case WEAPON_DATA::Wpn_Halberd:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Halberd");
		m_eWeaponType = WEAPON_Halberd;
		break;
	case WEAPON_DATA::Wpn_Halberd_Black:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Halberd_Black");
		m_eWeaponType = WEAPON_Halberd;
		break;
	case WEAPON_DATA::Wpn_Halberd_Military:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Halberd_Military");
		m_eWeaponType = WEAPON_Halberd;
		break;
	case WEAPON_DATA::Wpn_Halberd_Slave:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Halberd_Slave");
		m_eWeaponType = WEAPON_Halberd;
		break;

	case WEAPON_DATA::Wpn_Hammer:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Hammer");
		m_eWeaponType = WEAPON_Hammer;
		break;
	case WEAPON_DATA::Wpn_Hammer_Black:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Hammer_Black");
		m_eWeaponType = WEAPON_Hammer;
		break;
	case WEAPON_DATA::Wpn_Hammer_Military:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Hammer_Military");
		m_eWeaponType = WEAPON_Hammer;
		break;
	case WEAPON_DATA::Wpn_Hammer_Slave:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Hammer_Slave");
		m_eWeaponType = WEAPON_Hammer;
		break;

	case WEAPON_DATA::Wpn_Gun:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Gun");
		m_eWeaponType = WEAPON_Gun;
		break;
	case WEAPON_DATA::Wpn_Gun_Black:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Gun_Black");
		m_eWeaponType = WEAPON_Gun;
		break;
	case WEAPON_DATA::Wpn_Gun_Military:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Gun_Military");
		m_eWeaponType = WEAPON_Gun;
		break;
	case WEAPON_DATA::Wpn_Gun_Slave:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Gun_Slave");
		m_eWeaponType = WEAPON_Gun;
		break;

	case WEAPON_DATA::WPN_Shield_Normal:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_Shield");
		m_eWeaponType = WEAPON_Shield;
		break;	
	case WEAPON_DATA::WPN_DeerKingShield:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_DeerKingShield");
		m_eWeaponType = WEAPON_Shield;
		break;
	case WEAPON_DATA::WPN_QueenShield:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_QueenShield");
		m_eWeaponType = WEAPON_Shield;
		break;

	case WEAPON_DATA::WPN_QueenLance:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_QueenLance");
		m_eWeaponType = WEAPON_Halberd;
		break;
	case WEAPON_DATA::WPN_FrostBlood_IceGirl:
		lstrcpy(WeaponMeshName, L"Mesh_Wpn_FrostBlood_IceGirl");
		m_eWeaponType = WEAPON_SSword;
		break;

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

_v3 CWeapon::Get_HeadPos()
{
	_mat matWorld = m_pTransform->Get_Info().matWorld;
	_v3 vPos = _v3(matWorld._41, matWorld._42, matWorld._43);
	_v3 vLook = _v3(matWorld._31, matWorld._32, matWorld._33);

	m_vHeadPos = vPos + vLook * (0.4f + m_tWeaponParam[m_eWeaponData].fCol_Height);

	return m_vHeadPos;
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Mesh_Wpn_SSword", L"Com_StaticMesh", (CComponent**)&m_pMesh_Static)))
		return E_FAIL;

	// 최초 무기 이름
	lstrcpy(m_szName, L"Mesh_Wpn_SSword");

	// for.Com_Optimaization
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Optimization", L"Com_Optimization", (CComponent**)&m_pOptimization)))
		return E_FAIL;

	// for.Com_BattleAgent
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"BattleAgent", L"Com_BattleAgent", (CComponent**)&m_pBattleAgent)))
		return E_FAIL;


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

	//
	m_tObjParam.fDamage = m_tWeaponParam->fDamage;

	return S_OK;
}

HRESULT CWeapon::SetUp_WeaponData()
{
	//===========================================================================================
	// 한손검
	//===========================================================================================

	m_tWeaponParam[Wpn_SSword].iWeaponName = Wpn_SSword;
	m_tWeaponParam[Wpn_SSword].iWeaponName_InShop = WpnAll_SSword_Red;
	m_tWeaponParam[Wpn_SSword].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[Wpn_SSword].iPrice = 600;
	m_tWeaponParam[Wpn_SSword].iReinforce = 0;
	m_tWeaponParam[Wpn_SSword].fDamage = 350.f;
	m_tWeaponParam[Wpn_SSword].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_SSword].fRadius = 0.6f;
	m_tWeaponParam[Wpn_SSword].fTrail_Min = 0.6f;
	m_tWeaponParam[Wpn_SSword].fTrail_Max = 1.8f;
	m_tWeaponParam[Wpn_SSword].fCol_Height = 1.1f;

	m_tWeaponParam[Wpn_SSword_Black].iWeaponName = Wpn_SSword_Black;
	m_tWeaponParam[Wpn_SSword_Black].iWeaponName_InShop = WpnAll_SSword_Executioner;
	m_tWeaponParam[Wpn_SSword_Black].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[Wpn_SSword_Black].iPrice = 700;
	m_tWeaponParam[Wpn_SSword_Black].iReinforce = 0;
	m_tWeaponParam[Wpn_SSword_Black].fDamage = 380.f;
	m_tWeaponParam[Wpn_SSword_Black].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_SSword_Black].fRadius = 0.9f;
	m_tWeaponParam[Wpn_SSword_Black].fTrail_Min = 0.6f;
	m_tWeaponParam[Wpn_SSword_Black].fTrail_Max = 1.8f;
	m_tWeaponParam[Wpn_SSword_Black].fCol_Height = 0.6f;

	m_tWeaponParam[Wpn_SSword_Military].iWeaponName = Wpn_SSword_Military;
	m_tWeaponParam[Wpn_SSword_Military].iWeaponName_InShop = WpnAll_SSword_Nakil;
	m_tWeaponParam[Wpn_SSword_Military].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[Wpn_SSword_Military].iPrice = 800;
	m_tWeaponParam[Wpn_SSword_Military].iReinforce = 0;
	m_tWeaponParam[Wpn_SSword_Military].fDamage = 410.f;
	m_tWeaponParam[Wpn_SSword_Military].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_SSword_Military].fRadius = 0.6f;
	m_tWeaponParam[Wpn_SSword_Military].fTrail_Min = 0.6f;
	m_tWeaponParam[Wpn_SSword_Military].fTrail_Max = 1.8f;
	m_tWeaponParam[Wpn_SSword_Military].fCol_Height = 1.1f;

	m_tWeaponParam[Wpn_SSword_Slave].iWeaponName = Wpn_SSword_Slave;
	m_tWeaponParam[Wpn_SSword_Slave].iWeaponName_InShop = WpnAll_SSword_DarkBrodeSword;
	m_tWeaponParam[Wpn_SSword_Slave].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[Wpn_SSword_Slave].iPrice = 950;
	m_tWeaponParam[Wpn_SSword_Slave].iReinforce = 0;
	m_tWeaponParam[Wpn_SSword_Slave].fDamage = 450.f;
	m_tWeaponParam[Wpn_SSword_Slave].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_SSword_Slave].fRadius = 0.7f;
	m_tWeaponParam[Wpn_SSword_Slave].fTrail_Min = 0.6f;
	m_tWeaponParam[Wpn_SSword_Slave].fTrail_Max = 1.8f;
	m_tWeaponParam[Wpn_SSword_Slave].fCol_Height = 1.2f;

	//===========================================================================================
	// 대검
	//===========================================================================================

	m_tWeaponParam[Wpn_LSword].iWeaponName = Wpn_LSword;
	m_tWeaponParam[Wpn_LSword].iWeaponName_InShop = WpnAll_LSword_Tsubai;
	m_tWeaponParam[Wpn_LSword].iWeaponType = WEAPON_LSword;
	m_tWeaponParam[Wpn_LSword].iPrice = 1500;
	m_tWeaponParam[Wpn_LSword].iReinforce = 0;
	m_tWeaponParam[Wpn_LSword].fDamage = 650.f;
	m_tWeaponParam[Wpn_LSword].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_LSword].fRadius = 0.9f;
	m_tWeaponParam[Wpn_LSword].fTrail_Min = 0.8f;
	m_tWeaponParam[Wpn_LSword].fTrail_Max = 2.1f;
	m_tWeaponParam[Wpn_LSword].fCol_Height = 1.4f;

	m_tWeaponParam[Wpn_LSword_Black].iWeaponName = Wpn_LSword_Black;
	m_tWeaponParam[Wpn_LSword_Black].iWeaponName_InShop = WpnAll_LSword_Black;
	m_tWeaponParam[Wpn_LSword_Black].iWeaponType = WEAPON_LSword;
	m_tWeaponParam[Wpn_LSword_Black].iPrice = 1200;
	m_tWeaponParam[Wpn_LSword_Black].iReinforce = 0;
	m_tWeaponParam[Wpn_LSword_Black].fDamage = 680.f;
	m_tWeaponParam[Wpn_LSword_Black].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_LSword_Black].fRadius = 0.8f;
	m_tWeaponParam[Wpn_LSword_Black].fTrail_Min = 0.8f;
	m_tWeaponParam[Wpn_LSword_Black].fTrail_Max = 2.1f;
	m_tWeaponParam[Wpn_LSword_Black].fCol_Height = 1.3f;

	m_tWeaponParam[Wpn_LSword_Military].iWeaponName = Wpn_LSword_Military;
	m_tWeaponParam[Wpn_LSword_Military].iWeaponName_InShop = WpnAll_LSword_Nakil;
	m_tWeaponParam[Wpn_LSword_Military].iWeaponType = WEAPON_LSword;
	m_tWeaponParam[Wpn_LSword_Military].iPrice = 1200;
	m_tWeaponParam[Wpn_LSword_Military].iReinforce = 0;
	m_tWeaponParam[Wpn_LSword_Military].fDamage = 710.f;
	m_tWeaponParam[Wpn_LSword_Military].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_LSword_Military].fRadius = 0.8f;
	m_tWeaponParam[Wpn_LSword_Military].fTrail_Min = 0.8f;
	m_tWeaponParam[Wpn_LSword_Military].fTrail_Max = 2.1f;
	m_tWeaponParam[Wpn_LSword_Military].fCol_Height = 1.5f;

	m_tWeaponParam[Wpn_LSword_Slave].iWeaponName = Wpn_LSword_Slave;
	m_tWeaponParam[Wpn_LSword_Slave].iWeaponName_InShop = WpnAll_LSword_DarkTsubai;
	m_tWeaponParam[Wpn_LSword_Slave].iWeaponType = WEAPON_LSword;
	m_tWeaponParam[Wpn_LSword_Slave].iPrice = 1300;
	m_tWeaponParam[Wpn_LSword_Slave].iReinforce = 0;
	m_tWeaponParam[Wpn_LSword_Slave].fDamage = 750.f;
	m_tWeaponParam[Wpn_LSword_Slave].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_LSword_Slave].fRadius = 0.8f;
	m_tWeaponParam[Wpn_LSword_Slave].fTrail_Min = 0.8f;
	m_tWeaponParam[Wpn_LSword_Slave].fTrail_Max = 2.1f;
	m_tWeaponParam[Wpn_LSword_Slave].fCol_Height = 1.5f;

	//===========================================================================================
	// 도끼창
	//===========================================================================================

	m_tWeaponParam[Wpn_Halberd].iWeaponName = Wpn_Halberd;
	m_tWeaponParam[Wpn_Halberd].iWeaponName_InShop = WpnAll_Halberd_SolidPiercer;
	m_tWeaponParam[Wpn_Halberd].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Halberd].iPrice = 1100;
	m_tWeaponParam[Wpn_Halberd].iReinforce = 0;
	m_tWeaponParam[Wpn_Halberd].fDamage = 650.f;
	m_tWeaponParam[Wpn_Halberd].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_Halberd].fRadius = 0.7f;
	m_tWeaponParam[Wpn_Halberd].fTrail_Min = 1.2f;
	m_tWeaponParam[Wpn_Halberd].fTrail_Max = 2.3f;
	m_tWeaponParam[Wpn_Halberd].fCol_Height = 1.6f;

	m_tWeaponParam[Wpn_Halberd_Black].iWeaponName = Wpn_Halberd_Black;
	m_tWeaponParam[Wpn_Halberd_Black].iWeaponName_InShop = WpnAll_Halberd_Black;
	m_tWeaponParam[Wpn_Halberd_Black].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Halberd_Black].iPrice = 1150;
	m_tWeaponParam[Wpn_Halberd_Black].iReinforce = 0;
	m_tWeaponParam[Wpn_Halberd_Black].fDamage = 680.f;
	m_tWeaponParam[Wpn_Halberd_Black].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_Halberd_Black].fRadius = 0.7f;
	m_tWeaponParam[Wpn_Halberd_Black].fTrail_Min = 1.2f;
	m_tWeaponParam[Wpn_Halberd_Black].fTrail_Max = 2.3f;
	m_tWeaponParam[Wpn_Halberd_Black].fCol_Height = 1.4f;

	m_tWeaponParam[Wpn_Halberd_Military].iWeaponName = Wpn_Halberd_Military;
	m_tWeaponParam[Wpn_Halberd_Military].iWeaponName_InShop = WpnAll_Halberd_Nakil;
	m_tWeaponParam[Wpn_Halberd_Military].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Halberd_Military].iPrice = 1200;
	m_tWeaponParam[Wpn_Halberd_Military].iReinforce = 0;
	m_tWeaponParam[Wpn_Halberd_Military].fDamage = 710.f;
	m_tWeaponParam[Wpn_Halberd_Military].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_Halberd_Military].fRadius = 0.7f;
	m_tWeaponParam[Wpn_Halberd_Military].fTrail_Min = 1.2f;
	m_tWeaponParam[Wpn_Halberd_Military].fTrail_Max = 2.3f;
	m_tWeaponParam[Wpn_Halberd_Military].fCol_Height = 1.4f;

	m_tWeaponParam[Wpn_Halberd_Slave].iWeaponName = Wpn_Halberd_Slave;
	m_tWeaponParam[Wpn_Halberd_Slave].iWeaponName_InShop = WpnAll_Halberd_DarkBodish;
	m_tWeaponParam[Wpn_Halberd_Slave].iWeaponType = WEAPON_Halberd;
	m_tWeaponParam[Wpn_Halberd_Slave].iPrice = 1100;
	m_tWeaponParam[Wpn_Halberd_Slave].iReinforce = 0;
	m_tWeaponParam[Wpn_Halberd_Slave].fDamage = 750.f;
	m_tWeaponParam[Wpn_Halberd_Slave].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_Halberd_Slave].fRadius = 0.8f;
	m_tWeaponParam[Wpn_Halberd_Slave].fTrail_Min = 1.2f;
	m_tWeaponParam[Wpn_Halberd_Slave].fTrail_Max = 2.3f;
	m_tWeaponParam[Wpn_Halberd_Slave].fCol_Height = 1.3f;

	//===========================================================================================
	// 해머
	//===========================================================================================

	m_tWeaponParam[Wpn_Hammer].iWeaponName = Wpn_Hammer;
	m_tWeaponParam[Wpn_Hammer].iWeaponName_InShop = WpnAll_Hammer_ImperseAnker;
	m_tWeaponParam[Wpn_Hammer].iWeaponType = WEAPON_Hammer;
	m_tWeaponParam[Wpn_Hammer].iPrice = 1600;
	m_tWeaponParam[Wpn_Hammer].iReinforce = 0;
	m_tWeaponParam[Wpn_Hammer].fDamage = 650.f;
	m_tWeaponParam[Wpn_Hammer].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_Hammer].fRadius = 0.7f;
	m_tWeaponParam[Wpn_Hammer].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Hammer].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Hammer].fCol_Height = 1.1f;

	m_tWeaponParam[Wpn_Hammer_Black].iWeaponName = Wpn_Hammer_Black;
	m_tWeaponParam[Wpn_Hammer_Black].iWeaponName_InShop = WpnAll_Hammer_Black;
	m_tWeaponParam[Wpn_Hammer_Black].iWeaponType = WEAPON_Hammer;
	m_tWeaponParam[Wpn_Hammer_Black].iPrice = 1700;
	m_tWeaponParam[Wpn_Hammer_Black].iReinforce = 0;
	m_tWeaponParam[Wpn_Hammer_Black].fDamage = 680.f;
	m_tWeaponParam[Wpn_Hammer_Black].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_Hammer_Black].fRadius = 0.75f;
	m_tWeaponParam[Wpn_Hammer_Black].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Hammer_Black].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Hammer_Black].fCol_Height = 1.0f;

	m_tWeaponParam[Wpn_Hammer_Military].iWeaponName = Wpn_Hammer_Military;
	m_tWeaponParam[Wpn_Hammer_Military].iWeaponName_InShop = WpnAll_Hammer_Nakil;
	m_tWeaponParam[Wpn_Hammer_Military].iWeaponType = WEAPON_Hammer;
	m_tWeaponParam[Wpn_Hammer_Military].iPrice = 1800;
	m_tWeaponParam[Wpn_Hammer_Military].iReinforce = 0;
	m_tWeaponParam[Wpn_Hammer_Military].fDamage = 710.f;
	m_tWeaponParam[Wpn_Hammer_Military].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_Hammer_Military].fRadius = 1.1f;
	m_tWeaponParam[Wpn_Hammer_Military].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Hammer_Military].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Hammer_Military].fCol_Height = 1.9f;

	m_tWeaponParam[Wpn_Hammer_Slave].iWeaponName = Wpn_Hammer_Slave;
	m_tWeaponParam[Wpn_Hammer_Slave].iWeaponName_InShop = WpnAll_Hammer_DarkHeavyAxe;
	m_tWeaponParam[Wpn_Hammer_Slave].iWeaponType = WEAPON_Hammer;
	m_tWeaponParam[Wpn_Hammer_Slave].iPrice = 1600;
	m_tWeaponParam[Wpn_Hammer_Slave].iReinforce = 0;
	m_tWeaponParam[Wpn_Hammer_Slave].fDamage = 750.f;
	m_tWeaponParam[Wpn_Hammer_Slave].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_Hammer_Slave].fRadius = 0.7f;
	m_tWeaponParam[Wpn_Hammer_Slave].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Hammer_Slave].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Hammer_Slave].fCol_Height = 0.9f;

	//===========================================================================================
	// 총검
	//===========================================================================================

	m_tWeaponParam[Wpn_Gun].iWeaponName = Wpn_Gun;
	m_tWeaponParam[Wpn_Gun].iWeaponName_InShop = WpnAll_Gun_Bayonet;
	m_tWeaponParam[Wpn_Gun].iWeaponType = WEAPON_Gun;
	m_tWeaponParam[Wpn_Gun].iPrice = 1700;
	m_tWeaponParam[Wpn_Gun].iReinforce = 0;
	m_tWeaponParam[Wpn_Gun].fDamage = 450.f;
	m_tWeaponParam[Wpn_Gun].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_Gun].fRadius = 0.5f;
	m_tWeaponParam[Wpn_Gun].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Gun].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Gun].fCol_Height = 1.0f;

	m_tWeaponParam[Wpn_Gun_Black].iWeaponName = Wpn_Gun_Black;
	m_tWeaponParam[Wpn_Gun_Black].iWeaponName_InShop = WpnAll_Gun_ReverTardor;
	m_tWeaponParam[Wpn_Gun_Black].iWeaponType = WEAPON_Gun;
	m_tWeaponParam[Wpn_Gun_Black].iPrice = 1850;
	m_tWeaponParam[Wpn_Gun_Black].iReinforce = 0;
	m_tWeaponParam[Wpn_Gun_Black].fDamage = 480.f;
	m_tWeaponParam[Wpn_Gun_Black].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_Gun_Black].fRadius = 0.5f;
	m_tWeaponParam[Wpn_Gun_Black].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Gun_Black].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Gun_Black].fCol_Height = 1.1f;

	m_tWeaponParam[Wpn_Gun_Military].iWeaponName = Wpn_Gun_Military;
	m_tWeaponParam[Wpn_Gun_Military].iWeaponName_InShop = WpnAll_Gun_Nakil;
	m_tWeaponParam[Wpn_Gun_Military].iWeaponType = WEAPON_Gun;
	m_tWeaponParam[Wpn_Gun_Military].iPrice = 1900;
	m_tWeaponParam[Wpn_Gun_Military].iReinforce = 0;
	m_tWeaponParam[Wpn_Gun_Military].fDamage = 510.f;
	m_tWeaponParam[Wpn_Gun_Military].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_Gun_Military].fRadius = 0.5f;
	m_tWeaponParam[Wpn_Gun_Military].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Gun_Military].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Gun_Military].fCol_Height = 1.1f;

	m_tWeaponParam[Wpn_Gun_Slave].iWeaponName = Wpn_Gun_Slave;
	m_tWeaponParam[Wpn_Gun_Slave].iWeaponName_InShop = WpnAll_Gun_DarkBayonet;
	m_tWeaponParam[Wpn_Gun_Slave].iWeaponType = WEAPON_Gun;
	m_tWeaponParam[Wpn_Gun_Slave].iPrice = 1850;
	m_tWeaponParam[Wpn_Gun_Slave].iReinforce = 0;
	m_tWeaponParam[Wpn_Gun_Slave].fDamage = 550.f;
	m_tWeaponParam[Wpn_Gun_Slave].fPlusDamage = 0.f;
	m_tWeaponParam[Wpn_Gun_Slave].fRadius = 0.5f;
	m_tWeaponParam[Wpn_Gun_Slave].fTrail_Min = 0.75f;
	m_tWeaponParam[Wpn_Gun_Slave].fTrail_Max = 1.5f;
	m_tWeaponParam[Wpn_Gun_Slave].fCol_Height = 1.1f;

	//===========================================================================================
	// 방패
	//===========================================================================================

	m_tWeaponParam[WPN_Shield_Normal].iWeaponName = WPN_Shield_Normal;
	m_tWeaponParam[WPN_Shield_Normal].iWeaponName_InShop = WpnAll_END;
	m_tWeaponParam[WPN_Shield_Normal].iWeaponType = WEAPON_Shield;
	m_tWeaponParam[WPN_Shield_Normal].iPrice = 1000;
	m_tWeaponParam[WPN_Shield_Normal].iReinforce = 0;
	m_tWeaponParam[WPN_Shield_Normal].fDamage = 25.f;
	m_tWeaponParam[WPN_Shield_Normal].fPlusDamage = 0.f;
	m_tWeaponParam[WPN_Shield_Normal].fRadius = 0.7f;
	m_tWeaponParam[WPN_Shield_Normal].fTrail_Min = 0.f;
	m_tWeaponParam[WPN_Shield_Normal].fTrail_Max = 1.f;
	m_tWeaponParam[WPN_Shield_Normal].fCol_Height = 0.f;

	//===========================================================================================

	m_tWeaponParam[WPN_QueenLance].iWeaponName = WPN_QueenLance;
	m_tWeaponParam[WPN_QueenLance].iWeaponName_InShop = WpnAll_END;
	m_tWeaponParam[WPN_QueenLance].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[WPN_QueenLance].iPrice = 1000;
	m_tWeaponParam[WPN_QueenLance].iReinforce = 0;
	m_tWeaponParam[WPN_QueenLance].fDamage = 25.f;
	m_tWeaponParam[WPN_QueenLance].fPlusDamage = 0.f;
	m_tWeaponParam[WPN_QueenLance].fRadius = 1.4f;
	m_tWeaponParam[WPN_QueenLance].fTrail_Min = 0.3f;
	m_tWeaponParam[WPN_QueenLance].fTrail_Max = 3.4f;
	m_tWeaponParam[WPN_QueenLance].fCol_Height = 1.7f;

	m_tWeaponParam[WPN_QueenShield].iWeaponName = WPN_QueenShield;
	m_tWeaponParam[WPN_QueenShield].iWeaponName_InShop = WpnAll_END;
	m_tWeaponParam[WPN_QueenShield].iWeaponType = WEAPON_Shield;
	m_tWeaponParam[WPN_QueenShield].iPrice = 1000;
	m_tWeaponParam[WPN_QueenShield].iReinforce = 0;
	m_tWeaponParam[WPN_QueenShield].fDamage = 25.f;
	m_tWeaponParam[WPN_QueenShield].fPlusDamage = 0.f;
	m_tWeaponParam[WPN_QueenShield].fRadius = 0.8f;
	m_tWeaponParam[WPN_QueenShield].fTrail_Min = 0.f;
	m_tWeaponParam[WPN_QueenShield].fTrail_Max = 1.f;
	m_tWeaponParam[WPN_QueenShield].fCol_Height = 0.f;

	//===========================================================================================

	m_tWeaponParam[WPN_FrostBlood_IceGirl].iWeaponName = WPN_FrostBlood_IceGirl;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].iWeaponName_InShop = WpnAll_END;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].iWeaponType = WEAPON_SSword;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].iPrice = 1000;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].iReinforce = 0;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fDamage = 25.f;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fPlusDamage = 0.f;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fRadius = 0.7f;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fTrail_Min = 0.3f;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fTrail_Max = 1.6f;
	m_tWeaponParam[WPN_FrostBlood_IceGirl].fCol_Height = 0.9f;

	//===========================================================================================

	m_tWeaponParam[WPN_DeerKingShield].iWeaponName = WPN_DeerKingShield;
	m_tWeaponParam[WPN_DeerKingShield].iWeaponName_InShop = WpnAll_END;
	m_tWeaponParam[WPN_DeerKingShield].iWeaponType = WEAPON_Shield;
	m_tWeaponParam[WPN_DeerKingShield].iPrice = 1000;
	m_tWeaponParam[WPN_DeerKingShield].iReinforce = 0;
	m_tWeaponParam[WPN_DeerKingShield].fDamage = 25.f;
	m_tWeaponParam[WPN_DeerKingShield].fPlusDamage = 0.f;
	m_tWeaponParam[WPN_DeerKingShield].fRadius = 1.4f;
	m_tWeaponParam[WPN_DeerKingShield].fTrail_Min = 0.f;
	m_tWeaponParam[WPN_DeerKingShield].fTrail_Max = 1.f;
	m_tWeaponParam[WPN_DeerKingShield].fCol_Height = 0.f;


	return S_OK;
}

HRESULT CWeapon::SetUp_ConstantTable(CShader* pShader)
{
	if (nullptr == pShader)
		return E_FAIL;

	if (FAILED(pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;
	if (FAILED(pShader->Set_Value("g_fFxAlpha", &m_fFXAlpha, sizeof(_float))))
		return E_FAIL;

	//=============================================================================================
	// 쉐이더 재질정보 수치 입력
	//=============================================================================================
	_float	fEmissivePower = 5.f;	// 이미시브 : 높을 수록, 자체 발광이 강해짐.
	_float	fSpecularPower = 5.f;	// 메탈니스 : 높을 수록, 정반사가 강해짐.
	_float	fRoughnessPower = 0.5f;	// 러프니스 : 높을 수록, 빛 산란이 적어짐(빛이 응집됨).
	_float	fRimLightPower = 0.f;	// 림		: 높을 수록 빛이 퍼짐(림라이트의 범위가 넓어지고 , 밀집도가 낮아짐).
	_float	fMinSpecular = 1.f;	// 최소 빛	: 높을 수록 빛이 퍼짐(림라이트의 범위가 넓어지고 , 밀집도가 낮아짐).
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


	return NOERROR;
}

void CWeapon::Create_PointLight(_v3 _vPos)
{
	NEW_LIGHT		LightDesc;
	ZeroMemory(&LightDesc, sizeof(NEW_LIGHT));

	float fAmbient = 0.1f;

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(0.7f, 0.5f, 0.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(fAmbient, fAmbient, fAmbient, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	LightDesc.Position = _vPos;
	LightDesc.Range = 15.f;

	LightDesc.bLifeTime = true;
	LightDesc.fLifeTime_Cur = 0.1f;
	LightDesc.fLifeTime_Max = 0.1f;
	LightDesc.fAlpha = 1.f;

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Dev, LightDesc, CLight_Manager::Dynamic_Light)))
		return;
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
		MSG_BOX("Failed To Creating CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CWeapon::Clone_GameObject(void * pArg)
{
	CWeapon*	pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	m_pmatAttach = nullptr;
	m_pmatParent = nullptr;

	Safe_Release(m_pTransform);
	Safe_Release(m_pMesh_Static);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);

	Safe_Release(m_pBattleAgent);
	Safe_Release(m_pOptimization);

	for (auto& iter : m_listCollisionRecord)
		iter = nullptr;

	CGameObject::Free();
}
