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

	Input_Pool(L"Player_FootSmoke", 100);

	Input_Pool(L"ButterFly_SoftSmoke", 2000);
	Input_Pool(L"ButterFly_PointParticle", 100);
	Input_Pool(L"ButterFly_PointParticle_Plum", 100);
	Input_Pool(L"ButterFly_RingLine", 10);
	Input_Pool(L"ButterFly_RingLine_Distortion", 10);
	Input_Pool(L"ButterFly_Distortion", 10);
	Input_Pool(L"ButterFly_Distortion_Circle", 10);
	Input_Pool(L"ButterFly_Distortion_Smoke", 100);
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
	Input_Pool(L"ButterFly_VenomShot_SubSmoke", 1000);
	Input_Pool(L"ButterFly_VenomShot_Distortion", 300);
	Input_Pool(L"ButterFly_VenomShot_Chunk", 100);
	Input_Pool(L"ButterFly_VenomShot_PointParticle", 500);
	Input_Pool(L"ButterFly_VenomShot_Tail", 500);
	Input_Pool(L"ButterFly_VenomShot_DeadMist", 100);
	Input_Pool(L"ButterFly_VenomShot_DeadSmoke", 100);
	Input_Pool(L"ButterFly_VenomShot_DeadSplash", 100);
	
	Input_Pool(L"ButterFly_BackStepSand01", 50);
	Input_Pool(L"ButterFly_BackStepSand02", 50);
	Input_Pool(L"ButterFly_BackStepSand_Floor", 50);
	Input_Pool(L"ButterFly_PopSand", 50);

	Input_Pool(L"Boss_KnonkDown_Dust", 10);

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

				 // Ǯ �ȿ��� �̸� ������ ������Ʈ ������ ���
				for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
				{
					if (pFindedQueue->size() <= 20) // �˳��ϰ�... ������ 20 ���ϸ� �����Ͽ� ���
					{
						_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
						CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));
						pEffect->Set_ParticleName(szEffName);
						pEffect->Set_Desc((*iter_begin)->vCreatePos, (*iter_begin)->pFollowTrans);
						pEffect->Reset_Init();

						m_EffectList.push_back(pEffect);
						continue;
					}

					m_EffectList.push_back(pFindedQueue->front());
					pFindedQueue->front()->Set_Desc((*iter_begin)->vCreatePos, (*iter_begin)->pFollowTrans);
					pFindedQueue->front()->Reset_Init(); // ��� �� �ʱ�ȭ
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

	lstrcpy(pInfo->szName, szName);
	pInfo->fLifeTime = fLifeTime;
	pInfo->pFollowTrans = pFollowTrans;
	pInfo->vCreatePos = vPos;
	
	m_vecParticle.push_back(pInfo);
}

void CParticleMgr::Create_Effect(_tchar* szName, _v3 vPos, CTransform * pFollowTrans)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(szName);
	if (pFindedQueue == nullptr)
		return;

	// Ǯ �ȿ��� �̸� ������ ������Ʈ ������ ���
	for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
	{
		if (pFindedQueue->size() <= 20) // �˳��ϰ�... ������ 20 ���ϸ� �����Ͽ� ���
		{
			_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
			CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));
			pEffect->Set_ParticleName(szEffName);
			pEffect->Set_Desc(vPos, pFollowTrans);
			pEffect->Reset_Init();

			m_EffectList.push_back(pEffect);
			continue;
		}

		m_EffectList.push_back(pFindedQueue->front());
		pFindedQueue->front()->Set_Desc(vPos, pFollowTrans);
		pFindedQueue->front()->Reset_Init(); // ��� �� �ʱ�ȭ
		pFindedQueue->pop();
	}
}

void CParticleMgr::Create_DirEffect(_tchar * szName, _v3 vPos, _v3 vDir, CTransform * pFollowTrans)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(szName);
	if (pFindedQueue == nullptr)
		return;

	// Ǯ �ȿ��� �̸� ������ ������Ʈ ������ ���
	for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
	{
		if (pFindedQueue->size() <= 20) // �˳��ϰ�... ������ 20 ���ϸ� �����Ͽ� ���
		{
			_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
			CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));
			
			m_EffectList.push_back(pEffect);

			pEffect->Set_ParticleName(szEffName);
			pEffect->Set_Desc(vPos, pFollowTrans);
			pEffect->Set_Dir(vDir);
			pEffect->Reset_Init();

			continue;
		}

		m_EffectList.push_back(pFindedQueue->front());

		pFindedQueue->front()->Set_Desc(vPos, pFollowTrans);
		pFindedQueue->front()->Set_Dir(vDir);
		pFindedQueue->front()->Reset_Init(); // ��� �� �ʱ�ȭ

		pFindedQueue->pop();
	}
}

void CParticleMgr::Create_AngleEffect(_tchar * szName, _v3 vPos, _v3 vAngle, CTransform * pFollowTrans)
{
	queue<CEffect*>* pFindedQueue = Find_Queue(szName);
	if (pFindedQueue == nullptr)
		return;

	// Ǯ �ȿ��� �̸� ������ ������Ʈ ������ ���
	for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
	{
		if (pFindedQueue->size() <= 20) // �˳��ϰ�... ������ 20 ���ϸ� �����Ͽ� ���
		{
			_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
			CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));

			m_EffectList.push_back(pEffect);

			pEffect->Set_ParticleName(szEffName);
			pEffect->Set_Desc(vPos, pFollowTrans);
			pEffect->Set_Angle(vAngle);
			pEffect->Reset_Init();

			continue;
		}

		m_EffectList.push_back(pFindedQueue->front());

		pFindedQueue->front()->Set_Desc(vPos, pFollowTrans);
		pFindedQueue->front()->Set_Angle(vAngle);
		pFindedQueue->front()->Reset_Init(); // ��� �� �ʱ�ȭ

		pFindedQueue->pop();
	}
}

void CParticleMgr::Create_Effect_NoPool(_tchar* szName, _v3 vPos, CTransform* pFollowTrans)
{
	CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szName, nullptr));
	pEffect->Set_Desc(vPos, pFollowTrans);
	pEffect->Reset_Init();
	m_pManagement->Add_GameOject_ToLayer_NoClone(pEffect, SCENE_STAGE, L"Layer_Effect", nullptr);
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

				// �������� �ʰ� ť�� �ٽ� �ֱ�
				if (pFindedQueue->size() < 1000) // �ӽ�...
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
		// �̸� Ŭ�и� �س���
		CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szName, nullptr));
		pEffect->Set_ParticleName(szName); // ����Ʈ Info ���� �̸��� ������Ʈ Tag�̸��� �޶� �̷��� ����. (�ش� ť �ȿ� �ٽ� ��������)
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

