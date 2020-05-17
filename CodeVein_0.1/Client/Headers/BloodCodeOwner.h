#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CBloodCodeOwner final : public CUI
{
private:
	explicit CBloodCodeOwner(_Device pDevice);
	explicit CBloodCodeOwner(const CBloodCodeOwner& rhs);
	virtual ~CBloodCodeOwner() = default;

public:
	BloodCode_ID Get_CodeID() { return m_eID; }

public:
	void Set_CodeID(BloodCode_ID eID) { m_eID = eID; }
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }
	void Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void Set_BloodCodeOwner();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	BloodCode_ID			m_eID = BloodCode_End;
	_bool					m_bIsSelect = false;
	_float					m_fAlpha = 0.f;
	_bool					m_bIsSound = false;

public:
	static CBloodCodeOwner*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
