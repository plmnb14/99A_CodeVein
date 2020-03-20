#pragma once

#include "Management.h"
#include "UI.h"
#include "Client_Defines.h"

#include "ItemIcon.h"
#include "Menu_Btn.h"

BEGIN(Client)

class CMenuBaseUI final : public CUI
{
private:
	explicit CMenuBaseUI(_Device pGraphic_Device);
	explicit CMenuBaseUI(const CMenuBaseUI& rhs);
	virtual ~CMenuBaseUI() = default;
public:
	_uint Get_ItemIndex(_uint iSlotIndex);
	_uint Get_ItemCount(_uint iSlotIndex);

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
	HRESULT SetUp_MenuButton();
	_bool	Find_Item(_uint iItemIndex);

public:
	void Add_ItemIcon(const _tchar* pPrototypeTag, _uint iSceneID, const _tchar* pLayerTag, _uint iItemIndex);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

	_uint					m_iSlotCnt = 0;
	_uint					m_iMaxSlotCnt = 8;
	_bool					m_bIsOpenWindow = false;

	
	vector<CItemIcon*>		m_vecItemSlot[8];
	vector<CMenu_Btn*>		m_vecMenuBtn;

public:
	static CMenuBaseUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END