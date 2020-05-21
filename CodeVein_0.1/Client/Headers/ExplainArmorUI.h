#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CPlayerFontUI;
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
	void					SetUp_Default();
	void					SetUp_SubUI_Active(_bool bIsActive);
	void					SetUp_TexIdx();

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
	CPlayerFontUI*			m_pReinForceFont = nullptr; // 강화도
	CPlayerFontUI*			m_pDefFont = nullptr;		// 방어력
	CPlayerFontUI*			m_pPlusDefFont = nullptr;	// 추가 방어력
	CPlayerFontUI*			m_pHPFont = nullptr;		// 체력
	CPlayerFontUI*			m_pPlusHPFont = nullptr;	// 추가 체력
	_bool					m_bIsSubActive = false;

public:
	static CExplainArmorUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
