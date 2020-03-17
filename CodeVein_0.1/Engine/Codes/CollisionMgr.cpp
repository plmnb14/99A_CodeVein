#include "CollisionMgr.h"
#include "Management.h"

#include "RenderObject.h"
#include "GameObject.h"

CCollisionMgr::CCollisionMgr()
{
}

CCollisionMgr::~CCollisionMgr()
{
	Free();
}

void CCollisionMgr::Frostum_Culling(list<CGameObject*>& rDstList)
{
}

void CCollisionMgr::Collision_Physic(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			CCollider* rDstCol = TARGET_TO_COL(rDst);
			CCollider* rSrcCol = TARGET_TO_COL(rSrc);

			switch (rDstCol->Get_ColType())
			{
			case COL_AABB:
			{
				if (rDstCol->Check_AABB(rSrcCol))
				{
					CTransform* rDstTrans = TARGET_TO_TRANS(rDst);
					CTransform* rSrcTrans = TARGET_TO_TRANS(rSrc);

					rDstTrans->Set_Pos(rDstTrans->Get_Pos() + rDstCol->Get_Length());
					rSrcTrans->Set_Pos(rSrcTrans->Get_Pos() + rSrcCol->Get_Length());

					rDstCol->Update(rDstTrans->Get_Pos());
					rSrcCol->Update(rSrcTrans->Get_Pos());
				}

				break;
			}

			case COL_OBB:
			{
				////cout << "검사" << endl;

				if (rDstCol->Check_OBB(rSrcCol))
				{
					////cout << "OBB 충돌 합니다." << endl;
					////cout << "OBB 충돌" << endl;

					//ENGINE::CTransform* rDstTrans = TARGET_TO_TRANS(rDst);
					//ENGINE::CTransform* rSrcTrans = TARGET_TO_TRANS(rSrc);
					//
					//rDstTrans->Set_Pos(rDstTrans->Get_Pos() + rDstCol->Get_Length());
					//rSrcTrans->Set_Pos(rSrcTrans->Get_Pos() + rSrcCol->Get_Length());
					//
					//rDstCol->Update(rDstTrans->Get_Pos());
					//rSrcCol->Update(rSrcTrans->Get_Pos());
				}
			}
			}
		}
	}
}

void CCollisionMgr::Collision_Attack(list<CGameObject*>& rDstList, list<CGameObject*>& rSrcList)
{
	for (auto& rDst : rDstList)
	{
		for (auto& rSrc : rSrcList)
		{
			CCollider* rDstCol = TARGET_TO_COL(rDst);
			CCollider* rSrcCol = TARGET_TO_COL(rSrc);

			switch (rDstCol->Get_ColType())
			{
			case COL_AABB:
			{
				if (rDstCol->Check_AABB(rSrcCol))
				{

				}

				break;
			}
			}
		}
	}
}

void CCollisionMgr::Collision_Weapon(CGameObject * rDst, list<CGameObject*>& rSrcList)
{
	for (auto& rSrc : rSrcList)
	{
		CCollider* rDstCol = TARGET_TO_COL(rDst);
		CCollider* rSrcCol = TARGET_TO_COL(rSrc);

		switch (rDstCol->Get_ColType())
		{
		case COL_AABB:
		{
			if (rDstCol->Check_AABB(rSrcCol))
			{
				//cout << "충돌함" << endl;
			}

			break;
		}
		}
	}
}

CGameObject* CCollisionMgr::Collision_Ray(list<CGameObject*>& rDstList, RAY _tRay, _float*_vCrossLength)
{
	CGameObject*	tmpObj = nullptr;
	_float			tmpCross = 0;

	for (auto& rDst : rDstList)
	{
		if(static_cast<CRenderObject*>(rDst)->Get_Selected())
			continue;

		CCollider* rDstCol = TARGET_TO_COL(rDst);

		bool IsCollision = rDstCol->Check_RayCollision(rDstCol, _tRay , _vCrossLength);

		if (IsCollision)
		{
			if (tmpObj == nullptr)
			{
				tmpObj = rDst;
				tmpCross = *_vCrossLength;
			}
			else
			{
				if (*_vCrossLength < tmpCross)
				{
					tmpObj = rDst;
					tmpCross = *_vCrossLength;
				}
			}
		}
	}

	*_vCrossLength = tmpCross;

	return tmpObj;
}

CGameObject * CCollisionMgr::Collision_Ray(list<CGameObject*>& rDstList, RAY _tRay, _float * _vCrossLength, _bool _bLayered, _short _sLayerIdx)
{
	CGameObject*	tmpObj = nullptr;
	_float			tmpCross = 0;

	for (auto& rDst : rDstList)
	{
		if (false == _bLayered)
		{
			if (rDst->Get_LayerIdx() != _sLayerIdx)
				continue;
		}

		if (static_cast<CRenderObject*>(rDst)->Get_Selected())
			continue;

		CCollider* rDstCol = TARGET_TO_COL(rDst);

		bool IsCollision = rDstCol->Check_RayCollision(rDstCol, _tRay, _vCrossLength);

		if (IsCollision)
		{
			if (tmpObj == nullptr)
			{
				tmpObj = rDst;
				tmpCross = *_vCrossLength;
			}
			else
			{
				if (*_vCrossLength < tmpCross)
				{
					tmpObj = rDst;
					tmpCross = *_vCrossLength;
				}
			}
		}
	}

	*_vCrossLength = tmpCross;

	return tmpObj;
}

_bool CCollisionMgr::Collision_Ray(CGameObject * rDst, RAY _tRay, _float * _vCrossLength)
{
	CCollider* rDstCol = TARGET_TO_COL(rDst);

	bool IsCollision = rDstCol->Check_RayCollision(rDstCol, _tRay, _vCrossLength);

	return IsCollision;
}

void CCollisionMgr::Free()
{
}
