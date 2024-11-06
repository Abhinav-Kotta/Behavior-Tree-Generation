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
//--| Defines StateTree conditions related to Unit Level Combat Power.
//--|
//--|====================================================================|--
#pragma once

// Unreal
#include "CoreMinimal.h"

// MilVerse
#include "AI/MilVerseStateTreeCondition.h"
#include "CommonAI/CommonTypes.h"
#include "Components/Engagement/CombatPowerComponent.h"
#include "Components/UnitControllerComponent.h"
#include "SimTimer.h"

#include "UnitCombatPowerCondition.generated.h"

/// @brief Specifies the possible relatives combat power between two forces.
///
/// @ingroup SimulationBehaviors-Module
UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ERelativeCombatPower : uint8
{
    Invalid    = 0 UMETA(Hidden),
    Comparable = 1 << 0,
    Superior   = 1 << 1,
    Weaker     = 1 << 2,
};

ENUM_CLASS_FLAGS(ERelativeCombatPower);

/// @brief Instance data for `FUnitCombatPowerCondition`.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct FUnitCombatPowerConditionInstanceData
{
    GENERATED_BODY()
};

/// @brief Condition used for making decisions based on relative combat power such as the enemy 
/// being weaker or stronger.
///
/// Result of condition will depend on the EnemyCombatPowerIs setting. Also, if no enemy is detected
/// this condition will always fail.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct FUnitCombatPowerCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FUnitCombatPowerConditionInstanceData;

    /// @brief Constructor
    FUnitCombatPowerCondition() = default;

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

protected:

    // NOTE: Property is in the condition task because the values should not change between
    //       entities.

    /// @brief Relative combat power of the enemy for this condition to be met. More than one option
    /// can be selected.
    UPROPERTY(EditAnywhere, Category = "Parameter", meta = (Bitmask, BitmaskEnum = "/Script/SimulationBehaviors.ERelativeCombatPower"))
    int32 EnemyCombatPowerIs = 0;

private:
    /// @brief Handle for the `FCombatPowerComponent` ECS component.
    TStateTreeExternalDataHandle<FCombatPowerComponent> UnitCombatPowerHandle;
};