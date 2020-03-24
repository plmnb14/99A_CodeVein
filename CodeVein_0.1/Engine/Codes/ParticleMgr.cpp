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

	Input_Pool(L"Effect_TestSmoke", 500);

	return S_OK;
}

HRESULT CParticleMgr::Update_ParticleManager(const _double TimeDelta)
{
	static _int iCnt = 0;

	auto& iter_begin = m_vecParticle.begin();
	auto& iter_end = m_vecParticle.end();

	if (m_vecParticle.size() <= 0)
	{
		// nothing
	}
	else
	{
		for (; iter_begin != iter_end;)
		{
			if (nullptr != *iter_begin)
			{
				if ((*iter_begin)->fLifeTime > 0)
				{
					(*iter_begin)->fLifeTime -= _float(TimeDelta);

					queue<CEffect*>* pFindedQueue = Find_Queue((*iter_begin)->szName);
					if (pFindedQueue == nullptr)
						return E_FAIL;

					if (pFindedQueue->size() <= 20) // 넉넉하게... 남은게 20 이하면 생성하여 사용
					{
						//for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
						{
							_tchar* szEffName = pFindedQueue->front()->Get_ParticleName();
							CEffect* pEffect = static_cast<CEffect*>(m_pManagement->Clone_GameObject_Return(szEffName, nullptr));
							pEffect->Set_ParticleName(szEffName);
					
							m_EffectList.push_back(pEffect);
						}
					}
					else // 풀 안에서 미리 생성한 오브젝트 꺼내서 사용
					{
						//for (_int i = 0; i < pFindedQueue->front()->Get_Info()->iMaxCount; ++i)
						{
							m_EffectList.push_back(pFindedQueue->front());
							pFindedQueue->pop();
							++iCnt;
						}
						cout << iCnt << endl;
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

	}

	Update_Effect(TimeDelta);

	return S_OK;
}

void CParticleMgr::Create_ParticleEffect(_tchar* szName, _float fLifeTime, CTransform* pFollowTrans)
{
	static _int iCountTest = 0;

	//cout << iCountTest++ << endl;

	PARTICLE_INFO* pInfo = new PARTICLE_INFO;

	lstrcpy(pInfo->szName, szName);
	pInfo->fLifeTime = fLifeTime;
	pInfo->pFollowTrans = pFollowTrans;

	m_vecParticle.push_back(pInfo);
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

				// 삭제하지 않고 큐에 다시 넣기 (초기화만)
				(*iter_begin)->Reset_Init();

				if (pFindedQueue->size() < 500)
					pFindedQueue->emplace((*iter_begin));
				else
					Safe_Release((*iter_begin));
				
				cout << "Queue Size : " << Find_Queue((*iter_begin)->Get_ParticleName())->size() << endl;
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
		m_EffectPool[szName].emplace(pEffect);
	}
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
		_int iQueueSize = iter.second.size();
		for (_int i = 0; i < iQueueSize; ++i)
		{
			//iter.second.front()->Set_Dead();
			Safe_Release(iter.second.front());
			iter.second.pop();
		}
	}

	for (auto& iter : m_vecParticle)
	{
		Safe_Delete(iter);
	}
	

	//Safe_Release(m_pManagement);
}

