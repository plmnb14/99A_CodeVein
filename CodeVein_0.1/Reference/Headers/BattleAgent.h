#ifndef BattleAgent_h__
#define BattleAgent_h__

#include "Component.h"

BEGIN(Engine)

class CGameObject;
class CTransform;
class CShader;
class ENGINE_DLL CBattleAgent : public CComponent
{
private:
	explicit CBattleAgent(_Device pGraphicDev);
	explicit CBattleAgent(const CBattleAgent& rhs);
	virtual  ~CBattleAgent(void);

public:
	virtual _int Update_Component();
//=======================================================================
// ������Ʈ ����
//=======================================================================
public:
	// ������Ʈ ���� ��ȯ
	_float	Get_RimValue() { return m_tRimParam.fRimValue; }
	// ������Ʈ�� ���� ���� ��ȯ
	_v4		Get_RimColor() { return m_tRimParam.vRimColor_Cur; };

public:
	// ������Ʈ�� ���� ����
	void	Set_RimValue(_float _fRimValue) { m_tRimParam.fRimValue = _fRimValue; }
	// ������Ʈ�� ��ȭ�� ����
	void	Set_RimColorChange(_v4 _vRimColor) { m_tRimParam.vRimColor_Change = _vRimColor; }
	// ������Ʈ�� ������ ����
	void	Set_RimChangeData(_bool _bOnRimLight = true);

public:
	// ���ڰ����� ������ ���̴��� ������Ʈ ��ġ���� �Է�����
	void	Update_RimParam_OnShader(CShader* _pShader);
	// ������Ʈ ���õ� ������Ʈ�� ����
	void	Update_RimLight();
//=======================================================================
// ���� �Һ� ����
//=======================================================================
public:
	// ���̶���Ʈ ����Ʈ ����Ʈ�� �����մϴ�.
	void	Create_HitLight(_v3 vLightPos, _float fLightRange = 15.f, _float fLifetime = 0.1f);

//=======================================================================
// ���� ���� ���
//=======================================================================
public:
	// Ÿ���� �� Ư�� �þ߰� ���濡 �ִ���
	_bool	Check_TargetIsFrontOfMe(CTransform* _vMyTransform, CTransform* _vTargetTransform , _float fAlertRange = 45.f);
	//=======================================================================

//=======================================================================
// Ư�� ����Ʈ ������ �ڽŰ� ���� ����� ����� ����
//=======================================================================
public:
	// ���� ���� �Ÿ��� ���� ����� Ÿ���� ��ȯ�մϴ�.
	CGameObject*	Calc_LengthNearByMe(list<CGameObject*> _listGameObject, _v3 _vMyPos, _float _fMinLengthValue = 0.f , _float* _ppLength = nullptr);
//=======================================================================

private:
	RIM_PARAM		m_tRimParam;

private:
	HRESULT	Ready_BattleAgent();
	HRESULT	Ready_BattleAgent_Prototype();

public:
	static CBattleAgent*		Create(_Device _pGraphicDev, _bool bChoice = false, _ulong dwCntX = 0, _ulong dwCntZ = 0);
	virtual CComponent*			Clone_Component(void* pArg);
	virtual		void			Free(void);

};

END

#endif // BattleAgent_h__
