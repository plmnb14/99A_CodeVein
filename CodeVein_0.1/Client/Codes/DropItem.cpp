#include "stdafx.h"
#include "..\Headers\DropItem.h"

CDropItem::CDropItem(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CGameObject(pGraphic_Device)
{
}

CDropItem::CDropItem(const CDropItem & rhs)
	:CGameObject(rhs)
{
}

HRESULT CDropItem::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CDropItem::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component(pArg)))
		return E_FAIL;

	Ready_Status(pArg);

	m_pEffect = static_cast<CEffect*>(g_pManagement->Clone_GameObject_Return(L"Totem_Fire_BulletBody", nullptr));
	m_pEffect->Set_Desc(V3_NULL, m_pTransform);
	m_pEffect->Reset_Init();
	g_pManagement->Add_GameOject_ToLayer_NoClone(m_pEffect, SCENE_STAGE, L"Layer_Item", nullptr);

	return S_OK;
}

_int CDropItem::Update_GameObject(_double TimeDelta)
{
	if (false == m_bEnable)
		return NO_EVENT;

	CGameObject::Update_GameObject(TimeDelta);

	m_dCanGetItemLimitTimeCur += TimeDelta;
	m_fTempEffectLimitTime += (_float)TimeDelta;

	//Check_PosY(); //Ȥ�� �𸣴� �׺�y
	//��ȣ�ۿ� ���� �浹 ���� üũ
	Check_Dist();

	//if(true == m_bCanGetItem)
	//{
	//	"�ݴ´�" UI �ߵ�
	//}

	//0.05�ʸ��� ����Ʈȿ�� �߻�
	if (m_fTempEffectLimitTime > 0.05f)
	{
		m_fTempEffectLimitTime = 0.f;
		cout << "������ ������ ȿ�� �߻�" << endl;
		switch (m_eItemGrade)
		{
		case ITEM_GRADE_NORMAL:
			g_pManagement->Create_Effect(L"ItemObject", m_pTransform->Get_Pos());
			break;
		case ITEM_GRADE_RARE:
			g_pManagement->Create_Effect(L"ItemObject_Purple", m_pTransform->Get_Pos());
			break;
		case ITEM_GRADE_UNIQUE:
			g_pManagement->Create_Effect(L"ItemObject_Red", m_pTransform->Get_Pos());
			break;
		case ITEM_GRADE_LEGEND:
			g_pManagement->Create_Effect(L"ItemObject_Yellow", m_pTransform->Get_Pos());
			break;
		}
	}

	//1.���� �ð� �ʰ��� ����
	if (m_dCanGetItemLimitTimeMax <= m_dCanGetItemLimitTimeCur)
	{
		//������ ���
		switch (m_eItemGrade)
		{
		case ITEM_GRADE_NORMAL:
			break;
		case ITEM_GRADE_RARE:
			break;
		case ITEM_GRADE_UNIQUE:
			break;
		case ITEM_GRADE_LEGEND:
			break;
		}
		//�ܼ� �Ҹ�
		//g_pManagement->Create_Effect(L"Totem_Fire_Bullet_Dead_0", m_pTransform->Get_Pos());
		//g_pManagement->Create_Effect(L"Totem_Fire_Bullet_Dead_1", m_pTransform->Get_Pos());
		//g_pManagement->Create_Effect(L"Totem_Fire_Bullet_Dead_Particle", m_pTransform->Get_Pos());
		//m_pEffect->Set_Dead();
		m_bEnable = false;
		return DEAD_OBJ;
	}
	//2.���� �ð� �̳�
	else
	{
		if (true == m_bCanGetItem)
		{
			m_bCanGetItem = false;

			//"�ݴ´�" ���� ������ ���� ��쵵 �����Ұ�
			//��������Ʈ,��ƼŬ����Ʈ ��� ȹ���� Ƽ�� ���ְ�
			//ȹ�� ui�� ��������� ("ȹ��")
			//�κ����� ����

			//������ ���
			switch (m_eItemGrade)
			{
			case ITEM_GRADE_NORMAL:
				break;
			case ITEM_GRADE_RARE:
				break;
			case ITEM_GRADE_UNIQUE:
				break;
			case ITEM_GRADE_LEGEND:
				break;
			}
			//������ ����
			switch (m_eItemType)
			{
			case ITEM_MATERIAL:
				CUI_Manager::Get_Instance()->Get_Material_Inven()->Add_Material(m_eMaterialType);
				break;
			case ITEM_EXPENDABLES:
				CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(m_eExpendablesType);
				break;
			case ITEM_WEAPON:
				//CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon();
				break;
			case ITEM_PET:
				CUI_Manager::Get_Instance()->Get_Pet_Inven()->Add_Pet(m_eItemGrade, m_ePetType);
				break;
			}
			//"�ݴ´�" ���� ������ ���� ��쵵 �����Ұ�
			//��������Ʈ,��ƼŬ����Ʈ ��� ȹ���� Ƽ�� ���ְ�
			//ȹ�� ui�� ��������� ("�ݴ´�")
			//�κ����� ����
			//switch (m_eItemType)
			//{
			//case ITEM_TYPE::ITEM_MATERIAL:
			//	CUI_Manager::Get_Instance()->Get_Material_Inven()->Add_Material(m_eMaterialType);
			//	break;
			//case ITEM_TYPE::ITEM_EXPENDABLES:
			//	CUI_Manager::Get_Instance()->Get_Expendables_Inven()->Add_Expendables(m_eExpendablesType);
			//	break;
			//case ITEM_TYPE::ITEM_WEAPON:
			//	//CUI_Manager::Get_Instance()->Get_Weapon_Inven()->Add_Weapon(); //����Ÿ���� �޾Ƽ� �ش�������� �߰��ǰԲ�
			//	break;
			//case ITEM_TYPE::ITEM_PET:
			//	CUI_Manager::Get_Instance()->Get_Pet_Inven()->Add_Pet(m_eItemGrade, m_ePetType);
			//	break;
			//}

			//g_pManagement->Create_Effect(L"Bullet_DeadFlash", m_pTransform->Get_Pos(), nullptr);
			//g_pManagement->Create_Effect(L"Bullet_DeadSmoke_Base", m_pTransform->Get_Pos(), nullptr);
			//g_pManagement->Create_Effect(L"Bullet_DeadSmoke_Black", m_pTransform->Get_Pos(), nullptr);
			//m_pEffect->Set_Dead();
			m_bEnable = false;

			return DEAD_OBJ;
		}
	}

	return NO_EVENT;
}

