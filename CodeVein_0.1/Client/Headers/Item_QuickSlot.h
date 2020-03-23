#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

#include "MenuBaseUI.h"
#include "SlotCnt_UI.h"

BEGIN(Client)

class CItem_QuickSlot : public CUI
{
private:
	explicit CItem_QuickSlot(_Device pGraphic_Device);
	explicit CItem_QuickSlot(const CItem_QuickSlot& rhs);
	virtual ~CItem_QuickSlot() = default;

public:
	CItem::ITEM_TYPE Get_ItemType() { return m_eType; } // 나중에 퀵슬롯 아이템 사용시 사용한 아이템 타입.
	_uint Get_Index() { return m_iIndex; }
	void Set_Index(_uint iIndex) { m_iIndex = iIndex; }
	void Set_OpenUI(_bool bOpen) { m_bIsOpen = bOpen; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					SetUp_ItemCntUI();
	void					Update_Count();
	void					Update_UIPos();

public:
	void					Use_Item();

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

	CItem::ITEM_TYPE		m_eType;
	_uint					m_iCnt = 0;

	_uint					m_iIndex = 0;
	_uint					m_iMaxIndex = 0;

	_bool					m_bIsOpen = true;

	_v2						m_vOldPos;

public:
	static	CItem_QuickSlot*		Create(_Device pGraphic_Device);
	virtual CGameObject*			Clone_GameObject(void* pArg);
	virtual void					Free();
};

END