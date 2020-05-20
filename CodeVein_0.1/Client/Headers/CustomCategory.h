#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "CustomCategoryOption.h"

BEGIN(Client)
class CCustomInven;
class CCustomCategory final : public CUI
{
private:
	explicit CCustomCategory(_Device pDevice);
	explicit CCustomCategory(const CCustomCategory& rhs);
	virtual ~CCustomCategory() = default;

public:
	void	Set_Active(_bool bIsActive);

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

	void Active_CustomUIs();

	void Click_Option();
	void Reset_Option();


private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void	Change_Texture(const _tchar* _Name);
	void	SetUp_Default();

public:
	_ulong Get_ActiveSlotUI() { return m_dwCurentActiveUI; }

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	CCustomInven*						m_pCustomInven = nullptr;
	vector<CCustomCategoryOption*>		m_vecOption;

	_bool								m_bLateInit = false;
	_int								m_iTexIdx = 0;
	_ulong								m_dwCurentActiveUI = 0;

public:
	static CCustomCategory*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
