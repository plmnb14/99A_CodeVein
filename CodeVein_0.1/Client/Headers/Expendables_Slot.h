#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Management.h"

#include "Expendables.h"
#include "Select_UI.h"
#include "NumberUI.h"
#include "CursorUI.h"

BEGIN(Client)


class CExpendables_Slot final : public CUI
{
private:
	explicit CExpendables_Slot(_Device pDevice);
	explicit CExpendables_Slot(const CExpendables_Slot& rhs);
	virtual ~CExpendables_Slot() = default;

public:
	vector<CExpendables*> Get_Expendables() { return m_vecExpendables; }
	_bool Get_Select() { return m_bIsSelect; }
	CNumberUI* Get_NumberUI() { return m_pNumberUI; }

public:
	void Set_Select(_bool bIsSelect);

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	void Input_Item(CExpendables* pExpendables);
	void Input_Items(vector<CExpendables*> vecExpend) { m_vecExpendables = vecExpend; }
	_bool Pt_InRect();
	_uint Get_Size();
	_uint Get_Type();
	void Delete_Item();
	void Delete_Items();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void	SetUp_Default();
	

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

	vector<CExpendables*>	m_vecExpendables;
	CSelect_UI*				m_pSelectUI = nullptr;
	_bool					m_bIsSelect = false;
	CNumberUI*				m_pNumberUI = nullptr;
	CCursorUI*				m_pCursorUI = nullptr;

public:
	static CExpendables_Slot*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};

END