#include "..\Headers\ParticleMgr.h"
#include "Effect.h"

IMPLEMENT_SINGLETON(CParticleMgr)

CParticleMgr::CParticleMgr()
{
}


HRESULT CParticleMgr::Ready_ParticleManager()
{
	m_pManagement = CManagement::Get_Instance();
	if (nullptr == m_pManagement)
		return E_FAIL;

	//Safe_AddRef(m_pManagement);

	Input_Pool(L"Player_FootSmoke", 50);
	Input_Pool(L"Player_FootSmoke_Jump", 30);
	Input_Pool(L"Player_FootSmoke_DodgeBack", 30);
	Input_Pool(L"Player_SpaceBar_StepParticle", 500);

	Input_Pool(L"Player_Skill_Scratch_Hor", 10);
	Input_Pool(L"Player_Skill_Scratch_Ver", 10);
	Input_Pool(L"Player_Skill_ScratchBlur_Hor", 10);
	Input_Pool(L"Player_Skill_ScratchBlur_Ver", 10);
	Input_Pool(L"Player_Skill_ScratchBlur_Sub_Hor", 10);
	Input_Pool(L"Player_Skill_ScratchBlur_Sub_Ver", 10);
	Input_Pool(L"Player_Skill_Ring_Hor", 10);
	Input_Pool(L"Player_Skill_Ring_Ver", 10);
	Input_Pool(L"Player_Skill_RedParticle_Explosion", 110);
	Input_Pool(L"Player_Skill_RedParticle_Upper", 300);
	Input_Pool(L"Player_Skill_RedCircle_Flash", 10);
	Input_Pool(L"Player_Skill_Distortion_Circle", 10);
	Input_Pool(L"Player_Skill_RedOnion", 40);
	Input_Pool(L"Player_Skill_Floor_BlackRing", 50);
	Input_Pool(L"Player_Skill_Floor_RedRing", 50);
	Input_Pool(L"Player_Skill_SplitAssert_LaserBefore", 5);
	Input_Pool(L"Player_Skill_SplitAssert_LaserAfter", 5);
	Input_Pool(L"Player_Skill_SplitAssert_LaserBody", 5);
	Input_Pool(L"Player_Skill_SplitAssert_LaserAfter_Smoke", 5);
	Input_Pool(L"Player_Skill_SplitAssert_LaserAfter_RingLine", 5);
	Input_Pool(L"Player_Skill_DarkRedOnion_Explosion", 50);
	Input_Pool(L"Player_Skill_Blood_Explosion", 100);
	Input_Pool(L"Player_Skill_RotYRing_Black", 40);
	Input_Pool(L"Player_Skill_RotYRing_Red", 40);
	Input_Pool(L"Player_Skill_WindMesh", 40);
	Input_Pool(L"Player_Skill_WindTronadeMesh", 60);

	Input_Pool(L"ButterFly_SoftSmoke", 2000);
	Input_Pool(L"ButterFly_PointParticle", 1000);
	Input_Pool(L"ButterFly_PointParticle_Plum", 100);
	Input_Pool(L"ButterFly_RingLine", 10);
	Input_Pool(L"ButterFly_RingLine_Distortion", 10);
	Input_Pool(L"ButterFly_Distortion", 10);
	Input_Pool(L"ButterFly_Distortion_Circle", 10);
	Input_Pool(L"ButterFly_Distortion_Smoke", 600);
	Input_Pool(L"ButterFly_Distortion_SmokeGravity", 50);
	Input_Pool(L"ButterFly_SoftSmoke_Bottom", 200);
	Input_Pool(L"ButterFly_Smoke_Red_Once", 200);
	Input_Pool(L"ButterFly_Smoke_Red_Particle", 1000);
	Input_Pool(L"ButterFly_SoftSmoke_Floor", 500);
	Input_Pool(L"ButterFly_SoftSmoke_Ready_1", 100);
	Input_Pool(L"ButterFly_SoftSmoke_Ready_2", 100);
	Input_Pool(L"ButterFly_SoftSmoke_Mist", 300);
	Input_Pool(L"ButterFly_SoftSmoke_Chunk", 50);
	Input_Pool(L"ButterFly_WaterSplash", 100);
	Input_Pool(L"ButterFly_GlitterSand", 300);

	Input_Pool(L"ButterFly_VenomShot", 1000);
	Input_Pool(L"ButterFly_VenomShot_Body", 30);
	Input_Pool(L"ButterFly_VenomShot_SubSmoke", 1000);
	Input_Pool(L"ButterFly_VenomShot_Distortion", 500);
	Input_Pool(L"ButterFly_VenomShot_Chunk", 500);
	Input_Pool(L"ButterFly_VenomShot_PointParticle", 500);
	Input_Pool(L"ButterFly_VenomShot_Tail", 500);
	Input_Pool(L"ButterFly_VenomShot_DeadMist", 100);
	Input_Pool(L"ButterFly_VenomShot_DeadSmoke", 100);
	Input_Pool(L"ButterFly_VenomShot_DeadSplash", 100);
	
	Input_Pool(L"ButterFly_BackStepSand01", 50);
	Input_Pool(L"ButterFly_BackStepSand02", 50);
	Input_Pool(L"ButterFly_BackStepSand_Floor", 50);
	Input_Pool(L"ButterFly_PopSand", 50);

	Input_Pool(L"Boss_KnockDown_Dust", 10);

	Input_Pool(L"Hit_BloodParticle_0", 50);
	Input_Pool(L"Hit_BloodParticle_1", 50);
	Input_Pool(L"Hit_BloodParticle_2", 50);
	Input_Pool(L"Hit_BloodParticle_3", 50);
	Input_Pool(L"Hit_BloodParticle_4", 50);
	Input_Pool(L"Hit_BloodParticle_5", 50);
	Input_Pool(L"Hit_Blood_Direction_0", 50);
	Input_Pool(L"Hit_Blood_Direction_1", 50);
	Input_Pool(L"Hit_Blood_Direction_2", 50);
	Input_Pool(L"Hit_Blood_Direction_3", 50);
	Input_Pool(L"Hit_Blood_Direction_4", 50);
	Input_Pool(L"Hit_Blood_Direction_5", 50);
	Input_Pool(L"Hit_Blood_Direction_6", 50);

	Input_Pool(L"Hit_BloodDecal_0", 50);

	Input_Pool(L"Hit_Slash_Particle_0", 50);
	Input_Pool(L"Hit_Slash_Particle_1", 50);
	Input_Pool(L"Hit_Slash_Particle_2", 50);
	Input_Pool(L"Hit_Slash_Particle_3", 50);
	Input_Pool(L"Hit_Slash_0", 50);
	Input_Pool(L"Hit_Slash_1", 50);
	Input_Pool(L"Hit_Slash_2", 50);
	Input_Pool(L"Hit_Slash_3", 50);
	Input_Pool(L"Hit_SlashLine_0", 50);
	Input_Pool(L"Hit_Particle_Red"		, 100);
	Input_Pool(L"Hit_Particle_Yellow"	, 100);

	Input_Pool(L"SpawnParticle", 1000);
	Input_Pool(L"SpawnParticle_Sub", 1000);

	Input_Pool(L"Bullet_Body", 30);
	Input_Pool(L"Bullet_Body_Aura", 100);
	Input_Pool(L"Bullet_DeadFlash", 10);
	Input_Pool(L"Bullet_DeadSmoke_Base", 50);
	Input_Pool(L"Bullet_DeadSmoke_Black", 100);
	Input_Pool(L"Bullet_Fire_Flash", 10);
	Input_Pool(L"Bullet_Ready_Aura", 100);
	Input_Pool(L"Bullet_Ready_Flash", 10);
	Input_Pool(L"Bullet_Tail_Particle", 100);
	Input_Pool(L"RockBullet_Body", 10);
	
	Input_Pool(L"MistletoeParticle", 80);
	Input_Pool(L"MistletoeParticle_Sub", 80);
	
	return S_OK;
}

