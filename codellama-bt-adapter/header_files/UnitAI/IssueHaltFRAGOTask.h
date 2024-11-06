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
//--| Defines IssueHaltFRAGOTask that issues events to entities to halt or resume order
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/UnitControllerComponent.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "IssueHaltFRAGOTask.generated.h"

//--------------------------------------------------------------------------------------------------

class UMoveTacticallyOrder;
class UOrder;
class UOrderSubsystem;

//--------------------------------------------------------------------------------------------------

/// @brief Instance data for @ref FIssueHaltFRAGOTask
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FIssueHaltFRAGOTaskInstanceData
{
    GENERATED_BODY()

    /// @brief True if enemy entity is seen by the unit
    UPROPERTY(VisibleAnywhere, Category = Input)
    bool bEnemySpotted = false;

    bool bPrevEnemySpotted = false;

    // @brief Guid fpr the Unit Controller
    ECS::EntityGUID UnitControllerEntityID;
};

//--------------------------------------------------------------------------------------------------

/// @brief Task for issuing Halt / Resume FRAGOs .
///
/// This task uses the following ECS component(s). The task will fail if they are missing
///
/// @ref FUnitControllerComponent
/// @ingroup SimulationBehaviors-Module
USTRUCT(meta = (MilVerseUnitLevel))
struct SIMULATIONBEHAVIORS_API FIssueHaltFRAGOTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this node's instance data type.
    using FInstanceDataType = FIssueHaltFRAGOTaskInstanceData;

protected:
    /// @brief Called when the state tree asset is linked with data to allow the task to resolve
    /// references to other state tree data.
    ///
    /// @param Linker
    ///     Reference to the state tree's linker.
    /// @returns
    ///     Returns `true` if linking is successful; Otherwise returns `false`.
    virtual bool Link(FStateTreeLinker& Linker) override;

    /// @brief Returns the struct definition for this task's instance data.
    ///
    /// @sa FRemoveDeadEntitiesInstanceData
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Called when the state is entered that this task is part of.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param Transition
    ///     Information about the transition that caused the state to be selected. Not used for this
    ///     task.
    /// @returns
    ///     The running status of the task.
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override;

    /// @brief Called when a current state is exited and task is part of active states.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param Transition Describes the states involved in the transition
    ///     Information about the transition that caused the state to be exited. Not used for this
    ///     task.
    virtual void ExitState(FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override;

    /// @brief Called during the state tree tick when the task is part of an active state.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param DeltaTime
    ///     The time in seconds since `Tick` was last called.
    /// @returns
    ///     The running status of the task.
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;

protected:
    /// @brief Data handle for the @ref FUnitControllerComponent ECS component.
    TOptionalStateTreeExternalDataHandle<FUnitControllerComponent> UnitControllerHandle;

private:
};
