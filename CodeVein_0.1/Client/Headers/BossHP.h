#pragma once

#include "Management.h"
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CHPBack;
class CBossDecoUI;
class CBossHP final : public CUI
{
private:
	explicit CBossHP(_Device pGraphic_Device);
	explicit CBossHP(const CBossHP& rhs);
	virtual ~CBossHP() = default;

public:
	void Set_Target(CGameObject* pTarget) { m_pTarget = pTarget; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void	SetUp_State(_double TimeDelta);

private:
	_float m_fSpeed = 0.f;
	_float m_fBossHP = 0.f;
	_float m_fTotalHP = 0.f;
	_float m_fPercentage = 0.f;
	CHPBack* m_pHPBack = nullptr;
	CBossDecoUI* m_pDecoUI = nullptr;

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

public:
	static CBossHP*			Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

