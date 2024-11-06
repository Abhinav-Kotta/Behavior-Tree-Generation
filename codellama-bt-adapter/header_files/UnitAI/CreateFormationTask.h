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
//--| Defines the FCreateFormationTask state tree task.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/Units/UnitFormationComponent.h"
#include "Formations/FormationData.h"
#include "Formations/MilVerseFormationInstance.h"

// Unreal
#include "CoreMinimal.h"
#include "CreateFormationTask.generated.h"

//--------------------------------------------------------------------------------------------------

class UOrder;

//--------------------------------------------------------------------------------------------------

/// @brief Instance data for @ref FCreateFormationTask.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FCreateFormationTaskInstanceData
{
    GENERATED_BODY()

    /// @brief Flag to throw on the state tree when recieving a child order sent from a higher
    /// echelon. Has my formation already been created by a higher echelon if so, this flag in the
    /// task should be checked true.
    UPROPERTY(EditAnywhere, Category = Parameter)
    bool bFormationExists = false;

    /// @brief Id of the requested formation configuration.
    ///
    /// This is only valid if @ref bFormationValid is true. Otherwise, the formation hasn't been
    /// selected yet.
    UPROPERTY(VisibleAnywhere, Category = Parameter, meta = (EditCondition = "!bFormationExists"))
    FormationID Formation;

    /// @brief Indicates if @ref Formation is valid.
    UPROPERTY(VisibleAnywhere, Category = Parameter, meta = (EditCondition = "!bFormationExists"))
    bool bFormationValid = false;

    /// @brief The unit's current formation heirarchy.
    UPROPERTY(VisibleAnywhere, Category = Input)
    TObjectPtr<UUnitFormationHierarchy> UnitFormationHierarchy = nullptr;

    /// @brief Optional parameter for the order this formation is being created for.
    ///
    /// If not null, and the formation cannot be created, it will fail the order.
    UPROPERTY(VisibleAnywhere, Category = Parameter)
    TObjectPtr<UOrder> Order = nullptr;

    /// @brief Formation Instance ID if the formation has already been created.
    UPROPERTY(VisibleAnywhere, Category = Parameter, meta = (EditCondition = "bFormationExists"))
    int32 FormationInstanceID = INT32_MAX;

    /// @brief Has the formation instance ID been set.
    UPROPERTY(VisibleAnywhere, Category = Parameter, meta = (EditCondition = "bFormationExists"))
    bool bFormationInstanceIDValid = false;
};

//--------------------------------------------------------------------------------------------------
/// @brief State tree task building a formation.
///
/// This task will add the @ref FUnitFormationComponent to the entity, but it will not remove it.
/// @ref FDeleteFormationTask will handle deleting the formation and removing the component from the
/// entity.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT(meta = (DisplayName = "Create Formation Task", MilVerseUnitLevel))
struct SIMULATIONBEHAVIORS_API FCreateFormationTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FCreateFormationTaskInstanceData;

    /// @brief Constructor for the Create Formations task.
    FCreateFormationTask();

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
    /// @sa FCreateFormationTaskInstanceData
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

private:
    /// @brief Sets all entities and subentities of a unit as 'participating'.
    /// This is a recursive method.
    /// @param UnitHierarchy
    ///     The root of the unit hierarchy to traverse and set.
    void SetEntitiesParticipating(const UUnitFormationHierarchy* UnitHierarchy) const;

    /// @brief If the order was provided to this task and is currently executing, fail the order.
    ///
    /// @param InstanceData
    ///     Instance data for the current entity being processed by the state tree.
    /// @param Reason
    ///     Reason for the failure. Should be human readable.
    void FailOrder(const FInstanceDataType& InstanceData, const FText& Reason) const;
};
