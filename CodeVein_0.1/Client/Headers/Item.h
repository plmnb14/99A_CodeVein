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
		ITEM_MATERIAL, //���
		ITEM_EXPENDABLES, //�Ҹ�ǰ
		ITEM_WEAPON, //����
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

		ITEM_TYPE	eItemType; //����������
		ITEM_GRADE_TYPE	eItemGradeType; //�����۵��
		_v3			vBirthPos; //������ġ
		_double		dCanGetLimitTimeMax; //�Ҹ����� �ð�
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

	//��ȣ�ۿ� ���(Player,Pet)�� �浹�˻�
	virtual void Check_PosY();
	virtual void Check_Dist();
	virtual void Update_Collider();
	virtual void Render_Collider();
	virtual void Check_CollisionHit();
	//�ش� �ð��� �� �Ǹ� ȹ��Ұ�,�ش�����ۼҸ�
	virtual void Accumulate_CanGetLimitTime(_double _TimeDelta) { m_dCanGetItemLimitTimeCur += _TimeDelta; }
	//������ȹ�氡��?
	virtual void Play_GetItem() { m_bCanGetItem = true; } //������ ȹ�� ����

	virtual void Check_Ypos(); //�׺�޽�y���� �� Ÿ�� �ִ��� Ȯ��

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
	_double				m_dCanGetItemLimitTimeCur = 0; //������ ���� ��
	_double				m_dCanGetItemLimitTimeMax = 0; //�ð����̶� double��, �ش� �ð��� ������ ȹ�� �Ұ�, ���
	_bool				m_bCanGetItem = false; //�׽� false ��ȣ�ۿ�� true

	//������ �߰�-> �����ۿ� ȹ���ϴ� �ൿ-> m_bCanGetItem = true-> m_bDead = true, �Ѵ� Ʈ���� ��� �ش� ������ ȹ��
	//ȹ�� ���� �ð��� �ɸ� ��� m_bDead true, m_bCanGetItem = false

};

END

