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
//--| Class declaration for a Find Cover Task. This will locate and claim
//--| available cover.
//--|
//--|====================================================================|--

#pragma once

#include "AI/MilVerseStateTreeTask.h"
#include "CommonTypes/LatLonAlt.h"
#include "Components/CoverClaimedComponent.h"
#include "Components/CoverLocationsComponent.h"
#include "Components/EntityStateComponent.h"
#include "Components/PlannedRoutePointDataComponent.h"
#include "Components/Sensing/ShotAtDetectionComponent.h"
#include "CoreMinimal.h"
#include "Routes/RoutePoint.h"

#include "FindCoverTask.generated.h"

/// @brief Instance data for `FFindCoverTask`.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FFindCoverTaskInstanceData
{
    GENERATED_BODY()

    /// @brief List of waypoints to plan the route for. For this task it should contain the route
    /// point of the cover location
    UPROPERTY(VisibleAnywhere, Category = "Output")
    TArray<FRoutePoint> WayPointsToCover;

    /// @brief Speed to travel along the route.
    UPROPERTY(EditAnywhere, Category = Parameter)
    float InSpeed = 0.0f;
};

/// @brief State tree task for finding cover.
///
/// This task requires that the entity have the following components assigned:
/// * FEntityStateComponent
/// * FEntityInfoComponent
/// * FShotAtDetectionComponent
///
/// The task locates the nearest cover that offers protection from a threat. The
/// output includes the location of the cover, as well as boolean indicating if the
/// output is valid.
///
/// The task runs forever. It will return failed if cover cannot be found. It is
/// designed to be a parent task.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (DisplayName = "Find Cover Task", MilVerseEntityLevel))
struct SIMULATIONBEHAVIORS_API FFindCoverTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this node's instance data type.
    using FInstanceDataType = FFindCoverTaskInstanceData;

    /// @brief Constructor for the find cover task. It will look up
    /// configuration data for this task.
    FFindCoverTask();

protected:
    /// @brief Called when the state tree asset is linked with data to allow the task to resolve
    /// references to other state tree data.
    ///
    /// @param Linker
    ///     Reference to the state tree's linker.
    /// @returns
    ///     Returns `true` if linking is successful; Otherwise returns `false`.
    virtual bool Link(FStateTreeLinker& Linker) override;

    /// @brief Returns the struct definition for this nodes's instance data.
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /// @brief Called when the state is entered that this task is part of.
    ///
    /// @param Context
    ///     The state tree context.
    /// @param Transition
    ///     Information about the transition that caused the state to be selected.
    ///     Not used for this task.
    /// @returns
    ///     The running status of the task.
    ///     * EStateTreeRunStatus::Running If we are looking for a cover location or
    ///       a cover location was found.
    ///     * EStateTreeRunStatus::Succeeded - Never
    ///     * EStateTreeRunStatus::Failed if no cover location could be claimed.
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
    ///     * EStateTreeRunStatus::Running if successful and we are waiting for a cover location to
    ///     return.
    ///     * EStateTreeRunStatus::Succeeded if cover location was found and claimed.
    ///     * EStateTreeRunStatus::Failed if no cover location could be claimed.
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;

private:
    /// @brief Handle for the `FEntityStateComponent` ECS component.
    TStateTreeExternalDataHandle<FEntityStateComponent> EntityStateHandle;

    /// @brief Handle for the `FEntityInfoComponent` ECS component.
    TStateTreeExternalDataHandle<FShotAtDetectionComponent> ShotAtDetectionHandle;

    /// @brief Handle for the `FPlannedRoutePointDataComponent` ECS component.
    ///
    /// This provides the information about the Cover Location to the cover route plan task.
    TStateTreeExternalDataHandle<FPlannedRoutePointDataComponent> PlannedCoverRouteDataHandle;

    /// @brief Determines if a particular location provides cover from a threat.
    /// @param ShotFromDirection - A vector of the cardinal direction the entity is being shot from
    /// @param ProtectionSectorWidth - The protection sector width in degrees
    /// @param ProtectionDirection - The protection direction (expecting an Unreal angle)
    /// @return - True if the location provides protection from the threat.
    bool DoesCoverLocationProvideCover(FVector& ShotFromDirection,
        double ProtectionSectorWidth,
        double ProtectionDirection) const;

    /// @brief Distance in unreal engine units (cm) for the search range for cover locations.
    /// Default value is 2500 centimeters. This means that a 50m x 50m x 5om cube centered at
    /// the search location is created for finding cover spots. This can be customized in the
    /// project settings file.
    double CoverSearchRangeCm = 100.0 * 25;
};
