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
//--| Copyright 2024 by Lockheed Martin Corporation
//--|====================================================================|--
//--|
//--| Description:
//--| Checks whether a unit can engage the enemy (Is the enemy within the units max engagement
//--| distance?).
//--|
//--|====================================================================|--
#pragma once

// Unreal
#include "CoreMinimal.h"

// MilVerse
#include "AI/MilVerseStateTreeCondition.h"
#include "CommonAI/CommonTypes.h"
#include "Components/Engagement/CombatPowerComponent.h"
#include "Components/UnitControllerComponent.h"
#include "SimTimer.h"

#include "UnitCanEngageTargetCondition.generated.h"

/// @brief Instance data for `FUnitCanEngageTargetCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup UnitAI
USTRUCT()
struct FUnitCanEngageTargetConditionInstanceData
{
    GENERATED_BODY()

    /// @brief If set to true, Is the Enemy outside of our max engagement distance?
    UPROPERTY(EditAnywhere, Category = "Parameter")
    bool bInvert = false;
};

/// @brief Can our unit engage the target?
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup UnitAI
USTRUCT(meta = (MilVerseUnitLevel))
struct FUnitCanEngageTargetCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FUnitCanEngageTargetConditionInstanceData;

    /// @brief Constructor
    FUnitCanEngageTargetCondition() = default;

protected:
    /// @brief Called when the state tree asset is linked with data to allow the condition to
    /// resolve references to other state tree data.
    ///
    /// @param Linker
    ///     Reference to the state tree's linker.
    /// @returns
    ///     Returns `true` if linking is successful; Otherwise returns `false`.
    virtual bool Link(FStateTreeLinker& Linker) override;

    /// @brief Returns the struct definition for this condition's instance data.
    ///
    /// @sa FEnemyContactConditionInstanceData
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Tests the condition.
    ///
    /// @param Context
    ///     The state tree context.
    /// @returns
    ///     True if enemy contact is present, or false otherwise.
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

private:
    /// @brief Handle for the `FUnitControllerComponent` ECS component.
    TStateTreeExternalDataHandle<FUnitControllerComponent> UnitControllerHandle;
};