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
		//Teleport_Home_1, // 거점2 - 활동 거점, 수련의 영역 
		//Teleport_Home_2,
		Teleport_St01,
		//Teleport_St01_1, // St01 붕괴도시 시가지3 - 시가지 입구, 공원 부지, 독나비의 둥지
		//Teleport_St01_2,
		//Teleport_St01_3,
		Teleport_St02,
		//Teleport_St02_1, // St02 - 3
		//Teleport_St02_2,
		//Teleport_St02_3,
		Teleport_St03,
		//Teleport_St03_1, // St03 - 2
		//Teleport_St03_2,
		Teleport_St04,
		//Teleport_St04_1, // St04 - 2
		//Teleport_St04_2,
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
	Teleport_Menu			m_eTeleportMenu = Teleport_Home;
	_bool					m_bIsSelect = false;

public:
	static CStageUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
