#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CExpendables final : public CGameObject
{
public:
	enum EXPEND_TYPE
	{
		Expend_MaximumUp,		// ������ �ִ� ���� ���� ����
		Expend_Hp,				// ü�� ������
		Expend_Return,			// ��ȯ ������
		Expend_Blood,			// ���� ������
		Expend_Cheet,			// ġƮ ������
		Expend_SuperArmor,		// ���� �Ƹ�
		EXPEND_END
	};

private:
	explicit CExpendables(_Device pDevice);
	explicit CExpendables(const CExpendables& rhs);
	virtual ~CExpendables() = default;

public:
	void Set_Type(EXPEND_TYPE eType) { m_eType = eType; }

public:
	EXPEND_TYPE Get_Type() { return m_eType; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);

private:
	EXPEND_TYPE m_eType;

public:
	static CExpendables*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