_int CDropItem::Late_Update_GameObject(_double TimeDelta)
{
	IF_NULL_VALUE_RETURN(m_pRenderer, E_FAIL);

	if (FAILED(m_pRenderer->Add_RenderList(RENDER_NONALPHA, this)))
		return E_FAIL;

	return NO_EVENT;
}

HRESULT CDropItem::Render_GameObject()
{
	return S_OK;
}

void CDropItem::Check_Dist()
{
	_float fDist;
	//p�� ����, �ൿo, ���� �ð� �̳���� �Ұ� ��
	//p�� ����, �ൿo, ���� �ð� �ʰ���� �Ұ� ����

	//p�� ����, �ൿx ��ȹ�� �Ұ� ����

	//p���� �Ÿ��� ����� ��� ��, �ƴѰ�� ����
	auto& listPlayer = g_pManagement->Get_GameObjectList(L"Layer_Player", SCENE_MORTAL);
	auto& listPet = g_pManagement->Get_GameObjectList(L"Layer_Pet", SCENE_STAGE);

	for (auto& Player_iter : listPlayer)
	{
		if (true == Player_iter->Get_Dead())
			continue;
		else if (false == Player_iter->Get_Enable())
			continue;
		else if (nullptr == Player_iter)
			continue;

		fDist = V3_LENGTH(&(TARGET_TO_TRANS(Player_iter)->Get_Pos() - m_pTransform->Get_Pos()));

		if (m_fCanGetDist >= fDist)
		{
			m_bCanGetItem = true;
			return;
		}
		else
			m_bCanGetItem = false;
	}

	for (auto& Pet_iter : listPet)
	{
		if (true == Pet_iter->Get_Dead())
			continue;
		else if (false == Pet_iter->Get_Enable())
			continue;
		else if (nullptr == Pet_iter)
			continue;

		fDist = V3_LENGTH(&(TARGET_TO_TRANS(Pet_iter)->Get_Pos() - m_pTransform->Get_Pos()));

		if (m_fCanGetDist >= fDist)
		{
			m_bCanGetItem = true;
			return;
		}
		else
			m_bCanGetItem = false;
	}

	return;
}

void CDropItem::Check_PosY()
{
	//m_pTransform->Set_Pos(m_pNavMesh->Axis_Y_OnNavMesh(m_pTransform->Get_Pos()));

	return;
}

HRESULT CDropItem::Add_Component(void* _pArg)
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	//// For.Com_NaviMesh
	//if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"NavMesh", L"Com_NavMesh", (CComponent**)&m_pNavMesh)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CDropItem::Ready_Status(void* _pArg)
{
	if (nullptr == _pArg)
	{
		m_eItemType = ITEM_TYPE::ITEM_MATERIAL;
		m_eItemGrade = ITEM_GRADE_TYPE::ITEM_GRADE_NORMAL;

		m_eMaterialType = CMaterial::MATERIAL_TYPE::MATERIAL_1;

		m_pTransform->Set_Pos(_v3{ 1.f, 0.f, 1.f });
		m_dCanGetItemLimitTimeMax = 5;
	}
	else
	{
		ITEM_STATUS info = *(ITEM_STATUS*)_pArg;
		m_eItemType = info.eItemType;

		switch (m_eItemType)
		{
		case ITEM_TYPE::ITEM_MATERIAL:
			m_eMaterialType = info.eMaterialType;
			break;
		case ITEM_TYPE::ITEM_EXPENDABLES:
			m_eExpendablesType = info.eExpendablesType;
			break;
		case ITEM_TYPE::ITEM_WEAPON:
			m_eWeaponType = info.eWeaponType;
			break;
		case ITEM_TYPE::ITEM_PET:
			m_ePetType = info.ePetType;
		}

		m_eItemGrade = info.eItemGradeType;
		m_dCanGetItemLimitTimeMax = info.dCanGetLimitTimeMax;
		m_pTransform->Set_Pos(info.vBirthPos);
	}

	m_dCanGetItemLimitTimeCur = 0;
	m_pTransform->Set_Scale(V3_ONE);

	return S_OK;
}

HRESULT CDropItem::SetUp_ConstantTable()
{
	return S_OK;
}

CDropItem* CDropItem::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CDropItem* pInstance = new CDropItem(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CDropItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDropItem::Clone_GameObject(void * pArg)
{
	CDropItem* pInstance = new CDropItem(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CDropItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDropItem::Free()
{
	Safe_Release(m_pTransform);
	//Safe_Release(m_pNaviMesh);
	Safe_Release(m_pRenderer);

	CGameObject::Free();

	return;
}
