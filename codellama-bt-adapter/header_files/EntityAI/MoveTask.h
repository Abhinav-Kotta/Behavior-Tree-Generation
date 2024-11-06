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
//--| Defines the MoveTask state tree task.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/EntityStateComponent.h"
#include "Components/MoveToComponent.h"
#include "EntityAI/MoveTaskDataComponent.h"
#include "SimTimer.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "MoveTask.generated.h"

/// @brief Instance data for `FMoveTask`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct SIMULATIONBEHAVIORS_API FMoveTaskInstanceData
{
    GENERATED_BODY()

    /// @brief The Acceptable distance between the Entity and the target location.
    /// A point on the route is considered hit when the entity gets within this range
    /// of the point.
    UPROPERTY(EditAnywhere, Category = Parameter)
    float TargetRange = 0.5f;

    UPROPERTY(EditAnywhere, Category = Parameter)
    /// @brief Is this move task, A "Formation MoveTo Task"
    bool bFormationMoveTo = false;

    UPROPERTY(EditAnywhere, Category = Parameter, meta = (EditCondition = "bFormationMoveTo"))
    /// @brief How often do we check to see if our followers are out of formation.
    float UpdateInterval = 2.0f;

    UPROPERTY(EditAnywhere, Category = Parameter, meta = (EditCondition = "bFormationMoveTo"))
    /// @brief Maximum allowed distance out of formation in meters.
    float MaxAllowedDistanceOutOfFormation_Meters = 5.0f;

    UPROPERTY()
    /// @brief Time until the next update should be performed.  Counts down to 0, then is reset.
    float TimeTillNextUpdate = 0.0f;

    UPROPERTY(EditAnywhere, Category = Parameter)
    /// @brief Flag to determine whether we should halt movement.
    bool bIsHalted = false;

    /// @brief Clock used to track time between frames.
    SimTimer SimClock;
};

/// @brief State tree task for moving along a route.
///
/// This task requires that the entity have the following components assigned:
/// * FMoveTaskDataComponent
/// * FMoveToComponent
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
USTRUCT(meta = (MilVerseEntityLevel))
struct SIMULATIONBEHAVIORS_API FMoveTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FMoveTaskInstanceData;

    /// @brief Constructor.
    FMoveTask();

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
    /// @sa FMoveTaskInstanceData
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
    /// @brief Handle for the `FMoveToComponent` ECS component.
    TStateTreeExternalDataHandle<FMoveToComponent> MoveToHandle;

    /// @brief Handle for the `FMoveTaskDataComponent` ECS component.
    TStateTreeExternalDataHandle<FMoveTaskDataComponent> MoveTaskDataHandle;

    /// @brief Handle for the `FEntityStateComponent` ECS component.
    TStateTreeExternalDataHandle<FEntityStateComponent> EntityStateHandle;

private:
    /// @brief Adjusts the follower speed in order to maintain the formation.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param MoveTaskData
    ///     Move task data for the entity. It is assumed that the leader is valid as well as the
    ///     current route point index.
    /// @param EntityState
    ///     The entity state data for the entity.
    /// @param OutMoveTo
    ///     Movement control data for the entity. Speed will be updated.
    /// @returns
    ///     The running status of the task.
    ///     * EStateTreeRunStatus::Running if successful and the entity can move along the
    ///     route.
    ///     * EStateTreeRunStatus::Failed if an error occurred and the entity will not be able
    ///     to
    ///       move along the route.
    EStateTreeRunStatus AdjustFollowerSpeed(const FStateTreeExecutionContext& Context,
        const FMoveTaskDataComponent& MoveTaskData,
        const FEntityStateComponent& EntityState,
        FMoveToComponent& OutMoveTo) const;

    /// @brief Draws debug symbology common to both leaders and followers.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param MoveTaskData
    ///     Move task data for the entity. It is assumed that the leader is valid as well as the
    ///     current route point index.
    /// @param EntityState
    ///     The entity state data for the entity.
    void DrawDebugSymbology(const FStateTreeExecutionContext& Context,
        const FMoveTaskDataComponent& MoveTaskData,
        const FEntityStateComponent& EntityState) const;

    /// @brief Draw formation debug symbology for leaders.
    /// @param Context
    ///     The state tree context.
    /// @param EntityState
    ///     The entity state data for the entity.
    /// @param Point
    ///     The route point with the associated FormationInstanceID for debug.
    void DrawFormationDebugSymbology(const FStateTreeExecutionContext& Context,
        const FEntityStateComponent& EntityState,
        const FRoutePoint& Point) const;

private:
    // NOTE: These fields don't change after the task instance has been created. If they did, they
    //       would need to be in the instance data struct.

    /// @brief Distance in unreal engine units (cm) squared for the follower distance threshold.
    float FollowerDistanceThresholdCmSquared = FMath::Square(100);
};

//--------------------------------------------------------------------------------------------------

/// @brief Instance data for `FMoveTask`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct SIMULATIONBEHAVIORS_API FChangeSpeedTaskInstanceData
{
    GENERATED_BODY()

    /// @brief The desired new speed in meters per second.
    UPROPERTY(EditAnywhere, Category = Parameter)
    float DesiredSpeed = 0.0f;
};

/// @brief State tree task for changing speed of movement.
///
/// This task requires that the entity have the following components assigned:
/// * FMoveToComponent
///
/// Returns EStateTreeRunStatus::Succeeded when the speed is changed (First tick)
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct SIMULATIONBEHAVIORS_API FChangeSpeedTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FChangeSpeedTaskInstanceData;

    /// @brief Constructor.
    FChangeSpeedTask();

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
    /// @sa FMoveTaskInstanceData
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
    ///     * EStateTreeRunStatus::Running - Always returns running.
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
    ///     * EStateTreeRunStatus::Succeeded  Changes the speed and returns Succeeded
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;

protected:
    /// @brief Handle for the `FMoveToComponent` ECS component.
    TStateTreeExternalDataHandle<FMoveToComponent> MoveToHandle;
};