HRESULT CParticleMgr::Update_ParticleManager(const _double TimeDelta)
{
	static _int iCnt = 0;

	auto& iter_begin = m_vecParticle.begin();
	auto& iter_end = m_vecParticle.end();

	if (m_vecParticle.size() <= 0)
	{
		Update_Effect(TimeDelta);
		return S_OK;
	}

	for (; iter_begin != iter_end;)
	{
		m_fCreateDelay_Check -= _float(TimeDelta);
		if (nullptr != *iter_begin && 0.f >= m_fCreateDelay_Check)
		{
			m_fCreateDelay_Check = 0.3f;

			if ((*iter_begin)->fLifeTime > 0)
			{
				(*iter_begin)->fLifeTime -= _float(TimeDelta);

				queue<CEffect*>* pFindedQueue = Find_Queue((*iter_begin)->szName);
				if (pFindedQueue == nullptr)
					return E_FAIL;

				 // 풀 안에서 미리 생성한 오브젝트 꺼내서 사용
				for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
				{
					if (pFindedQueue->size() <= 20) // 넉넉하게... 남은게 20 이하면 생성하여 사용
					{
						_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
						CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));
						pEffect->Set_ParticleName(szEffName);
						pEffect->Set_Desc((*iter_begin)->vCreatePos, (*iter_begin)->pFollowTrans);
						pEffect->Set_Delay(((*iter_begin)->fDelayTime != 0), (*iter_begin)->fDelayTime);
						pEffect->Set_AutoFind((*iter_begin)->bAutoFind);
						if ((*iter_begin)->bFinishPos) pEffect->Set_FinishPos((*iter_begin)->vFinishPos);
						pEffect->Reset_Init();

						m_EffectList.push_back(pEffect);
						continue;
					}

					m_EffectList.push_back(pFindedQueue->front());
					pFindedQueue->front()->Set_Desc((*iter_begin)->vCreatePos, (*iter_begin)->pFollowTrans);
					pFindedQueue->front()->Set_Delay(((*iter_begin)->fDelayTime != 0), (*iter_begin)->fDelayTime);
					pFindedQueue->front()->Set_AutoFind((*iter_begin)->bAutoFind);
					if((*iter_begin)->bFinishPos) pFindedQueue->front()->Set_FinishPos((*iter_begin)->vFinishPos);
					pFindedQueue->front()->Reset_Init(); // 사용 전 초기화
					pFindedQueue->pop();
				}

				iter_begin++;
			}
			else
			{
				Safe_Delete((*iter_begin));
				iter_begin = m_vecParticle.erase(iter_begin);
			}
		}
	}

	Update_Effect(TimeDelta);

	return S_OK;
}

