#ifndef Trail_VFX_h__
#define Trail_VFX_h__

#include "GameObject.h"
#include "Management.h"

BEGIN(Engine)

class ENGINE_DLL CTrail_VFX : public CGameObject
{
public:
	enum TrailType
	{
		Trail_Normal, Trail_Distortion
	};

protected:
	explicit CTrail_VFX(_Device pGraphicDev);
	explicit CTrail_VFX(const CTrail_VFX& rhs);
	virtual ~CTrail_VFX();

public:
	virtual HRESULT Ready_GameObject_Prototype(); // 원복객체 생성 시, 호출될 함수.
	virtual HRESULT Ready_GameObject(void* pArg); // 인게임 객체 생성 시, 호출될 함수.
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual	HRESULT LateInit_GameObject();
	virtual HRESULT Render_GameObject();

public:
	virtual size_t	Get_ListSize() { return m_vecStart.size(); };

public:
	virtual void	Set_TrailIdx(_ulong _dwIdx) { m_iTrailIdx = _dwIdx; }

public:
	virtual void	Change_Tex(const _tchar* _TexName);
	virtual void	Clear_TrailInfo();
	virtual void	Clear_TrailInfo_From_Front();

public:
	virtual void	Ready_Info(_v3 _vBeginPos, _v3 _vEndPos);
	virtual void	CatmullRom_Pos();
	virtual void	Ready_Vtx();
	virtual void	Set_ParentTransform(const _mat* _pWorldMat);
	virtual void	Set_TrailType(TrailType _eTrailType) {m_eType = _eTrailType;}

private:
	virtual HRESULT	Shader_Init(const _uint & iIndex = 0);

private:
	HRESULT Add_Component();

public:
	static CTrail_VFX* Create(_Device pGraphicDev);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CTransform*		m_pTransform = nullptr;
	CRenderer*		m_pRenderer = nullptr;
	CTexture*		m_pTexture = nullptr;
	CShader*		m_pShader = nullptr;

private:
	TrailType		m_eType = Trail_Normal;

private:
	// 점 위치 저장하는 구조체
	typedef list<TRAIL_INFO>		TRAIL_LIST;
	typedef vector<_v3>				CALC_V3_VEC;

	TRAIL_LIST						m_listTrail;
	CALC_V3_VEC						m_vecStart;
	CALC_V3_VEC						m_vecEnd;

private:
	_ulong		m_iTrailIdx = 0;


	// 스플라인 선들간의 간격
private:
	_ulong		m_dwInterval = 0;
	_ulong		m_dwListCnt = 0;

private:
	_ulong		m_dwMaxCnt = 0;

	// 버텍스 
private:
	_ulong		m_dwVtxCnt = 0;
	_ulong		m_dwVtxFVF = 0;
	_ulong		m_dwVtxSize = 0;
	_ulong		m_dwTricnt = 0;

private:
	_bool		m_bUpdateVtx = false;
	_bool		m_bDrawTrail = true;

private:
	_float		m_fUpdateTime_Cur = 0.f;
	_float		m_fUpdateTime_Max = 0.f;

private:
	VTXTEX		m_pVtx[300];


private:
	_tchar		m_eTexName[MAX_STR] = L"";

private:
	_mat		m_matParent;

private:

};

END

#endif	// Trail_VFX_h__