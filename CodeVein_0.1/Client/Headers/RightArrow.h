#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

#include "Item_QuickSlot.h"
#include "ItemSlot.h"

BEGIN(Client)

class CRightArrow : public CUI
{
private:
	explicit CRightArrow(_Device pGraphic_Device);
	explicit CRightArrow(const CRightArrow& rhs);
	virtual ~CRightArrow() = default;

public:
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
	void					Click_RightArrow();
	void					Update_UIPos();

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

	_bool					m_bIsOpen = true;

public:
	static CRightArrow*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END