void CParticleMgr::Create_ParticleEffect(_tchar* szName, _float fLifeTime, _v3 vPos, CTransform* pFollowTrans)
{
	PARTICLE_INFO* pInfo = new PARTICLE_INFO;
	ZeroMemory(pInfo, sizeof(PARTICLE_INFO));

	lstrcpy(pInfo->szName, szName);
	pInfo->fLifeTime = fLifeTime;
	pInfo->pFollowTrans = pFollowTrans;
	pInfo->vCreatePos = vPos;
	pInfo->bAutoFind = false;
	pInfo->bFinishPos = false;

	m_vecParticle.push_back(pInfo);
}

void CParticleMgr::Create_ParticleEffect_Delay(_tchar * szName, _float fLifeTime, _float fDelay, _v3 vPos, CTransform * pFollowTrans)
{
	PARTICLE_INFO* pInfo = new PARTICLE_INFO;
	ZeroMemory(pInfo, sizeof(PARTICLE_INFO));

	lstrcpy(pInfo->szName, szName);
	pInfo->fLifeTime = fLifeTime;
	pInfo->fDelayTime = fDelay;
	pInfo->pFollowTrans = pFollowTrans;
	pInfo->vCreatePos = vPos;
	pInfo->bAutoFind = false;
	pInfo->bFinishPos = false;

	m_vecParticle.push_back(pInfo);
}

void CParticleMgr::Create_ParticleEffect_FinishPos(_tchar* szName, _float fLifeTime, _v3 vPos, _v3 vFinishPos, CTransform * pFollowTrans)
{
	PARTICLE_INFO* pInfo = new PARTICLE_INFO;
	ZeroMemory(pInfo, sizeof(PARTICLE_INFO));

	lstrcpy(pInfo->szName, szName);
	pInfo->fLifeTime = fLifeTime;
	pInfo->pFollowTrans = pFollowTrans;
	pInfo->vCreatePos = vPos;
	pInfo->vFinishPos = vFinishPos;
	pInfo->bAutoFind = false;
	pInfo->bFinishPos = true;

	m_vecParticle.push_back(pInfo);
}

