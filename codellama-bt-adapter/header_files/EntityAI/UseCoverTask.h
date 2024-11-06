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
//--| Class declaration for Use Cover Task.
//--|
//--|====================================================================|--

#pragma once

#include "AI/MilVerseStateTreeTask.h"
#include "AvatarEnumerations.h"
#include "CommonTypes/LatLonAlt.h"

#include "CoreMinimal.h"

#include "UseCoverTask.generated.h"

/// @brief Instance data for `FUseCoverTask`.
///
/// @ingroup SimulationBehaviors-Module
USTRUCT()
struct SIMULATIONBEHAVIORS_API FUseCoverTaskInstanceData
{
    GENERATED_BODY()

    /// @brief The desired stance for the avatar while in cover.
    UPROPERTY(EditAnywhere, Category = Output)
    EAvatarStances CoverStance = EAvatarStances::NONE;

    /// @brief The desired stance for the avatar while exposed from cover.
    UPROPERTY(EditAnywhere, Category = Output)
    EAvatarStances ExposedStance = EAvatarStances::NONE;

    /// @brief The desired location of the Avatar while in Cover
    UPROPERTY(EditAnywhere, Category = Output)
    FLatLonAlt CoverLocation;

    /// @brief The desired location of the Avatar while exposed from cover
    UPROPERTY(EditAnywhere, Category = Output)
    FLatLonAlt ExposedLocation;

    /// @brief The desired weapon stance of the Avatar while in cover
    UPROPERTY(EditAnywhere, Category = Output)
    EAvatarWeaponActions CoverWeaponStance = EAvatarWeaponActions::NONE;

    /// @brief The desired yaw orientation of the Avatar while in Cover (degrees, unreal
    /// orientation);
    UPROPERTY(EditAnywhere, Category = Output)
    double CoverOrientation_deg = 0.0;

    /// @brief The desired yaw orientation of the Avatar while exposed from Cover (degrees, unreal
    /// orientation);
    UPROPERTY(EditAnywhere, Category = Output)
    double ExposedOrientation_deg = 0.0;
};

/// @brief State tree task for using cover. It shares properties of the cover
/// with the state tree for use by sub-tasks.
///
/// Returns EStateTreeRunStatus::Running if properties of the cover are available
/// Returns EStateTreeRunStatus::Succeeded Never
/// Returns EStateTreeRunStatus::Failed when properties of the cover location are
/// not available.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (DisplayName = "Use Cover Task", MilVerseEntityLevel))
struct SIMULATIONBEHAVIORS_API FUseCoverTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this node's instance data type.
    using FInstanceDataType = FUseCoverTaskInstanceData;

    /// @brief Constructor for the Use cover task. It will look up
    /// configuration data for this task.
    FUseCoverTask();

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
    ///     Information about the transition that caused the state to be selected. Not used for this
    ///     task.
    /// @returns
    ///     The running status of the task.
    ///       * Returns EStateTreeRunStatus::Running if properties of the cover are available
    ///       * Returns EStateTreeRunStatus::Succeeded Never
    ///       * Returns EStateTreeRunStatus::Failed when properties of the cover location are
    ///         not available.
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
    ///       * Returns EStateTreeRunStatus::Running if properties of the cover are available
    ///       * Returns EStateTreeRunStatus::Succeeded Never
    ///       * Returns EStateTreeRunStatus::Failed when properties of the cover location are
    ///         not available.
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context,
        const float DeltaTime) const override;
};
