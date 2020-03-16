#include "stdafx.h"
#include "TreeFinder.h"

CTreeFinder::CTreeFinder()
{
}


CTreeFinder::~CTreeFinder()
{
}

HTREEITEM CTreeFinder::Find_Node_By_Name(CTreeCtrl& _TreeCtrl , const _tchar* _szTargetName, HTREEITEM _RootNode)
{
	HTREEITEM NodeFinder;
	//HTREEITEM TargetNode;
	HTREEITEM treeNext;

	// 시작 노드가 없다면, 최상위 부모를 받아온다.
	IF_NULL(_RootNode)
		NodeFinder = _TreeCtrl.GetRootItem();

	// 아니라면 시작 위치를 인자값 노드로 설정한다.
	else
	{
		NodeFinder = _RootNode;
	}

	// 찾는 이름과 같다면,
	if (_szTargetName == _TreeCtrl.GetItemText(NodeFinder))
		return NodeFinder;
	// 반환

	// 만약 형제가 있다면
	IF_NOT_NULL(treeNext = _TreeCtrl.GetNextSiblingItem(NodeFinder))
	{
		// 형제를 노드 인자값으로 넣고 다시 찾기.
		NodeFinder = Find_Node_By_Name(_TreeCtrl, _szTargetName, treeNext);

		// 만약 찾으면 반환
		IF_NOT_NULL(NodeFinder)
			return NodeFinder;
	}

	// 만약 자식이 있다면
	IF_NOT_NULL(treeNext = _TreeCtrl.GetChildItem(NodeFinder))
	{
		// 자식 노드 인자값으로 넣고 다시 찾기.
		NodeFinder = Find_Node_By_Name(_TreeCtrl, _szTargetName, treeNext);

		// 만약 찾으면 반환
		IF_NOT_NULL(NodeFinder)
			return NodeFinder;
	}

	return NULL;
}

void CTreeFinder::Free()
{
}
