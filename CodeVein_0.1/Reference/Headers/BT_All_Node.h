// Composite
#include "..\Headers\BT_Selector.h"
#define Node_Selector(_name) static_cast<CBT_Selector*>(CManagement::Get_Instance()->Clone_Node(L"Selector", CBT_Node_Manager::COMPOSITE, &CBT_Selector::INFO(_name, CBT_Selector::Mode::Normal)))
#define Node_Selector_Random(_name) static_cast<CBT_Selector*>(CManagement::Get_Instance()->Clone_Node(L"Selector", CBT_Node_Manager::COMPOSITE, &CBT_Selector::INFO(_name, CBT_Selector::Mode::Random)))

#include "..\Headers\BT_Sequence.h"
#define Node_Sequence(_name) static_cast<CBT_Sequence*>(CManagement::Get_Instance()->Clone_Node(L"Sequence", CBT_Node_Manager::COMPOSITE, &CBT_Sequence::INFO(_name)))

#include "..\Headers\BT_Simple_Parallel.h"
#define Node_Parallel_Immediate(_name) static_cast<CBT_Simple_Parallel*>(CManagement::Get_Instance()->Clone_Node(L"Simple_Parallel", CBT_Node_Manager::COMPOSITE, &CBT_Simple_Parallel::INFO(_name, CBT_Simple_Parallel::Mode::Immediate)));
#define Node_Parallel_Delay(_name) static_cast<CBT_Simple_Parallel*>(CManagement::Get_Instance()->Clone_Node(L"Simple_Parallel", CBT_Node_Manager::COMPOSITE, &CBT_Simple_Parallel::INFO(_name, CBT_Simple_Parallel::Mode::Delayed)));


// Decorator
#include "..\Headers\BT_Inverter.h"
#define Node_Inverter(_name) static_cast<CBT_Inverter*>(CManagement::Get_Instance()->Clone_Node(L"Inverter", CBT_Node_Manager::DECORATOR, &CBT_Inverter::INFO(_name)))

#include "..\Headers\BT_Loop.h"
#define Node_Loop(_nmae, _dTime) static_cast<CBT_Loop*>(CManagement::Get_Instance()->Clone_Node(L"Loop", CBT_Node_Manager::DECORATOR, &CBT_Loop::INFO(_nmae, _dTime)))

#include "..\Headers\BT_Cooldown.h"
#define Node_Cooldown(_name, _dCooltime) static_cast<CBT_Cooldown*>(CManagement::Get_Instance()->Clone_Node(L"Cooldown", CBT_Node_Manager::DECORATOR, &CBT_Cooldown::INFO(_name, _dCooltime)))

#include "..\Headers\BT_ConeCheck.h"
#define Node_ConeCheck(_name, _target_Key, _fDegreeOfFov, _fMaxDistance) static_cast<CBT_ConeCheck*>(CManagement::Get_Instance()->Clone_Node(L"ConeCheck", CBT_Node_Manager::DECORATOR, &CBT_ConeCheck::INFO(_name, _target_Key, m_pTransformCom, _fDegreeOfFov,  _fMaxDistance)))

#include "..\Headers\BT_DistCheck.h"
#define Node_DistCheck(_name, _target_Key, _fMaxDistance) static_cast<CBT_DistCheck*>(CManagement::Get_Instance()->Clone_Node(L"DistCheck", CBT_Node_Manager::DECORATOR, &CBT_DistCheck::INFO(_name, _target_Key, m_pTransformCom, _fMaxDistance)))

#include "..\Headers\BT_Compare.h"
#define Node_BOOL_A_Equal_B(_name, _TargetKey_A, _TargetKey_B) static_cast<CBT_Compare*>(CManagement::Get_Instance()->Clone_Node(L"Compare", CBT_Node_Manager::DECORATOR, &CBT_Compare::INFO(_name, CBT_Compare::Mode::EQUAL, _TargetKey_A, CBT_Compare::Value::BOOL, _TargetKey_B)))
#define Node_FLOAT_A_Equal_B(_name, _TargetKey_A, _TargetKey_B) static_cast<CBT_Compare*>(CManagement::Get_Instance()->Clone_Node(L"Compare", CBT_Node_Manager::DECORATOR, &CBT_Compare::INFO(_name, CBT_Compare::Mode::EQUAL, _TargetKey_A, CBT_Compare::Value::FLOAT, _TargetKey_B)))
#define Node_INT_A_Equal_B(_name, _TargetKey_A, _TargetKey_B) static_cast<CBT_Compare*>(CManagement::Get_Instance()->Clone_Node(L"Compare", CBT_Node_Manager::DECORATOR, &CBT_Compare::INFO(_name, CBT_Compare::Mode::EQUAL, _TargetKey_A, CBT_Compare::Value::INT, _TargetKey_B)))

