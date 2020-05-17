#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CInventory_Icon final : public CUI
{	
public:
	enum ICON_TYPE
	{
		ICON_EXPEND,
		ICON_MTRL,
		ICON_WEAPON,
		ICON_ARMOR,
		ICON_ALL,
		ICON_STATUS,
		ICON_EXIT,
		ICON_PET,
		ICON_SUMMONS,
		ICON_END
	};

private:
	explicit CInventory_Icon(_Device pDevice);
	explicit CInventory_Icon(const CInventory_Icon& rhs);
	virtual ~CInventory_Icon() = default;

public:
	ICON_TYPE Get_Type() { return m_eType; }

public:
	void Set_Type(ICON_TYPE eType) { m_eType = eType; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void	SetUp_Default();

public:
	_bool Pt_InRect();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	
private:
	ICON_TYPE				m_eType;

public:
	static CInventory_Icon*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
