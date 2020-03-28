#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

#include "MenuBaseUI.h"
#include "ItemSlot.h"
#include "SlotCnt_UI.h"

class CMenu_Item : public CUI
{
private:
	explicit CMenu_Item(_Device pGraphic_Device);
	explicit CMenu_Item(const CMenu_Item& rhs);
	virtual ~CMenu_Item() = default;

public:
	_bool Get_WindowState() { return m_bIsOpenWindow; }

public:
	void Set_WindowState(_bool bState) { m_bIsOpenWindow = bState; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void	SetUp_WindowPos();
	void	Click_ItemSlot();

public:
	// 대입한 수 만큼 슬롯 추가
	void Add_Slot(_uint iSlotCnt = 1);
	// 아이템 획득 시 템 메뉴창에 추가
	void Add_Item(CItem::ITEM_TYPE eType, _uint iSlotIndex);
	

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

	_bool					m_bIsOpenWindow = false;

	vector<CItemSlot*>		m_vecItemSlot;
	vector<CSlotCnt_UI*>	m_vecCntUI;
	_uint					m_iSlotCnt = 0;

	
	

public:
	static CMenu_Item*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

