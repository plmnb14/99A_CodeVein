#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CWeaponBuyPopupOptionUI final : public CUI
{
public:
	enum BUTTON_TYPE
	{
		BUTTON_LEFT,
		BUTTON_RIGHT,
		BUTTON_OK,
		BUTTON_END
	};
private:
	explicit CWeaponBuyPopupOptionUI(_Device pDevice);
	explicit CWeaponBuyPopupOptionUI(const CWeaponBuyPopupOptionUI& rhs);
	virtual ~CWeaponBuyPopupOptionUI() = default;

public:
	BUTTON_TYPE Get_Option() { return m_eOption; }
	_int		Get_Count() { return m_iCount; }
public:
	void		Set_IsSell(_bool bSell) { m_bSell = bSell; }
public:
	_bool		Pt_InRect();

public:
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }
	void Set_ButtonType(BUTTON_TYPE eMenu) { m_eOption = eMenu; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();


private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_int iIdx);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	_bool					m_bIsSelect = false;
	_bool					m_bSell = false;
	_int					m_iCount = 0;
	BUTTON_TYPE				m_eOption = BUTTON_END;

public:
	static CWeaponBuyPopupOptionUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
