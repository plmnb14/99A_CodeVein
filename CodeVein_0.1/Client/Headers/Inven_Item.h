#pragma once

#include "Management.h"
#include "UI.h"
#include "Client_Defines.h"


#include "Item_Slot.h"


BEGIN(Client)

class CInven_Item : public CUI
{
private:
	explicit CInven_Item(_Device pDevice);
	explicit CInven_Item(const CInven_Item& rhs);
	virtual ~CInven_Item() = default;

public:
	void	SetUp_WindowPosition();


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
	void SetUp_Slot(CItem::ITEM_TYPE eType, void* pArg = nullptr);
	void Add_Item_ToInven(CItem::ITEM_TYPE eType, _uint iItemNum, const _tchar * pPrototypeTag, const _tchar* pLayerTag, void * pArg = nullptr);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

public:
	static CInven_Item*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END