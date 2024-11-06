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
//--| Defines the FRemoveDeadEntitiesTask struct and supporting types.
//--| Removes dead entities from the formation instance.
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/UnitControllerComponent.h"
#include "Components/Units/UnitFormationComponent.h"
#include "UnitAI/UnitMemberDestroyedEvent.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "RemoveDeadEntitiesTask.generated.h"

//--------------------------------------------------------------------------------------------------

class UMoveTacticallyOrder;
class UOrder;
class UOrderSubsystem;

//--------------------------------------------------------------------------------------------------

/// @brief Instance data for @ref FRemoveDeadEntitiesTask
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FRemoveDeadEntitiesInstanceData
{
    GENERATED_BODY()

    /// @brief List of living entities in the unit
    UPROPERTY(VisibleAnywhere, Category = Input)
    TSet<FGuid> HealthyEntities;
};

//--------------------------------------------------------------------------------------------------

/// @brief Task for removing dead entities from unit.
///
/// This task uses the following ECS components. The task will not fail if they are missing, but
/// will not perform any actions until all are added.
///
/// @ref FUnitFormationComponent
/// @ref FUnitControllerComponent
/// @ingroup SimulationBehaviors-Module
USTRUCT(meta = (MilVerseUnitLevel))
struct SIMULATIONBEHAVIORS_API FRemoveDeadEntitiesTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this node's instance data type.
    using FInstanceDataType = FRemoveDeadEntitiesInstanceData;

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
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;

protected:
    /// @brief Data handle for the @ref FUnitFormationComponent ECS component.
    TOptionalStateTreeExternalDataHandle<FUnitFormationComponent> UnitFormationHandle;

    /// @brief Data handle for the @ref FUnitControllerComponent ECS component.
    TOptionalStateTreeExternalDataHandle<FUnitControllerComponent> UnitControllerHandle;

private:
};
