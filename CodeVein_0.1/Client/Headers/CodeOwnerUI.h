#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CCodeOwnerUI final : public CUI
{
private:
	explicit CCodeOwnerUI(_Device pDevice);
	explicit CCodeOwnerUI(const CCodeOwnerUI& rhs);
	virtual ~CCodeOwnerUI() = default;

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

private:
	BloodCode_ID			m_eBloodCode = BloodCode_End;
	_bool					m_bIsSelect = false;
	_float					m_fAlpha = 0.f;

public:
	static CCodeOwnerUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
