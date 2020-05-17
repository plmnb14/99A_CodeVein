#pragma once

#include "Management.h"
#include "UI.h"
#include "Client_Defines.h"


BEGIN(Client)
class CCursorEffect;
class CCustomCategoryOption final : public CUI
{
private:
	explicit CCustomCategoryOption(_Device pGraphic_Device);
	explicit CCustomCategoryOption(const CCustomCategoryOption& rhs);
	virtual ~CCustomCategoryOption() = default;

public:
	_bool Get_IsColl() { return m_bIsColl; }

public:
	void Set_Select(_bool bSelect) { m_bIsSelect = bSelect; }
public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	HRESULT SetUp_CursorEffect();

public:
	_bool Coll_Mouse();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

	_bool					m_bIsColl = false;
	_bool					m_bIsSelect = false;
	CCursorEffect*			m_pCursorEffect = nullptr;
	_float					m_fAlpha = 0.f;

public:
	static CCustomCategoryOption*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

