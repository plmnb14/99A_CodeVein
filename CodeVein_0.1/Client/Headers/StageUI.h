#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CStageUI final : public CUI
{
public:
	enum Teleport_Menu
	{
		Teleport_Home,
		Teleport_St01,
		Teleport_St02,
		Teleport_St03,
		Teleport_St04,
		Teleport_End
	};
private:
	explicit CStageUI(_Device pDevice);
	explicit CStageUI(const CStageUI& rhs);
	virtual ~CStageUI() = default;

public:
	_uint Get_Teleport_Menu() { return _uint(m_eTeleportMenu); }

public:
	void Set_Teleport_Menu(_uint eMenu) { m_eTeleportMenu = Teleport_Menu(eMenu); }
	void Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	_float					m_fAlpha = 0.f;
	Teleport_Menu			m_eTeleportMenu = Teleport_End;
	_bool					m_bIsSelect = false;
	_uint					m_iSubStage = 0;
	_uint					m_iMaxSubStage = 0;

public:
	static CStageUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
