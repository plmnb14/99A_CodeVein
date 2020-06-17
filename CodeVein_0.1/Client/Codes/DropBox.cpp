#include "stdafx.h"
#include "DropBox.h"


CDropBox::CDropBox(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CDropBox::CDropBox(const CDropBox& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDropBox::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CDropBox::Ready_GameObject(void * pArg)
{
	return S_OK;
}

_int CDropBox::Update_GameObject(_double TimeDelta)
{
	if (nullptr == m_pTarget)
		return E_FAIL;

	// 플레이어가 일정 거리 안에 들어오고 특정 키를 눌렀을 때 뚜껑이 열린다.

	_float fLength = D3DXVec3Length(&(TARGET_TO_TRANS(m_pTarget)->Get_Pos() - m_pTransform->Get_Pos()));

	if (5.f >= fLength)
	{
		// 박스를 열지 말지 선택한다. if문 안에 넣어놓은 것은 추후 UI를 추가할지도 모르기 때문에
		if (g_pInput_Device->Key_Down(DIK_F4) && false == m_bOpenBox)
		{
			// 박스 뚜껑을 연다
			m_bOpenBox = true;
			if (1.f >= m_pTransform->Get_Angle(AXIS_X))
				m_pTransform->Add_Angle(AXIS_X, 1.f);

		}
	}

	return S_OK;
}

_int CDropBox::Late_Update_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CDropBox::Render_GameObject()
{
	
	return S_OK;
}

HRESULT CDropBox::Add_Component(void * _pArg)
{
	return S_OK;
}

HRESULT CDropBox::Ready_Status(void * _pArg)
{
	return S_OK;
}

HRESULT CDropBox::SetUp_ConstantTable(CShader * pShader)
{
	return S_OK;
}

HRESULT CDropBox::SetUp_Default()
{
	m_pTarget = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);



	return S_OK;
}

HRESULT CDropBox::Default_Setting()
{
	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);

	//===================================================================
	// 림라이트 벨류 설정	
	//===================================================================
	m_pBattleAgent->Set_RimAlpha(0.5f);
	m_pBattleAgent->Set_RimValue(8.f);
	m_pBattleAgent->Set_OriginRimAlpha(0.5f);
	m_pBattleAgent->Set_OriginRimValue(8.f);
	//===================================================================

	return S_OK;
}



CDropBox* CDropBox::Create(_Device pGraphic_Device)
{
	CDropBox*	pInstance = new CDropBox(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CDropBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDropBox::Clone_GameObject(void * pArg)
{
	CDropBox*	pInstance = new CDropBox(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CDropBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDropBox::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pCollider);
	Safe_Release(m_pMesh_Static);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pShader);
	Safe_Release(m_pOptimization);
	Safe_Release(m_pBattleAgent);

	CGameObject::Free();
}
