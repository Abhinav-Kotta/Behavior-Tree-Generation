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
//--| Defines the FEntityStateEvaluator state tree evaluator.
//--|
//--|====================================================================|--
#pragma once

#include "AI/MilVerseStateTreeEvaluator.h"
#include "CommonTypes/LatLonAlt.h"
#include "Components/EntityStateComponent.h"

#include "CoreMinimal.h"

#include "FEntityStateEvaluator.generated.h"

/// @brief Instance data for `FEntityStateEvaluator`.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FEntityStateEvaluatorInstanceData
{
    GENERATED_BODY()

    /// @brief The current location of the entity.
    UPROPERTY(EditAnywhere, Category = Output)
    FLatLonAlt CurrentLocation;
};

/// @brief State tree evaluator for processing the entity state.
///
/// This evaluator requires that the entity have the following components assigned:
/// * FEntityStateComponent
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (DisplayName = "Entity State Evaluator", MilVerseGenericLevel))
struct SIMULATIONBEHAVIORS_API FEntityStateEvaluator : public FMilVerseStateTreeEvaluator
{
    GENERATED_BODY()

    /// @brief Alias for this node's instance data type.
    using FInstanceDataType = FEntityStateEvaluatorInstanceData;

    /// @brief Constructor for the entity state evaluator. It will look up
    /// configuration data for this evaluator.
    FEntityStateEvaluator();

protected:
    /// @brief Called when the state tree asset is linked with data to allow the evaluator to
    /// resolve references to other state tree data.
    ///
    /// @param Linker
    ///     Reference to the state tree's linker.
    /// @returns
    ///     Returns `true` if linking is successful; Otherwise returns `false`.
    virtual bool Link(FStateTreeLinker& Linker) override;

    /// @brief Returns the struct definition for this nodes's instance data.
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /**
     * Called each frame to update the evaluator.
     * @param Context Reference to current execution context.
     * @param DeltaTime Time since last StateTree tick, or 0 if called during preselection.
     */
    virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

private:
    /// @brief Handle for the `FEntityStateComponent` ECS component.
    TStateTreeExternalDataHandle<FEntityStateComponent> EntityStateHandle;
};
