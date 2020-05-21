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
	NAMETYPE_WpnAll_Gun_Bayonet,	// �ٿ��Ʈ
	NAMETYPE_WpnAll_Gun_ReverTardor,// ������Ÿ����
	NAMETYPE_WpnAll_Gun_Nakil,		// ������ �Ѱ�
	NAMETYPE_WpnAll_Gun_DarkBayonet,// Ÿ���� �ٿ��Ʈ
	NAMETYPE_WpnAll_Halberd_SolidPiercer,// �ָ����Ǿ
	NAMETYPE_WpnAll_Halberd_Black,	// ������ ����â
	NAMETYPE_WpnAll_Halberd_DarkBodish,// Ÿ���� �ٵ��
	NAMETYPE_WpnAll_Halberd_Nakil,	// ������ ����â
	NAMETYPE_WpnAll_Hammer_ImperseAnker,// ���޽� ��Ŀ
	NAMETYPE_WpnAll_Hammer_Black,	// �ұ��� ���� �ظ�
	NAMETYPE_WpnAll_Hammer_DarkHeavyAxe,// Ÿ���� ��� �׽�
	NAMETYPE_WpnAll_Hammer_Nakil, // ������ ���� ���ظ�
	NAMETYPE_WpnAll_LSword_Tsubai,// ���������
	NAMETYPE_WpnAll_LSword_Black, // ������ ���
	NAMETYPE_WpnAll_LSword_Nakil, // ������ ��հ�
	NAMETYPE_WpnAll_LSword_DarkTsubai, //Ÿ���� ���������
	NAMETYPE_WpnAll_SSword_Red,// ��ȫ�� ���
	NAMETYPE_WpnAll_SSword_Executioner,// ����ť�ų�
	NAMETYPE_WpnAll_SSword_Nakil, // ������ ��
	NAMETYPE_WpnAll_SSword_DarkBrodeSword, // Ÿ���� ��ε� �ҵ�

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

	// �����ڵ�
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

	// ��
	NAMETYPE_Pet_PoisonButterfly,
	NAMETYPE_Pet_DeerKing,

	NAMETYPE_End
};

enum MONSTER_NAMETYPE
{
	// �Ϲ� ����
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

	// ���� ����
	M_DeerKing,
	M_FireBoy,
	M_IceGirl,
	M_PoisonButterfly,
	M_QueensKnight

};