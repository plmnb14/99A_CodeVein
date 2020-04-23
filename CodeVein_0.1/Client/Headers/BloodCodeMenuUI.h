#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CBloodCodeMenuUI final : public CUI
{
private:
	explicit CBloodCodeMenuUI(_Device pDevice);
	explicit CBloodCodeMenuUI(const CBloodCodeMenuUI& rhs);
	virtual ~CBloodCodeMenuUI() = default;

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

public:
	static CBloodCodeMenuUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();

};
END
