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
//--| Wait For Bounding Overwatch Order Completion
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/UnitControllerComponent.h"
#include "OrderSubsystem.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "WaitForBoundingOverwatchCompletion.generated.h"

/// @brief Enums for the left and right units
UENUM()
enum class EBoundingOverWatchUnitSelection : uint8
{
    LEFT_UNIT = 0 UMETA(DisplayName = "Left Unit"),
    RIGHT_UNIT = 1 UMETA(DisplayName = "Right Unit")
};

/// @brief Instance data for @ref FWaitForBoundingOverwatchCompletion
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct FWaitForBoundingOverwatchCompletionInstanceData
{
    GENERATED_BODY()

    /// @brief Subunit selection
    UPROPERTY(EditAnywhere, Category = Parameter)
    EBoundingOverWatchUnitSelection UnitSelection;

    /// @brief Active order guid
    UPROPERTY()
    FGuid OrderGuid;
};

/// @brief Wait For Bounding Overwatch Order Completion Task
///
///
/// @ingroup SimulationBehaviors-Module
USTRUCT(meta = (MilVerseUnitLevel))
struct FWaitForBoundingOverwatchCompletion : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FWaitForBoundingOverwatchCompletionInstanceData;

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

    /// @brief Checks if the order in the instance data is completed
    ///
    /// @param InstanceData
    ///     Instance data for the state tree instance being processed.
    /// @param UnitController
    ///     The unit controller to check it's order completion
    /// @param OrderSubsystem
    ///     The order subsystem used to get the order status
    /// @return
    ///     The status of the order being checked
    EStateTreeRunStatus CheckForCompletion(FInstanceDataType& InstanceData,
        const FUnitControllerComponent& UnitController,
        const UOrderSubsystem* OrderSubsystem) const;

protected:
    /// @brief Handle for the `FUnitControllerComponent` ECS component.
    TStateTreeExternalDataHandle<FUnitControllerComponent> UnitControllerHandle;
};
