#pragma once

#include "Management.h"
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)

class CHitCheckUI final : public CUI
{
private:
	explicit CHitCheckUI(_Device pGraphic_Device);
	explicit CHitCheckUI(const CHitCheckUI& rhs);
	virtual ~CHitCheckUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	virtual void Set_LifeTime(_float _fLifeTime) { m_fLifeTime_Cur = m_fLifeTime_Max = _fLifeTime; }
	virtual void Set_OnLifeTime(_bool _bOnLifeTime) { m_bOnLifeTime = _bOnLifeTime; }

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);

private:
	_float	m_fLifeTime_Cur = 0.f;
	_float	m_fLifeTime_Max = 0.f;
	_float  m_fAlphaPercent = 1.f;

	_bool	m_bOnLifeTime = false;

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

public:
	static CHitCheckUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

