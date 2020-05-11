#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Expendables.h"

BEGIN(Client)

class CPlayerFontUI;
class CExpendables_Slot final : public CUI
{
private:
	explicit CExpendables_Slot(_Device pDevice);
	explicit CExpendables_Slot(const CExpendables_Slot& rhs);
	virtual ~CExpendables_Slot() = default;

public:
	vector<CExpendables*> Get_Expendables() { return m_vecExpendables; }
	_bool Get_Select() { return m_bIsSelect; }
	_uint Get_Size();
	_uint Get_Type();

public:
	void Set_Type(CExpendables::EXPEND_TYPE eType) { m_eType = eType; }

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
	void Delete_Item();
	void Delete_Items();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void	SetUp_Default();

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	vector<CExpendables*>	m_vecExpendables;
	_bool					m_bIsSelect = false;
	CExpendables::EXPEND_TYPE	m_eType = CExpendables::EXPEND_END;
	_bool					m_bIsCollMouse = false;
	CPlayerFontUI*			m_pItemCntFont = nullptr;

public:
	static CExpendables_Slot*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};

END