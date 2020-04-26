#ifndef Optimization_h__
#define Optimization_h__

#include "Component.h"
#include "QuadTree.h"
#include "Frustum_Culling.h"

BEGIN(Engine)

class ENGINE_DLL COptimization : public CComponent
{
private:
	explicit COptimization(_Device pGraphicDev);
	explicit COptimization(const COptimization& rhs);
	virtual  ~COptimization(void);

public:
	HRESULT		Ready_Optimization(_bool bChoice, const _ulong dwCntX, const _ulong dwCntZ);
	_bool		Check_InFrustumforObject(const _v3* pPos, const _float& fRadius);

private:
	CFrustum_Culling*		m_pFrustum = nullptr;
	CQuadTree*				m_pQuadTree = nullptr;

public:
	static COptimization*		Create(_Device _pGraphicDev, _bool bChoice = false, _ulong dwCntX = 0, _ulong dwCntZ = 0);
	virtual CComponent*			Clone_Component(void* pArg);
	virtual		void			Free(void);

};

END

#endif // Optimization_h__
