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
//--| Defines the Milverse delay state tree task.
//--|
//--|====================================================================|--

#pragma once

#include "AI/MilVerseStateTreeTask.h"
#include "CoreMinimal.h"
#include "SimTimer.h"
#include "MilverseStateTreeDelayTask.generated.h"

USTRUCT()
struct SIMULATIONBEHAVIORS_API FMilverseStateTreeDelayTaskInstanceData
{
    GENERATED_BODY()

    /// @brief Delay before the task ends in seconds.
    UPROPERTY(EditAnywhere,
        Category = Parameter,
        meta = (EditCondition = "!bRunForever", ClampMin = "0.0"))
    float Duration = 1.f;

    /// @brief Adds random range in seconds (+/- this value) to the Duration.
    UPROPERTY(EditAnywhere,
        Category = Parameter,
        meta = (EditCondition = "!bRunForever", ClampMin = "0.0"))
    float RandomDeviation = 0.f;

    /// @brief If true the task will run forever until a transition stops it.
    UPROPERTY(EditAnywhere, Category = Parameter)
    bool bRunForever = false;

    /// @brief Remaining time before the end of the task. Internal countdown in seconds.
    UPROPERTY(EditAnywhere, Category = Output)
    float RemainingTime = 0.f;

    /// @brief Used to track time between frames for a system.
    SimTimer SimClock;
};

/// @brief Simple task to wait indefinitely or for a given time (in seconds) before succeeding.
///
/// Returns Running while waiting.
/// Returns Succeeded when the time to wait has elapsed.
///
USTRUCT(meta = (MilVerseGenericLevel))
struct SIMULATIONBEHAVIORS_API FMilverseStateTreeDelayTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    using FInstanceDataType = FMilverseStateTreeDelayTaskInstanceData;

    /// @brief Constructor.
    FMilverseStateTreeDelayTask();

protected:
    /// @brief Returns the struct definition for this nodes's instance data.
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
    ///     * EStateTreeRunStatus::Running until the remaining time
    ///         has expired
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
    ///     * EStateTreeRunStatus::Running until the remaining time
    ///         has expired
    ///     * EStateTreeRunStatus::Succeeded remaining time is <= 0
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;
};
