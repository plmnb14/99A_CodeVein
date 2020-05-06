#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CBloodCodeSlot final : public CUI
{
private:
	explicit CBloodCodeSlot(_Device pDevice);
	explicit CBloodCodeSlot(const CBloodCodeSlot& rhs);
	virtual ~CBloodCodeSlot() = default;

public:
	BloodCode_ID Get_CodeID() { return m_eID; }
public:
	void Set_CodeID(BloodCode_ID eID) { m_eID = eID; }
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
	void SetUp_Default();
	void SetUp_BloodCodeIdx();
	
private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CCollider*				m_pCollider = nullptr;

private:
	BloodCode_ID			m_eID = BloodCode_End;
	_bool					m_bIsSelect = false;
	_float					m_fAlpha = 0.f;

public:
	static CBloodCodeSlot*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};

END