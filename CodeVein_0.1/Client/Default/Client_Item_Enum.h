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
	NAMETYPE_WpnAll_Gun_Bayonet,	// 바요네트
	NAMETYPE_WpnAll_Gun_ReverTardor,// 리베르타도르
	NAMETYPE_WpnAll_Gun_Nakil,		// 낙일의 총검
	NAMETYPE_WpnAll_Gun_DarkBayonet,// 타락한 바요네트
	NAMETYPE_WpnAll_Halberd_SolidPiercer,// 솔리드피어서
	NAMETYPE_WpnAll_Halberd_Black,	// 흑의의 도끼창
	NAMETYPE_WpnAll_Halberd_DarkBodish,// 타락한 바디시
	NAMETYPE_WpnAll_Halberd_Nakil,	// 낙일의 도끼창
	NAMETYPE_WpnAll_Hammer_ImperseAnker,// 임펄스 앵커
	NAMETYPE_WpnAll_Hammer_Black,	// 불굴의 대형 해머
	NAMETYPE_WpnAll_Hammer_DarkHeavyAxe,// 타락한 헤비 액스
	NAMETYPE_WpnAll_Hammer_Nakil, // 낙일의 대형 워해머
	NAMETYPE_WpnAll_LSword_Tsubai,// 츠바이헨더
	NAMETYPE_WpnAll_LSword_Black, // 흑의의 대검
	NAMETYPE_WpnAll_LSword_Nakil, // 낙일의 양손검
	NAMETYPE_WpnAll_LSword_DarkTsubai, //타락한 츠바이헨더
	NAMETYPE_WpnAll_SSword_Red,// 진홍의 장검
	NAMETYPE_WpnAll_SSword_Executioner,// 엑스큐셔너
	NAMETYPE_WpnAll_SSword_Nakil, // 낙일의 검
	NAMETYPE_WpnAll_SSword_DarkBrodeSword, // 타락한 브로드 소드

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

	// 블러드코드
	NAMETYPE_BloodCode_Artemis,
	NAMETYPE_BloodCode_Assassin,
	NAMETYPE_BloodCode_DarkKnight,
	NAMETYPE_BloodCode_Queen,
	NAMETYPE_BloodCode_Berserker,
	NAMETYPE_BloodCode_Hephaestus,
	NAMETYPE_BloodCode_Fighter,
	NAMETYPE_BloodCode_Heimdal,
	NAMETYPE_BloodCode_Hermes,
	NAMETYPE_BloodCode_Atlas,
	NAMETYPE_BloodCode_Prometheus,

	// 펫
	NAMETYPE_Pet_PoisonButterfly,
	NAMETYPE_Pet_DeerKing,

	NAMETYPE_End
};

enum MONSTER_NAMETYPE
{
	// 일반 몬스터
	M_Cocoon,
	M_GunGenji,
	M_SwordGenji,
	M_SwordShieldGenji,
	M_Hunter,
	M_Monkey,
	M_ThaiMan,
	M_Urchin,
	M_Wolf,
	M_YachaMan,
	M_Yeti,

	// 보스 몬스터
	M_DeerKing,
	M_FireBoy,
	M_IceGirl,
	M_PoisonButterfly,
	M_QueensKnight

};