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
//--| Defines the SuppressionFireTask state tree task.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/Engagement/AimComponent.h"
#include "Components/Engagement/FireWeaponComponent.h"
#include "Components/Engagement/SuppressionFireComponent.h"
#include "Components/EntityStateComponent.h"
#include "Components/InventoryComponent.h"
#include "SimTimer.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "SuppressionFireTask.generated.h"

/// @brief Instance data for 'FSuppressionFireTask'.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FSuppressionFireTaskInstanceData
{
    GENERATED_BODY()

    /// @brief If true then suppression fire will only last for a specific amount of time.
    UPROPERTY(EditAnywhere, Category = Parameter)
    bool bTimeBasedSuppressionFire = true;

    /// @brief The amount of time to perform suppression fire for.
    UPROPERTY(EditAnywhere, Category = Parameter, meta = (UIMin = 0.5, ClampMin = 0.5))
    float TimeToPerformTask = 2.0;

    /// @brief The time remaining to perform the task.
    float TaskTimeRemaining;

    /// @brief Used to track time between frames for a system.
    SimTimer SimClock;
};

/// @brief State tree task for performing suppression fire
///
/// This task requires that the entity have the following components assigned
/// * FSuppressionFireComponent
/// * FFireWeaponComponent
/// * FAimComponent
///
/// The task performs suppression fire for a fixed duration. It finishes after the
/// time has expired and the trigger is released.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT(meta = (MilVerseEntityLevel))
struct SIMULATIONBEHAVIORS_API FSuppressionFireTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Constructor
    FSuppressionFireTask();

    /// @brief Alias for this task's instance data type
    using FInstanceDataType = FSuppressionFireTaskInstanceData;

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
    /// @sa FSuppressionFireTaskInstanceData
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
    ///     The running status of the task. Returns running.
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override;

    /// @brief Called during the state tree tick when the task is part of an active state.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param DeltaSeconds
    ///     The time in seconds since `Tick` was last called.
    /// @returns
    ///     The running status of the task.
    ///     * EStateTreeRunStatus::Running while suppression fire is occurring.
    ///     * EStateTreeRunStatus::Succeeded when suppression fire stops and
    ///       the timer is exhausted.
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;

    /// @brief Called when a current state is exited and task is part of active states.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param Transition
    ///     Information about the transition that caused the state to be exited. Not used for this
    ///     task.
    virtual void ExitState(FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override;

protected:
    /// @brief Handle for the `FSuppressionFireComponent` ECS component.
    TStateTreeExternalDataHandle<FSuppressionFireComponent> SuppressionFireHandle;

    /// @brief Handle for the `FFireWeaponComponent` ECS component.
    TStateTreeExternalDataHandle<FFireWeaponComponent> FireWeaponHandle;

    /// @brief Handle for the `FEntityStateComponent` ECS component.
    TStateTreeExternalDataHandle<FEntityStateComponent> EntityStateHandle;

    /// @brief Handle for the `FAimComponent` ECS component.
    TStateTreeExternalDataHandle<FAimComponent> AimHandle;

    /// @brief Handle for the 'FInventoryComponent' ECS component
    TStateTreeExternalDataHandle<FInventoryComponent> InventoryComponentHandle;

    /// @brief Handle for the 'FInventoryWeaponsComponent' ECS component
    TStateTreeExternalDataHandle<FInventoryWeaponsComponent> WeaponsInventoryComponentHandle;
};
