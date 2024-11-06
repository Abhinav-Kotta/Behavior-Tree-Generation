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
//--| Defines the Change orientation state tree task.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/Engagement/AimComponent.h"
#include "Components/UnrealActorComponent.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "SimTimer.h"
#include "ChangeOrientationTask.generated.h"

/// @brief Instance data for `FChangeOrientationTask`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct SIMULATIONBEHAVIORS_API FChangeOrientationTaskInstanceData
{
    GENERATED_BODY()

    /// @brief The desired yaw orientation of the Avatar  (degrees, unreal orientation)
    UPROPERTY(EditAnywhere, Category = Input)
    double YawOrientation_deg = 0.0;

    /// @brief Used to track time between frames for a system.
    SimTimer SimClock;
};

/// @brief State tree task for changing the Orientation for the avatar.
///
/// This task requires that the entity have the following components assigned:
/// * FAvatarControlComponent
///
/// The input is a desired Orientation. The task is successful when the Avatar's Orientation
/// matches the desired Orientation.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct SIMULATIONBEHAVIORS_API FChangeOrientationTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FChangeOrientationTaskInstanceData;

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
    /// @sa FChangeOrientationTaskInstanceData
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
    ///     * EStateTreeRunStatus::Succeeded upon entering the state
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

    /// @brief Called when the state is entered that this task is part of.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param DeltaTime
    ///     Elapsed time for the simulation, in seconds
    /// @returns
    ///     The running status of the task.
    ///     * EStateTreeRunStatus::Succeeded If the task is complete
    ///     * EStateTreeRunStatus::Running when the task is still running
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;

protected:
    /// @brief Handle for the `FUnrealActorComponent` ECS component.
    TStateTreeExternalDataHandle<FUnrealActorComponent> UnrealActorHandle;

    /// @brief Handle for the `FAimComponent` ECS component.
    TStateTreeExternalDataHandle<FAimComponent> AimHandle;
};
