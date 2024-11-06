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
//--| Defines a StateTree condition related to combat readiness.
//--|
//--|====================================================================|--
#pragma once

#include "AI/MilVerseStateTreeCondition.h"
#include "Components/CombatReadinessComponent.h"

#include "CoreMinimal.h"
#include "CombatReadinessCondition.generated.h"

/// @brief Instance data for `FCombatReadinessCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FCombatReadinessConditionInstanceData
{
    GENERATED_BODY()

    /// @brief The minimum combat readiness value for this condition to return true.
    /// The condition returns true when combat readiness is greater than or equal to
    /// this value.
    /// If bInvert is true, then the condition returns true when combat readiness is
    /// less than this value.
    UPROPERTY(EditAnywhere, Category = Parameter)
    float MinimumCombatReadinessPercent = 0.0f;

    /// @brief If this is true, then the condition is inverted.
    UPROPERTY(EditAnywhere, Category = Parameter)
    bool bInvert = false;
};

/// @brief State tree condition for determining if an entity is ready to attack.
///
/// This condition requires that the entity have the following components assigned:
/// * `FCombatReadinessComponent`
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct FCombatReadinessCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FCombatReadinessConditionInstanceData;

    /// @brief Constructor
    FCombatReadinessCondition() = default;

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
    /// @sa FAttackConditionInstanceData
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Tests the condition.
    ///
    /// @param Context
    ///     The state tree context.
    /// @returns
    /// The condition returns true when combat readiness is greater than or equal to
    /// this value.
    /// If bInvert is true, then the condition returns true when combat readiness is
    /// less than this value
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

protected:
    /// @brief Handle for the `FCombatReadinessComponent` ECS component.
    TStateTreeExternalDataHandle<FCombatReadinessComponent> CombatReadinessHandle;
};
