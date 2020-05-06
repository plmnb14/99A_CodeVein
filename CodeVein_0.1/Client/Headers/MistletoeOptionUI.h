#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CMistletoeOptionUI final : public CUI
{
public:
	enum MISTLETOE_OPTION
	{
		OPTION_STAGE,
		OPTION_INHERIT,
		OPTION_LEVELUP,
		OPTION_END
	};
private:
	explicit CMistletoeOptionUI(_Device pDevice);
	explicit CMistletoeOptionUI(const CMistletoeOptionUI& rhs);
	virtual ~CMistletoeOptionUI() = default;

public:
	MISTLETOE_OPTION Get_Option() { return m_eOption; }

public:
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }
	void Set_Option(MISTLETOE_OPTION eOption) { m_eOption = eOption; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CCollider*				m_pCollider = nullptr;

private:
	_float					m_fAlpha = 0.f;
	_bool					m_bIsSelect = false;
	MISTLETOE_OPTION		m_eOption = OPTION_END;

public:
	static CMistletoeOptionUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
