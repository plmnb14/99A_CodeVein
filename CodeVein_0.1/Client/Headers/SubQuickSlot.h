#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"
#include "MenuBaseUI.h"
#include "Item_QuickSlot.h"

class CSubQuickSlot : public CUI
{
private:
	explicit CSubQuickSlot(_Device pGraphic_Device);
	explicit CSubQuickSlot(const CSubQuickSlot& rhs);
	virtual ~CSubQuickSlot() = default;

public:
	_bool Get_OpenUI() { return m_bIsOpen; }
	_uint Get_Index() { return m_iIndex; }

public:
	void Set_OpenUI(_bool bOpen) { m_bIsOpen = bOpen; }
	void Set_Index(_bool bIsLeft);

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					Update_UIPos();

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

	_bool					m_bIsOpen = true;

	_v2						m_vOldPos;
	_uint					m_iIndex = 0;

	CItem::ITEM_TYPE		m_eType;
	_uint					m_iMaxIndex = 0;
	
public:
	static CSubQuickSlot*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

