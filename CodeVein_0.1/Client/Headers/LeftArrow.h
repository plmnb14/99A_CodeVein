#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

#include "MenuBaseUI.h"
#include "Item_QuickSlot.h"

BEGIN(Client)

class CLeftArrow : public CUI
{
private:
	explicit CLeftArrow(_Device pGraphic_Device);
	explicit CLeftArrow(const CLeftArrow& rhs);
	virtual ~CLeftArrow() = default;

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
	void					Click_LeftArrow();
	void					Update_UIPos();

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

	_bool					m_bIsOpen = true;

public:
	static CLeftArrow*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

