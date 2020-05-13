#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Material.h"

BEGIN(Client)
class CPlayerFontUI;
class CExplainMaterialUI final : public CUI
{
private:
	explicit CExplainMaterialUI(_Device pDevice);
	explicit CExplainMaterialUI(const CExplainMaterialUI& rhs);
	virtual ~CExplainMaterialUI() = default;

public:
	void Set_Type(CMaterial::MATERIAL_TYPE eType) { m_eType = eType; }
	void Set_MaximumCnt(_uint iMaximum) { m_iMaximumCnt = iMaximum; }
	void Set_CurHaveCnt(_uint iCurHave) { m_iCurHaveCnt = iCurHave; }

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

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	CMaterial::MATERIAL_TYPE m_eType = CMaterial::MATERIAL_END;
	CPlayerFontUI*			m_pFontMaximum = nullptr; // 최대 소지수 개수 폰트
	CPlayerFontUI*			m_pFontCurHave = nullptr; // 현재 소지한 개수 폰트
	_uint					m_iMaximumCnt = 0;
	_uint					m_iCurHaveCnt = 0;

public:
	static CExplainMaterialUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
