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
//--| Defines StateTree conditions related to following a leader.
//--|
//--|====================================================================|--
#pragma once

#include "AI/MilVerseStateTreeCondition.h"

#include "Components/EntityStateComponent.h"
#include "CoreMinimal.h"
#include "FollowLeaderConditions.generated.h"

/// @brief Instance data for ` FLeaderMovementCompleteCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FLeaderMovementCompleteConditionInstanceData
{
    GENERATED_BODY()

    /// @brief Formation instance id.
    UPROPERTY(VisibleAnywhere, Category = Input)
    int32 FormationID = -1;
};

/// @brief State tree condition for determining if a leader has completed movement.
///
/// This condition requires that the entity have the following components assigned:
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct FLeaderMovementCompleteCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FLeaderMovementCompleteConditionInstanceData;

    /// @brief Constructor
    FLeaderMovementCompleteCondition() = default;

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
    ///     True if the leader's movement is complete. bInvert if true, inverts this.
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

protected:
    /// @brief Inverts the result of `TestCondition` when true.
    UPROPERTY(EditAnywhere, Category = Condition)
    bool bInvert = false;
};

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

/// @brief Instance data for ` FInFormationWithLeaderCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FInFormationWithLeaderConditionInstanceData
{
    GENERATED_BODY()

    /// @brief Formation instance id.
    UPROPERTY(VisibleAnywhere, Category = Input)
    int32 FormationID = -1;

    /// @brief The tolerance on the distance from the ideal formation position.
    /// This distance is in Meters. The condition will return that an entity is
    /// in formation if the distance from the formation is within this tolerance.
    UPROPERTY(EditAnywhere, Category = Parameter)
    float DistanceFromFormationPositionTolerance = 2;

    /// @brief If true, the trailing distance will ignore the Z axis.
    UPROPERTY(EditAnywhere, Category = Parameter)
    bool IgnoreZAxis = true;

    /// @brief Flag controlling if the "Formup" behavior step be skipped.
    UPROPERTY(VisibleAnywhere, Category = Input)
    bool bShouldSkipFormup = false;
};

/// @brief State tree condition for determining if an follower entity is in formation with
/// a leader.
///
/// An entity is out of formation when it has deviated from the expected position on the route
/// significantly.
///
/// This condition requires that the entity have the following components assigned:
/// FEntityStateComponent
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct FInFormationWithLeaderCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FInFormationWithLeaderConditionInstanceData;

    /// @brief Constructor
    FInFormationWithLeaderCondition() = default;

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
    ///     True if aligned in formation with a leader.
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

protected:
    /// @brief Inverts the result of `TestCondition` when true.
    UPROPERTY(EditAnywhere, Category = Condition)
    bool bInvert = false;

    /// @brief Handle for the `FMoveTaskDataComponent` ECS component.
    TStateTreeExternalDataHandle<FEntityStateComponent> EntityStateHandle;
};