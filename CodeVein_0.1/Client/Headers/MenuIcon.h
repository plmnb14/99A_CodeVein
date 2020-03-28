#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"
#include "UI_Manager.h"
#include "Menu_Status.h"

BEGIN(Client)

class CMenuIcon  : public CUI
{
public:
	enum MENU_TYPE
	{
		MENU_STATUS,
		MENU_ITEM,
		MENU_ACTIVE,
		MENU_END
	};
private:
	explicit CMenuIcon(_Device pGraphic_Device);
	explicit CMenuIcon(const CMenuIcon& rhs);
	virtual ~CMenuIcon() = default;

public:
	void Set_MenuType(MENU_TYPE eType) { m_eMenuType = eType; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void	Click_Icon();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

	MENU_TYPE				m_eMenuType;

public:
	static	CMenuIcon*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END