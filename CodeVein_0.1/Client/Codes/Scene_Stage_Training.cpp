
#include "stdafx.h"
#include "Scene_Stage_Training.h"

#include "CameraMgr.h"
#include "Effect.h"
#include "OrthoEffect.h"
#include "UI.h"
#include "UI_Manager.h"
#include "BossHP.h"
#include "ParticleMgr.h"

#include "MassageUI.h"
#include "MonsterUI.h"

#include "MonsterHeaders.h"
#include "Flag.h"
#include "ClothDynamic.h"
#include "ClothStatic.h"

#include "Player_Colleague.h"
#include "Player.h"

#include "NPC_Yakumo.h"
#include "NPC_Yokumo.h"

#include "CustomCategory.h"
#include "CustomCategoryOption.h"
#include "CustomInven.h"
#include "CustomSlot.h"
#include "CustomSliderBar.h"

CScene_Stage_Training::CScene_Stage_Training(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CScene(pGraphic_Device)
{

}

HRESULT CScene_Stage_Training::Ready_Scene()
{
	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Layer_Player")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Enemies()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Environment()))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Custom(L"Layer_Custom")))
	//	return E_FAIL;

	CRenderer* pRenderer = static_cast<CRenderer*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Renderer"));
	pRenderer->Fog_On(false);
	Safe_Release(pRenderer);

	g_pManagement->LoadCreateObject_FromPath(m_pGraphic_Device, L"Object_Stage_Training.dat");

	g_pSoundManager->Set_Volume(CSoundManager::BGM_Volume, 1.f);
	g_pSoundManager->Stop_Sound(CSoundManager::Ambient_Loop);
	g_pSoundManager->Play_BGM(L"SE_AMB_St01_Base_Out.ogg", true);

	return S_OK;
}

_int CScene_Stage_Training::Update_Scene(_double TimeDelta)
{
	//CUI_Manager::Get_Instance()->Update_UI();

	//Create_Fog(TimeDelta);
	//Create_Snow(TimeDelta);

	CGameObject* pInstance = nullptr;

	if (true == g_bSummons_Mode)
	{
		if (g_pInput_Device->Key_Down(DIK_NUMPAD9))
		{
			pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Colleague",
				&CPlayer_Colleague::JACK_INFO(_v3(0.f, 0.f, 0.f), 0.f, 0));
			g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Colleague", nullptr);
		}
		else if (g_pInput_Device->Key_Down(DIK_NUMPAD8))
		{
			pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter",
				&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_SSword,
					true, _v3(4.f, 0.f, 4.f), V3_NULL, 0));
			g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
		}

		if (g_pInput_Device->Key_Down(DIK_O))
		{
			auto& Moniter = g_pManagement->Get_GameObjectList(L"Layer_Monster", SCENE_STAGE);
			//auto& Colleague = g_pManagement->Get_GameObjectList(L"Layer_Colleague", SCENE_STAGE);

			CGameObject* Coll = g_pManagement->Get_GameObjectBack(L"Layer_Colleague", SCENE_STAGE);

			if (!Moniter.empty())
			{
				for (auto& Monster_iter : Moniter)
				{
					Monster_iter->Set_Enable(false);
					Monster_iter->Set_Dead();
					Monster_iter = nullptr;
				}
			}
			if (nullptr != Coll)
			{
				Coll->Set_Enable(false);
				Coll->Set_Dead();
				Coll = nullptr;
			}
		}
	}

	return _int();
}

