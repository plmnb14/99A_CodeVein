#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CInven_Icon final : public CUI
{
public:
	enum ICON_TYPE
	{
		ICON_STATUS,
		ICON_ITEM,
		ICON_SKILL,
		ICON_CONFIG,
		ICON_EXIT,
		ICON_END
	};
private:
	explicit CInven_Icon(_Device pDevice);
	explicit CInven_Icon(const CInven_Icon& rhs);
	virtual ~CInven_Icon() = default;

public:
	ICON_TYPE Get_IconType() { return m_eIconType; }

public:
	void Set_IconType(ICON_TYPE eType) { m_eIconType = eType; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					Click_Icon();

public:
	_bool					Coll_Mouse();

private:
	ICON_TYPE				m_eIconType;


private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	

public:
	static CInven_Icon*		Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END