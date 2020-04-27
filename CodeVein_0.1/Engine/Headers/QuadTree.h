#ifndef QuadTree_h__
#define QuadTree_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "Frustum_Culling.h"

BEGIN(Engine)

class ENGINE_DLL CQuadTree : public CBase
{
private:
	enum CHILD { CHILD_LT, CHILD_RT, CHILD_LB, CHILD_RB, CHILD_END };
	enum CORNER { CORNER_LT, CORNER_RT, CORNER_LB, CORNER_RB, CORNER_END };
	enum NEIGHBOR { NEIGHBOR_LEFT, NEIGHBOR_TOP, NEIGHBOR_RIGHT, NEIGHBOR_BOTTOM, NEIGHBOR_END };
	enum QuadLocation { Frustum_Out, Frustum_PartIn, Frustum_In , Frustum_Unknown };

private:
	explicit CQuadTree(void);
	virtual ~CQuadTree(void);

public:
	HRESULT		Ready_QuadTree(const _ulong& dwCntX, const _ulong& dwCntZ);
	HRESULT		Ready_Neighbor(void);
	_bool		Check_QuadTree_Frustum_Object(CFrustum_Culling* pFrustum, const _v3* pVtxPos, const _v3 pObjPos);

private:
	void	Make_ChildQuadTree(void);

	void	Make_ChildCorner(CHILD eType,
		const _ulong& dwPLT,
		const _ulong& dwPRT,
		const _ulong& dwPLB,
		const _ulong& dwPRB,
		const _ulong& dwPCenter,
		_ulong* pLT,
		_ulong* pRT,
		_ulong* pLB,
		_ulong* pRB);

	void	Make_Neighbor(void);

	void	Set_Corner(const _ulong& dwLT,
		const _ulong& dwRT,
		const _ulong& dwLB,
		const _ulong& dwRB);

private:
	CQuadTree*		m_pChild[CHILD_END] = {};
	CQuadTree*		m_pNeighbor[NEIGHBOR_END] = {};
	_ulong			m_dwCorner[CORNER_END] = {};
	_ulong			m_dwCenter = 0;
	_float			m_fRadius = 0.f;
	_bool			m_bCulled = false;

public:
	static CQuadTree*		Create(const _ulong& dwCntX, const _ulong& dwCntZ);
	CQuadTree*				Create(const _ulong& dwLT, const _ulong& dwRT,

		const _ulong& dwLB, const _ulong& dwRB);
	virtual void			Free(void);

};

END
#endif // QuadTree_h__
