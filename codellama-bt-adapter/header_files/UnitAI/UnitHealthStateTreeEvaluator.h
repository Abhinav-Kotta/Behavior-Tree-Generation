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
//--| Defines the FUnitHealthStateTreeEvaluator class.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeEvaluator.h"
#include "Components/EntityInfoComponent.h"
#include "Components/UnitControllerComponent.h"
#include "Components/Units/UnitFormationComponent.h"

#include "SimTimer.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "UnitHealthStateTreeEvaluator.generated.h"

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------

/// @brief Instance data for `FUnitHealthStateTreeEvaluator`.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FUnitHealthStateTreeEvaluatorInstanceData
{
    GENERATED_BODY()

    /// @brief The rate at which this evaluator should run. Default 1 second.
    UPROPERTY(EditAnywhere, Category = Parameter, meta = (ClampMin = "0.0"))
    float UpdateRate = 1.0f;

    /// @brief True if verbose log output is desired. Default is false.
    UPROPERTY(EditAnywhere, Category = Parameter)
    bool VerboseLogging = false;

    /// @brief TSet of healthy entities (health > 0)
    UPROPERTY(VisibleAnywhere, Category = Output)
    TSet<FGuid> HealthyEntities;

    /// @brief Used to track time between frames for a system.
    SimTimer SimClock;

    /// @brief Used to track the elapsed time.
    float ElapsedTime = 0.0f;
};

//--------------------------------------------------------------------------------------------------

/// @brief State tree evaluator for evaluating entity health.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT(meta = (MilVerseUnitLevel))
struct SIMULATIONBEHAVIORS_API FUnitHealthStateTreeEvaluator : public FMilVerseStateTreeEvaluator
{
    GENERATED_BODY()

    /// @brief Alias for this evaluators's instance data type.
    using FInstanceDataType = FUnitHealthStateTreeEvaluatorInstanceData;

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
    /// @sa FUnitHealthStateTreeEvaluatorInstanceData
    const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Called during the state tree tick.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param DeltaSeconds
    ///     The time in seconds since `Tick` was last called.
    void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
    /// @brief Handle for the @ref FUnitControllerComponent ECS component.

private:
    /// @brief Handle for the @ref FEntityInfoComponent ECS component.
    TStateTreeExternalDataHandle<FEntityInfoComponent> EntityInfoHandle;

    /// @brief Handle for the @ref FUnitControllerComponent ECS component.
    TStateTreeExternalDataHandle<FUnitControllerComponent> UnitControllerHandle;

    /// @brief Handle for the @ref FUnitFormationComponent ECS component. Optional because we may
    /// not have this if we are not executing an order in formation.
    TOptionalStateTreeExternalDataHandle<FUnitFormationComponent> UnitFormationHandle;
};
