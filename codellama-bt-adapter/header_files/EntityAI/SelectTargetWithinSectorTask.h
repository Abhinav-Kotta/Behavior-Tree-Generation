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
//--| Defines the SelectTargetWithinSectorTask state tree task.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "CommonAI/CommonTypes.h"
#include "Components/Engagement/AimComponent.h"
#include "Components/Engagement/AssignedFiringSectorComponent.h"
#include "Components/Engagement/CombatPowerComponent.h"
#include "Components/Engagement/RulesOfEngagementComponent.h"
#include "Components/Engagement/TargetComponent.h"
#include "Components/EntityStateComponent.h"
#include "Components/Sensing/SensedEntitiesComponent.h"
#include "Components/UnrealActorComponent.h"

// Unreal
#include "CoreMinimal.h"
#include "SelectTargetWithinSectorTask.generated.h"

/// @brief Instance data for 'FSelectWeaponAndFiringModeTask'.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FSelectTargetWithinSectorTaskInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Input")
    /// @brief An array of potential targets
    FEnemySituation EnemySituation;
};

/// @brief State tree task for performing suppression fire
///
/// This task requires that the entity have the following components assigned
/// * `FAssignedFiringSectorComponent`
/// * `FEntityStateComponent`
/// * `FSensedEntitiesComponent`
/// * `FTargetComponent`
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FSelectTargetWithinSectorTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FSelectTargetWithinSectorTaskInstanceData;

    /// @brief Constructor.
    FSelectTargetWithinSectorTask();

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
    /// @sa FSelectTargetWithinSectorTaskInstanceData
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

    /// @brief Debug symbology for the Sector
    void ShowSuppressionSectorDebug(const FStateTreeExecutionContext& Context,
        const FTargetComponent&) const;

protected:
    /// @brief Handle for the `FAssignedFiringSectorComponent` ECS component.
    TStateTreeExternalDataHandle<FAssignedFiringSectorComponent> AssignedFiringSectorHandle;

    /// @brief Handle for the `FEntityStateComponent` ECS component.
    TStateTreeExternalDataHandle<FEntityStateComponent> EntityStateHandle;

    /// @brief Handle for the `FTargetComponent` ECS component.
    TStateTreeExternalDataHandle<FTargetComponent> TargetHandle;
};