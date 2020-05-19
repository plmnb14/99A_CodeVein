#include "stdafx.h"
#include "..\Headers\CustomInven.h"
//#include "UI_Manager.h"
#include "CustomSlot.h"
#include "CustomSliderBar.h"

#include "Player.h"
#include "Costume_Hair.h"
#include "Costume_Head.h"
#include "Costume_Mask.h"

CCustomInven::CCustomInven(_Device pDevice)
	: CUI(pDevice)
{
}

CCustomInven::CCustomInven(const CCustomInven & rhs)
	: CUI(rhs)
{
}

void CCustomInven::Set_ActiveSlot(INVEN_TYPE eType)
{
	Active_SlotType(false, &m_vecHairSlot);
	Active_SlotType(false, &m_vecFaceSlot);
	Active_SlotType(false, &m_vecEyeSlot);
	Active_SlotType(false, &m_vecMaskSlot);

	m_eActiveType = eType;

	for (_int i = 0; i < 4; i++)
		m_pHairSlider[i]->Set_Active(false);

	switch (m_eActiveType)
	{
	case Client::CCustomInven::TYPE_HAIR:
	{
		Active_SlotType(true, &m_vecHairSlot);

		for (_int i = 0; i < 4; i++)
			m_pHairSlider[i]->Set_Active(true);

		break;
	}
	case Client::CCustomInven::TYPE_FACE:
		Active_SlotType(true, &m_vecFaceSlot);
		break;
	case Client::CCustomInven::TYPE_EYE:
		Active_SlotType(true, &m_vecEyeSlot);
		break;
	case Client::CCustomInven::TYPE_MASK:
		Active_SlotType(true, &m_vecMaskSlot);
		break;
	}
}

void CCustomInven::Set_Active(_bool bActive)
{
	Active_SlotType(bActive, &m_vecHairSlot);
	Active_SlotType(bActive, &m_vecFaceSlot);
	Active_SlotType(bActive, &m_vecEyeSlot);
	Active_SlotType(bActive, &m_vecMaskSlot);

	for (_int i = 0; i < 4; i++)
		m_pHairSlider[i]->Set_Active(bActive);

	m_bIsActive = bActive;
}

HRESULT CCustomInven::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CCustomInven::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);

	m_fPosX = 350.5f;
	m_fPosY = 325.5f;
	m_fSizeX = 280.f;
	m_fSizeY = 471.f;
	m_fViewZ = 4.f;

	m_bIsActive = false;

	SetUp_Default();

	return NOERROR;
}

_int CCustomInven::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	Late_Init();
	Click_Inven();
		
	return NO_EVENT;
}

_int CCustomInven::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;

	return NO_EVENT;
}

HRESULT CCustomInven::Render_GameObject()
{
	if (!m_bIsActive)
		return NOERROR;

	return NOERROR;
}

HRESULT CCustomInven::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CCustomInven::SetUp_ConstantTable()
{

	return NOERROR;
}

void CCustomInven::SetUp_Default()
{
	// Hair
	const _int HAIR_COUNT = 7;
	for (_int i = 0; i < HAIR_COUNT; i++)
	{
		Add_Slot(TYPE_HAIR, i);
	}

	// Face
	const _int FACE_COUNT = 10;
	for (_int i = 0; i < FACE_COUNT; i++)
	{
		Add_Slot(TYPE_FACE, i);
	}

	// Eye
	const _int EYE_COUNT = 1;
	for (_int i = 0; i < EYE_COUNT; i++)
	{
		Add_Slot(TYPE_EYE, i);
	}

	// Mask
	const _int MASK_COUNT = 10;
	for (_int i = 0; i < MASK_COUNT; i++)
	{
		Add_Slot(TYPE_MASK, i);
	}

	LOOP(4)
	{
		m_pHairSlider[i] = static_cast<CCustomSliderBar*>(g_pManagement->Clone_GameObject_Return(L"GameObject_CustomSlider", nullptr));
		m_pHairSlider[i]->Set_Type(CCustomSliderBar::SLIDER_TYPE(i));

		_float fPosX = WINCX * 0.8f;
		_float fPosY = 270.f;

		m_pHairSlider[i]->Set_UI_Pos(fPosX, fPosY + (i * 50.f));
		if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(m_pHairSlider[i], SCENE_STAGE_BASE, L"Layer_Custom", nullptr)))
			return;
	}
}

void CCustomInven::Late_Init()
{
	if (m_bLateInit)
		return;

	m_bLateInit = true;

	m_pPlayer = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));
}

