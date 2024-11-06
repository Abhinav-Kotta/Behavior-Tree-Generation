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
//--| Defines StateTree condition related to Route points.
//--|
//--|====================================================================|--
#pragma once

#include "AI/MilVerseStateTreeCondition.h"
#include "AITypes.h"
#include "CommonTypes/LatLonAlt.h"
#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "Routes/RoutePoint.h"
#include "SimConstants.h"
#include "StateTreeAnyEnum.h"
#include "StateTreeConditionBase.h"
#include "RouteConditions.generated.h"

/// @brief Instance data for `FValidRouteCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FValidRouteConditionInstanceData
{
    GENERATED_BODY()

    /// @brief If this is true, then the condition is inverted.
    UPROPERTY(EditAnywhere, Category = Parameter)
    bool bInvert = false;

    /// @brief List of waypoints to plan the route for.
    UPROPERTY(VisibleAnywhere, Category = Parameter)
    TArray<FRoutePoint> WayPoints;
};

/// @brief Instance data for `FMilverseRouteDistanceCompareCondition`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FMilverseRouteDistanceCompareConditionInstanceData : public FValidRouteConditionInstanceData
{
    GENERATED_BODY()

    /// @brief Distance in meters used for conditions to compare against
    /// the distance between source and target.
    UPROPERTY(EditAnywhere, Category = Parameter)
    double DistanceMeters = 0.0;
};

/// @brief State tree condition if a route plan contains route points.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct FValidRouteCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FValidRouteConditionInstanceData;

    /// @brief Constructor
    FValidRouteCondition() = default;

protected:
    /// @brief Called when the state tree asset is linked with data to allow the condition to
    /// resolve references to other state tree data.
    ///
    /// @param Linker
    ///     Reference to the state tree's linker.
    /// @returns
    ///     Returns `true` if linking is successful; Otherwise returns `false`.
    bool Link(FStateTreeLinker& Linker) override;
    /// @brief Returns the struct definition for this condition's instance data.
    ///
    /// @sa FAttackConditionInstanceData
    const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Tests the condition.
    ///
    /// @param Context
    ///     The state tree context.
    /// @returns
    /// The condition returns true if a route plan contains route points.
    bool TestCondition(FStateTreeExecutionContext& Context) const override;
};

/// @brief Checks if the route length is less than the DistanceMeters.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct FMilverseRouteDistanceCompareCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FMilverseRouteDistanceCompareConditionInstanceData;

    FMilverseRouteDistanceCompareCondition() = default;

protected:
    /// @brief Returns the struct definition for this task's instance data.
    const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief The test condition.
    /// @param Context - Entity context for this state tree.
    /// @return Normally returns true if the route length is
    /// less than the DistanceMeters. bInvert can change this.
    bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
