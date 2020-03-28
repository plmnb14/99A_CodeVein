#pragma once

#include "Component.h"
#include "Cell.h"

BEGIN(Engine)

class CRigidBody;
class ENGINE_DLL CNavMesh : public CComponent
{
protected:
	explicit	CNavMesh(_Device pGraphicDev);
	explicit	CNavMesh(const CNavMesh& rhs);
	virtual		~CNavMesh();

public:
	void		Move_NaviMesh_Y(const _ulong _dwSubIdx , const _float _fYDist);
	HRESULT		Ready_NaviMesh(_Device _pGraphicDev, const _tchar * _NavMeshPath);
	HRESULT		Ready_Prototype_NaviMesh();
	void		Render_NaviMesh();
	_v3			Move_OnNaviMesh(CRigidBody* _pRigid, const _v3* pTargetPos, const _v3* pTargetDir, const _float _fSpeedDelta = 0.f);
	_v3			Axis_Y_OnNavMesh(const _v3 _vPos);
	_bool		Goto_Next_Subset(const _v3 _vPos , CRigidBody* _pRigid);

public:
	void		Trigger_Camera();
	void		Trigger_Spawn();
	void		Trigger_Event();

public:
	void		Set_Index(const _ulong& dwIndex) { m_dwIndex = dwIndex; }
	void		Set_SubsetIndex(const _ulong& _dwSubsetIndex) { m_dwSubsetIdx = _dwSubsetIndex; }
	_ulong		Get_CellIndex() { return m_dwIndex; };
	_ulong		Get_SubSetIndex() { return m_dwSubsetIdx; };
	CELL_PARAM	Get_CellParam();

public:
	virtual void Reset_NaviMesh();
	

public:
	_bool		Get_CanMove() { return m_bCanMove; }

private:
	HRESULT		Link_Cell();
	void		Check_Line(_v3* _DstLine, CCell* _DstCell, CCell* _RscCell, _int _LineNum);

public:
	void Check_OnNavMesh(const _v3 * pTargetPos);

private:
	typedef vector<CCell*>		VEC_CELL;
	typedef vector<VEC_CELL*>	VEC_SUBSET;

	vector<VEC_CELL*>		m_vecSubset_Cell;

	_ulong					m_dwSubsetIdx = 0;
	_ulong					m_dwIndex = 0;
	_bool					m_bCanMove = false;


public:
	static	CNavMesh*		Create(_Device pGraphicDev);
	virtual CComponent*		Clone_Component(void* pArg);
	virtual void			Free();
};

END