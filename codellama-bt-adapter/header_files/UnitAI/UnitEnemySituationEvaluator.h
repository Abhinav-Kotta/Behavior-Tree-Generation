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
//--| FUnitEnemySituationEvaluator class declaration.
//--|
//--|====================================================================|--
#pragma once

#include "CoreMinimal.h"

#include "AI/MilVerseStateTreeEvaluator.h"
#include "CommonAI/CommonTypes.h"
#include "Components/UnitControllerComponent.h"
#include "Components/UnitControllerSensedEntitiesComponent.h"
#include "Events/SensedEntityUpdateEvent.h"

#include "UnitEnemySituationEvaluator.generated.h"

/// @brief Instance data for `FUnitEnemySituationEvaluator`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FUnitEnemySituationEvaluatorInstanceData
{
    GENERATED_BODY()

    /// @brief Has our Unit spotted an enemy?
    UPROPERTY(VisibleAnywhere, Category = "Output")
    bool bEnemySpotted = false;

    /// @brief Has our Unit spotted an enemy? (previous frame)
    UPROPERTY(VisibleAnywhere, Category = "Output")
    bool bEnemySpottedPrev = false;

#if !UE_BUILD_SHIPPING
    bool bHasSentEvent = false;

    bool bHasSetLocation = false;

    FLatLonArea DebugAreaToSearch;
#endif
};

/// @brief Aggregates all of the units sensed entities and is able to make decisions based on
/// sensing.
///
///
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FUnitEnemySituationEvaluator : public FMilVerseStateTreeEvaluator
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FUnitEnemySituationEvaluatorInstanceData;

    /// @brief Constructor
    FUnitEnemySituationEvaluator() = default;

    /// @brief Called when the state tree asset is linked with data to allow the condition to
    /// resolve references to other state tree data.
    ///
    /// @param Linker
    ///     Reference to the state tree's linker.
    /// @returns
    ///     Returns `true` if linking is successful; Otherwise returns `false`.
    virtual bool Link(FStateTreeLinker& Linker) override;

    /// @brief Returns the struct definition for this condition's instance data.
    ///
    /// @sa FUnitEnemySituationEvaluatorInstanceData
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /**
     * Called when StateTree is started.
     * @param Context Reference to current execution context.
     */
    virtual void TreeStart(FStateTreeExecutionContext& Context) const override
    {
    }

    /**
     * Called when StateTree is stopped.
     * @param Context Reference to current execution context.
     */
    virtual void TreeStop(FStateTreeExecutionContext& Context) const override
    {
    }

    /**
     * Called each frame to update the evaluator.
     * @param Context Reference to current execution context.
     * @param DeltaTime Time since last StateTree tick, or 0 if called during preselection.
     */
    virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

protected:
    /// @brief Handle for the `FUnitControllerSenesedEntities` ECS component.
    TStateTreeExternalDataHandle<FUnitControllerSensedEntitiesComponent>
        UnitControlledSensedEntitiesHandle;

    /// @brief Handle for the `FUnitControllerComponent` ECS component.
    TStateTreeExternalDataHandle<FUnitControllerComponent> UnitControllerHandle;
};
