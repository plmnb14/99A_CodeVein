#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Armor.h"

BEGIN(Client)
class CSelect_UI;
class CCursorUI;
class CArmor_Slot final : public CUI
{
private:
	explicit CArmor_Slot(_Device pDevice);
	explicit CArmor_Slot(const CArmor_Slot& rhs);
	virtual ~CArmor_Slot() = default;

public:
	bool Get_Select() { return m_bIsSelect; }
	CArmor::ARMOR_TYPE Get_Type();

public:
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }
	void Set_Type(CArmor::ARMOR_TYPE eType) { m_eType = eType; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	_bool Pt_InRect();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void	SetUp_Default();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

	CSelect_UI*				m_pSelectUI = nullptr;
	CCursorUI*				m_pCursorUI = nullptr;
	_bool					m_bIsSelect = false;
	CArmor::ARMOR_TYPE		m_eType = CArmor::ARMOR_END;

public:
	static CArmor_Slot*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END