#define Node_BOOL_A_NotEqual_B(_name, _TargetKey_A, _TargetKey_B) static_cast<CBT_Compare*>(CManagement::Get_Instance()->Clone_Node(L"Compare", CBT_Node_Manager::DECORATOR, &CBT_Compare::INFO(_name, CBT_Compare::Mode::NOT_EQUAL, _TargetKey_A, CBT_Compare::Value::BOOL, _TargetKey_B)))
#define Node_FLOAT_A_NotEqual_B(_name, _TargetKey_A, _TargetKey_B) static_cast<CBT_Compare*>(CManagement::Get_Instance()->Clone_Node(L"Compare", CBT_Node_Manager::DECORATOR, &CBT_Compare::INFO(_name, CBT_Compare::Mode::NOT_EQUAL, _TargetKey_A, CBT_Compare::Value::FLOAT, _TargetKey_B)))
#define Node_INT_A_NotEqual_B(_name, _TargetKey_A, _TargetKey_B) static_cast<CBT_Compare*>(CManagement::Get_Instance()->Clone_Node(L"Compare", CBT_Node_Manager::DECORATOR, &CBT_Compare::INFO(_name, CBT_Compare::Mode::NOT_EQUAL, _TargetKey_A, CBT_Compare::Value::INT, _TargetKey_B)))

#define Node_FLOAT_A_Greater_Than_B(_name, _TargetKey_A, _TargetKey_B) static_cast<CBT_Compare*>(CManagement::Get_Instance()->Clone_Node(L"Compare", CBT_Node_Manager::DECORATOR, &CBT_Compare::INFO(_name, CBT_Compare::Mode::GREATER_THAN, _TargetKey_A, CBT_Compare::Value::FLOAT, _TargetKey_B)))
#define Node_INT_A_Greater_Than_B(_name, _TargetKey_A, _TargetKey_B) static_cast<CBT_Compare*>(CManagement::Get_Instance()->Clone_Node(L"Compare", CBT_Node_Manager::DECORATOR, &CBT_Compare::INFO(_name, CBT_Compare::Mode::GREATER_THAN, _TargetKey_A, CBT_Compare::Value::INT, _TargetKey_B)))

#define Node_FLOAT_A_Smaller_Than_B(_name, _TargetKey_A, _TargetKey_B) static_cast<CBT_Compare*>(CManagement::Get_Instance()->Clone_Node(L"Compare", CBT_Node_Manager::DECORATOR, &CBT_Compare::INFO(_name, CBT_Compare::Mode::SMALLER_THAN, _TargetKey_A, CBT_Compare::Value::FLOAT, _TargetKey_B)))
#define Node_INT_A_Smaller_Than_B(_name, _TargetKey_A, _TargetKey_B) static_cast<CBT_Compare*>(CManagement::Get_Instance()->Clone_Node(L"Compare", CBT_Node_Manager::DECORATOR, &CBT_Compare::INFO(_name, CBT_Compare::Mode::SMALLER_THAN, _TargetKey_A, CBT_Compare::Value::INT, _TargetKey_B)))

#include "..\Headers\BT_CompareValue.h"
#define Node_BOOL_A_Equal_Value(_name, _TargetKey_A, _Value) static_cast<CBT_CompareValue*>(CManagement::Get_Instance()->Clone_Node(L"CompareValue", CBT_Node_Manager::DECORATOR, &CBT_CompareValue::INFO(_name, CBT_CompareValue::Value::BOOL, _TargetKey_A, CBT_CompareValue::Mode::EQUAL, _Value, 0.f, 0)))
#define Node_INT_A_Equal_Value(_name, _TargetKey_A, _Value) static_cast<CBT_CompareValue*>(CManagement::Get_Instance()->Clone_Node(L"CompareValue", CBT_Node_Manager::DECORATOR, &CBT_CompareValue::INFO(_name, CBT_CompareValue::Value::INT, _TargetKey_A, CBT_CompareValue::Mode::EQUAL, false, 0.f, _Value)))

