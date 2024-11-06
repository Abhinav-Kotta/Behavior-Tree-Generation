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
//--| Defines the UnitChangedEvent class.
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

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_MILVERSE_UNIT_CHANGED_EVENT)

//--------------------------------------------------------------------------------------------------

/// @brief Event emitted whenever a unit's composition changes.
///
/// @ingroup SimulationBehaviors-Module
class SIMULATIONBEHAVIORS_API UnitChangedEvent : public MilVerseStateTreeEvent<UnitChangedEvent>
{
    MILVERSE_LOCAL_SIM_EVENT(UnitChangedEvent)

public:
    /// @brief Returns the gameplay tag associated with the state tree event.
    FGameplayTag GetGameplayTag() const override;
};