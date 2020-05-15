#pragma once
enum ITEM_TYPE
{
	ITEM_MATERIAL,		//재료
	ITEM_EXPENDABLES,	//소모품
	ITEM_WEAPON,		//무기
	ITEM_PET,			//펫
	ITEM_TYPE_END
};

enum ITEM_GRADE_TYPE
{
	ITEM_GRADE_NORMAL,
	ITEM_GRADE_RARE,
	ITEM_GRADE_UNIQUE,
	ITEM_GRADE_LEGEND,
	ITEM_GRADE_TYPE_END
};

enum ITEM_NAMETYPE
{
	// 무기
	NAMETYPE_WpnAll_Gun_Bayonet,
	NAMETYPE_WpnAll_Gun_ReverTardor,
	NAMETYPE_WpnAll_Gun_Nakil,
	NAMETYPE_WpnAll_Gun_DarkBayonet,
	NAMETYPE_WpnAll_Halberd_SolidPiercer,
	NAMETYPE_WpnAll_Halberd_Black,
	NAMETYPE_WpnAll_Halberd_DarkBodish,
	NAMETYPE_WpnAll_Halberd_Nakil,
	NAMETYPE_WpnAll_Hammer_ImperseAnker,
	NAMETYPE_WpnAll_Hammer_Black,
	NAMETYPE_WpnAll_Hammer_DarkHeavyAxe,
	NAMETYPE_WpnAll_Hammer_Nakil,
	NAMETYPE_WpnAll_LSword_Tsubai,
	NAMETYPE_WpnAll_LSword_Black,
	NAMETYPE_WpnAll_LSword_Nakil,
	NAMETYPE_WpnAll_LSword_DarkTsubai,
	NAMETYPE_WpnAll_SSword_Red,
	NAMETYPE_WpnAll_SSword_Executioner,
	NAMETYPE_WpnAll_SSword_Nakil,
	NAMETYPE_WpnAll_SSword_DarkBrodeSword,

	// 아이템
	NAMETYPE_Expend_MaximumUp,		// 아이템 최대 소지 개수 증가
	NAMETYPE_Expend_Hp,				// 체력 아이템
	NAMETYPE_Expend_Return,			// 귀환 아이템
	NAMETYPE_Expend_Blood,			// 영혈 아이템
	NAMETYPE_Expend_Cheet,			// 치트 아이템
	NAMETYPE_Expend_SuperArmor,		// 슈퍼 아머

	// 재료
	NAMETYPE_Queen_Steel,
	NAMETYPE_Queen_Titanium,
	NAMETYPE_Queen_Tungsten,

	NAMETYPE_End
};