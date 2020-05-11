#pragma once

#include "Client_Item_Enum.h"
#include "UI_Manager.h"
#include "GameObject.h"
#include "Management.h"
#include "Trail_VFX.h"
#include "Effect.h"

#include "Expendables.h"
#include "Material.h"
#include "Weapon.h"
#include "Pet.h"

BEGIN(Client)

class CDropItem :public CGameObject
{
public:
	struct ITEM_STATUS
	{
		ITEM_STATUS(ITEM_TYPE _eType, ITEM_GRADE_TYPE _eGrade, CMaterial::MATERIAL_TYPE _eMaterial, _v3 _vBirthPos, _double _dLimitTime)
			:eItemType(_eType), eItemGradeType(_eGrade), eMaterialType(_eMaterial), vBirthPos(_vBirthPos), dCanGetLimitTimeMax(_dLimitTime)
		{
		}

		ITEM_STATUS(ITEM_TYPE _eType, ITEM_GRADE_TYPE _eGrade, CExpendables::EXPEND_TYPE _eExpendables, _v3 _vBirthPos, _double _dLimitTime)
			:eItemType(_eType), eItemGradeType(_eGrade), eExpendablesType(_eExpendables), vBirthPos(_vBirthPos), dCanGetLimitTimeMax(_dLimitTime)
		{
		}

		ITEM_STATUS(ITEM_TYPE _eType, ITEM_GRADE_TYPE _eGrade, WEAPON_DATA _eWeapon, _v3 _vBirthPos, _double _dLimitTime)
			:eItemType(_eType), eItemGradeType(_eGrade), eWeaponType(_eWeapon), vBirthPos(_vBirthPos), dCanGetLimitTimeMax(_dLimitTime)
		{
		}

		ITEM_STATUS(ITEM_TYPE _eType, ITEM_GRADE_TYPE _eGrade, CPet::PET_TYPE _ePet, _v3 _vBirthPos, _double _dLimitTime)
			:eItemType(_eType), eItemGradeType(_eGrade), ePetType(_ePet), vBirthPos(_vBirthPos), dCanGetLimitTimeMax(_dLimitTime)
		{
		}

		ITEM_TYPE						eItemType; //�ֻ��� - ������ ����
		ITEM_GRADE_TYPE					eItemGradeType; //�ֻ��� - ������ ���

		CMaterial::MATERIAL_TYPE		eMaterialType; //���� - ��� ����

		CExpendables::EXPEND_TYPE		eExpendablesType; //���� - �Ҹ�ǰ ����

		WEAPON_DATA			eWeaponType;	//���� - ���� ����

		CPet::PET_TYPE					ePetType; //���� - �� ����

		_v3			vBirthPos; //������ġ
		_double		dCanGetLimitTimeMax; //�Ҹ����� �ð�
	};
protected:
	explicit CDropItem(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CDropItem(const CDropItem& rhs);
	virtual ~CDropItem() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject(void * pArg) override;
	virtual _int Update_GameObject(_double TimeDelta) override;
	virtual _int Late_Update_GameObject(_double TimeDelta) override;
	virtual HRESULT Render_GameObject() override;

private:
	void Check_Dist();
	void Check_PosY();

protected:
	HRESULT Add_Component(void* _pArg);
	HRESULT Ready_Status(void * _pArg);
	HRESULT SetUp_ConstantTable();

public:
	static CDropItem* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void * pArg) override;
	virtual void Free();
 
private:
	CRenderer*						m_pRenderer = nullptr;
	CTransform*						m_pTransform = nullptr;
	CTrail_VFX*						m_pTrailEffect = nullptr;
	CEffect*						m_pEffect = nullptr;

	ITEM_TYPE						m_eItemType = ITEM_TYPE::ITEM_TYPE_END; //������ ����

	ITEM_GRADE_TYPE					m_eItemGrade = ITEM_GRADE_TYPE::ITEM_GRADE_TYPE_END; //��� ����

	CMaterial::MATERIAL_TYPE		m_eMaterialType = CMaterial::MATERIAL_TYPE::MATERIAL_END; //��� ����

	CExpendables::EXPEND_TYPE		m_eExpendablesType = CExpendables::EXPEND_TYPE::EXPEND_END; //�Ҹ�ǰ ����

	WEAPON_DATA						m_eWeaponType = WEAPON_DATA::WPN_DATA_End;	//���� ����

	CPet::PET_TYPE					m_ePetType = CPet::PET_TYPE::PET_TYPE_END; //�� ����

	_double							m_dCanGetItemLimitTimeCur = 0; //������ ���� ��
	_double							m_dCanGetItemLimitTimeMax = 0; //�ð����̶� double��, �ش� �ð��� ������ ȹ�� �Ұ�, ���
	_float							m_fTempEffectLimitTime = 0.f; //����Ʈ�� ���� ����, �ӽ÷� ���� ����, ���� ����
	_float							m_fCanGetDist = 1.5f; //ȹ�� ���ɹ���
	_bool							m_bCanGetItem = false; //�׽� false ��ȣ�ۿ�� true
};

END