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
//--| Copyright 2024 by Lockheed Martin Corporation
//--|====================================================================|--
//--|
//--| Description:
//--| FUnitAssignTargetsTask class declaration.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "CommonAI/CommonTypes.h"
#include "CommonAI/EnemySituationSubsystem.h"

// Unreal
#include "CoreMinimal.h"

#include "UnitAssignTargetsTask.generated.h"

/// @brief Instance data for 'FUnitAssignTargetsTask'.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FUnitAssignTargetsTaskInstanceData
{
    GENERATED_BODY()

    /// @brief Monitor for Enemy Situation Changes, (Forces the task to run forever and reassign
    /// targets every time the entity situation changes)
    UPROPERTY(EditAnywhere, Category = "Parameter")
    bool bMonitorForEnemyChanges;
};

/// @brief State tree task that assigns targets to each of the entities in the unit.
///
/// Assignments are provided to the entity through the `UEnemySituationSubsystem`.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT(meta = (MilVerseUnitLevel))
struct SIMULATIONBEHAVIORS_API FUnitAssignTargetsTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FUnitAssignTargetsTaskInstanceData;

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
    /// @sa FSelectClosestTargetTaskInstanceData
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
    /// @param DeltaSeconds
    ///     The time in seconds since `Tick` was last called.
    /// @returns
    ///     The running status of the task.
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;

protected:
    /// @brief Calculate the target assignments given the current unit enemy situation.
    /// @param UnitSituation Current Unit Enemy Situation.
    static void CalculateAssignments(FUnitEnemySituation& UnitSituation);
};