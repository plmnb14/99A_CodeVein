#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CExplainArmorUI final : public CUI
{
private:
	explicit CExplainArmorUI(_Device pDevice);
	explicit CExplainArmorUI(const CExplainArmorUI& rhs);
	virtual ~CExplainArmorUI() = default;

public:
	void Set_ArmorParam(ARMOR_PARAM tArmorParam) { m_tArmorParam = tArmorParam; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable(_uint iIndex);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	ARMOR_PARAM				m_tArmorParam;
	_uint					m_iInfoIdx = 0;
	_uint					m_iArmorIconIdx = 0;

public:
	static CExplainArmorUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
