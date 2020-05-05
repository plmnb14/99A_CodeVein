#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Pet.h"

BEGIN(Client)

class CPet_UI final : public CUI
{
private:
	explicit CPet_UI(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPet_UI(const CPet_UI& rhs);
	virtual ~CPet_UI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	void Close_UI();

private:
	HRESULT	Add_Component();
	HRESULT	SetUp_Default();
	HRESULT	SetUp_ConstantTable();

public:
	static CPet_UI*	Create(_Device pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void * pArg) override;
	virtual void Free();

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

};

END
