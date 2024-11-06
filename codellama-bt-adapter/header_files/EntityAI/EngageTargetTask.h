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
//--| Defines the FEngageTargetTask state tree task.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/Engagement/AimComponent.h"
#include "Components/Engagement/AttackTargetComponent.h"
#include "Components/Engagement/FireWeaponComponent.h"
#include "Components/Engagement/TargetComponent.h"
#include "Components/HealthComponent.h"
#include "Components/Sensing/SensedEntitiesComponent.h"

// Unreal
#include "CoreMinimal.h"
#include "EngageTargetTask.generated.h"

/// @brief Instance data for 'FEngageTargetTask'.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FEngageTargetTaskInstanceData
{
    GENERATED_BODY()

    /// @brief If true, fire at enemy if stunned.
    UPROPERTY(EditAnywhere, Category = Parameter)
    bool bIsAggressive = false;
};

/// @brief State tree task for performing engaging.
///
/// This task requires that the entity have the following components assigned
/// `FAttackTargetComponent`
/// `FFireWeaponComponent`
/// `FSensedEntitiesComponent`
/// `FTargetComponent`
///
/// Shoots at a target until the target is dead.
/// Returns Success when the target is dead.
/// Returns Failed when the target is no longer sensed.
/// Returns Running if the target is alive and sensed.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT(meta = (MilVerseEntityLevel))
struct SIMULATIONBEHAVIORS_API FEngageTargetTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FEngageTargetTaskInstanceData;

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
    /// @sa FEngageTargetTaskInstanceData
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
    /// @param Transition
    ///     Information about the transition that caused the state to be exited.
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
    ///     Returns Ssuccess when the target is dead.
    ///     Returns Failed when the target is no longer sensed.
    ///     Returns Running if the target is alive and sensed.
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;

protected:
    /// @brief Handle for the `FAttackTargetComponent` ECS component.
    ///
    /// This component is used to instruct the entity to attack a target. The target to attack is
    /// specified in the `FTargetComponent` (see: `TargetHandle`).
    TStateTreeExternalDataHandle<FAttackTargetComponent> AttackTargetHandle;

    /// @brief Handle for the `FFireWeaponComponent` ECS component.
    TStateTreeExternalDataHandle<FFireWeaponComponent> FireWeaponHandle;

    /// @brief Handle for the `FSensedEntitiesComponent` ECS component.
    TStateTreeExternalDataHandle<FSensedEntitiesComponent> SensedEntitiesHandle;

    /// @brief Handle for the `FTargetComponent` ECS component.
    ///
    /// This component used used to specify the target to attack.
    TStateTreeExternalDataHandle<FTargetComponent> TargetHandle;

    /// @brief Handle for the `FAimComponent` ECS component.
    TStateTreeExternalDataHandle<FAimComponent> AimHandle;

    /// @brief Handle for the `FHealthComponent` ECS component.
    TStateTreeExternalDataHandle<FHealthComponent> HealthHandle;
};
