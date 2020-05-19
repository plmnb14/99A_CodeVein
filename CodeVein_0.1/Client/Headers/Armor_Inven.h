#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CPlayer;
class CArmor_Slot;
class CExplainArmorUI;
class CArmor_Inven final : public CUI
{
private:
	explicit CArmor_Inven(_Device pDevice);
	explicit CArmor_Inven(const CArmor_Inven& rhs);
	virtual ~CArmor_Inven() = default;

public:
	ARMOR_PARAM Get_UseArmorParam();
	vector<CArmor_Slot*>* Get_VecArmorSlot() { return &m_vecArmorSlot; }

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
	void					Click_Inven();
	void					Regist_Armor(CArmor_Slot* pArmorSlot);
	void					UnRegist_Armor(CArmor_Slot* pArmorSlot);
	void					Reset_SelectSlot();
	void					SetUp_SlotPos();
	void					SetUp_SubUI_Active(_bool bIsActive);
	void					Late_Init();

public:
	void					Add_Armor(ARMOR_PARAM tArmorParam);
	void					Sell_Armor();

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	vector<CArmor_Slot*>	m_vecArmorSlot;
	ARMOR_PARAM				m_tRegistParam;
	CExplainArmorUI*		m_pExplainUI = nullptr;
	_bool					m_bIsSubActive = false;


	_bool					m_bLateInit = false;
	CPlayer*				m_pPlayer = nullptr;

public:
	static CArmor_Inven*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END