#define Node_BOOL_A_NotEqual_Value(_name, _TargetKey_A, _Value) static_cast<CBT_CompareValue*>(CManagement::Get_Instance()->Clone_Node(L"CompareValue", CBT_Node_Manager::DECORATOR, &CBT_CompareValue::INFO(_name, CBT_CompareValue::Value::BOOL, _TargetKey_A, CBT_CompareValue::Mode::NOT_EQUAL, _Value, 0.f, 0)))
#define Node_INT_A_NotEqual_Value(_name, _TargetKey_A, _Value) static_cast<CBT_CompareValue*>(CManagement::Get_Instance()->Clone_Node(L"CompareValue", CBT_Node_Manager::DECORATOR, &CBT_CompareValue::INFO(_name, CBT_CompareValue::Value::INT, _TargetKey_A, CBT_CompareValue::Mode::NOT_EQUAL, false, 0.f, _Value)))

#define Node_FLOAT_A_Greater_Than_Value(_name, _TargetKey_A, _Value) static_cast<CBT_CompareValue*>(CManagement::Get_Instance()->Clone_Node(L"CompareValue", CBT_Node_Manager::DECORATOR, &CBT_CompareValue::INFO(_name, CBT_CompareValue::Value::FLOAT, _TargetKey_A, CBT_CompareValue::Mode::GREATER_THAN, false, _Value, 0)))
#define Node_INT_A_Greater_Than_Value(_name, _TargetKey_A, _Value) static_cast<CBT_CompareValue*>(CManagement::Get_Instance()->Clone_Node(L"CompareValue", CBT_Node_Manager::DECORATOR, &CBT_CompareValue::INFO(_name, CBT_CompareValue::Value::INT, _TargetKey_A, CBT_CompareValue::Mode::GREATER_THAN, false, 0.f, _Value)))

#define Node_FLOAT_A_Smaller_Than_Value(_name, _TargetKey_A, _Value) static_cast<CBT_CompareValue*>(CManagement::Get_Instance()->Clone_Node(L"CompareValue", CBT_Node_Manager::DECORATOR, &CBT_CompareValue::INFO(_name, CBT_CompareValue::Value::FLOAT, _TargetKey_A, CBT_CompareValue::Mode::SMALLER_THAN, false, _Value, 0)))
#define Node_INT_A_Smaller_Than_Value(_name, _TargetKey_A, _Value) static_cast<CBT_CompareValue*>(CManagement::Get_Instance()->Clone_Node(L"CompareValue", CBT_Node_Manager::DECORATOR, &CBT_CompareValue::INFO(_name, CBT_CompareValue::Value::INT, _TargetKey_A, CBT_CompareValue::Mode::SMALLER_THAN, false, 0, _Value)))

// Service
#include "..\Headers\BT_UpdatePos.h"
#define Node_UpdatePos(_name, _Target_Key, _Target_Transform, _iMaxCount_Of_Execution, _dCoolTime, _dOffset, _Service_Mode) static_cast<CBT_UpdatePos*>(CManagement::Get_Instance()->Clone_Node(L"UpdatePos", CBT_Node_Manager::SERVICE, &CBT_UpdatePos::INFO(_name, _Target_Key, _Target_Transform, _iMaxCount_Of_Execution, _dCoolTime, _dOffset, _Service_Mode)))

#include "..\Headers\BT_UpdateGageRatio.h"
#define Node_UpdateGageRatio(_name, _Key_Save_HPRatio, _Key_MAXHP, _Key_HP, _iMaxCount_Of_Execution, _dCoolTime, _dOffset, _Service_Mode) static_cast<CBT_UpdateGageRatio*>(CManagement::Get_Instance()->Clone_Node(L"UpdateHPRatio", CBT_Node_Manager::SERVICE, &CBT_UpdateGageRatio::INFO(_name, _Key_Save_HPRatio, _Key_MAXHP, _Key_HP, _iMaxCount_Of_Execution, _dCoolTime, _dOffset, _Service_Mode)))

#include "..\Headers\BT_UpdateParam.h"
#define Node_UpdateParam(_name, _pObject_Param, _eParam, _dService_Start_Time, _MaxCount_Of_Execution, _dCoolTime, _dOffset) static_cast<CBT_UpdateParam*>(CManagement::Get_Instance()->Clone_Node(L"UpdateParam", CBT_Node_Manager::SERVICE, &CBT_UpdateParam::INFO(_name, _pObject_Param, _eParam, _dService_Start_Time, _MaxCount_Of_Execution, _dCoolTime, _dOffset, CBT_Service_Node::Finite)))

