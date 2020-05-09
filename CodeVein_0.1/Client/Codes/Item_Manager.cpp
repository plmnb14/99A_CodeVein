#include "stdafx.h"
#include "..\Headers\Item_Manager.h"

IMPLEMENT_SINGLETON(CItem_Manager)

CItem_Manager::CItem_Manager()
{

}


CItem_Manager::~CItem_Manager()
{
	Free();

}



HRESULT CItem_Manager::Add_Item_Prototype(_Device pDevice)
{
	g_pManagement->Add_Prototype(L"GameObject_Expendables", CExpendables::Create(pDevice));
	g_pManagement->Add_Prototype(L"GameObject_Material", CMaterial::Create(pDevice));

	return NOERROR;
}




void CItem_Manager::Free()
{
}
