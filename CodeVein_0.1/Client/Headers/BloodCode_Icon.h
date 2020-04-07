#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CBloodCode_Icon final : public CUI
{
private:
	explicit CBloodCode_Icon(_Device pDevice);
	explicit CBloodCode_Icon(const CBloodCode_Icon& rhs);
	virtual ~CBloodCode_Icon() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void	SetUp_Default();

public:
	_bool Pt_InRect();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

public:
	static CBloodCode_Icon*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
