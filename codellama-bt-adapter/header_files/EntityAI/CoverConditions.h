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
//--| Defines a StateTree conditions related to cover.
//--|
//--|====================================================================|--
#pragma once

#include "AI/MilVerseStateTreeCondition.h"

#include "CoreMinimal.h"
#include "CoverConditions.generated.h"

/// @brief Instance data for `FCanFireFromCoverCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FCanFireFromCoverConditionInstanceData
{
    GENERATED_BODY()

    /// @brief If this is true, then the condition is inverted.
    UPROPERTY(EditAnywhere, Category = Parameter)
    bool bInvert = false;
};

/// @brief State tree condition for determining if an entity can fire from cover.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct FCanFireFromCoverCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FCanFireFromCoverConditionInstanceData;

    /// @brief Constructor
    FCanFireFromCoverCondition() = default;

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
    /// @sa FAttackConditionInstanceData
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Tests the condition.
    ///
    /// @param Context
    ///     The state tree context.
    /// @returns
    /// The condition returns true when the avatar can expose and return fire from a
    /// cover condition.
    /// If bInvert is true, then the condition returns true when the cover condition
    /// does not allow returning fire.
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
