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
//--| Generic Statetree condition used to check if a unit's platform matches a specific type.
//--|
//--|====================================================================|--
#pragma once

#include "AI/MilVerseStateTreeCondition.h"
#include "Components/EntityInfoComponent.h"

#include "CoreMinimal.h"
#include "CheckUnitPlatformCondition.generated.h"

/// @brief Instance data for `FUnitPlatformCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FUnitPlatformInstanceData
{
    GENERATED_BODY()

    /// @brief Platform Type to check against.
    UPROPERTY(EditAnywhere, Category = Parameter)
    PlatformTypes PlatformTypeToCheck = PlatformTypes::Person;
};

/// @brief State tree condition for checking a Unit Controllers platform
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseGenericLevel))
struct FUnitPlatformCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FUnitPlatformInstanceData;

    /// @brief Constructor
    FUnitPlatformCondition() = default;

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
    /// The condition returns true Unit level platform matches the specified platform in the
    /// instance data.
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

protected:
    /// @brief Handle for the @ref FEntityInfoComponent ECS component.
    TStateTreeExternalDataHandle<FEntityInfoComponent> UnitInfoHandle;
};
