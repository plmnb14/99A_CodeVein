#pragma once

#include "GameObject.h"

BEGIN(Engine)

class CTransform;
class ENGINE_DLL CCell : public CGameObject
{
public:
	enum POINTS		{ POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINES		{ LINE_AB, LINE_BC, LINE_CA, LINE_END };
	enum COMPARE	{ COMPARE_MOVE, COMPARE_STAY , COMPARE_FALL};
	enum COMPARE_LR { COMPARE_LEFT, COMPARE_RIGHT};

protected:
	explicit CCell(_Device pGraphicDev);
	explicit CCell(const CCell& rhs);
	virtual ~CCell();

public:
	HRESULT Ready_Cell(_v3* _pVertex);
	HRESULT Update_Cell(_v3* _pVertex);

public:
	virtual _int	Update();
	virtual void	LateUpdate();
	virtual void	Render();

public:
	Engine::NAV_LINE	m_tNavLine[POINT_END];

public:
	_v3		m_vAB[2];
	_v3		m_vBC[2];
	_v3		m_vCA[2];

public:
	CCell::COMPARE		Compare(const _v3* pEndPos, _ulong* pIndex, _v2* _pDirection, _ulong* _iIndex = nullptr, _bool _bIsUpper = false);
	CCell::COMPARE_LR	Compare_Line(const _v2* pEndPos, _int _iLine, _v2* _pDirection = nullptr, _ulong* _iIndex = nullptr);
	void				Calc_Line();

public:
	void	Set_CenterPos(_v3* _pVertex);

public:
	void	Set_VtxSelected(_bool _bVtxSeleted) { m_bVtxSelected = _bVtxSeleted; }
	void	Set_Selected(_bool _bSelected);

	void	Set_IndexVtxPos(_int _iIndex, _v3 _vPos);
	void	Set_VtxPos(_v3* _vPos);

public:
	void	Set_CellIdx(_int _iCellIndex);
	void	Set_Vertex();
	void	Set_Vertex(POINTS _ePoint, _v3 _vPos);
	void	Set_Vertex(_v3* _vPos);

public:
	void	Set_CellParam(CELL_PARAM _eParam) { m_eCellParam = _eParam; }

public:
	_v3*				Get_Line(LINES _eLineName);
	_v2*				Get_NavLine(LINES _eLineName);
	Engine::NAV_LINE	Get_NavLineInfo(LINES _eLineName);

public:
	_bool	Get_VtxSelected() { return m_bVtxSelected; }
	_bool	Get_Seletect() { return m_bSelected; }

public:
	_v3*	Get_VertexPos();
	_v3		Get_CenterPos();
	_int	Get_CellIdx() { return m_iCellIndex;}
	_int*	Get_SiblingIdx();

public:
	CELL_PARAM	Get_CellParam() { return m_eCellParam; }

public:
	void	Add_Vtx_Y(_float _fHight);
	void	Add_Vtx_Y(_float _fHight , _int iIndex);
	void	Add_Center_Y(_float _fHight);

	void	Set_Sibling(CCell*	_Sibling, _int _LineNum, _int	_iLineIdx);


public:
	void CCell::DrawTriangle(_v3* _pVertex);

	_bool Compare_Inner_Only(const _v3 pPos);

public:
	static CCell*	Create(_Device pGraphicDev , _v3* _vPos);

public:
	virtual void Free();

protected:
	virtual HRESULT Initialize();
	virtual HRESULT LateInit();
	virtual HRESULT Add_Components();

protected:
	virtual CGameObject* Clone_GameObject(void* pArg);

private:
	Engine::CTransform*		m_pTransform;

private:
	VTX_COL					m_pVertex[3];
	//ENGINE::CGameObject*	m_arrVertex[3];

private:
	CCell*				m_pSibling[LINE_END];
	_int				m_pSiblingIdx[LINE_END];
	CELL_PARAM			m_eCellParam;

private:
	_v3			m_vCenter;
	_v3			m_pVertexPos[3];
		
private:
	_int		m_iCellIndex;

private:
	_bool		m_bSelected;
	_bool		m_bVtxSelected;

private:
	_bool		m_bCam_Trig;
	_bool		m_bSpawn_Trig;
	_bool		m_bEvent_Trig;
};

END