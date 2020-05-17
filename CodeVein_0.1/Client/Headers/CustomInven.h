#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CCustomSlot;
class CCustomInven final : public CUI
{
public:
	enum INVEN_TYPE
	{
		TYPE_HAIR,
		TYPE_FACE,
		TYPE_EYE,
		TYPE_MASK,
		TYPE_END
	};

private:
	explicit CCustomInven(_Device pDevice);
	explicit CCustomInven(const CCustomInven& rhs);
	virtual ~CCustomInven() = default;

public:
	void Set_ActiveSlot(INVEN_TYPE eType);

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
	void					Select_Elem(CCustomSlot* pArmorSlot);
	void					Unselect_Elem(CCustomSlot* pArmorSlot);
	void					Reset_SelectSlot(vector<CCustomSlot*>* pVecSlot);

private:
	void					Add_Slot(INVEN_TYPE eType, _int iIdx);
	void					Slot_PositionRearrange(vector<CCustomSlot*>* pVecSlot);
	void					Active_SlotType(_bool bActive, vector<CCustomSlot*>* pVecSlot);

private:
	CTransform*				m_pTransformCom = nullptr;

private:
	vector<CCustomSlot*>	m_vecHairSlot;
	vector<CCustomSlot*>	m_vecFaceSlot;
	vector<CCustomSlot*>	m_vecEyeSlot;
	vector<CCustomSlot*>	m_vecMaskSlot;

	INVEN_TYPE				m_eActiveType = TYPE_END;

	_int					m_iSelectIndex[TYPE_END];

public:
	static CCustomInven*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END