HRESULT CScene_Stage_Training::Render_Scene()
{
	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CPlayer* pInstance = static_cast<CPlayer*>(g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL));

	pInstance->Teleport_ResetOptions(g_eSceneID_Cur, g_eSTeleportID_Cur);
	pInstance->Set_Enable(true);

	pInstance = nullptr;

	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Enemies()
{
	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Colleague")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Monster")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Boss")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Pet")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Item")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_MonsterProjectile")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_BossUI")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_Mistletoe")))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Layer(SCENE_STAGE, L"Layer_NPC")))
		return E_FAIL;

	// ���� ui�� ���� ���ܵ�.
	if (FAILED(g_pManagement->Add_GameObject_ToLayer(L"GameObject_BossMassageUI", SCENE_STAGE, L"Layer_BossMassageUI")))
		return E_FAIL;

	CGameObject* pInstance = nullptr;

	//pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_DropItem", &CDropItem::ITEM_STATUS(
	//	ITEM_GRADE_TYPE::ITEM_GRADE_LEGEND, ITEM_NAMETYPE::NAMETYPE_Queen_Steel, _v3(5.f, 0.f, 5.f), 5000.f));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_MORTAL, L"Layer_Item", nullptr);

	//pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_DropItem", &CDropItem::ITEM_STATUS(
	//	ITEM_GRADE_TYPE::ITEM_GRADE_LEGEND, ITEM_NAMETYPE::NAMETYPE_Queen_Titanium, _v3(-15.f, 0.f, -15.f), 5000.f));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_MORTAL, L"Layer_Item", nullptr);

	// ���籸 
	//pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Colleague",
	//	&CPlayer_Colleague::JACK_INFO(_v3(0.f, 0.f, 0.f), 0.f, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Colleague", nullptr);


	// ������
	//g_pManagement->Add_GameObject_ToLayer(L"GameObject_NPC_Yakumo", SCENE_STAGE, L"Layer_NPC", &CNPC_Yakumo::NPC_INFO(_v3(-4.46f, 0.f, -5.294f), D3DXToRadian(90.f)));

	// ������
	//g_pManagement->Add_GameObject_ToLayer(L"GameObject_NPC_Yokumo", SCENE_STAGE, L"Layer_NPC", &CNPC_Yokumo::NPC_INFO(_v3(-1.46f, 0.f, -1.294f), D3DXToRadian(90.f)));


	// ����
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
	//	&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::WHITE, WEAPON_STATE::WEAPON_None,
	//		true, _v3(8.f, 0.f, -8.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	// ����
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Cocoon",
	//	&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
	//		true, _v3(-5.f, 0.f, -5.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//// ����
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Hunter",
	//	&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_SSword,
	//		true, _v3(4.f, 0.f, 4.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	// ��Ű
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Monkey",
	//	&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
	//		true, _v3(4.f, 0.f, 4.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//// ���̸�
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_ThaiMan",
	//	&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
	//		true, _v3(4.f, 0.f, 4.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//// ����
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Urchin",
	//	&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
	//		true, _v3(4.f, 0.f, 4.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//// ����
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Wolf",
	//	&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
	//		true, _v3(4.f, 0.f, 4.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//// ������
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_YachaMan",
	//	&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
	//		true, _v3(4.f, 0.f, 4.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//// ��Ƽ
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_Yeti",
	//	&CMonster::MONSTER_STATUS(CMonster::MONSTER_COLOR_TYPE::COLOR_NONE, WEAPON_STATE::WEAPON_None,
	//		true, _v3(4.f, 0.f, 4.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//// ��_������, ��ȯ��ġ�� �׻� player�� �����ʰ�
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Pet_PoisonButterFly", nullptr);
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Pet", nullptr);

	////// ��_�罿ŷ
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Pet_DeerKing", nullptr);
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Pet", nullptr);

	//====================================================================================================================================================
	// �˰���
	//====================================================================================================================================================

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
	//	&CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::Sit1, 70.f, 2.f, 1.f,
	//		true, _v3(5.f, 0.f, -2.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
	//	&CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::Sit1, 70.f, 2.f, 1.f,
	//		true, _v3(10.f, 0.f, -2.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordGenji",
	//	&CSwordGenji::INFO(CSwordGenji::Normal, CSwordGenji::Sit1, 70.f, 2.f, 1.f,
	//		true, _v3(-5.f, 0.f, -2.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//====================================================================================================================================================


	//====================================================================================================================================================
	// �Ѱ���
	//====================================================================================================================================================

