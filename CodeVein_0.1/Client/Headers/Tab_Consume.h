#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Item.h"


BEGIN(Client)


class CItem_Slot;
class CButton_UI;
class CTab_Consume final : public CUI
{
private:
	explicit CTab_Consume(_Device pDevice);
	explicit CTab_Consume(const CTab_Consume& rhs);
	virtual ~CTab_Consume() = default;

public:
	CItem_Slot* Get_Slot(_uint iInx);

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					Add_Item(CItem::ITEM_ID eID, _uint iIndex = 0);
	void					Delete_Item();
	void					Regist_Item(_uint iIndex = 0);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	

private:
	vector<CItem_Slot*>		m_vecSlot;
	vector<CItem_Slot*>		m_vecRegist;
	_uint					m_iSlotCnt = 5; // ½½·Ô °³¼ö
	vector<CButton_UI*>		m_vecButton;

public:
	static CTab_Consume*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END