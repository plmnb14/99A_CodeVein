#include "..\Headers\UI.h"


CUI::CUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	D3DXMatrixIdentity(&m_matOldView);
	D3DXMatrixIdentity(&m_matOldProj);

	
	
}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
{
	
}

void CUI::Set_UI_Pos(_float fPosX, _float fPosY)
{
	m_fPosX = fPosX;
	m_fPosY = fPosY;
}

void CUI::Set_UI_Size(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
}

HRESULT CUI::Ready_GameObject_Prototype()
{
	
	return NOERROR;
}

HRESULT CUI::Ready_GameObject(void * pArg)
{
	if (nullptr != pArg)
	{
		m_pUIDesc = static_cast<UI_DESC*>(pArg);

		m_fPosX = m_pUIDesc->fPosX;
		m_fPosY = m_pUIDesc->fPosY;
		m_fSizeX = m_pUIDesc->fSizeX;
		m_fSizeY = m_pUIDesc->fSizeY;
		m_iIndex = m_pUIDesc->iIndex;
	}
	else
	{
		m_fPosX = 0.f;
		m_fPosY = 0.f;
		m_fSizeX = 0.f;
		m_fSizeY = 0.f;
		m_iIndex = 0;
	}

	return NOERROR;
}

_int CUI::Update_GameObject(_double TimeDelta)
{
	CGameObject::Update_GameObject(TimeDelta);

	return NO_EVENT;
}

_int CUI::Late_Update_GameObject(_double TimeDelta)
{
	return 0;
}

HRESULT CUI::Render_GameObject()
{
	return NOERROR;
}



void CUI::Free()
{
	Safe_Delete(m_pUIDesc);
	CGameObject::Free();
}
