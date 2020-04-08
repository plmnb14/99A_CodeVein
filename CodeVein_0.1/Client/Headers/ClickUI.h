#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CClickUI final : public CUI
{
private:
	explicit CClickUI(_Device pDevice);
	explicit CClickUI(const CClickUI& rhs);
	virtual ~CClickUI() = default;

public:
	_bool Get_Click() { return m_bIsClick; }

public:
	void Set_Click(_bool bIsClick) { m_bIsClick = bIsClick; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

public:
	_bool Pt_InRect();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	_bool					m_bIsClick = false;

public:
	static CClickUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
