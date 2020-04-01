#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Expendables_Slot.h"

BEGIN(Client)

class CExpendables_Inven final : public CUI
{
private:
	explicit CExpendables_Inven(_Device pDevice);
	explicit CExpendables_Inven(const CExpendables_Inven& rhs);
	virtual ~CExpendables_Inven() = default;

public:
	vector<CExpendables_Slot*>* Get_QuickSlot() { return &m_vecQuickSlot; }

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					Load_Expendables(CExpendables* pExpendables, _uint iIndex = 0);
	void					Click_Inven();

public:
	void Add_Expendables(CExpendables::EXPEND_TYPE eType);
	void Sell_Expendables(_uint iDelete);
	void Delete_QuickSlot(CExpendables_Slot* pSlot);
	
private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	vector<CExpendables_Slot*>	m_vecSlot;
	vector<CExpendables_Slot*>	m_vecQuickSlot;
	vector<CExpendables*>	m_vecExpendables;
	

public:
	static CExpendables_Inven*	Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};

END

