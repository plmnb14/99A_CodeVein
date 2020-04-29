#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CBloodCodeUI;
class CInventory_Icon;
class CBloodCode_Inven final : public CUI
{
private:
	explicit CBloodCode_Inven(_Device pDevice);
	explicit CBloodCode_Inven(const CBloodCode_Inven& rhs);
	virtual ~CBloodCode_Inven() = default;

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					SetUp_Default();
	void					Click_Icon();
	void					Reset_Select();

public:
	void					Add_BloodCode(BloodCode_ID eID);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	vector<CBloodCodeUI*>	m_vecBloodCodeUI;
	CInventory_Icon*		m_pExitIcon = nullptr;

public:
	static CBloodCode_Inven*		Create(_Device pGraphic_Device);
	virtual CGameObject*			Clone_GameObject(void* pArg);
	virtual void					Free();
};
END
