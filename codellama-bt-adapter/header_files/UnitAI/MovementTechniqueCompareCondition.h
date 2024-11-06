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
//--| Movement Technique Compare Condition
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeCondition.h"
#include "Orders/MoveTacticallyOrder.h"
// Unreal
#include "CoreMinimal.h"
#include "MovementTechniqueCompareCondition.generated.h"

// TODO MR Comment:
//
// For the future: As soon as we have the need for more than one int/enumeration based equality
// compare condition, then this should be changed to template based, and then you can use manual
// template instantiation typedefs to define:
//
// typedef FMovementTechniqueCompareConditionInstanceData  FConstantCompareConditionInstanceData;
// typedef FMovementTechniqueCompareCondition FConstantCompareCondition;
//
// End MR Comment
/// @brief Instance data for `FMovementTechniqueCompareCondition`.
///
/// @ingroup SimulationBehaviors-module
USTRUCT()
struct FMovementTechniqueCompareConditionInstanceData
{
    GENERATED_BODY()

    /// @brief Inverts the condition
    UPROPERTY(EditAnywhere, Category = Parameter)
    bool bInvert = false;

    /// @brief Passed in movement technique
    UPROPERTY(EditAnywhere, Category = Input)
    EMovementTechnique MovementTechnique = EMovementTechnique::INVALID;

    /// @brief Which movement type to check against.
    UPROPERTY(EditAnywhere, Category = Parameter)
    EMovementTechnique MovementTechniqueToCheck = EMovementTechnique::INVALID;
};

/// @brief Movement Technique Compare Condition
///
/// @ingroup SimulationBehaviors-Plugin
USTRUCT(meta = (MilVerseUnitLevel))
struct FMovementTechniqueCompareCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FMovementTechniqueCompareConditionInstanceData;

    /// @brief Constructor
    FMovementTechniqueCompareCondition() = default;

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
    /// @sa FMovementTechniqueCompareConditionInstanceData
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Tests the condition.
    ///
    /// @param Context
    ///     The state tree context.
    /// @returns
    ///
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
