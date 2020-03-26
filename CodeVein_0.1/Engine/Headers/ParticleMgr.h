#pragma once

#include "Transform.h"

BEGIN(Engine)
class CManagement;
class CEffect;
class ENGINE_DLL CParticleMgr final : public CBase
{
	DECLARE_SINGLETON(CParticleMgr)
private:
	explicit CParticleMgr();
	virtual ~CParticleMgr() = default;

public:
	HRESULT Ready_ParticleManager();
	HRESULT Update_ParticleManager(const _double TimeDelta);
	void Create_ParticleEffect(_tchar* szName, _float fLifeTime, CTransform* pFollowTrans = nullptr);
	void Create_Effect(_tchar* szName, _v3 vPos, CTransform* pFollowTrans = nullptr);

private:
	HRESULT Update_Effect(const _double TimeDelta);
	void Input_Pool(_tchar* szName, _int iCount);
	void Pop_Pool(_tchar* szPoolName);
	queue<CEffect*>* Find_Queue(_tchar* szName);

private:
	typedef struct tagParticleInfo
	{
		_tchar szName[256];
		_float fLifeTime;
		Engine::CTransform* pFollowTrans;
	}PARTICLE_INFO;

private:
	map<_tchar*, queue<CEffect*>>	m_EffectPool;	// �̸� Ŭ���س��� ť
	list<PARTICLE_INFO*>			m_vecParticle;	// ����� ��ƼŬ ����Ʈ
	list<CEffect*>					m_EffectList;	// ���� ��ü�� (���̾� ���� ���⼭ ���ư�)

	_float							m_fCreateDelay_Check = 0.f;

	CManagement*					m_pManagement = nullptr;

public:
	virtual void Free();
};

END