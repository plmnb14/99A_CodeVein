#pragma once

#include "Client_Defines.h"
#include "Client_Item_Enum.h"
#include "UI.h"
#include "Pet.h"

BEGIN(Client)

class CPet_Slot final : public CUI
{
private:
	explicit CPet_Slot(_Device pDevice);
	explicit CPet_Slot(const CPet_Slot& rhs);
	virtual ~CPet_Slot() = default;

public:
	CPet::PET_TYPE Get_PetType() { return m_ePetType; }
	_uint Get_PetLevel() { return m_iPetLevel; }

public:
	void Set_PetType(CPet::PET_TYPE eType) { m_ePetType = eType; }
	void Set_PetLevel(_uint iLevel) { m_iPetLevel = iLevel; }
	void Set_Select(_bool bIsSelect) { m_bIsSelect = bIsSelect; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	vector<CPet*> Get_Pets() { return m_vecPet; }
	CNumberUI* Get_NumberUI() { return m_pNumberUI; }
	_bool Get_Select() { return m_bIsSelect; }
	_uint Get_Size();

	_uint Get_Plus();
	_uint Get_Type();
	_uint Get_Grade();

	void Set_Select(_bool bIsSelect);
	void Set_Plus(CPet::PET_PLUS_TYPE _ePlusType) { m_ePlusType = _ePlusType; }
	void Set_Type(CPet::PET_TYPE _eType) { m_ePetType = _eType; }
	void Set_Grade(ITEM_GRADE_TYPE _eGradeType) { m_eGradeType = _eGradeType; }

	_bool Pt_InRect();
	void Input_Item(CPet* pPet);
	void Input_Items(vector<CPet*> vecPet) { m_vecPet = vecPet; }
	void Delete_Item();
	void Delete_Items();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable(_uint iIndex);

public:
	_bool					Pt_InRect();

private:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CTexture*				m_pTexture = nullptr;
	CShader*				m_pShader = nullptr;
	CBuffer_RcTex*			m_pBuffer = nullptr;

private:
	_bool					m_bIsCollMouse = false;
	_bool					m_bIsSelect = false;
	CPet::PET_TYPE			m_ePetType = CPet::PET_TYPE_END;
	_uint					m_iPetLevel = 1;

public:
	static CPet_Slot*			Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();

	CPet::PET_PLUS_TYPE		m_ePlusType = CPet::PET_PLUS_TYPE::PET_PLUS_END;
	ITEM_GRADE_TYPE			m_eGradeType = ITEM_GRADE_TYPE::ITEM_GRADE_TYPE_END;
	CPet::PET_TYPE			m_ePetType = CPet::PET_TYPE::PET_TYPE_END;

};

END