void CParticleMgr::Create_Effect(_tchar* szName, _v3 vPos, CTransform * pFollowTrans)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(szName);
	if (pFindedQueue == nullptr)
		return;

	// 풀 안에서 미리 생성한 오브젝트 꺼내서 사용
	for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
	{
		if (pFindedQueue->size() <= 20) // 넉넉하게... 남은게 20 이하면 생성하여 사용
		{
			_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
			CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));
			pEffect->Set_ParticleName(szEffName);
			pEffect->Set_Desc(vPos, pFollowTrans);
			pEffect->Set_Delay(false);
			pEffect->Reset_Init();

			m_EffectList.push_back(pEffect);
			continue;
		}

		m_EffectList.push_back(pFindedQueue->front());
		pFindedQueue->front()->Set_Desc(vPos, pFollowTrans);
		pFindedQueue->front()->Set_Delay(false);
		pFindedQueue->front()->Reset_Init(); // 사용 전 초기화
		pFindedQueue->pop();
	}
}

void CParticleMgr::Create_DirEffect(_tchar * szName, _v3 vPos, _v3 vDir, CTransform * pFollowTrans)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(szName);
	if (pFindedQueue == nullptr)
		return;

	// 풀 안에서 미리 생성한 오브젝트 꺼내서 사용
	for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
	{
		if (pFindedQueue->size() <= 20) // 넉넉하게... 남은게 20 이하면 생성하여 사용
		{
			_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
			CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));
			
			m_EffectList.push_back(pEffect);

			pEffect->Set_ParticleName(szEffName);
			pEffect->Set_Desc(vPos, pFollowTrans);
			pEffect->Set_Dir(vDir);
			pEffect->Set_Delay(false);
			pEffect->Reset_Init();

			continue;
		}

		m_EffectList.push_back(pFindedQueue->front());

		pFindedQueue->front()->Set_Desc(vPos, pFollowTrans);
		pFindedQueue->front()->Set_Dir(vDir);
		pFindedQueue->front()->Set_Delay(false);
		pFindedQueue->front()->Reset_Init(); // 사용 전 초기화

		pFindedQueue->pop();
	}
}

void CParticleMgr::Create_AngleEffect(_tchar * szName, _v3 vPos, _v3 vAngle, CTransform * pFollowTrans)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(szName);
	if (pFindedQueue == nullptr)
		return;

	// 풀 안에서 미리 생성한 오브젝트 꺼내서 사용
	for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
	{
		if (pFindedQueue->size() <= 20) // 넉넉하게... 남은게 20 이하면 생성하여 사용
		{
			_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
			CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));

			m_EffectList.push_back(pEffect);

			pEffect->Set_ParticleName(szEffName);
			pEffect->Set_Desc(vPos, pFollowTrans);
			pEffect->Set_Delay(false);
			pEffect->Set_Angle(vAngle);
			pEffect->Reset_Init();

			continue;
		}

		m_EffectList.push_back(pFindedQueue->front());

		pFindedQueue->front()->Set_Desc(vPos, pFollowTrans);
		pFindedQueue->front()->Set_Delay(false);
		pFindedQueue->front()->Set_Angle(vAngle);
		pFindedQueue->front()->Reset_Init(); // 사용 전 초기화

		pFindedQueue->pop();
	}
}

void CParticleMgr::Create_AutoFindEffect(_tchar* szName, _float fLifeTime, _v3 vPos, CTransform* pFollowTrans)
{
	PARTICLE_INFO* pInfo = new PARTICLE_INFO;
	ZeroMemory(pInfo, sizeof(PARTICLE_INFO));

	lstrcpy(pInfo->szName, szName);
	pInfo->fLifeTime = fLifeTime;
	pInfo->pFollowTrans = pFollowTrans;
	pInfo->vCreatePos = vPos;
	pInfo->bAutoFind = true;
	pInfo->bFinishPos = false;

	m_vecParticle.push_back(pInfo);
}

void CParticleMgr::Create_Effect_NoPool(_tchar* szName, _v3 vPos, CTransform* pFollowTrans)
{
	CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szName, nullptr));
	pEffect->Set_Desc(vPos, pFollowTrans);
	pEffect->Set_Delay(false);
	pEffect->Reset_Init();
	m_pManagement->Add_GameOject_ToLayer_NoClone(pEffect, SCENE_STAGE, L"Layer_Effect", nullptr);
}

