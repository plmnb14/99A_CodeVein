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

public:

	virtual void UpdateTrails(_double TimeDelta);

public:

	virtual void Change_WeaponData(WEAPON_DATA _eWpnData);

public:
	virtual WEAPON_STATE	Get_WeaponType() { return m_eWeaponType; }

public:
	// 무기를 붙일 뼈의 메트릭스 주소를 받습니다.
	virtual void			Set_AttachBoneMartix(_mat* _matAttachBone) { m_pmatAttach = _matAttachBone; }
	// 무기를 붙인 부모의 메트릭스 주소를 받습니다.
	virtual void			Set_ParentMatrix(_mat* _matParent) { m_pmatParent = _matParent; }
	virtual void			Set_WeaponType(WEAPON_STATE _eWeaponType) { m_eWeaponType = _eWeaponType; }

private:
	CTransform*				m_pTransform	= nullptr;
	CRenderer*				m_pRenderer		= nullptr;
	CShader*				m_pShader		= nullptr;
	CMesh_Static*			m_pMesh_Static	= nullptr;

	CCollider*				m_pCollider		= nullptr;

	CTrail_VFX*				m_pTrailEffect  = nullptr;
	CTrail_VFX*				m_pDistortionEffect = nullptr;
	CTrail_VFX*				m_pStaticTrailEffect = nullptr;


private:
	_tchar					m_szName[MAX_STR] = L"";

private:
	_mat*					m_pmatAttach = nullptr;
	_mat*					m_pmatParent = nullptr;


private:
	_bool					m_bEquip = false;				// 무기 장착 여부

private:
	WEAPON_STATE			m_eWeaponType = WEAPON_Ssword;
	WEAPON_DATA				m_eWeaponData = WPN_SSword_Normal;

private:
	HRESULT Add_Component();
	HRESULT SetUp_Default();
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