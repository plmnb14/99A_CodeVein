#pragma once

#include "Transform.h"
#include "ObjectPool_Manager.h"

BEGIN(Engine)
class CManagement;
class CEffect;
class CCollider;
class CTrail_VFX;

class ENGINE_DLL CParticleMgr final : public CBase
{
	DECLARE_SINGLETON(CParticleMgr)
private:
	explicit CParticleMgr();
	virtual ~CParticleMgr() = default;

public:
	HRESULT Ready_ParticleManager_Essential();
	HRESULT Ready_ParticleManager();
	HRESULT Ready_Trail();
	HRESULT Ready_TitleEffect();
	HRESULT Update_ParticleManager(const _double TimeDelta);
	void Create_ParticleEffect(_tchar* szName, _float fLifeTime, _v3 vPos, CTransform* pFollowTrans = nullptr);
	void Create_ParticleEffect_Delay(_tchar* szName, _float fLifeTime, _float fDelay, _v3 vPos, CTransform* pFollowTrans = nullptr);
	void Create_ParticleEffect_Delay(_tchar* szName, _float fLifeTime, _float fDelay, _v3 vPos, CTransform* pFollowTrans, _mat* pTargetMat);
	void Create_ParticleEffect_FinishPos(_tchar* szName, _float fLifeTime, _float fDelay, _v3 vPos, _v3 vFinishPos, CTransform* pFollowTrans = nullptr);
	void Create_Effect(_tchar* szName, _v3 vPos, CTransform* pFollowTrans = nullptr, _v3 vDir = V3_NULL, _v3 vAngle = V3_NULL);
	void Create_DirEffect(_tchar* szName, _v3 vPos, _v3 vDir, CTransform* pFollowTrans = nullptr);
	void Create_AngleEffect(_tchar* szName, _v3 vPos, _v3 vAngle, CTransform* pFollowTrans = nullptr);
	void Create_AutoFindEffect(_tchar* szName, _float fLifeTime, _v3 vPos, CTransform* pFollowTrans = nullptr);
	void Create_Effect_NoPool(_tchar* szName, _v3 vPos, CTransform* pFollowTrans = nullptr);
	void Create_Effect_Offset(_tchar* szName, _float fOffset, _v3 vPos, CTransform* pFollowTrans = nullptr);
	void Create_Effect_Delay(_tchar* szName, _float fDelay, _v3 vPos, CTransform* pFollowTrans = nullptr, _v3 vAngle = V3_NULL);
	void Create_Effect_Delay(_tchar* szName, _float fDelay, _v3 vPos, CTransform* pFollowTrans, _int iLayer);
	void Create_Effect_Delay(_tchar* szName, _float fDelay, _v3 vPos, CTransform* pFollowTrans, _mat* pTargetMat);
	void Create_Effect_FinishPos(_tchar* szName, _float fDelay, _v3 vPos, _v3 vFinishPos, CTransform* pFollowTrans = nullptr);
	void Create_Effect_Curve(_tchar* szName, _v3 vPos, CTransform* pTargetTrans, _float fPower);
	void Create_Effect_Decal(_tchar* szName, _v3 vPos);

public:
	void Create_Hit_Effect(CCollider* pAttackCol, CCollider* pHittedCol, CTransform* pHittedTrans, _float fPower = 1.5f);
	void Create_Spawn_Effect(_float fDelay, _v3 vPos, _v3 vFinishPos, CTransform* pFollowTrans = nullptr);
	void Create_FootSmoke_Effect(_v3 vPos, _float fOffset); // 지금은 한 객체만 사용가능
	void Create_BossDeadParticle_Effect(_v3 vPos, _float fDelay, _float fLength);
	void Create_BossDead_Text(_float fDelay);
	void Create_Skill_Start_Effect(_v3 vPos, _v3 vEffPos, CTransform* pFollowTrans = nullptr);
public:
	CTrail_VFX* Create_Trail();
	CEffect* Create_TitleEffect(_tchar* szName);
	CEffect* Create_EffectReturn(_tchar* szName);

	void Clear_Fog();

private:
	HRESULT Update_Effect(const _double TimeDelta);
	HRESULT Update_Trail(const _double TimeDelta);

	void Input_Pool(_tchar* szName, _int iCount);
	void Input_Pool_Trail(_tchar* szName, _int iCount);
	queue<CEffect*>* Find_Queue(_tchar* szName);
	queue<CTrail_VFX*>* Find_TrailQueue(_tchar* szName);

private:
	typedef struct tagParticleInfo
	{
		_tchar	szName[STR_128];
		_float	fLifeTime;
		_float	fDelayTime;
		_v3		vCreatePos;
		_v3		vFinishPos;
		_bool	bAutoFind;
		_bool	bFinishPos;
		Engine::CTransform* pFollowTrans;
		_mat*				pTargetMatrix;
	}PARTICLE_INFO;

	typedef struct tagEffInfo
	{
		_v3 vCreatePos;
		_v3 vDirection;
	}EFF_INFO;

	typedef struct tagEffFireFlowerInfo
	{
		_v3 vCreatePos;
		_v3 vDirection;
		_float fDelay;
		_tchar	szName[STR_128];
	}EFF_FIREFLOWER_INFO;

	typedef struct tagEffParentInfo
	{
		_v3 vCreatePos;
		_v3 vDirection;
		_float fDelay;
		_float fSpeed;
		_tchar	szName[STR_128];
	}EFF_PARENT_INFO;

	typedef struct tagEffDecalInfo
	{
		_v3 vCreatePos;
		_v3 vDirection;
		_bool bWall;
	}EFF_DECAL_INFO;

private:
	map<_tchar*, queue<CEffect*>>	m_EffectPool;	// 미리 클론해놓은 큐
	list<PARTICLE_INFO*>			m_vecParticle;	// 재생될 파티클 리스트
	list<CEffect*>					m_EffectList;	// 실제 객체들 (레이어 말고 여기서 돌아감)
	map<_tchar*, float>				m_mapEffectOffset; // 이펙트 생성 오프셋을 주는 타임값 저장

	map<_tchar*, queue<CTrail_VFX*>>	m_TrailPool;	// 트레일 객체 담을 풀
	list<CTrail_VFX*>					m_TrailList;
	list<CTrail_VFX*>					m_TrailList_ForDelete;
	_float								m_fCreateDelay_Check = 0.f;

	_float							m_fFootSmokeeDelay_Check = 0.f;

	CManagement*					m_pManagement = nullptr;

public:
	virtual void Free();
};

END