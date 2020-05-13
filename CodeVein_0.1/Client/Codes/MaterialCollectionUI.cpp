#include "stdafx.h"
#include "..\Headers\MaterialCollectionUI.h"
#include "UI_Manager.h"

CMaterialCollectionUI::CMaterialCollectionUI(_Device pDevice)
	: CUI(pDevice)
{
}

CMaterialCollectionUI::CMaterialCollectionUI(const CMaterialCollectionUI & rhs)
	: CUI(rhs)
{
}

HRESULT CMaterialCollectionUI::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CMaterialCollectionUI::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;
	CUI::Ready_GameObject(pArg);

	SetUp_Default();

	return NOERROR;
}

_int CMaterialCollectionUI::Update_GameObject(_double TimeDelta)
{
	CUI::Update_GameObject(TimeDelta);

	Update_SubUI();
	Click_SubUI();
	return NO_EVENT;
}

HRESULT CMaterialCollectionUI::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	return NOERROR;
}

void CMaterialCollectionUI::SetUp_Default()
{
	CMaterial_Slot* pSlot = nullptr;
	LOOP(3)
	{
		pSlot = static_cast<CMaterial_Slot*>(g_pManagement->Clone_GameObject_Return(L"GameObject_MaterialSlot", nullptr));
		pSlot->Set_UI_Size(50.f, 50.f);
		g_pManagement->Add_GameOject_ToLayer_NoClone(pSlot, SCENE_MORTAL, L"Layer_PlayerUI", nullptr);
	}
}

void CMaterialCollectionUI::Update_SubUI()
{
	_uint iIdx = 0;
	for (auto& iter : m_vecSlot)
	{
		iter->Set_UI_Pos(80.f + 52.f * (iIdx % 5), 200.f + 52.f * (iIdx / 5));
		iter->Set_Active(m_bIsActive);
		iIdx++;
	}
}

void CMaterialCollectionUI::Click_SubUI()
{
	if (!m_bIsActive)
		return;

	for (auto& iter : m_vecSlot)
	{
		
	}
}

CMaterialCollectionUI * CMaterialCollectionUI::Create(_Device pGraphic_Device)
{
	CMaterialCollectionUI* pInstance = new CMaterialCollectionUI(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CMaterialCollectionUI::Clone_GameObject(void * pArg)
{
	CMaterialCollectionUI* pInstance = new CMaterialCollectionUI(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMaterialCollectionUI::Free()
{
	Safe_Release(m_pTransformCom);

	CUI::Free();
}
