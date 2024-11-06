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
//--| Defines the MoveForDurationTask state tree task.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "EntityAI/MoveTask.h"
#include "SimTimer.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "MoveForDurationTask.generated.h"

/// @brief Instance data for `FMoveForDurationTask`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct SIMULATIONBEHAVIORS_API FMoveForDurationTaskInstanceData : public FMoveTaskInstanceData
{
    GENERATED_BODY()

    /// @brief The entity will move along the route for this specified
    /// duration in seconds.
    UPROPERTY(EditAnywhere, Category = Parameter, meta = (ClampMin = "0.0"))
    float Duration = 1.f;

    /// @brief Adds random value in the range of +/- this
    /// value in seconds to the duration.
    UPROPERTY(EditAnywhere, Category = Parameter, meta = (ClampMin = "0.0"))
    float RandomDeviation = 0.f;

    /// @brief The remaining time to move in seconds.
    UPROPERTY(EditAnywhere, Category = Output)
    float RemainingTime = 0.f;

    /// @brief Used to track time between frames for a system.
    SimTimer SimClock;
};

/// @brief State tree task for moving along a route for a specified amount of time.
///
/// Inherits from FMoveTask.
///
/// Returns Succeeded when the time to move expires.
/// Returns FMoveTask::Tick if the time has not expired.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct SIMULATIONBEHAVIORS_API FMoveForDurationTask : public FMoveTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FMoveForDurationTaskInstanceData;

    /// @brief Constructor.
    FMoveForDurationTask();

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
    /// @sa FMoveForDurationTaskInstanceData
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
    ///     * EStateTreeRunStatus::Running if successful and the entity can move along the route.
    ///     * EStateTreeRunStatus::Succeeded if there are no route points meaning the task is
    ///       complete. Also returns succeeded if the time to move along the route has expired.
    ///     * EStateTreeRunStatus::Failed if an error occurred and the entity will not be able to
    ///       move along the route.
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
    /// @param DeltaSeconds
    ///     The time in seconds since `Tick` was last called.
    /// @returns
    ///     The running status of the task.
    ///     * EStateTreeRunStatus::Running if successful and the entity can move along the route.
    ///     * EStateTreeRunStatus::Succeeded  if the entity completed its route.
    ///     * EStateTreeRunStatus::Failed if an error occurred and the entity will not be able to
    ///       move along the route.
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;
};
