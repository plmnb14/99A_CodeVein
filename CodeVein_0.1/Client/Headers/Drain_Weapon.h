#pragma once

#include "Client_Defines.h"
#include "RenderObject.h"
#include "Management.h"

BEGIN(Client)

class CDrain_Weapon : public CGameObject
{
public:
	typedef enum CDrain_Weapon_Type
	{
		Drain_Tail, Drain_Wolf, Drain_Ivy, Drain_End
	}DRAIN_TYPE;

protected:
	explicit CDrain_Weapon(_Device pGraphic_Device);
	explicit CDrain_Weapon(const CDrain_Weapon& rhs);
	virtual ~CDrain_Weapon() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	virtual void			Set_Active(_bool _bActiveDrain) { m_bEquip = _bActiveDrain; };
	// ���⸦ ���� ���� ��Ʈ���� �ּҸ� �޽��ϴ�.
	virtual void			Set_AttachBoneMartix(_mat* _matAttachBone) { m_pmatAttach = _matAttachBone; }
	// ���⸦ ���� �θ��� ��Ʈ���� �ּҸ� �޽��ϴ�.
	virtual void			Set_ParentMatrix(_mat* _matParent) { m_pmatParent = _matParent; }
	virtual void			Set_DrainType(DRAIN_TYPE _eDrainType) { meDrainType = _eDrainType; }

	virtual void			Set_ActiveCollider(_bool _bActiveCollider);

private:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CMesh_Dynamic*			m_pMesh_Dynamic = nullptr;
	CCollider*				m_pCollider = nullptr;

private:
	_tchar					m_szName[MAX_STR] = L"";

private:
	_mat*					m_pmatAttach;
	_mat*					m_pmatParent;

private:
	_float					m_fAnimMultiply = 0.f;

private:
	_bool					m_bEquip = false;				// ���� ���� ����

	DRAIN_TYPE				meDrainType = Drain_Tail;

private:
	HRESULT Add_Component();
	HRESULT SetUp_Default();
	HRESULT SetUp_ConstantTable();

private:
	virtual void Cacl_AttachBoneTransform();
	virtual void Change_WeaponMesh(const _tchar* _MeshName);

public:
	static	CDrain_Weapon* Create(_Device pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);

public:
	virtual void Free();
};

END