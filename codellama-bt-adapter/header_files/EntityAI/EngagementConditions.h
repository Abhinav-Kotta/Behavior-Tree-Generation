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
//--| Copyright 2022 by Lockheed Martin Corporation
//--|====================================================================|--
//--|
//--| Description:
//--| Defines StateTree conditions related to engagement.
//--|
//--|====================================================================|--
#pragma once

// Unreal
#include "CoreMinimal.h"

// MilVerse
#include "AI/MilVerseStateTreeCondition.h"
#include "CommonAI/CommonTypes.h"
#include "Components/Engagement/CombatPowerComponent.h"
#include "Components/Engagement/RulesOfEngagementComponent.h"
#include "Components/EntityStateComponent.h"
#include "Components/InventoryComponent.h"
#include "SimTimer.h"

#include "EngagementConditions.generated.h"

/// @brief Instance data for `FEnemyContactCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FEnemyContactConditionInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Input")
    FEnemySituation EnemySituation;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    /// @brief percentage of maximum range to use
    float WeaponEngagementZonePercentage = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    /// @brief Ignore if potential target is within sector of fire.
    bool IgnoreSectorOfFire = false;

    UPROPERTY(EditAnywhere, Category = Output)
    /// @brief distance to closest potential target in meters
    float DistanceToClosestTarget = 0.0f;

    UPROPERTY()
    /// @brief Max engagement distance for entity.
    float MaxEngagementDistance = 0.0;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    /// @brief How much time between updating cached max engagement distance, in seconds.
    double EvalMaxEngagementDistanceSecs = 5.0;

    UPROPERTY()
    /// @brief How much time is left, in seconds, before the next max engagement cache update.
    double RemainingTimeToEvalMaxEngagementDistanceSecs = -1.0;

    /// @brief Used to track time between frames for a system.
    SimTimer SimClock;
};

/// @brief Is there currently enemy contact?
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct FEnemyContactCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FEnemyContactConditionInstanceData;

    /// @brief Constructor
    FEnemyContactCondition() = default;

protected:
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
    /// @sa FEnemyContactConditionInstanceData
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Tests the condition.
    ///
    /// @param Context
    ///     The state tree context.
    /// @returns
    ///     True if enemy contact is present, or false otherwise.
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

private:
    /// @brief Handle for the `FEntityStateComponent` ECS component.
    TStateTreeExternalDataHandle<FEntityStateComponent> EntityStateHandle;

    /// @brief Handle for the @ref FEntityInfoComponent ECS component.
    TStateTreeExternalDataHandle<FEntityInfoComponent> EntityInfoHandle;

    /// @brief Handle for the @ref FInventoryWeaponsComponent ECS component.
    TStateTreeExternalDataHandle<FInventoryWeaponsComponent> WeaponsComponentHandle;
};

// --------------------------------------------------------------------------

/// @brief Instance data for `FRulesOfEngagementCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FRulesOfEngagementConditionInstanceData
{
    GENERATED_BODY()
};

/// @brief State tree condition for checking the Rules of Engagement (ROE)
///
/// This condition requires that the entity have the following components assigned:
/// * `FRulesOfEngagementComponent`
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct FRulesOfEngagementCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FRulesOfEngagementConditionInstanceData;

    /// @brief Constructor
    FRulesOfEngagementCondition() = default;

protected:
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
    /// @sa FEnemyContactConditionInstanceData
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Tests the condition.
    ///
    /// @param Context
    ///     The state tree context.
    /// @returns
    ///     True if the entity's current ROE type matches any of
    ///     the `SelectedROEs` or false otherwise.
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

protected:
    /// @brief Expected ROE types for condition.
    /// The condition returns true if the entity's current ROE type
    /// matches any of the 'SelectedROEs` or false otherwise.
    UPROPERTY(EditAnywhere, Category = Condition)
    TArray<ERulesOfEngagementTypes> SelectedROEs = {ERulesOfEngagementTypes::NONE};

protected:
    /// @brief Handle for the `FRulesOfEngagementComponent` ECS component.
    TStateTreeExternalDataHandle<FRulesOfEngagementComponent> ROEHandle;
};