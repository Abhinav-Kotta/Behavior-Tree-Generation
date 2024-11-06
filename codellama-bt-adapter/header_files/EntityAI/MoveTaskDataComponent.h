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
//--| Defines the FMoveTaskDataComponent.
//--|
//--|====================================================================|--
#pragma once

#include "ECS/SimComponent.h"
#include "Routes/RoutePoint.h"

#include "CoreMinimal.h"
#include "MoveTaskDataComponent.generated.h"

/// @brief Component for `FMoveTask` state tree task.
///
/// Contains the data required to move an entity along a route. This component is required for any
/// entity being controlled by a state tree using the `FMoveTask`.
///
/// @sa FMoveToComponent
/// @sa FMoveTask
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct SIMULATIONBEHAVIORS_API FMoveTaskDataComponent : public FSimComponent
{
    GENERATED_BODY()

    /// @brief List of waypoints the entity will travel along.
    UPROPERTY()
    TArray<FRoutePoint> RoutePoints;

    /// @brief Id of the formation leader.
    ///
    /// If this value is not valid (`Leader.IsValid() == false`), then the entity is the leader of
    /// the formation or is not in a formation.
    UPROPERTY()
    FGuid Leader;

    /// @brief Acceptable distance this follower can be from the leader in Unreal Engine units (cm).
    ///
    /// This is only valid if `Leader` is valid. Note that this is the squared distance which is
    /// used because it is more performant to compute the squared distances between points for
    /// comparison then it is to compute the actual distance.
    UPROPERTY()
    float MinDistanceToLeaderSquared = 0;

    /// @brief The index for the route point the entity is moving towards.
    UPROPERTY()
    int32 RoutePointsIndex = 0;

    /// @brief Formation offset from the leader in Unreal Engine local space.
    UPROPERTY()
    FVector FormationOffset = FVector::ZeroVector;

    /// @brief Indicates if the entity has been ordered to move.
    ///
    /// When ordered to move, this field will remain true until the completion of the route.
    UPROPERTY()
    bool bMoveOrdered = false;

    /// @brief Used in conjunction with `bMoveOrdered` to determine when a move is first ordered.
    ///
    /// This field should not be updated outside of the task handling the move. Normally, this will
    /// be the `FMoveTask` state tree task.
    UPROPERTY()
    bool bMoveOrderedPrev = false;

    UPROPERTY()
    bool bRotateOnCompletion = false;

    UPROPERTY()
    FRotator EndingRotation = FRotator::ZeroRotator;

    UINT RouteID = 0;
};
