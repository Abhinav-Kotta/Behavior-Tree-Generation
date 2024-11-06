//--|====================================================================|--
//--| CLASSIFICATION: UNCLASSIFIED
//--|====================================================================|--
//--| This program is the sole property of the Lockheed Martin Corporation
//--| and contains proprietary and confidential information. Use or
//--| disclosure of this program is subject to the terms and conditions of
//--| a license agreement with the Lockheed Martin Corporation. Unauthorized
//--| use or distribution will be subject to action as prescribed by the
//--| license agreement.
//--|
//--| Copyright 2023 by Lockheed Martin Corporation
//--|====================================================================|--
//--|
//--| Description:
//--| Defines the UnitMemberDestroyedEvent class.
//--|
//--|====================================================================|--
#pragma once

// MILVERSE
#include "AI/MilVerseStateTreeEvent.h"
#include "ECS/SimComponent.h"

// UNREAL ENGINE
#include "CoreMinimal.h"
#include "NativeGameplayTags.h"
#include "UnitMemberDestroyedEvent.generated.h"

//--------------------------------------------------------------------------------------------------

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MILVERSE_UNIT_MEMBER_DESTROYED_EVENT)

//--------------------------------------------------------------------------------------------------

/// @brief State tree event payload for @ref UnitMemberDestroyedEvent.
///
/// @ingroup MilVerseCore-Plugin
USTRUCT()
struct FUnitMemberDestroyedEventPayload
{
    GENERATED_BODY()

    /// @brief Flag to denote whether or not the true leader has been killed.
    UPROPERTY()
    bool bTrueLeaderKilled = false;
};

//--------------------------------------------------------------------------------------------------

/// @brief Event emitted whenever a member of a unit is destroyed.
///
/// @ingroup SimulationBehaviors-Module
class SIMULATIONBEHAVIORS_API UnitMemberDestroyedEvent
    : public MilVerseStateTreeEvent<UnitMemberDestroyedEvent>
{
    MILVERSE_LOCAL_SIM_EVENT(UnitMemberDestroyedEvent)

public:
    /// @brief Flag to denote whether or not the true leader has been killed. Passed to the State
    /// Tree Payload for use within the tree.
    bool bTrueLeaderKilled = false;

public:
    /// @brief Returns the gameplay tag associated with the state tree event.
    FGameplayTag GetGameplayTag() const override;

    /// @brief Returns the payload to include in the state tree event.
    FInstancedStruct GetPayload() const override;
};