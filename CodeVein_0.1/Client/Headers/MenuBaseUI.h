#pragma once

#include "Management.h"
#include "UI.h"
#include "Client_Defines.h"

#include "ItemSlot.h"
#include "SlotCnt_UI.h"
#include "MenuIcon.h"


BEGIN(Client)

class CMenuIcon;
class CMenuBaseUI  : public CUI
{
private:
	explicit CMenuBaseUI(_Device pGraphic_Device);
	explicit CMenuBaseUI(const CMenuBaseUI& rhs);
	virtual ~CMenuBaseUI() {};

public:
	_uint Get_SlotSize(_uint iSlotIndex);
	CItem::ITEM_TYPE Get_SlotItemType(_uint iSlotIndex);
	_uint Get_SlotCnt() { return m_iSlotCnt; }
	_bool Get_Full() { return m_bIsFull; }

public:
	void Set_WindowState(_bool WinState) { m_bIsOpenWindow = WinState; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	
	void	SetUp_WindowPosition();
	
private:
	// 슬롯 개수
	void Add_Slot(_uint iSlotCnt);
	void Add_CntUI(_uint iCountUICnt);
	void Add_MenuIcon(_uint iMenuIconCnt);

public:
	void Add_Item(CItem::ITEM_TYPE eType, _uint iSlotIndex);
	void Delete_Item(_uint iSlotIndex);
	void Add_Manu_Slot(CItemSlot* pSlot);
	

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

	_uint					m_iSlotCnt = 0; // 소비아이템 슬롯 개수
	_uint					m_iCountUICnt = 0; // Count UI 개수
	_uint					m_iMenuIconCnt = 0; // 메뉴 아이콘 개수

	//_uint					m_iMaxSlotCnt = 8;
	_bool					m_bIsOpenWindow = false;

	
	vector<CItemSlot*>			m_vecItemSlot;
	vector<CSlotCnt_UI*>		m_vecCntUI;
	vector<CMenuIcon*>			m_vecMenuIcon;
	_bool						m_bIsFull = false;
	

public:
	static CMenuBaseUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END