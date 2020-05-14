#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Expendables.h"

BEGIN(Client)
class CExpend_InfoUI final : public CUI
{
private:
	explicit CExpend_InfoUI(_Device pDevice);
	explicit CExpend_InfoUI(const CExpend_InfoUI& rhs);
	virtual ~CExpend_InfoUI() = default;

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
	HRESULT SetUp_ConstantTable();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	CExpendables::EXPEND_TYPE m_eType = CExpendables::EXPEND_END;

public:
	static CExpend_InfoUI*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