void CParticleMgr::Create_Effect_Offset(_tchar* szName, _float fOffset, _v3 vPos, CTransform * pFollowTrans)
{
	auto	iter = find_if(m_mapEffectOffset.begin(), m_mapEffectOffset.end(), CTag_Finder(szName));
	if (iter == m_mapEffectOffset.end())
		m_mapEffectOffset.insert(pair<_tchar*, float>(szName, fOffset));

	m_mapEffectOffset[szName] += DELTA_60;
	if (m_mapEffectOffset[szName]  < fOffset)
		return;

	m_mapEffectOffset[szName] = 0.f;
	Create_Effect(szName, vPos, pFollowTrans);
}

void CParticleMgr::Create_Effect_Delay(_tchar * szName, _float fDelay, _v3 vPos, CTransform * pFollowTrans, _v3 vAngle)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(szName);
	if (pFindedQueue == nullptr)
		return;

	// 풀 안에서 미리 생성한 오브젝트 꺼내서 사용
	for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
	{
		if (pFindedQueue->size() <= 20) // 넉넉하게... 남은게 20 이하면 생성하여 사용
		{
			_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
			CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));

			m_EffectList.push_back(pEffect);

			pEffect->Set_ParticleName(szEffName);
			pEffect->Set_Desc(vPos, pFollowTrans);
			pEffect->Set_Delay(true, fDelay);
			pEffect->Set_Angle(vAngle);
			pEffect->Reset_Init();

			continue;
		}

		m_EffectList.push_back(pFindedQueue->front());

		pFindedQueue->front()->Set_Desc(vPos, pFollowTrans);
		pFindedQueue->front()->Set_Delay(true, fDelay);
		pFindedQueue->front()->Set_Angle(vAngle);
		pFindedQueue->front()->Reset_Init(); // 사용 전 초기화

		pFindedQueue->pop();
	}
}

void CParticleMgr::Create_Hit_Effect(CCollider* pAttackCol, CCollider* pHittedCol, CTransform* pHittedTrans, _float fPower)
{
	_v3 vAttackPos = pAttackCol->Get_CenterPos();
	_v3 vHittedPos = pHittedCol->Get_CenterPos(); // pHittedTrans->Get_Pos() + _v3(0.f, 1.f, 0.f);

	_v3 vRight = V3_NULL;
	memcpy(&vRight, &pHittedTrans->Get_WorldMat().m[0][0], sizeof(_v3));

	_v3 vDir = vHittedPos - vAttackPos;
	vDir.y *= 0.5f; // y는 반감
	D3DXVec3Normalize(&vDir, &vDir);
	_float fAngle = acosf(D3DXVec3Dot(&vDir, &vRight));
	fAngle *= -1.f;

	//cout << D3DXToDegree(fAngle) << endl;
	//cout << "[Attack Pos] " << vAttackPos.x << ", " << vAttackPos.y << ", " << vAttackPos.z << endl;
	//cout << "[Hitted Pos] " << vHittedPos.x << ", " << vHittedPos.y << ", " << vHittedPos.z << endl;
	//cout << vDir.x<<", "<< vDir.y<<", "<< vDir.z << endl;

	// 피가 칼에서 나옴

	_v3 vAngle = pHittedTrans->Get_Angle();
	vAngle.z += D3DXToDegree(fAngle);
	Create_AngleEffect(L"Hit_SlashLine_0", vAttackPos, vAngle);

	_tchar szBuff[256] = L"";
	wsprintf(szBuff, L"Hit_Slash_Particle_%d", CCalculater::Random_Num(0, 3));
	Create_Effect(szBuff, vAttackPos);

	Create_Effect(L"Hit_Slash_0", vAttackPos);
	Create_Effect(L"Hit_Slash_1", vAttackPos);
	Create_Effect(L"Hit_Slash_2", vAttackPos);
	Create_Effect(L"Hit_Slash_3", vAttackPos);
	Create_Effect(L"Hit_Particle_Red", vAttackPos);
	Create_Effect(L"Hit_Particle_Yellow", vAttackPos);

	Create_Effect(L"Hit_BloodParticle_0", vAttackPos);
	Create_Effect(L"Hit_BloodParticle_1", vAttackPos);
	Create_Effect(L"Hit_BloodParticle_2", vAttackPos);
	Create_Effect(L"Hit_BloodParticle_3", vAttackPos);
	Create_Effect(L"Hit_BloodParticle_4", vAttackPos);
	Create_Effect(L"Hit_BloodParticle_5", vAttackPos);

	//_float fMinus = (fAngle > 0) ? 1.f : -1.f;
	//_v3 vMyDir = pHittedTrans->Get_Axis(AXIS_Z);
	_v3 vBloodDir = vDir * fPower;// +(vRight * fMinus);

	Create_DirEffect(L"Hit_Blood_Direction_0", vAttackPos, vBloodDir);
	Create_DirEffect(L"Hit_Blood_Direction_1", vAttackPos, vBloodDir);
	Create_DirEffect(L"Hit_Blood_Direction_2", vAttackPos, vBloodDir);
	Create_DirEffect(L"Hit_Blood_Direction_3", vAttackPos, vBloodDir);
	Create_DirEffect(L"Hit_Blood_Direction_4", vAttackPos, vBloodDir);
	Create_DirEffect(L"Hit_Blood_Direction_5", vAttackPos, vBloodDir);
	Create_DirEffect(L"Hit_Blood_Direction_6", vAttackPos, vBloodDir);
}

