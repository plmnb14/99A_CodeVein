#pragma once

#include "Client_Defines.h"
#include "UI.h"

#include "Expendables_Slot.h"

BEGIN(Client)


class CQuickSlot final : public CUI
{
private:
	explicit CQuickSlot(_Device pDevice);
	explicit CQuickSlot(const CQuickSlot& rhs);
	virtual ~CQuickSlot() = default;

public:


public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	vector<CExpendables_Slot*>	m_vecQuickSlot;
	_uint						m_iSelect = 0;

public:
	static CQuickSlot*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

