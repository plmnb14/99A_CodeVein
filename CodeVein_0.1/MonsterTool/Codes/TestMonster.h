#pragma once

#include "GameObject.h"
#include "Management.h"

class CTestMonster final : public Engine::CGameObject
{
protected:
	explicit CTestMonster(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CTestMonster(LPDIRECT3DDEVICE9 pGraphic_Device, _tchar* szMeshname);
	explicit CTestMonster(const CTestMonster& rhs);
	virtual ~CTestMonster() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

	void	Check_ComboPlayBtn(_bool onoff) { m_bisPlayCombo = onoff; }
	void	Set_AniSpeed(_float _Speed) { m_fAniPlayMul = _Speed; }
	void	Set_Combo(_uint _Idx, _float _Ratio);
	void	Reset_Combo();
	void	Play_Combo();

public:
	static CTestMonster* Create(LPDIRECT3DDEVICE9 pGraphic_Device, _tchar* szMeshname);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

private:
	vector<pair<_uint, _float>> m_vectorIndexAniRatio;

	map<_uint, map<_uint, _float>>	m_mapComboNumberAniIndexAniRatio;
	CTransform*			m_pTransform = nullptr;
	CRenderer*			m_pRenderer = nullptr;
	CShader*			m_pShader = nullptr;
	CMesh_Dynamic*		m_pMesh = nullptr;

	_bool				m_bisPlayCombo = false;
	_uint				m_iMaxCount;
	_uint				m_iComboCount;

	_tchar*				m_pszMeshName;
	_double				m_dTimeDelta = 0;
	_float				m_fAniPlayMul = 2.f;
};

