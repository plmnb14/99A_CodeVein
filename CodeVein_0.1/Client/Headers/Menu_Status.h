#pragma once

#include "Management.h"
#include "UI.h"
#include "Client_Defines.h"

class CMenu_Status final : public CUI
{
private:
	explicit CMenu_Status(_Device pGraphic_Device);
	explicit CMenu_Status(const CMenu_Status& rhs);
	virtual ~CMenu_Status() = default;

public:
	_bool Get_WindowState() { return m_bIsOpenWindow; }

public:
	void Set_WindowState(_bool bState) { m_bIsOpenWindow = bState; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void	SetUp_WindowPos();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

	_bool					m_bIsOpenWindow = false;

public:
	static CMenu_Status*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

