#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Expendables.h"

BEGIN(Client)
class CExpendOptionUI final : public CUI
{
private:
	explicit CExpendOptionUI(_Device pDevice);
	explicit CExpendOptionUI(const CExpendOptionUI& rhs);
	virtual ~CExpendOptionUI() = default;

public:
	CExpendables::EXPEND_TYPE Get_Type() { return m_eType; }

public:
	void Set_Type(CExpendables::EXPEND_TYPE eType) { m_eType = eType; }

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
	CExpendables::EXPEND_TYPE m_eType = CExpendables::EXPEND_END;

public:
	static CExpendOptionUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
