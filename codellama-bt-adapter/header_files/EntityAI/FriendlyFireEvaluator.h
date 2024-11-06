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
//--| Defines the FEntityStateEvaluator state tree evaluator. This evaluator determines
//--| when it's safe to move in front of friendlies and when it's not
//--|
//--|====================================================================|--
#pragma once

#include "AI/MilVerseStateTreeEvaluator.h"
#include "Components/Engagement/FriendlyFireImpedingMovementComponent.h"
#include "GameplayTagContainer.h"

#include "CoreMinimal.h"

#include "FriendlyFireEvaluator.generated.h"

/// @brief Instance data for `FFriendlyFireEvaluator`.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FFriendlyFireEvaluatorInstanceData
{
    GENERATED_BODY()

    bool bHasSignificantChange = false;
};

/// @brief Fires a MilVerse.FriendlyFireStopMovement FStateTreeEvent when movement is impeded and
/// fires a MilVerse.FriendlyFireStartMovement FStateTreeEvent when movement is no longer impeded.
///
/// This evaluator requires that the entity have the following components assigned:
/// * FFriendlyFireImpedingMovementComponent
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (DisplayName = "Friendly Fire Evaluator", MilVerseEntityLevel))
struct SIMULATIONBEHAVIORS_API FFriendlyFireEvaluator : public FMilVerseStateTreeEvaluator
{
    GENERATED_BODY()

    /// @brief Alias for this node's instance data type.
    using FInstanceDataType = FFriendlyFireEvaluatorInstanceData;

    /// @brief Constructor for the entity state evaluator. It will look up
    /// configuration data for this evaluator.
    FFriendlyFireEvaluator();

    static void InitTags();

public:
    /// @brief Gameplay tag for the Friendly fire state tree event that is used to stop character
    /// movement.
    inline static FGameplayTag FriendlyFireStopStateTreeEvent;

    /// @brief Gameplay tag for the Friendly fire state tree event that is used to start character
    /// movement.
    inline static FGameplayTag FriendlyFireStartStateTreeEvent;

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
    /// @brief Handle for the `FriendlyFireImpedingMovementComponent` ECS component.
    TStateTreeExternalDataHandle<FFriendlyFireImpedingMovementComponent>
        FriendlyFireImpedingMovementHandle;
};
