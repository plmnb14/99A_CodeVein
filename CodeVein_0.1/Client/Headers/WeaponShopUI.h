#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CWeaponShopUI final : public CUI
{
private:
	explicit CWeaponShopUI(_Device pDevice);
	explicit CWeaponShopUI(const CWeaponShopUI& rhs);
	virtual ~CWeaponShopUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(CShader* pShader);
	void Click_Option();
	void Reset_Option();

public:
	//void Active_SubUI(CMistletoeOptionUI* pSelectOption);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	_float					m_fAlpha = 0.f;
	_uint					m_iSelectIndex = 0;
	_float m_fCross = 100.f;

public:
	static CWeaponShopUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