void CCustomInven::Click_Inven()
{
	if (!m_bIsActive)
		return;

	vector<CCustomSlot*>* pvecActiveSlot = nullptr;
	switch (m_eActiveType)
	{
	case Client::CCustomInven::TYPE_HAIR:
		pvecActiveSlot = &m_vecHairSlot;
		break;
	case Client::CCustomInven::TYPE_FACE:
		pvecActiveSlot = &m_vecFaceSlot;
		break;
	case Client::CCustomInven::TYPE_EYE:
		pvecActiveSlot = &m_vecEyeSlot;
		break;
	case Client::CCustomInven::TYPE_MASK:
		pvecActiveSlot = &m_vecMaskSlot;
		break;
	}

	_int iIdx = 0;
	for (auto& pSlot : (*pvecActiveSlot))
	{
		if (pSlot->Pt_InRect() && g_pInput_Device->Get_DIMouseState(CInput_Device::DIM_LB))
		{
			Reset_SelectSlot(pvecActiveSlot);
			pSlot->Set_Select(true);
			m_iSelectIndex[m_eActiveType] = iIdx;

			if (TYPE_HAIR == m_eActiveType)
				m_pPlayer->Get_Costume_Hair()->Change_HairMesh(CClothManager::Cloth_Static(iIdx));
			if (TYPE_FACE == m_eActiveType)
				m_pPlayer->Get_Costume_Head()->Change_HeadMesh(CCostume_Head::CHAR_HEAD(iIdx));
			if (TYPE_MASK == m_eActiveType)
				m_pPlayer->Get_Costume_Mask()->Change_AccMesh(CCostume_Mask::MASK_TYPE(iIdx));
		}
		iIdx++;
	}
}

void CCustomInven::Select_Elem(CCustomSlot* pSlot)
{
	pSlot->Set_Select(true);
}

void CCustomInven::Unselect_Elem(CCustomSlot* pSlot)
{
	pSlot->Set_Select(false);
}

void CCustomInven::Reset_SelectSlot(vector<CCustomSlot*>* pVecSlot)
{
	for (auto& iter : (*pVecSlot))
		iter->Set_Select(false);
}

void CCustomInven::Add_Slot(INVEN_TYPE eType, _int iIdx)
{
	CUI::UI_DESC* pDesc = nullptr;
	CCustomSlot* pSlot = nullptr;

	pDesc = new CUI::UI_DESC;

	pDesc->fSizeX = 50.f;
	pDesc->fSizeY = 50.f;
	
	g_pManagement->Add_GameObject_ToLayer(L"GameObject_CustomSlot", SCENE_STAGE_BASE, L"Layer_CustomUI", pDesc);

	pSlot = static_cast<CCustomSlot*>(g_pManagement->Get_GameObjectBack(L"Layer_CustomUI", SCENE_STAGE_BASE));
	pSlot->Set_Type(CCustomSlot::SLOT_TYPE(eType));
	pSlot->Set_SlotIdx(iIdx);

	switch (eType)
	{
	case Client::CCustomInven::TYPE_HAIR:
	{
		m_vecHairSlot.push_back(pSlot);
		Slot_PositionRearrange(&m_vecHairSlot);
		break;
	}
	case Client::CCustomInven::TYPE_FACE:
	{
		m_vecFaceSlot.push_back(pSlot);
		Slot_PositionRearrange(&m_vecFaceSlot);
		break;
	}
	case Client::CCustomInven::TYPE_EYE:
	{
		m_vecEyeSlot.push_back(pSlot);
		Slot_PositionRearrange(&m_vecEyeSlot);
		break;
	}
	case Client::CCustomInven::TYPE_MASK:
	{
		m_vecMaskSlot.push_back(pSlot);
		Slot_PositionRearrange(&m_vecMaskSlot);
		break;
	}
	}
}

void CCustomInven::Slot_PositionRearrange(vector<CCustomSlot*>* pVecSlot)
{
	for (_uint i = 0; i < pVecSlot->size(); ++i)
	{
		(*pVecSlot)[i]->Set_Active(m_bIsActive);
		(*pVecSlot)[i]->Set_ViewZ(m_fViewZ - 0.1f);
		(*pVecSlot)[i]->Set_UI_Pos(m_fPosX - 103.f + 52.f * (i % 5), m_fPosY - 140.f + 52.f * (i / 5));
	}
}

void CCustomInven::Active_SlotType(_bool bActive, vector<CCustomSlot*>* pVecSlot)
{
	for (auto& iter : (*pVecSlot))
		iter->Set_Active(bActive);
}

CCustomInven * CCustomInven::Create(_Device pGraphic_Device)
{
	CCustomInven* pInstance = new CCustomInven(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CCustomInven::Clone_GameObject(void * pArg)
{
	CCustomInven* pInstance = new CCustomInven(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CCustomInven::Free()
{
	Safe_Release(m_pTransformCom);

	CUI::Free();
}
