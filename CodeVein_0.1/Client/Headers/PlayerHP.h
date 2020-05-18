#pragma once

#include "Management.h"
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)

class CPlayerFontUI;
class CPlayerHP final : public CUI
{
private:
	explicit CPlayerHP(_Device pGraphic_Device);
	explicit CPlayerHP(const CPlayerHP& rhs);
	virtual ~CPlayerHP() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void	SetUp_Default();
	void	SetUp_State(_double TimeDelta);

private:
	_float m_fSpeed = 0.f;
	_float m_fPlayerHP = 0.f;
	_float m_fTotalHP = 0.f;
	_float m_fPercentage = 0.f;
	_v2		m_vecNoiseDir;

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CPlayerFontUI*			m_pFontCurHP = nullptr;
	CPlayerFontUI*			m_pFontTotalHP = nullptr;

public:
	static CPlayerHP*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