void CParticleMgr::Create_Spawn_Effect(_v3 vPos, _v3 vFinishPos, CTransform* pFollowTrans)
{
	/// 렉걸려서 막아둠
	Create_ParticleEffect_FinishPos(L"SpawnParticle", 2.f, vPos, vFinishPos, pFollowTrans);
	Create_ParticleEffect_FinishPos(L"SpawnParticle_Sub", 2.f, vPos, vFinishPos, pFollowTrans);

}

void CParticleMgr::Create_FootSmoke_Effect(_v3 vPos, _float fOffset)
{
	m_fFootSmokeeDelay_Check += DELTA_60;
	if (m_fFootSmokeeDelay_Check < fOffset)
		return;

	m_fFootSmokeeDelay_Check = 0.f;
	Create_Effect(L"Player_FootSmoke", vPos);
}

HRESULT CParticleMgr::Update_Effect(const _double TimeDelta)
{
	_int iProgress;
	auto& iter_begin = m_EffectList.begin();
	auto& iter_end = m_EffectList.end();
	for (; iter_begin != iter_end; )
	{
		if (nullptr != *iter_begin)
		{
			iProgress = (*iter_begin)->Update_GameObject(TimeDelta);

			if (DEAD_OBJ == iProgress)
			{
				queue<CEffect*>* pFindedQueue = Find_Queue((*iter_begin)->Get_ParticleName());
				if (pFindedQueue == nullptr)
					return E_FAIL;

				//cout << "Queue Size : " << Find_Queue((*iter_begin)->Get_ParticleName())->size() << endl;

				// 삭제하지 않고 큐에 다시 넣기
				if (pFindedQueue->size() < 1000) // 임시...
					pFindedQueue->push((*iter_begin));
				else
					Safe_Release((*iter_begin));
				
				iter_begin = m_EffectList.erase(iter_begin);
			}
			else
				iter_begin++;
		}
	}

	for (auto& pGameObject : m_EffectList)
	{
		if (nullptr != pGameObject)
		{
			iProgress = pGameObject->Late_Update_GameObject(TimeDelta);

			if (0 > iProgress)
				break;
		}
	}

	return S_OK;
}

void CParticleMgr::Input_Pool(_tchar* szName, _int iCount)
{
	m_EffectPool.emplace(szName, queue<CEffect*>());

	for (_int i = 0; i <iCount; ++i)
	{
		// 미리 클론만 해놓기
		CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szName, nullptr));
		pEffect->Set_ParticleName(szName); // 이펙트 Info 안의 이름과 오브젝트 Tag이름이 달라서 이렇게 해줌. (해당 큐 안에 다시 넣으려고)
		m_EffectPool[szName].push(pEffect);
	}
}

void CParticleMgr::Pop_Pool(_tchar * szPoolName)
{
}

queue<CEffect*>* CParticleMgr::Find_Queue(_tchar* szName)
{
	for (auto& iter : m_EffectPool)
	{
		if (!lstrcmp(iter.first, szName))
			return &iter.second;
	}

	return nullptr;
}

void CParticleMgr::Free()
{
	for (auto& iter : m_EffectPool)
	{
		_int iQueueSize = _int(iter.second.size());
		for (_int i = 0; i < iQueueSize; ++i)
		{
			Safe_Release(iter.second.front());
			iter.second.pop();
		}
	}
	m_EffectPool.clear();

	for(auto& iter : m_EffectList)
		Safe_Release(iter);

	for (auto& iter : m_vecParticle)
	{
		Safe_Delete(iter);
	}
	

	//Safe_Release(m_pManagement);
}