#include "..\Headers\BT_CreateEffect.h"
#define Node_CreateEffect_Infinite(_name, _Effect_Tag, _Create_Pos_Key, _iMaxCount_Of_Execution, _dCoolTime, _dOffset) static_cast<CBT_CreateEffect*>(CManagement::Get_Instance()->Clone_Node(L"CreateEffect", CBT_Node_Manager::SERVICE, &CBT_CreateEffect::INFO(_name, _Effect_Tag, _Create_Pos_Key,  0, 0, _iMaxCount_Of_Execution, _dCoolTime, _dOffset, CBT_Service_Node::Mode::Infinite, CBT_CreateEffect::One)))
#define Node_CreateParticle_Infinite(_name, _Effect_Tag, _Create_Pos_Key, _dEffect_Life_Time, _iMaxCount_Of_Execution, _dCoolTime, _dOffset) static_cast<CBT_CreateEffect*>(CManagement::Get_Instance()->Clone_Node(L"CreateEffect", CBT_Node_Manager::SERVICE, &CBT_CreateEffect::INFO(_name, _Effect_Tag, _Create_Pos_Key, _dEffect_Life_Time, 0, _iMaxCount_Of_Execution, _dCoolTime, _dOffset, CBT_Service_Node::Mode::Infinite, CBT_CreateEffect::Particle)))

#define Node_CreateEffect_Finite(_name, _Effect_Tag, _Create_Pos_Key, _dService_Start_Time, _iMaxCount_Of_Execution, _dCoolTime, _dOffset) static_cast<CBT_CreateEffect*>(CManagement::Get_Instance()->Clone_Node(L"CreateEffect", CBT_Node_Manager::SERVICE, &CBT_CreateEffect::INFO(_name, _Effect_Tag, _Create_Pos_Key, 0, _dService_Start_Time, _iMaxCount_Of_Execution, _dCoolTime, _dOffset, CBT_Service_Node::Mode::Finite, CBT_CreateEffect::One)))
#define Node_CreateParticle_Finite(_name, _Effect_Tag, _Create_Pos_Key, _dEffect_Life_Time, _dService_Start_Time, _iMaxCount_Of_Execution, _dCoolTime, _dOffset) static_cast<CBT_CreateEffect*>(CManagement::Get_Instance()->Clone_Node(L"CreateEffect", CBT_Node_Manager::SERVICE, &CBT_CreateEffect::INFO(_name, _Effect_Tag, _Create_Pos_Key, _dEffect_Life_Time, _dService_Start_Time, _iMaxCount_Of_Execution, _dCoolTime, _dOffset, CBT_Service_Node::Mode::Finite, CBT_CreateEffect::Particle)))

#include "..\Headers\BT_CreateBullet.h"
#define Node_CreateBullet(_name, _Object_Tag, _Create_Pos_Key, _Dir_Key, _fSpeed, _dBulletLifeTime, _dService_Start_Time, _MaxCount_Of_Execution, _dCoolTime, _dOffset, _Service_Mode) static_cast<CBT_CreateBullet*>(CManagement::Get_Instance()->Clone_Node(L"CreateBullet", CBT_Node_Manager::SERVICE, &CBT_CreateBullet::INFO(_name, _Object_Tag, _Create_Pos_Key, _Dir_Key, _fSpeed, _dBulletLifeTime, _dService_Start_Time, _MaxCount_Of_Execution, _dCoolTime, _dOffset, _Service_Mode)))


// Task
#include "..\Headers\BT_Wait.h"
#define Node_Wait(_name, _dTime, _dOffset) static_cast<CBT_Wait*>(CManagement::Get_Instance()->Clone_Node(L"Wait", CBT_Node_Manager::TASK, &CBT_Wait::INFO(_name, _dTime, _dOffset)))

