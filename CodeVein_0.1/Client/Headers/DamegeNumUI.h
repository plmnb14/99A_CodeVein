#pragma once

#include "Client_Defines.h"
#include "Management.h"
#include "UI.h"

BEGIN(Client)

class CMonsterUI;
class CDamegeNumUI final : public CUI
{
private:
	explicit CDamegeNumUI(_Device Graphic_Device);
	explicit CDamegeNumUI(const CDamegeNumUI& rhs);
	virtual ~CDamegeNumUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual	HRESULT	LateInit_GameObject();
	virtual HRESULT Render_GameObject();

public:
	void Set_Target(CGameObject* Target) { m_pTarget = Target; }

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint TextureIndex);
	void	SetUp_State(_double TimeDelta);

private:
	_uint	m_iNowHP = 0;
	_uint	m_iMaxHP = 0;

	_uint	m_iGet_Damege = 0;

	_uint	m_iSave_Damage = 0;

	_uint	Temp_GetDamage[3] = {};

	_uint	m_iCheck_DamagePos = 0;

	_v3		m_vPosOne[3] = { V3_NULL, V3_NULL, V3_NULL };
	_v3		m_vPosTwo[3] = { V3_NULL, V3_NULL, V3_NULL };
	_v3		m_vPosThree[3] = { V3_NULL, V3_NULL, V3_NULL };

	_mat	m_matWorld;
	_mat	m_matWorldPlus[3];

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

public:
	static CDamegeNumUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END