#pragma once


#include "Management.h"
#include "UI.h"
#include "Client_Defines.h"

#include "LogoBtn.h"

BEGIN(Client)

class CCursorSelectBar : public CUI
{
private:
	explicit CCursorSelectBar(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCursorSelectBar(const CCursorSelectBar& rhs);
	virtual ~CCursorSelectBar() = default;

public:
	void Set_Dead() { m_bIsDead = true; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT			Add_Component();
	HRESULT			SetUp_ConstantTable();
	void			Coll_ToLogoBtn();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_Terrain*		m_pBufferCom = nullptr;

	_float					m_fTime;

public:
	static CCursorSelectBar*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};

END