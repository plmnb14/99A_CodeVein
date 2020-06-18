#pragma once

#include "Client_Item_Enum.h"
#include "UI_Manager.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CDropItem :public CGameObject
{
public:
	struct ITEM_STATUS
	{
		ITEM_STATUS(ITEM_GRADE_TYPE _eGrade, ITEM_NAMETYPE _eItemName, _v3 _vBirthPos, _double _dLimitTime)
			: eItemGradeType(_eGrade), eItem_NameType(_eItemName), vBirthPos(_vBirthPos), dCanGetLimitTimeMax(_dLimitTime)
		{
		}

		ITEM_GRADE_TYPE		eItemGradeType; //아이템등급
		ITEM_NAMETYPE		eItem_NameType; //아이템이름
		_v3					vBirthPos; //생성위치
		_double				dCanGetLimitTimeMax; //소멸제한 시간
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

public:
	_uint	Get_Count_GetItem() { return m_iCount_GetItem; }

private:
	void Check_Dist();
	void Check_Condition();

protected:
	HRESULT Add_Component(void* _pArg);
	HRESULT Ready_Status(void * _pArg);
	HRESULT SetUp_ConstantTable(CShader* pShader);

public:
	static CDropItem* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void * pArg) override;
	virtual void Free();
 
private:
	CRenderer*						m_pRenderer = nullptr;
	CTransform*						m_pTransform = nullptr;
	COptimization*					m_pOptimization = nullptr;
	CGet_ItemUI*					m_pGet_ItemUI = nullptr;
	CPickUp_ItemUIManager*			m_pPickUpMgr = nullptr;

	ITEM_GRADE_TYPE					m_eItemGrade = ITEM_GRADE_TYPE::ITEM_GRADE_TYPE_END;
	ITEM_NAMETYPE					m_eItem_NameType = ITEM_NAMETYPE::NAMETYPE_End;

	_double							m_dCanGetItemLimitTimeCur = 0; //누적을 위한 값
	_double							m_dCanGetItemLimitTimeMax = 0; //시간값이라 double형, 해당 시간이 지나면 획득 불가, 사망

	_float							m_fCanGetDist = 1.5f; //획득 가능범위
	
	_bool							m_bCanGetItem = false; //항시 false 상호작용시 true
	_bool							m_bCheck_Start_GetItemUI = false;

	_uint							m_iRenderIndex = 0;
	_uint							m_iCount_GetItem = 0;
};

END