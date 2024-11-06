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
//--| State Tree Condition to check if the true leader has been killed, whenever a
//--| UnitMemberDestroyedEvent gets fired off.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeCondition.h"

// Unreal
#include "CoreMinimal.h"
#include "CheckTrueLeaderKilledCondition.generated.h"

/// @brief Instance data for `FCheckIfTrueLeaderKilledCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup UnitAI
USTRUCT()
struct FCheckIfTrueLeaderKilledConditionInstanceData
{
    GENERATED_BODY()
};

/// @brief State tree condition for checking if the true leader has been killed.
///
/// The "True Leader" is the entity that is leading the entire formation (Out in front of everyone)
/// They may also be the doctrinal leader in some formations.
/// @ingroup SimulationBehaviors-Plugin
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseUnitLevel))
struct FCheckIfTrueLeaderKilledCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FCheckIfTrueLeaderKilledConditionInstanceData;

    /// @brief Constructor
    FCheckIfTrueLeaderKilledCondition() = default;

protected:
    /// @brief Called when the state tree asset is linked with data to allow the condition to
    /// resolve references to other state tree data.
    ///
    /// @param Linker
    ///     Reference to the state tree's linker.
    /// @returns
    ///     Returns `true` if linking is successful; Otherwise returns `false`.
    virtual bool Link(FStateTreeLinker& Linker) override;

    /// @brief Tests the condition.
    ///
    /// @param Context
    ///     The state tree context.
    /// @returns
    /// The condition returns true if the true leader has been killed.
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
