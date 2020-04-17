#pragma once

#include "Management.h"
#include "UI.h"
#include "Client_Defines.h"


BEGIN(Client)
class CCursorEffect;
class CLogoBtn final : public CUI
{
private:
	explicit CLogoBtn(_Device pGraphic_Device);
	explicit CLogoBtn(const CLogoBtn& rhs);
	virtual ~CLogoBtn() = default;

public:
	_bool Get_CollMose() { return m_bIsColl; }

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
	CCursorEffect*			m_pCursorEffect = nullptr;

public:
	static CLogoBtn*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

