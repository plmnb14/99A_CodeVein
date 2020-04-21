#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CExpUI final : public CUI
{
private:
	explicit CExpUI(_Device pDevice);
	explicit CExpUI(const CExpUI& rhs);
	virtual ~CExpUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void	SetUp_State(_double TimeDelta);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	_float					m_fPercentage = 0.f;
	_float					m_fMaxExp = 50.f;
	_float					m_fCurExp = 0.f;

public:
	static CExpUI*			Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
