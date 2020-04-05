#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CTotal_Inven final : public CUI
{
private:
	explicit CTotal_Inven(_Device pDevice);
	explicit CTotal_Inven(const CTotal_Inven& rhs);
	virtual ~CTotal_Inven() = default;

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					SetUp_Default();
	void					Click_Inven();

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

public:
	static CTotal_Inven*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
