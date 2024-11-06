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
//--| State tree task, that calculates the ideal engagement location to engage the enemy at, Terrain
//--| and LOS considerations are not made currently.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVersePersistentStateTreeTask.h"
#include "CommonAI/CommonTypes.h"
#include "Components/UnitControllerComponent.h"
#include "PerceivedTruth/GlobalPerceivedTruthDataEntry.h"
#include "Routes/RoutePoint.h"

// Unreal
#include "CoreMinimal.h"
#include "FindEngagementLocationTask.generated.h"

/// @brief Instance data for 'FFindEngagementLocationTask'.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FFindEngagementLocationTaskInstanceData
    : public FMilVersePersistentStateTreeTaskInstanceData
{
    GENERATED_BODY()

    /// @brief List of waypoints to plan the route for.
    UPROPERTY(VisibleAnywhere, Category = "Output")
    TArray<FRoutePoint> Route;

    /// @brief LLA coordiates of area to search for enemy entities
    FLatLonAlt AreaToSearch;

    /// @brief Area to search data to be stored on enter state that will be used in tick
    TArray<FLatLonArea> AreaOfInterest;

    /// @brief Global id of the unit
    FGuid RootUnitGlobalID;

    /// @brief Bluefor unit's average position In Unreal Coordinates.
    FVector AverageBluforPosition = FVector::ZeroVector;

    /// @brief Blufor's average heading as a unit vector
    FVector AverageBluforUnitVector = FVector::ZeroVector;

    /// @brief Max Engagement Distance of currently selected weapon, In Meters.
    float MaxEngagementDistance = 0.0f;

    /// @brief Shared pointer to the promise that is generated when querying.
    TSharedPtr<TPromise<TArray<FGlobalPerceivedTruthDataEntry>>> Promise;

    /// @brief Shared pointer to the future that cooresponds to the promise.
    TSharedPtr<TFuture<TArray<FGlobalPerceivedTruthDataEntry>>> PTFuture;

    /// @brief The speed to use along the route in meters per second.
    UPROPERTY(EditAnywhere, Category = Parameter)
    float InSpeed = 0.0f;

    /// @brief Whether to have the engagement location be found on the flank of the enemy.
    UPROPERTY(EditAnywhere, Category = Parameter)
    bool bFlankingBehavior = false;

#if !UE_BUILD_SHIPPING
    /// @brief bool for keeping track of setting the location to search for via console command.
    bool bHasSetLocation = false;

    /// @brief Area to search within, Set via console command.
    FLatLonArea DebugAreaToSearch;
#endif
};

/// @brief State tree task for finding the engagement location position
///
/// This task requires that the entity have the following components assigned:
/// * `FUnitControllerComponent`
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FFindEngagementLocationTask : public FMilVersePersistentStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FFindEngagementLocationTaskInstanceData;

protected:
    /// @brief Called when the state tree asset is linked with data to allow the task to resolve
    /// references to other state tree data.
    ///
    /// @param Linker
    ///     Reference to the state tree's linker.
    /// @returns
    ///     Returns `true` if linking is successful; Otherwise returns `false`.
    virtual bool Link(FStateTreeLinker& Linker) override;

    /// @brief Returns the struct definition for this task's instance data.
    ///
    /// @sa FSelectClosestTargetTaskInstanceData
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Called when the state is entered that this task is part of.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param Transition
    ///     Information about the transition that caused the state to be selected. Not used for this
    ///     task.
    /// @returns
    ///     The running status of the task.
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override;

    /// @brief Called when a current state is exited and task is part of active states.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param Transition Describes the states involved in the transition
    ///     Information about the transition that caused the state to be exited. Not used for this
    ///     task.
    virtual void ExitState(FStateTreeExecutionContext& Context,
        const FStateTreeTransitionResult& Transition) const override;

    /// @brief Called during the state tree tick when the task is part of an active state.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param DeltaSeconds
    ///     The time in seconds since `Tick` was last called.
    /// @returns
    ///     The running status of the task.
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;

protected:
    /// @brief Data handle for the @ref FUnitControllerComponent ECS component.
    TStateTreeExternalDataHandle<FUnitControllerComponent> UnitControllerHandle;

private:
    /// @brief Calculate the engagement location given the average enemy location and heading.
    /// @param InEnemyLocation Average enemy location
    /// @param InEnemyHeading Average enemy heading
    /// @param InFriendlyLocation Average friendly location
    /// @param InFriendlyHeading Average friendly heading
    /// @param InMaxEngagementDistance max engagement distance of the unit.
    /// @param InMaxEngagementDistancePercentage max engagement distance to move within, IE 50% of
    /// InMaxEngagementDistance.
    /// @return Unreal position of the 'Ideal' engagement location for the enemy.
    const FVector CalculateEngagementLocation(const FVector& InEnemyLocation,
        const FVector& InEnemyHeading,
        const FVector& InFriendlyLocation,
        const FVector& InFriendlyHeading,
        const float& InMaxEngagementDistance,
        const float& InMaxEngagementDistancePercentage) const;

    /// @brief Calculates the Area To Search given a single point and a radius
    /// @param InLatLonAlt Center of the box
    /// @param InRadius Radius of the search in meters.
    /// @return FLatLonArea Struct
    const FLatLonArea CalculateAreaToSearch(const FLatLonAlt& InLatLonAlt,
        const float& InRadius) const;
};