#pragma once

#include "Client_Defines.h"
#include "RenderObject.h"
#include "Management.h"
#include "Trail_VFX.h"

BEGIN(Client)

class CWeapon : public CGameObject
{
protected:
	explicit CWeapon(_Device pGraphic_Device);
	explicit CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	WPN_PARAM Get_WeaponParam(WEAPON_DATA _eData);

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur = false);

public:
	virtual void OnCollisionEnter();
	virtual void OnCollisionEvent(list<CGameObject*> plistGameObject , _bool _bIsPlayer = false);

public:
	virtual void Update_Trails(_double TimeDelta);
	virtual void Update_Collider();
	virtual void Draw_Collider();

public:
	virtual _bool Update_CollisionRecord(CGameObject* pObject);

public:
	virtual void Change_WeaponData(WEAPON_DATA _eWpnData);
	virtual void Clear_CollisionRecordList();

public:
	virtual WEAPON_STATE	Get_WeaponType() { return m_eWeaponType; }
	// 이펙트 생성될 위치
	_v3						Get_HeadPos();

public:
	// 무기를 붙일 뼈의 메트릭스 주소를 받습니다.
	virtual void			Set_AttachBoneMartix(_mat* _matAttachBone) { m_pmatAttach = _matAttachBone; }
	// 무기를 붙인 부모의 메트릭스 주소를 받습니다.
	virtual void			Set_ParentMatrix(_mat* _matParent) { m_pmatParent = _matParent; }
	// 무기의 타입을 정합니다.
	virtual void			Set_WeaponType(WEAPON_STATE _eWeaponType) { m_eWeaponType = _eWeaponType; }
	// 무기의 아군 유무를 정합니다.
	virtual void			Set_Friendly(_bool _bFriendly) { m_bPlayerFriendly = _bFriendly; }
	// 트레일 On/Off
	virtual void			Set_Enable_Trail(_bool _bEnable);
	virtual void			Set_SkillMode(_bool _bSkill);
	virtual void			Set_TrailIndex(_int iIdx, _bool bStaticTrail = false);
	virtual void			Set_TrailUseMask(_bool bUse, _int iIdx, _bool bStaticTrail = false);
	virtual void			Set_Enable_Record(_bool _bRecord);
	// 스킬일 경우, 퍼센트 데미지 적용
	virtual void			Set_SkillPercent(_float _fPercent);
	// 무기를 렌더할지 말지
	virtual void			Set_Invisible(_bool _bInvisible) { m_tObjParam.bInvisible = _bInvisible; }

private:
	list<CGameObject*>		m_listCollisionRecord;

private:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CMesh_Static*			m_pMesh_Static = nullptr;
	COptimization*			m_pOptimization = nullptr;
	CBattleAgent*			m_pBattleAgent = nullptr;

private:
	CTrail_VFX*				m_pTrailEffect = nullptr;
	CTrail_VFX*				m_pDistortionEffect = nullptr;
	CTrail_VFX*				m_pStaticTrailEffect = nullptr;

private:
	_float					m_fTrailHeight_Min = 0.f;		// 트레일 시작점 보정수치
	_float					m_fTrailHeight_Max = 1.f;		// 트레일 끝점 보정수치
	_float					m_fSkillPercent = 1.f;

	_v3						m_vHeadPos = V3_NULL;

private:
	_tchar					m_szName[STR_128] = L"";

private:
	_mat*					m_pmatAttach;
	_mat*					m_pmatParent;

private:
	_bool					m_bEquip = false;				// 무기 장착 여부
	_bool					m_bPlayerFriendly = false;		// 플레이어 껀지
	_bool					m_bTrailEnable = false;
	_bool					m_bSingleTrail = false;			// 기본 트레일 한개만 사용
	_bool					m_bRecordCollision = false;		// 기록할껀지 말껀지 

	_bool					m_tmpEmissiveTest = false;

private:
	WEAPON_STATE			m_eWeaponType = WEAPON_SSword;
	WEAPON_DATA				m_eWeaponData = Wpn_SSword;
	WPN_PARAM				m_tWeaponParam[WPN_DATA_End];

private:
	HRESULT Add_Component();
	HRESULT SetUp_Default();
	HRESULT SetUp_WeaponData();
	HRESULT SetUp_ConstantTable(CShader* pShader);

private:
	virtual void Create_PointLight(_v3 _vPos);

private:
	virtual void Cacl_AttachBoneTransform();
	virtual void Change_WeaponMesh(const _tchar* _MeshName);

public:
	static	CWeapon* Create(_Device pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);

public:
	virtual void Free();
};

END