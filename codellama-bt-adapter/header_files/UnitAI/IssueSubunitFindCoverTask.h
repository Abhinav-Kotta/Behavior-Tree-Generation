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
//--| Defines the FIssueSubunitFindCoverTask struct and supporting types.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/Orders/MoveTacticallySuborderComponent.h"
#include "Components/UnitControllerComponent.h"
#include "Components/Units/SegmentedRouteComponent.h"
#include "Components/Units/UnitFormationComponent.h"
#include "Orders/FindCoverOrder.h"
#include "Routes/RoutePoint.h"
#include "UnitAI/WaitForBoundingOverwatchCompletion.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "IssueSubunitFindCoverTask.generated.h"

//--------------------------------------------------------------------------------------------------

class UFindCoverOrder;
class UOrder;
class UOrderSubsystem;

//--------------------------------------------------------------------------------------------------

/// @brief Specifies the stages of issuing movement orders to lower echelons.
///
/// @ingroup SimulationBehaviors-Module
UENUM()
enum class EIssueSubunitFindCoverMovementOrderStage : uint8
{
    CREATE_SUBUNIT_ORDER,      ///< @brief Order being created for the SubUnits.
    VALIDATING_SUBUNIT_ORDER,  ///< @brief Waiting for order validation to complete.
    EXECUTING_SUBUNIT_ORDER,   ///< @brief Sending order to SubUnit.
    COMPLETE,                  ///< @brief Task completed successfully.
    FAILED                     ///< @brief Task failed.
};

//--------------------------------------------------------------------------------------------------

/// @brief Instance data for @ref FIssueSubunitFindCoverTask
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FIssueSubunitFindCoverInstanceData
{
    GENERATED_BODY()

    /// @brief The order sent from the parent unit
    UPROPERTY(EditAnywhere, Category = Input)
    TObjectPtr<UOrder> ParentOrder;

    /// @brief Subunit that is selected
    UPROPERTY(EditAnywhere, Category = Parameter)
    EBoundingOverWatchUnitSelection UnitSelection;

    /// @brief The current stage of this task.
    UPROPERTY()
    EIssueSubunitFindCoverMovementOrderStage Stage =
        EIssueSubunitFindCoverMovementOrderStage::CREATE_SUBUNIT_ORDER;

    /// @brief The move tactically order to be issued to subunits
    UPROPERTY()
    TObjectPtr<UFindCoverOrder> FindCoverOrder;
};

//--------------------------------------------------------------------------------------------------

/// @brief Task for run to cover orders to SubUnits, Note: This task will only run when executed
/// on a unit with only 2 subunits. The subunits are issued alternating orders which contain a route
/// segment that specifies their next bounding point. This outputs a route to either the right or
/// left units and alternates between the two until bounding overwatch is complete
///
/// This task uses the following ECS components. The task will not fail if they are missing, but
/// will not perform any actions until all are added.
///
/// @ref FUnitControllerComponent
///
/// @ingroup SimulationBehaviors-Module
USTRUCT(meta = (MilVerseUnitLevel))
struct SIMULATIONBEHAVIORS_API FIssueSubunitFindCoverTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this node's instance data type.
    using FInstanceDataType = FIssueSubunitFindCoverInstanceData;

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
    /// @sa FIssueMovementOrdersTaskInstanceData
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
    /// @brief Data handle for the @ref FUnitControllerComponent ECS component.
    TStateTreeExternalDataHandle<FUnitControllerComponent> UnitControllerHandle;

private:
    /// @brief Creates the parent unit's movement order and requests its validation.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param OrderSubsystem
    ///     The order subsystem which is used to created, validate, and execute orders.
    /// @param InstanceData
    ///     Instance data for the state tree instance being processed.
    void CreateSubUnitOrder(FStateTreeExecutionContext& Context,
        UOrderSubsystem* OrderSubsystem,
        FInstanceDataType& InstanceData) const;

    /// @brief Waits for the parent unit's movement order to complete validation.
    ///
    /// @param InstanceData
    ///     Instance data for the state tree instance being processed.
    void WaitForSubUnitOrderValidation(FInstanceDataType& InstanceData) const;

    /// @brief Sends the parent unit's movement order.
    ///
    /// @param OrderSubsystem
    ///     The order subsystem which is used to created, validate, and execute orders.
    /// @param InstanceData
    ///     Instance data for the state tree instance being processed.
    void ExecuteSubUnitOrder(UOrderSubsystem* OrderSubsystem,
        FInstanceDataType& InstanceData) const;

private:
    /// @brief If parent order is not null and is currently executing, fail the order.
    ///
    /// @param InstanceData
    ///     Instance data for the current entity being processed by the state tree.
    /// @param Reason
    ///     Reason for the failure. Should be human readable.
    void FailOrder(const FInstanceDataType& InstanceData, const FText& Reason) const;
};
