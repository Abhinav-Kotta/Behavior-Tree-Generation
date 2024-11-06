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
//--| Defines the SignalUnitEnemySpottedEvent class.
//--| Used for signaling the state tree to switch states.
//--|
//--|====================================================================|--
#pragma once

// MILVERSE
#include "AI/MilVerseStateTreeEvent.h"
#include "CommonAI/CommonTypes.h"
#include "ECS/SimComponent.h"

// UNREAL ENGINE
#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

//--------------------------------------------------------------------------------------------------

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MILVERSE_SIGNAL_UNIT_ENEMY_SPOTTED_EVENT)

//--------------------------------------------------------------------------------------------------

/// @brief Event emitted to signal unit level state tree to switch states.
///
/// @ingroup SimulationBehaviors-Module
class SIMULATIONBEHAVIORS_API SignalUnitEnemySpottedEvent
    : public MilVerseStateTreeEvent<SignalUnitEnemySpottedEvent>
{
    MILVERSE_LOCAL_SIM_EVENT(SignalUnitEnemySpottedEvent)

public:
    /// @brief Returns the gameplay tag associated with the state tree event.
    FGameplayTag GetGameplayTag() const override;
};