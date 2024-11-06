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
//--| Defines the NotifyRouteCompletedTask state tree task.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/UnitControllerComponent.h"
#include "Components/Units/SegmentedRouteComponent.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "NotifyRouteCompletedTask.generated.h"

//--------------------------------------------------------------------------------------------------

class UOrder;

//--------------------------------------------------------------------------------------------------

/// @brief Instance data for @ref FNotifyRouteCompletedTask
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct FNotifyRouteCompletedTaskInstanceData
{
    GENERATED_BODY()

    /// @brief Order sent to the unit that triggered the route movement.
    ///
    /// If the order has a parent, it will be assumed that the parent order belongs to the parent
    /// unit of the unit this task intance is attached to.
    UPROPERTY(VisibleAnywhere, Category = Input)
    TObjectPtr<UOrder> Order = nullptr;
};

//--------------------------------------------------------------------------------------------------

/// @brief Task used to sent notifications to units and entities when the leader reaches the end of
/// their route.
///
/// When the leader completes their route, the unit the entity belongs to will notify the followers
/// in the unit and its parent unit using the @ref LeaderCompletedRouteEvent state tree event.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT(meta = (MilVerseGenericLevel))
struct FNotifyRouteCompletedTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this node's instance data type.
    using FInstanceDataType = FNotifyRouteCompletedTaskInstanceData;

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
    /// @sa FNotifyRouteCompletedTaskInstanceData
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

private:
    /// @brief
    /// @brief Sends a LeaderCompletedRouteEvent event to an entity.
    ///
    /// @param SrcEntity
    ///     Entity id for entity sending the event (e.g. the entity the task belongs to).
    /// @param DstEntity
    ///     Entity id for entity the event is being sent to.
    void NotifyEntity(const FGuid& SrcEntity, const FGuid& DstEntity) const;

protected:
    /// @brief Handle for the @ref FSegmentedRouteComponent ECS component.
    TOptionalStateTreeExternalDataHandle<FSegmentedRouteComponent> SegmentedRouteHandle;
};
