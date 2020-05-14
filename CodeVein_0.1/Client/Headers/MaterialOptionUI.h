#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Material.h"

BEGIN(Client)
class CMaterialOptionUI final : public CUI
{
private:
	explicit CMaterialOptionUI(_Device pDevice);
	explicit CMaterialOptionUI(const CMaterialOptionUI& rhs);
	virtual ~CMaterialOptionUI() = default;

public:
	CMaterial::MATERIAL_TYPE Get_Type() { return m_eType; }

public:
	void Set_Type(CMaterial::MATERIAL_TYPE eType) { m_eType = eType; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);

public:
	_bool Pt_InRect();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	CMaterial::MATERIAL_TYPE m_eType = CMaterial::MATERIAL_END;

public:
	static CMaterialOptionUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
