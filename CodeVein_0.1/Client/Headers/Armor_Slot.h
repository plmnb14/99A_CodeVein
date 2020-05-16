#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CArmor_Slot final : public CUI
{
private:
	explicit CArmor_Slot(_Device pDevice);
	explicit CArmor_Slot(const CArmor_Slot& rhs);
	virtual ~CArmor_Slot() = default;

public:
	bool Get_Select() { return m_bIsSelect; }
	bool Get_SelectShop() { return m_bIsSelectShop; }
	ARMOR_PARAM Get_ArmorParam();

public:
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }
	void Set_SelectShop(_bool bIsSelect) { m_bIsSelectShop = bIsSelect; }
	void Set_ArmorParam(ARMOR_PARAM tArmorParam) { m_tArmorParam = tArmorParam; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	_bool Pt_InRect();
	void Init_Param();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void	SetUp_Default();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	ARMOR_PARAM				m_tArmorParam;
	_bool					m_bIsSelect = false;
	_bool					m_bIsSelectShop = false;
	_bool					m_bIsCollMouse = false;

public:
	static CArmor_Slot*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END