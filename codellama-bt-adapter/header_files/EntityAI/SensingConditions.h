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
//--| Defines StateTree conditions related to sensing.
//--|
//--|====================================================================|--
#pragma once

#include "AI/MilVerseStateTreeCondition.h"
#include "Components/Engagement/AssignedFiringSectorComponent.h"
#include "Components/EntityStateComponent.h"
#include "Components/Sensing/SensedEntitiesComponent.h"
#include "Components/Sensing/ShotAtDetectionComponent.h"

#include "SensingConditions.generated.h"

/// @brief Instance data for `FEnemySpottedCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FEnemySpottedConditionInstanceData
{
    GENERATED_BODY()
};

/// @brief State tree condition for checking if an enemy has been spotted.
///
/// This condition requires that the entity have the following components assigned:
/// * `FSensedEntitiesComponent`
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct FEnemySpottedCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FEnemySpottedConditionInstanceData;

    /// @brief Constructor
    FEnemySpottedCondition() = default;

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
    /// @sa FEnemySpottedConditionInstanceData
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Tests the condition.
    ///
    /// @param Context
    ///     The state tree context.
    /// @returns
    ///     True if at least one enemy has been spotted unless `bInvert` is set to true in which
    ///     case it will return true if an enemy hasn't been spotted.
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

protected:
    /// @brief Inverts the result of `TestCondition` when true.
    /// By default the test condition returns true if at least one enemy has been spotted
    /// unless `bInvert` is set to true in which case it will return true if an enemy hasn't
    /// been spotted.
    UPROPERTY(EditAnywhere, Category = Condition)
    bool bInvert = false;

protected:
    /// @brief Handle for the `FSensedEntitiesComponent` ECS component.
    TStateTreeExternalDataHandle<FSensedEntitiesComponent> SensedEntitiesHandle;
};

//--------------------------------------------------------------------------------------------------

/// @brief Instance data for `FEnemySpottedInSectorCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FEnemySpottedInSectorConditionInstanceData
{
    GENERATED_BODY()
};

/// @brief State tree condition for checking if an enemy has been spotted within its assigned
/// sector.
///
/// This condition requires that the entity have the following components assigned:
/// * `FAssignedFiringSectorComponent`
/// * `FEntityStateComponent`
/// * `FSensedEntitiesComponent`
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct FEnemySpottedInSectorCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FEnemySpottedInSectorConditionInstanceData;

    /// @brief Constructor
    FEnemySpottedInSectorCondition() = default;

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
    /// @sa FEnemySpottedInSectorConditionInstanceData
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Tests the condition.
    ///
    /// @param Context
    ///     The state tree context.
    /// @returns
    ///     True if at least one enemy has been spotted unless `bInvert` is set to true in which
    ///     case it will return true if an enemy hasn't been spotted.
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

protected:
    /// @brief Inverts the result of `TestCondition` when true.
    UPROPERTY(EditAnywhere, Category = Condition)
    bool bInvert = false;

protected:
    /// @brief Handle for the `FAssignedFiringSectorComponent` ECS component.
    TStateTreeExternalDataHandle<FAssignedFiringSectorComponent> AssignedFiringSectorHandle;

    /// @brief Handle for the `FEntityStateComponent` ECS component.
    TStateTreeExternalDataHandle<FEntityStateComponent> EntityStateHandle;

    /// @brief Handle for the `FSensedEntitiesComponent` ECS component.
    TStateTreeExternalDataHandle<FSensedEntitiesComponent> SensedEntitiesHandle;
};

//--------------------------------------------------------------------------------------------------

/// @brief Instance data for `FShotAtCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FShotAtConditionInstanceData
{
    GENERATED_BODY()
};

/// @brief State tree condition for checking if the entity is being shot at.
///
/// This condition requires that the entity have the following components assigned:
/// * `FShotAtDetectionComponent`
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct FShotAtCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FShotAtConditionInstanceData;

    /// @brief Constructor
    FShotAtCondition() = default;

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
    /// @sa FShotAtConditionInstanceData
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Tests the condition.
    ///
    /// @param Context
    ///     The state tree context.
    /// @returns
    ///     True if at least one enemy is shooting at this entity unless `bInvert` is set to true in
    ///     which case it will return true if this entity is not being shot at.
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

protected:
    /// @brief Inverts the result of `TestCondition` when true.
    UPROPERTY(EditAnywhere, Category = Condition)
    bool bInvert = false;

protected:
    /// @brief Handle for the `FShotAtDetectionComponent` ECS component.
    TStateTreeExternalDataHandle<FShotAtDetectionComponent> ShotAtDetectionHandle;

    /// @brief Handle for the `FSensedEntitiesComponent` ECS component.
    TStateTreeExternalDataHandle<FSensedEntitiesComponent> SensedEntitiesHandle;
};
