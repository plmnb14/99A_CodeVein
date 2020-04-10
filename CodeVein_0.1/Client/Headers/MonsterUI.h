#pragma once

#include "Client_Defines.h"
#include "Management.h"
#include "UI.h"

BEGIN(Client)

class CMonsterUI final : public CUI
{
	enum MONSTER_TYPE { TYPE_GUNGENJU,  };

private:
	explicit CMonsterUI(_Device Graphic_Device);
	explicit CMonsterUI(const CMonsterUI& rhs);
	virtual ~CMonsterUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual	HRESULT	LateInit_GameObject();
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint TextureIndex);
	void	SetUp_State(_double TimeDelta);

private:
	void	Ready_HPBonMatrix();

public:
	_float		Get_MonsterHP() { return m_fMonsterHp; }
	_float		Get_TotalHP() { return m_fTotalHP; }
	_v3			Get_MonsterUI_Pos() { return m_pTransformCom->Get_Pos(); }

public:
	void		Set_MonsterHP(const _float fMonsterHP) { m_fMonsterHp = fMonsterHP; }
	void		Set_TotalHP(const _float fTotalHP) { m_fTotalHP = fTotalHP; }
	void		Set_Target(CGameObject* Target) { m_pTarget = Target; }
	void		Set_Bonmatrix(_mat* Bonmatrix) { m_matMonsterBon = Bonmatrix; }

private:
	_float		m_fSpeed = 0.f;
	_float		m_fMonsterHp = 0.f;
	_float		m_fTotalHP = 0.f;
	_float		m_fPercentage = 0.f;
	_float		m_fWhite_Percentage = 1.f;

	_float		m_fGapHP = 0.f;
	_float		m_fOldHP = 0.f;

	_float		m_fDamage = 10.f;
	_float		m_fAlpha = 0.f;

	_bool		m_bCheck_Dir = false;

	_uint		m_iCheck_Renderindex = 0;

	_mat*		m_matMonsterBon;

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

public:
	static CMonsterUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END