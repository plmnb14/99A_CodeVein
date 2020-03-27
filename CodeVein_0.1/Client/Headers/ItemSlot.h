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
	void Set_SlotType(CItem::ITEM_TYPE eType) { m_eSlotType = eType; }

public:
	
	CItem::ITEM_TYPE Get_SlotItemType();
	_uint Get_SlotSize();
	_uint Get_ItemNum();
	_uint Get_Cnt() { return m_iCnt; }

public:
	void Set_Select(_bool bSelect) { m_bIsSelect = bSelect; }
	void Set_ItemData(vector<CItem*> vecItem) { m_vecItem = vecItem; }
	void Set_ItemNum(_uint iNum) { m_iItemNum = iNum; }
	void Set_Cnt(_uint iCnt) { m_iCnt = iCnt; }


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
	void					Add_Item(const _tchar * pLayerTag);
	void					Add_Item(CItem* pItem);//
	
	void					Pop_Item();
	_bool					Pt_InSlot();
	CItem::ITEM_TYPE		m_eSlotType;

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

	
	vector<CItem*>			m_vecItem;
	_uint					m_iSlotItemType;
	_uint					m_iItemNum;
	_bool					m_bIsSelect = false;
	_uint					m_iTexNum = 0;
	_uint					m_iCnt = 0;

public:
	static CItemSlot*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};