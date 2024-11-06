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
//--| Defines the EnemySituationChangedEvent class. This should be used when
//--| the situation of the highest priority threat has changed. For example:
//--| if the range to the highest priority threat closes, the highest priority
//--| threat is killed, or a different threat becomes the highest priority.
//--|
//--|====================================================================|--
#pragma once

// MILVERSE
#include "AI/MilVerseStateTreeEvent.h"
#include "ECS/SimComponent.h"

// UNREAL ENGINE
#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

//--------------------------------------------------------------------------------------------------

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MILVERSE_ENEMY_SITUATION_CHANGED_EVENT)

//--------------------------------------------------------------------------------------------------

/// @brief Event emitted whenever a unit's composition changes.
///
/// @ingroup SimulationBehaviors-Module
class SIMULATIONBEHAVIORS_API EnemySituationChangedEvent
    : public MilVerseStateTreeEvent<EnemySituationChangedEvent>
{
    MILVERSE_LOCAL_SIM_EVENT(EnemySituationChangedEvent)

public:
    /// @brief Returns the gameplay tag associated with the state tree event.
    FGameplayTag GetGameplayTag() const override;
};