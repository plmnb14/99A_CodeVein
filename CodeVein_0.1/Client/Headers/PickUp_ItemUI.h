#pragma once

#include "Client_Defines.h"
#include "Management.h"
#include "UI.h"

BEGIN(Client)

class CPickUp_ItemUI final : public CUI
{
private:
	explicit CPickUp_ItemUI(_Device Graphic_Device);
	explicit CPickUp_ItemUI(const CPickUp_ItemUI& rhs);
	virtual ~CPickUp_ItemUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual	HRESULT	LateInit_GameObject();
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint TextureIndex);
	void	SetUp_State(_double TimeDelta);

private:
	_uint		m_uiRenderNumber = 0;

	_uint		m_iRenderNum = 0;

	_bool		m_bOne_PickupUIEnd = false;
	_bool		m_bTwo_PickupUIEnd = false;
	_bool		m_bThree_PickupUIEnd = false;

	_float		m_fPercentage = 0.f;
	_float		m_fSparkleBox = 1.f;

	_float		m_fPickup_Itembar = 0.f;
	_float		m_fPickup_Itembar2 = 0.f;
	_float		m_fPickup_Itembar3 = 0.f;

	_float		m_fTimer = 0.f;
	_float		m_fTwoTimer = 0.f;
	_float		m_fAllTimer = 0.f;

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

public:
	static	CPickUp_ItemUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END