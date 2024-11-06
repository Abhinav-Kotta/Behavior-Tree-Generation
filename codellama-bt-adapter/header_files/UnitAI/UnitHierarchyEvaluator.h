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
//--| Defines the FUnitHierarchyEvaluator class
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeEvaluator.h"
#include "Components/EntityInfoComponent.h"
#include "Components/EntityRoleInfoComponent.h"
#include "Components/UnitControllerComponent.h"
#include "Formations/MilVerseFormationInstance.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "UnitHierarchyEvaluator.generated.h"

//--------------------------------------------------------------------------------------------------

/// @brief Instance data for @ref FUnitHierarchyEvaluator.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FUnitHierarchyEvaluatorInstanceData
{
    GENERATED_BODY()

    /// @brief The unit's current formation heirarchy.
    UPROPERTY(VisibleAnywhere, Category = Output)
    TObjectPtr<UUnitFormationHierarchy> UnitFormationHierarchy = nullptr;
};

//--------------------------------------------------------------------------------------------------

/// @brief Evaluator for generating a unit's formation hierarchy.
///
/// The formation hierarchy will be an output of this evaluator. It will only ever be updated when
/// the unit receives a new order to avoid unecessary processing.
///
/// This evaluator requires the following components to exist on the entity:
/// * @ref FEntityInfoComponent
/// * @ref FEntityRoleInfoComponent
/// * @ref FUnitControllerComponent
///
/// @ingroup SimulationBehaviors-Module
USTRUCT(meta = (MilVerseUnitLevel))
struct SIMULATIONBEHAVIORS_API FUnitHierarchyEvaluator : public FMilVerseStateTreeEvaluator
{
    GENERATED_BODY()

    /// @brief Alias for this evaluators's instance data type.
    using FInstanceDataType = FUnitHierarchyEvaluatorInstanceData;

public:
    /// @brief Called when the state tree asset is linked with data to allow the evaluator to
    /// resolve references to other state tree data.
    ///
    /// @param Linker
    ///     Reference to the state tree's linker.
    /// @returns
    ///     Returns `true` if linking is successful; Otherwise returns `false`.
    bool Link(FStateTreeLinker& Linker) override;

    /// @brief Returns the struct definition for this evaluator's instance data.
    ///
    /// @sa FUnitHierarchyEvaluatorInstanceData
    const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Called during the state tree tick.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param DeltaTime
    ///     The time in seconds since `Tick` was last called.
    void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

    /// @brief Creates a formation hierarchy for the given UnitController
    /// @param InUnitControllerComponent
    ///     The ECS Component containing the state data used by UnitController Entity
    /// @param OutUnitHierarchy
    ///     A structured grouping of entities that comprise the unit.
    void CreateHierarchyForUnitController(const FUnitControllerComponent& InUnitControllerComponent,
        UUnitFormationHierarchy& OutUnitHierarchy) const;

private:
    /// @brief Handle for the @ref FEntityInfoComponent ECS component.
    TStateTreeExternalDataHandle<FEntityInfoComponent> EntityInfoHandle;

    /// @brief Handle for the @ref FEntityRoleInfoComponent ECS component.
    TStateTreeExternalDataHandle<FEntityRoleInfoComponent> EntityRoleInfoHandle;

    /// @brief Handle for the @ref FUnitControllerComponent ECS component.
    TStateTreeExternalDataHandle<FUnitControllerComponent> UnitControllerHandle;
};
