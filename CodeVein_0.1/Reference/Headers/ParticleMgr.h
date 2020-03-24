#pragma once

#include "Transform.h"

BEGIN(Engine)
class CManagement;
class CEffect;
class CParticleMgr final : public CBase
{
	DECLARE_SINGLETON(CParticleMgr)
private:
	explicit CParticleMgr();
	virtual ~CParticleMgr() = default;

public:
	HRESULT Ready_ParticleManager();
	HRESULT Update_ParticleManager(const _double TimeDelta);
	void Create_ParticleEffect(_tchar* szName, _float fLifeTime, CTransform* pFollowTrans = nullptr);

private:
	HRESULT Update_Effect(const _double TimeDelta);
	void Input_Pool(_tchar* szName, _int iCount);
	queue<CEffect*>* Find_Queue(_tchar* szName);

private:
	typedef struct tagParticleInfo
	{
		_tchar szName[256];
		_float fLifeTime;
		Engine::CTransform* pFollowTrans;
	}PARTICLE_INFO;

private:
	map<_tchar*, queue<CEffect*>>	m_EffectPool;	// 미리 클론해놓은 큐
	list<PARTICLE_INFO*>			m_vecParticle;	// 재생될 파티클 리스트
	list<CEffect*>					m_EffectList;	// 실제 객체들 (레이어 말고 여기서 돌아감)

	CManagement*					m_pManagement;

public:
	virtual void Free();
};

END