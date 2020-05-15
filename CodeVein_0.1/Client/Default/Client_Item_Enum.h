#pragma once
enum ITEM_TYPE
{
	ITEM_MATERIAL,		//���
	ITEM_EXPENDABLES,	//�Ҹ�ǰ
	ITEM_WEAPON,		//����
	ITEM_PET,			//��
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
	// ����
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

	// ������
	NAMETYPE_Expend_MaximumUp,		// ������ �ִ� ���� ���� ����
	NAMETYPE_Expend_Hp,				// ü�� ������
	NAMETYPE_Expend_Return,			// ��ȯ ������
	NAMETYPE_Expend_Blood,			// ���� ������
	NAMETYPE_Expend_Cheet,			// ġƮ ������
	NAMETYPE_Expend_SuperArmor,		// ���� �Ƹ�

	// ���
	NAMETYPE_Queen_Steel,
	NAMETYPE_Queen_Titanium,
	NAMETYPE_Queen_Tungsten,

	NAMETYPE_End
};