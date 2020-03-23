#include "..\Headers\ParticleMgr.h"

IMPLEMENT_SINGLETON(CParticleMgr)

CParticleMgr::CParticleMgr()
{
}


HRESULT CParticleMgr::Ready_ParticleManager()
{
	m_pManagement = CManagement::Get_Instance();
	if (nullptr == m_pManagement)
		return E_FAIL;

	Safe_AddRef(m_pManagement);

	//_tchar szBuff[256] = L"DustEffect";
	Input_Pool(L"DustEffect", 100);

	return S_OK;
}

void CParticleMgr::Create_ParticleEffect(_tchar* szName, _float fLifeTime, CTransform* pFollowTrans)
{
	PARTICLE_INFO tInfo;

	lstrcpy(tInfo.szName, szName);
	tInfo.fLifeTime = fLifeTime;
	tInfo.pFollowTrans = pFollowTrans;

	m_vecParticle.emplace_back(tInfo);
}

HRESULT CParticleMgr::Update_ParticleManager(const _double TimeDelta)
{
	Update_Effect(TimeDelta);

	auto& iter_begin = m_vecParticle.begin();
	auto& iter_end = m_vecParticle.end();

	for (; iter_begin != iter_end;)
	{
		if ((*iter_begin).fLifeTime > 0)
		{
			(*iter_begin).fLifeTime -= _float(TimeDelta);

			// 클론 안하고 넣기
			// add(m_EffectPool[(*iter_begin).szName]);

			//if (FAILED(m_pManagement->Add_GameObject_ToLayer((*iter_begin).szName, SCENE_STAGE, L"Layer_Effect")))
			//	return E_FAIL;

			iter_begin++;
		}
		else
			m_vecParticle.erase(iter_begin);
	}

	return S_OK;
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
				//삭제하지 않고 큐에 다시 넣기
				//m_EffectPool[(*iter_begin)->Get_ParticleName()].push((*iter_begin));
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

void CParticleMgr::Free()
{
	for (auto& iter : m_EffectPool)
	{
		iter.second.front()->Set_Dead();
		iter.second.pop();
	}

	Safe_Release(m_pManagement);
}

void CParticleMgr::Input_Pool(_tchar* szName, _int iCount)
{
	m_EffectPool.emplace(szName, queue<CEffect*>());
	
	for (_int i = 0; i <iCount; ++i)
	{
		// 미리 클론만 해놓기
		CEffect* pEffect = nullptr;
		pEffect->Set_ParticleName(szName);
		m_EffectPool[szName].push(pEffect);
	}
}
