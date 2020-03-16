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

	// ���� ��尡 ���ٸ�, �ֻ��� �θ� �޾ƿ´�.
	IF_NULL(_RootNode)
		NodeFinder = _TreeCtrl.GetRootItem();

	// �ƴ϶�� ���� ��ġ�� ���ڰ� ���� �����Ѵ�.
	else
	{
		NodeFinder = _RootNode;
	}

	// ã�� �̸��� ���ٸ�,
	if (_szTargetName == _TreeCtrl.GetItemText(NodeFinder))
		return NodeFinder;
	// ��ȯ

	// ���� ������ �ִٸ�
	IF_NOT_NULL(treeNext = _TreeCtrl.GetNextSiblingItem(NodeFinder))
	{
		// ������ ��� ���ڰ����� �ְ� �ٽ� ã��.
		NodeFinder = Find_Node_By_Name(_TreeCtrl, _szTargetName, treeNext);

		// ���� ã���� ��ȯ
		IF_NOT_NULL(NodeFinder)
			return NodeFinder;
	}

	// ���� �ڽ��� �ִٸ�
	IF_NOT_NULL(treeNext = _TreeCtrl.GetChildItem(NodeFinder))
	{
		// �ڽ� ��� ���ڰ����� �ְ� �ٽ� ã��.
		NodeFinder = Find_Node_By_Name(_TreeCtrl, _szTargetName, treeNext);

		// ���� ã���� ��ȯ
		IF_NOT_NULL(NodeFinder)
			return NodeFinder;
	}

	return NULL;
}

void CTreeFinder::Free()
{
}
