#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Engine)

class CYetiTrap :public CGameObject
{
public:
	struct YETITRAPINFO
	{
		YETITRAPINFO(_uint _Idx, _v3 _vPos, _v3 _vAngle, _v3 _vScale)
			:iIndex(_Idx), vPos(_vPos), vAngle(_vAngle), vScale(_vScale)
		{
		}
		_v3 vPos;
		_v3 vAngle;
		_v3 vScale;
		_uint iIndex;
	};

protected:
	explicit CYetiTrap(_Device _pGraphicDev);
	explicit CYetiTrap(const CYetiTrap& rhs);
	virtual ~CYetiTrap();

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void * pArg);
	virtual _int Update_GameObject(_double _TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur = false);
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);

	_uint	Get_YetiIndex() { return m_iIndex; }
	_bool	Get_SummonYeti() { return m_bCanSummonYeti; }

private:
	void Check_Dist();
	HRESULT	Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable(CShader* pShader);
	HRESULT Ready_Default(void* pArg);

public:
	static CYetiTrap* Create(_Device _pGraphicDev);
	virtual CGameObject* Clone_GameObject(void * pArg);
	virtual void Free();

private:
	CTransform*		m_pTransform = nullptr;
	CMesh_Static*	m_pMesh_Static = nullptr;
	CCollider*		m_pCollider = nullptr;
	CRenderer*		m_pRenderer = nullptr;
	CShader*		m_pShader = nullptr;
	CBattleAgent*	m_pBattleAgent = nullptr;
	COptimization*	m_pOptimization = nullptr;

	_ulong			m_PassNum = 0;

	CGameObject*	m_pPlayer = nullptr;
	_bool			m_bCanSummonYeti = false;
	_uint			m_iIndex = 0;

};

END
