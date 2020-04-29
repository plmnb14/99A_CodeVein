#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CBloodCodeUI final : public CUI
{
private:
	explicit CBloodCodeUI(_Device pDevice);
	explicit CBloodCodeUI(const CBloodCodeUI& rhs);
	virtual ~CBloodCodeUI() = default;

public:
	BloodCode_ID Get_Type() { return m_eType; }

public:
	void Set_Type(BloodCode_ID eID) { m_eType = eID; }
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
	_bool					m_bIsCollMouse = false;
	BloodCode_ID			m_eType = BloodCode_End;
	_bool					m_bIsSelect = false;

public:
	static CBloodCodeUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END