//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
//	&CGunGenji::INFO(CGunGenji::Normal, CGunGenji::CheckGun, 10.f, 3.f, 2.f,
//		true, _v3(5.f, 0.f, -8.f), V3_NULL, 0));
//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
	//	&CGunGenji::INFO(CGunGenji::Normal, CGunGenji::CheckGun, 10.f, 3.f, 2.f,
	//		true, _v3(10.f, 0.f, -8.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);
	//
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_GunGenji",
	//	&CGunGenji::INFO(CGunGenji::Normal, CGunGenji::CheckGun, 10.f, 3.f, 2.f,
	//		true, _v3(0.f, 0.f, -8.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//====================================================================================================================================================


	//====================================================================================================================================================
	// â ���� ����
	//====================================================================================================================================================

	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_SwordShieldGenji",
	//	&CSwordShieldGenji::INFO(CSwordShieldGenji::White, CSwordShieldGenji::LookAround1, 10.f, 5.f, 2.f,
	//		true, _v3(5.f, 0.f, -8.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr);

	//====================================================================================================================================================

	//CGameObject* pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Dummy", nullptr);
	//
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3{ 0.f, 0.f, 4.f });
	//
	//if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Monster", nullptr)))
	//	return E_FAIL;
	
	// ������
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_PoisonButterfly",
	//	&CPoisonButterfly::INFO(10.f, 5.f, 2.f,
	//		true, _v3(-5.f, 0.f, 8.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);


	//// ������ ���
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_QueensKnight",
	//	&CQueensKnight::INFO(10.f, 5.f, 2.f,
	//		true, _v3(-8.f, 0.f, 3.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);

	//// ��������
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_IceGirl",
	//	&CIceGirl::INFO(10.f, 5.f, 2.f,
	//		true, _v3(3.f, 0.f, -8.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);

	////// �ҳ���
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_FireBoy",
	//	&CFireBoy::INFO(10.f, 5.f, 2.f,
	//		true, _v3(8.f, 0.f, -5.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);

	//// �罿��
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Monster_DeerKing",
	//	&CDeerKing::INFO(10.f, 5.f, 2.f,
	//		true, _v3(8.f, 0.f, -8.f), V3_NULL, 0));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);

	////CActiveObject
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Mesh_Itembox",
	//	&CActiveObject::ACTOBJ_INFO(L"Mesh_Itembox", _v3(0.f, 0.f, 0.f), _v3(0.f, 0.f, 0.f), _v3(1.f, 1.f, 1.f),
	//		0, 2/*�ڽ�*/));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);

	////CActiveObject
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Mesh_Itembox_Lid",
	//	&CActiveObject::ACTOBJ_INFO(L"Mesh_Itembox_Lid", _v3(0.f, 0.f, 0.f),  _v3(0.f, 0.f, 0.f), _v3(1.f, 1.f, 1.f),
	//		0, 3/*�ڽ� Lid*/));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Boss", nullptr);
	
	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Environment()
{
	CGameObject* pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Sky", nullptr);
	TARGET_TO_TRANS(pInstance)->Set_Angle(AXIS_Y, D3DXToRadian(0.f));
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Sky", nullptr);

	pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Sky_Blur", nullptr);
	TARGET_TO_TRANS(pInstance)->Set_Angle(AXIS_Y, D3DXToRadian(0.f));
	g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Sky", nullptr);

	//pInstance = g_pManagement->Clone_GameObject_Return(L"GameObject_Flag", &CFlag::INFO(5, 5, 2.f, 1.f));
	//TARGET_TO_TRANS(pInstance)->Set_Pos(_v3(0.f, 3.f, 3.f));
	////static_cast<CFlag*>(pInstance)->Set_Wind();	// �������ϸ� ����Ʈ ����
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Sky", nullptr);


	// ���̳��� ��
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Cloth_Dynamic", &CClothDynamic::INFO(CClothDynamic::ClothDynamic_Name::Drape));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Cloth", nullptr);

	// ����ƽ ��
	//pInstance = g_pManagement->Clone_GameObject_Return(L"Cloth_Static", &CClothStatic::INFO(CClothStatic::ClothStatic_Name::Hair_Long));
	//g_pManagement->Add_GameOject_ToLayer_NoClone(pInstance, SCENE_STAGE, L"Layer_Cloth", nullptr);

	return S_OK;
}

HRESULT CScene_Stage_Training::Ready_Layer_Custom(const _tchar * pLayerTag)
{
	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_Custom_UI", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/Customize/Custom_UI/Custom_UI_%d.dds", 13))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_Custom_Hair", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/Customize/Custom_Hair/Custom_Hair_%d.dds", 7))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_Custom_Eye", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/Customize/Custom_Eye/Custom_Eye_%d.dds", 1))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_Custom_Face", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/Customize/Custom_Face/Custom_Face_%d.dds", 10))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_Custom_ToxicGuard", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/Customize/Custom_ToxicGuard/Custom_ToxicGuard_%d.dds", 5))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(SCENE_STATIC, L"DefaultTex_Custom_Inner", CTexture::Create(m_pGraphic_Device, CTexture::TYPE_GENERAL, L"../Resources/Texture/DefaultUI/Customize/Custom_Inner/Custom_Inner_%d.dds", 4))))
		return E_FAIL;

	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_CustomInven", CCustomInven::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_CustomSlot", CCustomSlot::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_CustomSlider", CCustomSliderBar::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_CustomCategory", CCustomCategory::Create(m_pGraphic_Device))))
		return E_FAIL;
	if (FAILED(g_pManagement->Add_Prototype(L"GameObject_CustomCategoryOption", CCustomCategoryOption::Create(m_pGraphic_Device))))
		return E_FAIL;

	CCustomCategory* pCustomCategory = static_cast<CCustomCategory*>(g_pManagement->Clone_GameObject_Return(L"GameObject_CustomCategory", nullptr));
	if (FAILED(g_pManagement->Add_GameOject_ToLayer_NoClone(pCustomCategory, SCENE_STAGE_BASE, pLayerTag, nullptr)))
		return E_FAIL;

	return S_OK;
}

