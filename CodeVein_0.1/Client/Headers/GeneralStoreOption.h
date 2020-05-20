#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CGeneralStoreOption final : public CUI
{
public:
	enum OPTION_TYPE
	{
		OPTION_PURCHASE,
		OPTION_SELL,
		OPTION_BYE,
		OPTION_END
	};
private:
	explicit CGeneralStoreOption(_Device pDevice);
	explicit CGeneralStoreOption(const CGeneralStoreOption& rhs);
	virtual ~CGeneralStoreOption() = default;

public:
	void Set_OptionType(OPTION_TYPE eType) { m_eOptionType = eType; }
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }

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
	_bool m_bIsSelect = false;
	OPTION_TYPE			m_eOptionType = OPTION_END;

public:
	static CGeneralStoreOption*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
