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
//--| Defines Milverse Distance compare conditions of two FLatLon locations.
//--|
//--|====================================================================|--
#pragma once

#include "AI/MilVerseStateTreeCondition.h"
#include "AITypes.h"
#include "CommonTypes/LatLonAlt.h"
#include "InstancedStruct.h"
#include "SimConstants.h"
#include "StateTreeAnyEnum.h"
#include "StateTreeConditionBase.h"
#include "MilverseDistanceCompareConditions.generated.h"

/**
 * Condition comparing distance between two vectors.
 */
USTRUCT()
struct FMilverseStateTreeCompareDistanceConditionInstanceData
{
    GENERATED_BODY()

    /// @brief Source Location
    UPROPERTY(EditAnywhere, Category = Input)
    FLatLonAlt Source;

    /// @brief Target Location
    UPROPERTY(EditAnywhere, Category = Input)
    FLatLonAlt Target;

    /// @brief Distance in meters used for conditions to compare against
    /// the distance between source and target.
    UPROPERTY(EditAnywhere, Category = Parameter)
    double DistanceMeters = 0.0;
};

/// @brief State tree condition for determining conditions related to the distance between two
/// locations.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct FMilverseDistanceCompareCondition : public FMilVerseStateTreeCondition
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FMilverseStateTreeCompareDistanceConditionInstanceData;

    FMilverseDistanceCompareCondition() = default;

protected:
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }
    /// @brief The test condition.
    /// @param Context - Entity context for this state tree.
    /// @return Normally returns true if the calculated distance is
    /// less than the DistanceMeters. bInvert can change this.
    virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;

    /// @brief Inverts the test condition. The test condition normally returns true
    /// if the calculated distance is less than DistanceMeters of the location. If
    /// bInvert is true, then the test condition will return false if the calculated
    /// distance is greater than DistanceMeters.
    UPROPERTY(EditAnywhere, Category = Condition)
    bool bInvert = false;
};
