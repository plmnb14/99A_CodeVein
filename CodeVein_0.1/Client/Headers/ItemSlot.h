#pragma once


#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

#include "Item.h"

class CItemSlot : public CUI
{
private:
	explicit CItemSlot(_Device pGraphic_Device);
	explicit CItemSlot(const CItemSlot& rhs);
	virtual ~CItemSlot() = default;

public:
	
	CItem::ITEM_TYPE Get_SlotItemType();
	_uint Get_SlotSize();

public:
	void Set_Select(_bool bSelect) { m_bIsSelect = bSelect; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();

public:
	void					Add_Item(CItem::ITEM_TYPE eType);
	void					Pop_Item();
	

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

	
	vector<CItem*>			m_vecItem;
	_uint					m_iSlotItemType;
	_uint					m_iTexIndex = 0;
	_bool					m_bIsSelect = false;

public:
	static CItemSlot*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};