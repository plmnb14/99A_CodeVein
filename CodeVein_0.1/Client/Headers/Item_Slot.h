#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Item.h"

BEGIN(Client)

class CItem;
class CItem_Slot final : public CUI
{
private:
	explicit CItem_Slot(_Device pDevice);
	explicit CItem_Slot(const CItem_Slot& rhs);
	virtual ~CItem_Slot() = default;

public:
	CItem::ITEM_ID Get_ItemID();
	_uint			Get_SlotSize();

public:

	void Set_SlotIndex(_uint iIndex) { m_iIndex = iIndex; }

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
	void					Add_Item_ToSlot(CItem* pItem);
	void					Delete_Item_ToSlot();
	_bool					Coll_Mouse();

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	_uint					m_iIndex = 0;
	vector<CItem*>			m_vecItem;
	
	
public:
	static	CItem_Slot*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END