#pragma once

#include "GameObject.h"
#include "Transform.h"
#include "Management.h"
#include "Effect.h"

BEGIN(Engine)

class CParticleMgr final : public CBase
{
	DECLARE_SINGLETON(CParticleMgr)

public:
	typedef struct tagParticleInfo
	{
		_tchar szName[256];
		_float fLifeTime;
		CTransform* pFollowTrans;
	}PARTICLE_INFO;
	
private:
	explicit CParticleMgr();
	virtual ~CParticleMgr() = default;

public:
	HRESULT Ready_ParticleManager();
	void Create_ParticleEffect(_tchar* szName, _float fLifeTime, CTransform* pFollowTrans = nullptr);

private:
	HRESULT Update_ParticleManager(const _double TimeDelta);
	HRESULT Update_Effect(const _double TimeDelta);
	void Input_Pool(_tchar* szName, _int iCount);

private:
	map<_tchar*, queue<CEffect*>>	m_EffectPool;	// �̸� Ŭ���س��� ť
	vector<PARTICLE_INFO>				m_vecParticle;	// ����� ��ƼŬ ����Ʈ
	list<CEffect*>					m_EffectList;	// ���� ��ü�� (���̾� ���� ���⼭ ���ư�)

	CManagement*						m_pManagement;

public:
	virtual void Free();
};

END