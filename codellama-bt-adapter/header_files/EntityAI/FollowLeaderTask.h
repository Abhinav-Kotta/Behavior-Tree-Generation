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
//--| Defines the FFollowLeaderTask state tree task.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/EntityStateComponent.h"
#include "Components/HealthComponent.h"
#include "Components/MoveToComponent.h"
#include "EntityAI/MoveTaskDataComponent.h"

#include "SimTimer.h"

// Unreal
#include "CoreMinimal.h"
#include "FollowLeaderTask.generated.h"

/// @brief Instance data for `FFollowLeaderTask`.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FFollowLeaderTaskInstanceData
{
    GENERATED_BODY()

    //
    // Inputs
    //

    /// @brief Formation instance id.
    UPROPERTY(VisibleAnywhere, Category = Input)
    int32 FormationID = -1;

    /// @brief Speed to move when leader is not moving.
    UPROPERTY(VisibleAnywhere, Category = Input)
    float FormupSpeed = 0.0f;

    //
    // General Parameters
    //

    /// @brief The acceptable distance in meters between the Entity and the target location.
    UPROPERTY(EditAnywhere, Category = Parameter)
    float ThresholdDistance = 0.5f;

    /// @brief Distance ahead of the entity in meters to generate a point.
    UPROPERTY(EditAnywhere, Category = Parameter)
    // TODO - Consider making below entity-type and speed specific
    float LookAheadDistance = 20.0f;

    /// @brief If true, task will run continuously, otherwise it will run once and complete
    /// successfully.
    ///
    /// When moving in formation, set to true to continuously update. Set to false when forming the
    /// formation.
    UPROPERTY(EditAnywhere, Category = Parameter)
    bool bRunContinuously = true;

    /// @brief If True, Follwers will be able to skip the "Form Up" behavior stage, This flag is
    /// mainly set through a binding from the UI
    UPROPERTY(EditAnywhere, Category = Parameter)
    bool bShouldSkipFormup = false;

    /// @brief If True, Follwers will be able to execute the "Form Up" behavior again, Should be set
    /// to false if only needing to form up once per order.
    UPROPERTY(EditAnywhere, Category = Parameter)
    bool bAllowedToFormUpAgain = false;

    /// @brief Flag to determine whether we should halt movement.
    UPROPERTY(EditAnywhere, Category = Parameter)
    bool bIsHalted = false;

    //
    // Conditional Parameters
    //

    /// @brief When running continuously, time between updates in seconds.
    ///
    /// If using dynamic polling, this is the update interval when not near the start and end route
    /// points.
    UPROPERTY(EditAnywhere, Category = Parameter, meta = (EditCondition = "bRunContinuously"))
    float UpdateInterval = 1.5f;

    /// @brief When true, will use the FRouteStatusComponent to updated the polling rate based on
    /// where the leader is within the route.
    ///
    /// When the leader is near the start of the route, the polling interval will be set to
    /// @ref NearStartUpdateInterval and it will be set to @ref NearEndUpdateInterval when near the
    /// end of the route. If in the middle, will be set to @ref UpdateInterval.
    UPROPERTY(EditAnywhere, Category = Parameter, meta = (EditCondition = "bRunContinuously"))
    bool bUseDynamicPolling = true;

    /// @brief Time between updates when the leader is near the start of the route when using the
    /// dynamic polling option.
    UPROPERTY(EditAnywhere,
        Category = Parameter,
        meta = (EditCondition = "bRunContinuously && bUseDynamicPolling"))
    float NearStartUpdateInterval = 0.1f;

    /// @brief Time between updates when the leader is near the end of the route when using the
    /// dynamic polling option.
    UPROPERTY(EditAnywhere,
        Category = Parameter,
        meta = (EditCondition = "bRunContinuously && bUseDynamicPolling"))
    float NearEndUpdateInterval = 0.1f;

    /// @brief Time between updates when the leader is near a turn in the route when using the
    /// dynamic polling option.
    UPROPERTY(EditAnywhere,
        Category = Parameter,
        meta = (EditCondition = "bRunContinuously && bUseDynamicPolling"))
    float NearTurnUpdateInterval = 0.1f;

    //
    // Internal Data
    //

    /// @brief The currently selected update interval in seconds.
    UPROPERTY()
    float ActiveUpdateInterval = 0.0f;

    /// @brief Time in seconds until the next update.
    UPROPERTY()
    float TimeTillNextUpdate = 0.0f;

    /// @brief If true, will wait for current move to complete before issueing any more movement
    /// updates.
    UPROPERTY()
    bool WaitForMoveToComplete = false;

    /// @brief Clock used to track time between frames.
    SimTimer SimClock;
};

/// @brief State tree task following leader.
///
/// This task requires that the entity have the following components assigned:
/// FMoveTaskDataComponent
/// FMoveToComponent
/// FEntityStateComponent
///
/// @ingroup SimulationBehaviors-Module
USTRUCT(meta = (DisplayName = "Follow Leader Task", MilVerseEntityLevel))
struct SIMULATIONBEHAVIORS_API FFollowLeaderTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FFollowLeaderTaskInstanceData;

    /// @brief Constructor for the Create Formations task.
    FFollowLeaderTask();

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
    ///     * EStateTreeRunStatus::Running if successful and a route is not yet planned..
    ///     * EStateTreeRunStatus::Succeeded if a route was planned with at least 2 points.
    ///     * EStateTreeRunStatus::Failed if a route could not be planned.
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
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;

protected:
    /// @brief Handle for the `FMoveTaskDataComponent` ECS component.
    TStateTreeExternalDataHandle<FMoveTaskDataComponent> MoveTaskDataHandle;

    /// @brief Handle for the `FMoveTaskDataComponent` ECS component.
    TStateTreeExternalDataHandle<FEntityStateComponent> EntityStateHandle;

    /// @brief Handle for the `FMoveToComponent` ECS component.
    TStateTreeExternalDataHandle<FMoveToComponent> MoveToHandle;

    /// @brief Handle for the 'FHealthComponent' ECS component.
    TStateTreeExternalDataHandle<FHealthComponent> HealthHandle;

private:
    /// @brief Update the follower's movement based on the data provided by the formation manager.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param InstanceData
    ///     The instance data for the entity being processed.
    bool UpdateFollowerMovement(FStateTreeExecutionContext& Context,
        FInstanceDataType& InstanceData) const;

    /// @brief Returns true if the current move request has been completed.
    ///
    /// @param Context
    ///     The state tree context.
    bool IsCurrentMoveComplete(FStateTreeExecutionContext& Context) const;

    /// @brief Updates the polling rate based on the position of the leader within its route.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param InstanceData
    ///     The instance data for the entity being processed.
    void UpdatePollingRate(FStateTreeExecutionContext& Context,
        FInstanceDataType& InstanceData) const;
};
