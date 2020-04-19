#pragma once

#include "Transform.h"

BEGIN(Engine)
class CManagement;
class CEffect;
class CCollider;
class ENGINE_DLL CParticleMgr final : public CBase
{
	DECLARE_SINGLETON(CParticleMgr)
private:
	explicit CParticleMgr();
	virtual ~CParticleMgr() = default;

public:
	HRESULT Ready_ParticleManager();
	HRESULT Update_ParticleManager(const _double TimeDelta);
	void Create_ParticleEffect(_tchar* szName, _float fLifeTime, _v3 vPos, CTransform* pFollowTrans = nullptr);
	void Create_ParticleEffect_Delay(_tchar* szName, _float fLifeTime, _float fDelay, _v3 vPos, CTransform* pFollowTrans = nullptr);
	void Create_ParticleEffect_Delay(_tchar* szName, _float fLifeTime, _float fDelay, _v3 vPos, CTransform* pFollowTrans, _mat* pTargetMat);
	void Create_ParticleEffect_FinishPos(_tchar* szName, _float fLifeTime, _v3 vPos, _v3 vFinishPos, CTransform* pFollowTrans = nullptr);
	void Create_Effect(_tchar* szName, _v3 vPos, CTransform* pFollowTrans = nullptr, _v3 vDir = V3_NULL, _v3 vAngle = V3_NULL);
	void Create_DirEffect(_tchar* szName, _v3 vPos, _v3 vDir, CTransform* pFollowTrans = nullptr);
	void Create_AngleEffect(_tchar* szName, _v3 vPos, _v3 vAngle, CTransform* pFollowTrans = nullptr);
	void Create_AutoFindEffect(_tchar* szName, _float fLifeTime, _v3 vPos, CTransform* pFollowTrans = nullptr);
	void Create_Effect_NoPool(_tchar* szName, _v3 vPos, CTransform* pFollowTrans = nullptr);
	void Create_Effect_Offset(_tchar* szName, _float fOffset, _v3 vPos, CTransform* pFollowTrans = nullptr);
	void Create_Effect_Delay(_tchar* szName, _float fDelay, _v3 vPos, CTransform* pFollowTrans = nullptr, _v3 vAngle = V3_NULL);
	void Create_Effect_Delay(_tchar* szName, _float fDelay, _v3 vPos, CTransform* pFollowTrans, _mat* pTargetMat);

public:
	void Create_Hit_Effect(CCollider* pAttackCol, CCollider* pHittedCol, CTransform* pHittedTrans, _float fPower = 1.5f);
	void Create_Spawn_Effect(_v3 vPos, _v3 vFinishPos, CTransform* pFollowTrans = nullptr);
	void Create_FootSmoke_Effect(_v3 vPos, _float fOffset); // ������ �� ��ü�� ��밡��

private:
	HRESULT Update_Effect(const _double TimeDelta);
	void Input_Pool(_tchar* szName, _int iCount);
	void Pop_Pool(_tchar* szPoolName);
	queue<CEffect*>* Find_Queue(_tchar* szName);

private:
	typedef struct tagParticleInfo
	{
		_tchar	szName[256];
		_float	fLifeTime;
		_float	fDelayTime;
		_v3		vCreatePos;
		_v3		vFinishPos;
		_bool	bAutoFind;
		_bool	bFinishPos;
		Engine::CTransform* pFollowTrans;
		_mat*				pTargetMatrix;
	}PARTICLE_INFO;

private:
	map<_tchar*, queue<CEffect*>>	m_EffectPool;	// �̸� Ŭ���س��� ť
	list<PARTICLE_INFO*>			m_vecParticle;	// ����� ��ƼŬ ����Ʈ
	list<CEffect*>					m_EffectList;	// ���� ��ü�� (���̾� ���� ���⼭ ���ư�)
	map<_tchar*, float>				m_mapEffectOffset; // ����Ʈ ���� �������� �ִ� Ÿ�Ӱ� ����

	_float							m_fCreateDelay_Check = 0.f;

	_float							m_fFootSmokeeDelay_Check = 0.f;

	CManagement*					m_pManagement = nullptr;

public:
	virtual void Free();
};

END