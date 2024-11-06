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
//--| Defines the FAssembleFormationTask struct and supporting types.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/Orders/MoveTacticallySuborderComponent.h"
#include "Components/UnitControllerComponent.h"
#include "Components/Units/SegmentedRouteComponent.h"
#include "Components/Units/UnitFormationComponent.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "AssembleFormationTask.generated.h"

//--------------------------------------------------------------------------------------------------

class UOrder;
class UOrderSubsystem;

//--------------------------------------------------------------------------------------------------

/// @brief Specifies the stages of the @ref FAssembleFormationTask task.
///
/// @ingroup SimulationBehaviors-Module
UENUM()
enum class EAssembleFormationStage
{
    CREATING_ORDERS,         ///< @brief Creating the assemble order for each follower.
    WAITING_FOR_VALIDATION,  ///< @brief Waiting for the assemble order to be validated.
    EXECUTE_ORDERS,          ///< @brief Sending the assemble orders.
    WAITING_FOR_COMPLETION,  ///< @brief Waiting for each follower to complete assemble order.
    COMPLETED,               ///< @brief Assembly completed successfully.
    FAILED                   ///< @brief Assembly could not be completed.
};

//--------------------------------------------------------------------------------------------------

/// @brief Instance data for @ref FAssembleFormationTask
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FAssembleFormationTaskInstanceData
{
    GENERATED_BODY()

    /// @brief Parent movement order.
    UPROPERTY(VisibleAnywhere, Category = Input)
    TObjectPtr<UOrder> ParentOrder = 0;

    UPROPERTY(VisibleAnywhere, Category = Input)
    float Speed = 0.0f;

    /// @brief Flag to skip "Formup" behavior
    UPROPERTY(VisibleAnywhere, Category = Input)
    bool bShouldSkipFormup = false;

    /// @brief TSet of healthy entities (health > 0)
    UPROPERTY(VisibleAnywhere, Category = Input)
    TSet<FGuid> HealthyEntities;

    /// @brief Current stage of the task's execution.
    UPROPERTY()
    EAssembleFormationStage Stage = EAssembleFormationStage::CREATING_ORDERS;

    /// @brief Order sent to the leader.
    UPROPERTY()
    TObjectPtr<UOrder> LeaderOrder;

    /// @brief Order sent to the followers.
    UPROPERTY()
    TArray<TObjectPtr<UOrder>> FollowerOrders;
};

//--------------------------------------------------------------------------------------------------

/// @brief Task used to assemble a unit in formation.
///
/// This task uses the @ref FUnitFormationComponent, @ref FSegmentedRouteComponent, and @ref
/// FMoveTacticallySuborderComponent ECS components. The task will wait until both components are
/// added to the entity before performing any actions.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT(meta = (MilVerseUnitLevel))
struct SIMULATIONBEHAVIORS_API FAssembleFormationTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this node's instance data type.
    using FInstanceDataType = FAssembleFormationTaskInstanceData;

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
    /// @sa FAssembleFormationTaskInstanceData
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
    /// @brief Creates the assemble orders for each follower in the unit.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param OrderSubsystem
    ///     The order subsystem which is used to created, validate, and execute orders.
    /// @param InstanceData
    ///     Instance data for the state tree instance being processed.
    /// @param UnitFormation
    ///     Component containing data about the current formation.
    void CreateOrders(FStateTreeExecutionContext& Context,
        UOrderSubsystem* OrderSubsystem,
        FInstanceDataType& InstanceData,
        const FUnitFormationComponent& UnitFormation) const;

    /// @brief Waits for the assemble orders to each be validated.
    ///
    /// @param InstanceData
    ///     Instance data for the state tree instance being processed.
    void WaitForValidation(FInstanceDataType& InstanceData) const;

    /// @brief Executes each of the assemble orders.
    ///
    /// @param OrderSubsystem
    ///     The order subsystem which is used to created, validate, and execute orders.
    /// @param InstanceData
    ///     Instance data for the state tree instance being processed.
    void ExecuteOrders(UOrderSubsystem* OrderSubsystem, FInstanceDataType& InstanceData) const;

    /// @brief Waits for the assemble orders to be completed.
    ///
    /// @param InstanceData
    ///     Instance data for the state tree instance being processed.
    void WaitForCompletion(FInstanceDataType& InstanceData) const;

    /// @brief Fails the parent order.
    ///
    /// @param InstanceData
    ///     Instance data for the current entity being processed by the state tree.
    /// @param Reason
    ///     Reason for the failure. Should be human readable.
    void FailOrder(const FInstanceDataType& InstanceData, const FText& Reason) const;
};
