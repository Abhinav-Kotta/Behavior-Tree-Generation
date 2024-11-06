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
//--| Defines the FIssueDefendOrdersTask struct and supporting types.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/UnitControllerComponent.h"
#include "Components/Units/UnitFormationComponent.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "IssueDefendPositionOrdersTask.generated.h"

//--------------------------------------------------------------------------------------------------

class UDefendPositionOrder;
class UOrder;
class UOrderSubsystem;

//--------------------------------------------------------------------------------------------------
// @brief Specifies the stages of issuing defend position orders to lower echelons.
///
/// @ingroup SimulationBehaviors-Module
UENUM()
enum class EIssueDefendPositionOrdersStage : uint8
{
    CREATE_ORDER,      ///< @brief Order being created.
    VALIDATING_ORDER,  ///< @brief Waiting for order validation to complete.
    EXECUTING_ORDER,   ///< @brief Sending order.
    COMPLETE,          ///< @brief Task completed successfully.
    FAILED             ///< @brief Task failed.
};

//--------------------------------------------------------------------------------------------------

/// @brief Instance data for @ref FIssueDefendPositionOrdersTask
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FIssueDefendPositionOrdersTaskInstanceData
{
    GENERATED_BODY()

    /// @brief Parent defend order.
    UPROPERTY(VisibleAnywhere, Category = Input)
    TObjectPtr<UOrder> ParentOrder = 0;

    /// @brief TSet of healthy entities (health > 0)
    UPROPERTY(VisibleAnywhere, Category = Input)
    TSet<FGuid> HealthyEntities;

    /// @brief The current stage of this task.
    UPROPERTY()
    EIssueDefendPositionOrdersStage Stage = EIssueDefendPositionOrdersStage::CREATE_ORDER;

    /// @brief Array or orders to be processed.
    UPROPERTY()
    TArray<TObjectPtr<UOrder>> Orders;
};

//--------------------------------------------------------------------------------------------------

/// Task for issuing defend position orders to lower echelon units/individuals.
///
/// This task uses the following ECS components. The task will not fail if they are missing, but
/// will not perform any actions until all are added.
///
/// @ref FUnitControllerComponent
/// @ref FUnitFormationComponent
///
/// @ingroup SimulationBehaviors-Module
USTRUCT(meta = (MilVerseUnitLevel))
struct SIMULATIONBEHAVIORS_API FIssueDefendPositionOrdersTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this node's instance data type.
    using FInstanceDataType = FIssueDefendPositionOrdersTaskInstanceData;

protected:
    /// @brief Called when the state tree asset is linked with data to allow the task to resolve
    /// references to other state tree data.
    ///
    /// @param Linker
    ///     Reference to the state tree's linker.
    /// @returns
    ///     Returns `true` if linking is successful; Otherwise returns `false`.
    bool Link(FStateTreeLinker& Linker) override;

    /// @brief Returns the struct definition for this task's instance data.
    ///
    /// @sa FIssueDefendOrdersTaskInstanceData
    const UStruct* GetInstanceDataType() const override
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
    EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override;

    /// @brief Called when a current state is exited and task is part of active states.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param Transition Describes the states involved in the transition
    ///     Information about the transition that caused the state to be exited. Not used for this
    ///     task.
    void ExitState(FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override;

    /// @brief Called during the state tree tick when the task is part of an active state.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param DeltaTime
    ///     The time in seconds since `Tick` was last called.
    /// @returns
    ///     The running status of the task.
    EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;

protected:
    /// @brief Data handle for the @ref FUnitControllerComponent ECS component.
    TOptionalStateTreeExternalDataHandle<FUnitControllerComponent> UnitControllerHandle;

    /// @brief Data handle for the @ref FUnitFormationComponent ECS component.
    TOptionalStateTreeExternalDataHandle<FUnitFormationComponent> UnitFormationHandle;

private:
    /// @brief If paranet order is not null and is currently executing, fail the order.
    ///
    /// @param InstanceData
    ///     Instance data for the current entity being processed by the state tree.
    /// @param Reason
    ///     Reason for the failure. Should be human readable.
    void FailOrder(const FInstanceDataType& InstanceData, const FText& Reason) const;
};
