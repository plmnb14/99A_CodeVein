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
// 림라이트 관리
//=======================================================================
public:
	// 림라이트 강도 반환
	_float	Get_RimValue() { return m_tRimParam.fRimValue; }
	// 림라이트의 현재 색상 반환
	_v4		Get_RimColor() { return m_tRimParam.vRimColor_Cur; };

public:
	// 림라이트의 강도 설정
	void	Set_RimValue(_float _fRimValue) { m_tRimParam.fRimValue = _fRimValue; }
	// 림라이트의 변화색 설정
	void	Set_RimColorChange(_v4 _vRimColor) { m_tRimParam.vRimColor_Change = _vRimColor; }
	// 림라이트의 데이터 갱신
	void	Set_RimChangeData(_bool _bOnRimLight = true);

public:
	// 인자값으로 들어오는 쉐이더에 림라이트 수치들을 입력해줌
	void	Update_RimParam_OnShader(CShader* _pShader);
	// 림라이트 관련된 업데이트를 실행
	void	Update_RimLight();
//=======================================================================
// 전투 불빛 생성
//=======================================================================
public:
	// 하이라이트 포인트 라이트를 생성합니다.
	void	Create_HitLight(_v3 vLightPos, _float fLightRange = 15.f, _float fLifetime = 0.1f);

//=======================================================================
// 각도 방향 계산
//=======================================================================
public:
	// 타겟이 내 특정 시야각 전방에 있는지
	_bool	Check_TargetIsFrontOfMe(CTransform* _vMyTransform, CTransform* _vTargetTransform , _float fAlertRange = 45.f);
	//=======================================================================

//=======================================================================
// 특정 리스트 내에서 자신과 가장 가까운 대상을 리턴
//=======================================================================
public:
	// 나로 부터 거리가 가장 가까운 타겟을 반환합니다.
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
