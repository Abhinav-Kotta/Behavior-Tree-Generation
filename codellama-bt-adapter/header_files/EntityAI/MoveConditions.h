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
//--| Copyright 2022 by Lockheed Martin Corporation
//--|====================================================================|--
//--|
//--| Description:
//--| Defines StateTree conditions related to movement.
//--|
//--|====================================================================|--
#pragma once

#include "AI/MilVerseStateTreeCondition.h"
#include "EntityAI/MoveTaskDataComponent.h"

#include "CoreMinimal.h"
#include "MoveConditions.generated.h"

/// @brief Instance data for `FMoveRequestedCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FMoveRequestedConditionInstanceData
{
    GENERATED_BODY()
};

/// @brief State tree condition for determining if an entity has been ordered to move.
///
/// This condition requires that the entity have the following components assigned:
/// * `FMoveTaskDataComponent`
///
/// The condition is true if a movement was ordered, and a movement wasn't previously
/// ordered.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct FMoveRequestedCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FMoveRequestedConditionInstanceData;

    /// @brief Constructor
    FMoveRequestedCondition() = default;

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
    /// @sa FMoveRequestedConditionInstanceData
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Tests the condition.
    ///
    /// @param Context
    ///     The state tree context.
    /// @returns
    ///     True if movement has been requested for the entity or false otherwise. Uses
    ///     `FMoveTaskDataComponent::bMoveOrdered`.
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

protected:
    /// @brief Handle for the `FMoveTaskDataComponent` ECS component.
    TStateTreeExternalDataHandle<FMoveTaskDataComponent> MoveTaskDataHandle;
};

/// @brief Instance data for `FMoveCompletedCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FMoveCompletedConditionInstanceData
{
    GENERATED_BODY()
};

/// @brief State tree condition for determining if an entity has completed an order to move.
///
/// This condition requires that the entity have the following components assigned:
/// * `FMoveTaskDataComponent`
///
/// The condition is true if a movement was completed.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct FMoveCompletedCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FMoveCompletedConditionInstanceData;

    /// @brief Constructor
    FMoveCompletedCondition() = default;

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
    /// @sa FMoveCompletedConditionInstanceData
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Tests the condition.
    ///
    /// @param Context
    ///     The state tree context.
    /// @returns
    ///     True if movement has been requested for the entity or false otherwise. Uses
    ///     `FMoveTaskDataComponent::bMoveOrdered`.
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

protected:
    /// @brief Handle for the `FMoveTaskDataComponent` ECS component.
    TStateTreeExternalDataHandle<FMoveTaskDataComponent> MoveTaskDataHandle;

    /// @brief Inverts the result of `TestCondition` when true.
    UPROPERTY(EditAnywhere, Category = Condition)
    bool bInvert = false;
};
