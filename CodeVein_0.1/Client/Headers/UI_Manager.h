#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "Item_Manager.h"


BEGIN(Client)

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)

private:
	explicit CUI_Manager();
	virtual ~CUI_Manager();

public:
	_uint Get_CoundItem() { return m_uiCoundItem; }
	void Set_CoundItem(_uint _Cound) { m_uiCoundItem = _Cound; }

public:
	HRESULT Add_UI_Prototype(_Device pDevice);
	HRESULT SetUp_UILayer();
	_int	Update_UI();
	CExpendables::EXPEND_TYPE	Use_Item(); // �����Կ��� ������ ���� �ش� ������ ���� �� ��� ������ Ÿ�� ��ȯ
	WEAPON_STATE Get_UseWeaponState(_uint iIndex); // �ش� �ε����� ������ ���� Ÿ�� ��ȯ
	CArmor::ARMOR_TYPE Get_UseArmorType(); // ������ ���(����)Ÿ�� ��ȯ
	void OnOff_BossUI(_bool bIsActive); // ����UI On/Off(true�� ��� Ȱ��ȭ)
	void Add_Expendables(CExpendables::EXPEND_TYPE eType, _uint iCnt = 1); // �Һ� ������ ȹ�� �Լ� -> ������ Ÿ��, ���� �Է�
	void Add_Material(CMaterial::MATERIAL_TYPE eType, _uint iCnt = 1); // ��� ������ ȹ��
	void Add_Weapon(WEAPON_STATE eType); // ���� ȹ�� �Լ�
	void Add_Armor(CArmor::ARMOR_TYPE eType); // ���� ȹ��
	_bool Get_UI_Active(const _tchar* pLayerTag); // �ش� ���̾� �±� �Է��ϸ� UI Ȱ��ȭ ���� ��ȯ
	void Set_UI_Active(const _tchar* pLayerTag, _bool bIsActive); // �ش� ���̾��� UI Ȱ��ȭ/��Ȱ��ȭ
	void Set_BossHP_Active(_bool bIsActive);

private:
	_uint m_uiCoundItem = 0;
	_bool m_bTest = false;
public:
	virtual void Free();
};

END