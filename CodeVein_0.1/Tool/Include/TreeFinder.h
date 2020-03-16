
#ifndef TreeFinder_h__
#define TreeFinder_h__

#include "stdafx.h"
#include "Base.h"

typedef class CTreeFinder : public CBase
{
private:
	explicit CTreeFinder();
	virtual ~CTreeFinder();

public:
	static HTREEITEM Find_Node_By_Name(CTreeCtrl& _TreeCtrl , const _tchar* _szTargetName, HTREEITEM _RootNode = NULL);

public:
	virtual void Free();

}TREE_FINDER;

#endif // TreeFinder_h__
