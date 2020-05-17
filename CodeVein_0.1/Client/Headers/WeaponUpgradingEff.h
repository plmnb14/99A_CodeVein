#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Material.h"

BEGIN(Client)
class CWeaponUpgradingEff final : public CUI
{
private:
	explicit CWeaponUpgradingEff(_Device pDevice);
	explicit CWeaponUpgradingEff(const CWeaponUpgradingEff& rhs);
	virtual ~CWeaponUpgradingEff() = default;

public:
	void	Set_Alpha(_float fAlpha);
	void	Set_Active(_bool bIsActive);

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void	Change_Texture(const _tchar* _Name);
	void	SetUp_Default();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTexture*			m_pTextureCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CBuffer_RcTex*		m_pBufferCom = nullptr;

private:
	_bool				m_bLateInit = false;
	_int				m_iTexIdx = 0;
	_float				m_fAlpha = 0.f;

public:
	static CWeaponUpgradingEff*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