void CScene_Stage_Training::Create_Fog(_double TimeDelta)
{
	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	if (!pPlayer)
		return;

	CTransform* pPlayerTrans = TARGET_TO_TRANS(pPlayer);
	_v3 vPlayerPos = pPlayerTrans->Get_Pos();

	const _float FOG_OFFSET = 10.f;

	//m_fMapFogDelay += _float(TimeDelta);
	//if (m_fMapFogDelay > FOG_OFFSET)
	//{
	//	m_fMapFogDelay = 0.f;
	//
	//	for (_int i = 0; i < 30; ++i)
	//	{
	//		_mat matRotY;
	//		_v3 vDir = _v3(1.f, 0.f, 1.f);
	//		D3DXMatrixIdentity(&matRotY);
	//
	//		D3DXMatrixRotationY(&matRotY, D3DXToRadian(_float(CCalculater::Random_Num_Double(0, 360))));
	//		D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
	//		D3DXVec3Normalize(&vDir, &vDir);
	//
	//		_float fMinRange = 5.f;
	//		_float fRandRange = _float(CCalculater::Random_Num_Double(0, 20));
	//		_v3 vRandPos = vDir * (fMinRange + fRandRange);
	//
	//		g_pManagement->Create_Effect(L"MapMist", vRandPos + _v3(0.f, -0.5f, 0.f), pPlayerTrans);
	//	}
	//}
	//
	const _float DUST_OFFSET = 0.3f;

	m_fMapWindDustDelay += _float(TimeDelta);
	if (m_fMapWindDustDelay > DUST_OFFSET)
	{
		m_fMapWindDustDelay = 0.f;

		for (_int i = 0; i < 10; ++i)
		{
			_mat matRotY;
			_v3 vDir = _v3(1.f, 0.f, 1.f);
			D3DXMatrixIdentity(&matRotY);

			D3DXMatrixRotationY(&matRotY, D3DXToRadian(_float(CCalculater::Random_Num_Double(0, 360))));
			D3DXVec3TransformNormal(&vDir, &vDir, &matRotY);
			D3DXVec3Normalize(&vDir, &vDir);

			_float fMinRange = 1.f;
			_float fRandRange = _float(CCalculater::Random_Num_Double(0, 30));
			_v3 vRandPos = vDir * (fMinRange + fRandRange) + _v3(0.f, 0.2f, 0.f);

			g_pManagement->Create_Effect(L"MapDust", vPlayerPos + vRandPos + _v3(0.f, _float(CCalculater::Random_Num_Double(0, 0.5)), 0.f), nullptr);

			fRandRange = _float(CCalculater::Random_Num_Double(0, 20));
			vRandPos = vDir * (fMinRange + fRandRange);
			g_pManagement->Create_Effect(L"MapDust_2", vPlayerPos + vRandPos + _v3(0.f, _float(CCalculater::Random_Num_Double(0, 0.5)), 0.f), nullptr);
		}
	}
}

void CScene_Stage_Training::Create_Snow(_double TimeDelta)
{
	CGameObject* pPlayer = g_pManagement->Get_GameObjectBack(L"Layer_Player", SCENE_MORTAL);
	if (!pPlayer)
		return;

	const _float SNOW_OFFSET = 0.3f;

	m_fMapSnowDelay += _float(TimeDelta);
	if (m_fMapSnowDelay > SNOW_OFFSET)
	{
		m_fMapSnowDelay = 0.f;

		CTransform* pPlayerTrans = TARGET_TO_TRANS(pPlayer);
		_v3 vPlayerPos = pPlayerTrans->Get_Pos();

		g_pManagement->Create_Effect(L"MapSnow", vPlayerPos + _v3(0.f, 10.f, 0.f), nullptr);
	}
}

HRESULT CScene_Stage_Training::Ready_LightDesc()
{
	NEW_LIGHT		LightDesc;
	ZeroMemory(&LightDesc, sizeof(NEW_LIGHT));

	_v3 vLightPos = _v3(-50.f, -50.f, 0.f);
	V3_NORMAL_SELF(&vLightPos);

	LightDesc.Type = D3DLIGHT_DIRECTIONAL;
	LightDesc.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	// In.WorldSpace
	LightDesc.Direction = vLightPos;

	LightDesc.fAlpha = 1.f;

	if (FAILED(g_pManagement->Add_Light(m_pGraphic_Device, LightDesc, CLight_Manager::Static_Light)))
		return E_FAIL;

	return NOERROR;
}

CScene_Stage_Training * CScene_Stage_Training::Create(LPDIRECT3DDEVICE9 pGraphic_Device, _bool _bLoadStatic)
{
	CScene_Stage_Training*	pInstance = new CScene_Stage_Training(pGraphic_Device);

	pInstance->m_bLoadStaticMesh = _bLoadStatic;

	if (FAILED(pInstance->Ready_Scene()))
	{
		MSG_BOX("Failed To Creating CScene_Stage_Training");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScene_Stage_Training::Free()
{
	CScene::Free();
}

