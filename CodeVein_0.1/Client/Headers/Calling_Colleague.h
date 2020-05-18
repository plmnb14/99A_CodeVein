#pragma once

#include "Client_Defines.h"
#include "Management.h"
#include "UI.h"

BEGIN(Client)

class CCalling_Colleague final : public CUI
{
private:
	explicit CCalling_Colleague(_Device Graphic_Device);
	explicit CCalling_Colleague(const CCalling_Colleague& rhs);
	virtual ~CCalling_Colleague() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual	HRESULT	LateInit_GameObject();
	virtual HRESULT Render_GameObject();

public:
	void	Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }

private:
	HRESULT		Add_Component();
	HRESULT		SetUp_ConstantTable(_uint iIndex);
	void		SetUp_State(_double TimeDelta);
	void		SetUp_RenderIndex();
	void		SetUp_Default();

private:
	void		Check_Calling_Colleague(_bool Check_Call_Colleague);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	_float		m_fAlpha = 0.f;

	_uint		m_iRender_Index = 0;

	_bool		m_bIsSelect = false;


public:
	static CCalling_Colleague*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};

END