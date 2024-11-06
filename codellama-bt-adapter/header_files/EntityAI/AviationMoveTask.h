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
//--| Defines the AviationMoveTask state tree task.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/EntityStateComponent.h"
#include "Components/Flight/FlightRouteComponent.h"
#include "Routes/RoutePoint.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "AviationMoveTask.generated.h"

/// @brief Instance data for `FAviationMoveTask`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct SIMULATIONBEHAVIORS_API FAviationMoveTaskInstanceData
{
    GENERATED_BODY()

    /// Using ground routes as input for now until avaition routes are ready
    UPROPERTY(VisibleAnywhere, Category = Input)
    TArray<FRoutePoint> RoutePoints;
};

/// @brief State tree task for moving along a route.
///
/// This task requires that the entity have the following components assigned:
/// * FAviationMoveTaskDataComponent
/// * FEntityStateComponent
///
/// Returns EStateTreeRunStatus::Running if successful and the entity can move along the route.
/// Returns EStateTreeRunStatus::Succeeded if there are no route points meaning the task is
///       complete.
/// Returns EStateTreeRunStatus::Failed if an error occurred and the entity will not be able to
///       move along the route.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct SIMULATIONBEHAVIORS_API FAviationMoveTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FAviationMoveTaskInstanceData;

    /// @brief Constructor.
    FAviationMoveTask();

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
    /// @sa FAviationMoveTaskInstanceData
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
    ///       complete.
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

protected:
    /// @brief Handle for the `FFlightRouteComponent` ECS component.
    TStateTreeExternalDataHandle<FFlightRouteComponent> FlightRouteHandle;

    /// @brief Handle for the `FEntityStateComponent` ECS component.
    TStateTreeExternalDataHandle<FEntityStateComponent> EntityStateHandle;

private:
};

//--------------------------------------------------------------------------------------------------
