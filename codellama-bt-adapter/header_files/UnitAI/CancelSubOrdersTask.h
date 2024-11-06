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
//--| Defines CancelSubOrdersTask Cancels all lower unit level and entity level orders that the
//--| currently executing unit controller owns.
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/Orders/MoveTacticallySuborderComponent.h"
#include "Components/UnitControllerComponent.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "CancelSubOrdersTask.generated.h"

//--------------------------------------------------------------------------------------------------

class UMoveTacticallyOrder;
class UOrder;
class UOrderSubsystem;

//--------------------------------------------------------------------------------------------------

/// @brief Instance data for @ref FCancelSubOrdersTask
///
/// @ingroup SimulationBehaviors-Plugin
USTRUCT()
struct SIMULATIONBEHAVIORS_API FCancelSubOrdersTaskInstanceData
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, Category = Input)
    TSet<FGuid> HealthyEntities;
};

//--------------------------------------------------------------------------------------------------

/// @brief Task for canceling all lower unit level and entity level orders that the currently
/// executing unit controller owns.
///
/// This task uses the following ECS component(s). The task will fail if they are missing
///
/// @ref FUnitControllerComponent
/// @ingroup SimulationBehaviors-Plugin
USTRUCT(meta = (MilVerseUnitLevel))
struct SIMULATIONBEHAVIORS_API FCancelSubOrdersTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this node's instance data type.
    using FInstanceDataType = FCancelSubOrdersTaskInstanceData;

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
    /// @sa FRemoveDeadEntitiesInstanceData
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
    /// @param DeltaTime
    ///     The time in seconds since `Tick` was last called.
    /// @returns
    ///     The running status of the task.
    // virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
    //     const float DeltaTime) const override;

protected:
    /// @brief Data handle for the @ref FUnitControllerComponent ECS component.
    TOptionalStateTreeExternalDataHandle<FUnitControllerComponent> UnitControllerHandle;

    /// @brief Data handle for the @ref FMoveTacticallySuborderComponent ECS component.
    TOptionalStateTreeExternalDataHandle<FMoveTacticallySuborderComponent> MoveTacSubOrderHandle;
};