#include "..\Headers\BT_MoveDirectly.h"
#define Node_MoveDirectly_Chase(_name, _Target_Key, _fMove_Speed, _fAcceptable_Radius) static_cast<CBT_MoveDirectly*>(CManagement::Get_Instance()->Clone_Node(L"MoveDirectly", CBT_Node_Manager::TASK, &CBT_MoveDirectly::INFO(_name, m_pTransformCom, m_pNavMesh, _Target_Key, _fMove_Speed, _fAcceptable_Radius, 0, 0, CBT_MoveDirectly::CHASE)))
#define Node_MoveDirectly_Rush(_name, _fMove_Speed, _dMoveTime, _dTimeOffset) static_cast<CBT_MoveDirectly*>(CManagement::Get_Instance()->Clone_Node(L"MoveDirectly", CBT_Node_Manager::TASK, &CBT_MoveDirectly::INFO(_name, m_pTransformCom, m_pNavMesh, nullptr, _fMove_Speed, 0, _dMoveTime, _dTimeOffset, CBT_MoveDirectly::RUSH)))

#include "..\Headers\BT_MoveAround.h"
#define Node_MoveAround(_name, _Target_Key, _fMove_Speed, _dMoveTime, _dTimeOffset) static_cast<CBT_MoveAround*>(CManagement::Get_Instance()->Clone_Node(L"MoveAround", CBT_Node_Manager::TASK, &CBT_MoveAround::INFO(_name, m_pTransformCom, m_pNavMesh, _Target_Key, _fMove_Speed, _dMoveTime, _dTimeOffset)))

#include "..\Headers\BT_MoveDir.h"
#define Node_MoveDir(_name, _Dir_Key, _fMove_Speed, _dMoveTime, _dTimeOffset) static_cast<CBT_MoveDir*>(CManagement::Get_Instance()->Clone_Node(L"MoveDir", CBT_Node_Manager::TASK, &CBT_MoveDir::INFO(_name, m_pTransformCom, _Dir_Key, _fMove_Speed, _dMoveTime, _dTimeOffset)))

#include "..\Headers\BT_RotationDir.h"
#define Node_RotationDir(_name, _Target_Key, _dTurnTime) static_cast<CBT_RotationDir*>(CManagement::Get_Instance()->Clone_Node(L"RotationDir", CBT_Node_Manager::TASK, &CBT_RotationDir::INFO(_name, m_pTransformCom, _Target_Key, _dTurnTime)))

#include "..\Headers\BT_ChaseDir.h"
#define Node_ChaseDir(_name, _Target_Key, _dRunTime, _dTimeOffset) static_cast<CBT_ChaseDir*>(CManagement::Get_Instance()->Clone_Node(L"ChaseDir", CBT_Node_Manager::TASK, &CBT_ChaseDir::INFO(_name, m_pTransformCom, _Target_Key, _dRunTime, _dTimeOffset)))

#include "..\Headers\BT_MoveTo.h"

#include "..\Headers\BT_Play_Ani.h"
#define Node_Ani(_name, _Index, _fWeight) static_cast<CBT_Play_Ani*>(CManagement::Get_Instance()->Clone_Node(L"Play_Ani", CBT_Node_Manager::TASK, &CBT_Play_Ani::INFO(_name, m_pMeshCom, _Index, _fWeight)))

#include "..\Headers\BT_SetValue.h"
#define Node_BOOL_SetValue(_name, _Target_Key, _Value) static_cast<CBT_SetValue*>(CManagement::Get_Instance()->Clone_Node(L"SetValue", CBT_Node_Manager::TASK, &CBT_SetValue::INFO(_name, _Target_Key, _Value, 0.f, 0, _v3(0.f, 0.f, 0.f), CBT_SetValue::BOOL)))
#define Node_FLOAT_SetValue(_name, _Target_Key, _Value) static_cast<CBT_SetValue*>(CManagement::Get_Instance()->Clone_Node(L"SetValue", CBT_Node_Manager::TASK, &CBT_SetValue::INFO(_name, _Target_Key, 0, _Value, 0, _v3(0.f, 0.f, 0.f), CBT_SetValue::FLOAT)))
#define Node_INT_SetValue(_name, _Target_Key, _Value) static_cast<CBT_SetValue*>(CManagement::Get_Instance()->Clone_Node(L"SetValue", CBT_Node_Manager::TASK, &CBT_SetValue::INFO(_name, _Target_Key, 0, 0.f, _Value, _v3(0.f, 0.f, 0.f), CBT_SetValue::INT)))
#define Node_VEC3_SetValue(_name, _Target_Key, _Value) static_cast<CBT_SetValue*>(CManagement::Get_Instance()->Clone_Node(L"SetValue", CBT_Node_Manager::TASK, &CBT_SetValue::INFO(_name, _Target_Key, 0, 0.f, 0, _Value, CBT_SetValue::VEC3)))
