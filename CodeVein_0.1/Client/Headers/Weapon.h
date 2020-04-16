#pragma once

#include "Client_Defines.h"
#include "RenderObject.h"
#include "Management.h"
#include "Trail_VFX.h"

BEGIN(Client)

class CWeapon : public CGameObject
{
public:
	enum WEAPON_DATA
	{
		WPN_SSword_Normal,
		WPN_LSword_Normal,
		WPN_Hammer_Normal,
		WPN_Halverd_Normal,
		WPN_Gun_Normal,
		WPN_Shield_Normal,
		WPN_Hammer_YachaMan,
		WPN_QueenLance,
		WPN_QueenShield,
		WPN_DATA_End
	};

protected:
	explicit CWeapon(_Device pGraphic_Device);
	explicit CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass);

public:
	virtual void OnCollisionEnter();
	virtual void OnCollisionEvent(list<CGameObject*> plistGameObject);

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
	virtual void			Set_TrailIndex(_int iIdx);
	virtual void			Set_TrailUseMask(_int iIdx);
	virtual void			Set_TrailSize(_float fBegin, _float fEnd);
	virtual void			Set_Enable_Record(_bool _bRecord);
	virtual void			Set_SkillPercent(_float _fPercent);

private:
	list<CGameObject*>		m_listCollisionRecord;

private:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CMesh_Static*			m_pMesh_Static = nullptr;

private:
	CTrail_VFX*				m_pTrailEffect = nullptr;
	CTrail_VFX*				m_pDistortionEffect = nullptr;
	CTrail_VFX*				m_pStaticTrailEffect = nullptr;

private:
	_float					m_fTrailHeight_Min = 0.f;		// 트레일 시작점 보정수치
	_float					m_fTrailHeight_Max = 1.f;		// 트레일 끝점 보정수치
	_float					m_fTrailBegin = 0.5f;
	_float					m_fTrailEnd = 1.4f;
	_float					m_fSkillPercent = 1.f;


private:
	_tchar					m_szName[MAX_STR] = L"";

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
	WEAPON_STATE			m_eWeaponType = WEAPON_Ssword;
	WEAPON_DATA				m_eWeaponData = WPN_SSword_Normal;
	WPN_PARAM				m_tWeaponParam[WPN_DATA_End];

private:
	HRESULT Add_Component();
	HRESULT SetUp_Default();
	HRESULT SetUp_WeaponData();
	HRESULT SetUp_ConstantTable();

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