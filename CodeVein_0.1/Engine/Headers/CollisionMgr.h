#pragma once

#include "Engine_Defines.h"
#include "Base.h"

//#include "Transform.h"
//#include "Collider.h"
//#include "GameObject.h"

BEGIN(Engine)

class CCollider;
class CGameObject;
typedef class ENGINE_DLL CCollisionMgr : public CBase
{
private:
	explicit CCollisionMgr();
	virtual ~CCollisionMgr();

public:
	static void Frostum_Culling(list<CGameObject*>& rDstList);

public:
	static void Collision_Physic(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList);
	static void Collision_Attack(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList);

	static void Collision_Weapon(CGameObject* rDst, list<CGameObject*>& rSrcList);

public:
	static CGameObject* Collision_Ray(list<CGameObject*>& rDstList, RAY _tRay, _float*_vCrossLength);
	static CGameObject* Collision_Ray(list<CGameObject*>& rDstList, RAY _tRay, _float*_vCrossLength, _bool _bLayered , _short _sLayerIdx);
	static _bool		Collision_Ray(CGameObject* rDst, RAY _tRay, _float*_vCrossLength);

public:
	virtual void Free();

}COL_MGR;
END
