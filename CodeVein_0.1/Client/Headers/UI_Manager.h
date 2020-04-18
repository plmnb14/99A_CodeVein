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
	CExpendables::EXPEND_TYPE	Use_Item(); // 퀵슬롯에서 아이템 사용시 해당 아이템 제거 및 사용 아이템 타입 반환
	WEAPON_STATE Get_UseWeaponState(_uint iIndex); // 해당 인덱스에 설정된 무기 타입 반환
	CArmor::ARMOR_TYPE Get_UseArmorType(); // 선택한 장비(아장)타입 반환
	void OnOff_BossUI(_bool bIsActive); // 보스UI On/Off(true일 경우 활성화)
	void Add_Expendables(CExpendables::EXPEND_TYPE eType, _uint iCnt = 1); // 소비 아이템 획득 함수 -> 아이템 타입, 개수 입력
	void Add_Material(CMaterial::MATERIAL_TYPE eType, _uint iCnt = 1); // 재료 아이템 획득
	void Add_Weapon(WEAPON_STATE eType); // 무기 획득 함수
	void Add_Armor(CArmor::ARMOR_TYPE eType); // 아장 획득
	_bool Get_UI_Active(const _tchar* pLayerTag); // 해당 레이어 태그 입력하면 UI 활성화 여부 반환
	void Set_UI_Active(const _tchar* pLayerTag, _bool bIsActive); // 해당 레이어의 UI 활성화/비활성화
	void Set_BossHP_Active(_bool bIsActive);

private:
	_uint m_uiCoundItem = 0;
	_bool m_bTest = false;
public:
	virtual void Free();
};

END