#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"
#include "Trail_VFX.h"
#include "Effect.h"

BEGIN(Client)

class CItem : public CGameObject
{
public:
	enum ITEM_TYPE
	{
		ITEM_MATERIAL, //재료
		ITEM_EXPENDABLES, //소모품
		ITEM_WEAPON, //무기
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

	struct ITEM_STATUS
	{
		ITEM_STATUS(ITEM_TYPE _eType, ITEM_GRADE_TYPE _eGrade, _v3 _vBirthPos, _double _dLimitTime)
			:eItemType(_eType), eItemGradeType(_eGrade), vBirthPos(_vBirthPos), dCanGetLimitTimeMax(_dLimitTime)
		{
		}

		ITEM_TYPE	eItemType; //아이템종류
		ITEM_GRADE_TYPE	eItemGradeType; //아이템등급
		_v3			vBirthPos; //생성위치
		_double		dCanGetLimitTimeMax; //소멸제한 시간
	};

protected:
	explicit CItem(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CItem(const CItem& rhs);
	virtual ~CItem() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() PURE;
	virtual HRESULT Ready_GameObject(void* pArg) PURE;
	virtual _int Update_GameObject(_double TimeDelta) PURE;
	virtual _int Late_Update_GameObject(_double TimeDelta) PURE;
	virtual HRESULT Render_GameObject() PURE;
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass);

	//상호작용 대상(Player,Pet)의 충돌검사
	virtual void Check_PosY();
	virtual void Check_Dist();
	virtual void Update_Collider();
	virtual void Render_Collider();
	virtual void Check_CollisionHit();
	//해당 시간이 다 되면 획득불가,해당아이템소멸
	virtual void Accumulate_CanGetLimitTime(_double _TimeDelta) { m_dCanGetItemLimitTimeCur += _TimeDelta; }
	//아이템획득가능?
	virtual void Play_GetItem() { m_bCanGetItem = true; } //아이템 획득 가능

	virtual void Check_Ypos(); //네비메쉬y축을 잘 타고 있는지 확인

protected:
	virtual HRESULT Add_Component(void* _pArg) PURE;
	virtual HRESULT SetUp_ConstantTable() PURE;
	virtual HRESULT Ready_Status(void* _pArg);
	virtual HRESULT Ready_Collider();

public:
	virtual CGameObject * Clone_GameObject(void * pArg) PURE;
	virtual void Free();

protected:
	CRenderer*			m_pRenderer = nullptr;
	CTransform*			m_pTransform = nullptr;
	CCollider*			m_pCollider = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CTrail_VFX*			m_pTrailEffect = nullptr;
	CEffect*			m_pEffect = nullptr;

	ITEM_TYPE			m_eItemType = ITEM_TYPE_END;
	ITEM_GRADE_TYPE		m_eItemGrade = ITEM_GRADE_TYPE_END;

	_float				m_fCanGetDist= 1.5f;
	_double				m_dCanGetItemLimitTimeCur = 0; //누적을 위한 값
	_double				m_dCanGetItemLimitTimeMax = 0; //시간값이라 double형, 해당 시간이 지나면 획득 불가, 사망
	_bool				m_bCanGetItem = false; //항시 false 상호작용시 true

	//아이템 발견-> 아이템에 획득하는 행동-> m_bCanGetItem = true-> m_bDead = true, 둘다 트루인 경우 해당 아이템 획득
	//획득 제한 시간이 걸린 경우 m_bDead true, m_bCanGetItem = false

};

END

