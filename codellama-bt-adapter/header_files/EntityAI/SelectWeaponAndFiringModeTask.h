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
//--| Defines the SelectWeaponAndFiringModeTask state tree task
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "AvatarWeaponControlHelper.h"
#include "Components/Engagement/FireWeaponComponent.h"
#include "Components/Engagement/TargetComponent.h"
#include "Components/EntityInfoComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/Weapons/WeaponEnumerations.h"

#include "Config/WeaponFiringConfig.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "SelectWeaponAndFiringModeTask.generated.h"

/// @brief Instance data for 'FSelectWeaponAndFiringModeTask'.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT()
struct SIMULATIONBEHAVIORS_API FSelectWeaponAndFiringModeTaskInstanceData
{
    GENERATED_BODY()

    /// @brief The selected firing mode.
    ///
    /// Used to ensure the desired firing mode isn't changed outside of this task.
    UPROPERTY()
    EFiringModes ChosenFireMode = EFiringModes::SAFE;

    /// @brief How long the trigger is held when firing.
    ///
    /// Pulled from weapon data and is passed onto the appropriate component when the state is
    /// exited. Avoids an extra component lookup.
    UPROPERTY()
    float TriggerHoldTime = 0.0f;
};

/// @brief State tree task for performing suppression fire
///
/// This task requires that the entity have the following components assigned
/// * FAvatarWeaponControlComponent
/// * FFireWeaponComponent
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct SIMULATIONBEHAVIORS_API FSelectWeaponAndFiringModeTask : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FSelectWeaponAndFiringModeTaskInstanceData;

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
    /// @sa FSelectWeaponAndFiringModeTaskInstanceData
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
    /// @brief array carrying the fire mode types the user wants
    ///      to try to set the weapon to
    UPROPERTY(EditAnywhere, Category = Parameter)
    TArray<EFiringModes> FiringModeTypes;

    /// @brief Minimum time between shots.
    UPROPERTY(EditAnywhere, Category = Parameter)
    float FireIntervalMin = 0.0f;

    /// @brief Maximum time between shots.
    UPROPERTY(EditAnywhere, Category = Parameter)
    float FireIntervalMax = 0.0f;

protected:
    /// @brief Handle for the `FEntityInfoComponent` ECS component.
    TStateTreeExternalDataHandle<FEntityInfoComponent> EntityInfoComponentHandle;

    /// @brief Handle for the `FFireWeaponComponent` ECS component.
    TStateTreeExternalDataHandle<FFireWeaponComponent> FireWeaponComponentHandle;

    /// @brief Handle for the `FTargetComponent` ECS component.
    TStateTreeExternalDataHandle<FTargetComponent> TargetHandle;

    /// @brief Handle for the `FInventoryComponent` ECS component.
    TStateTreeExternalDataHandle<FInventoryComponent> InventoryHandle;

    /// @brief Handle for the `FInventoryWeaponsComponent` ECS component.
    TStateTreeExternalDataHandle<FInventoryWeaponsComponent> InventoryWeaponsHandle;

    /// @brief Handle for the `FInventoryEquippedWeaponsComponent` ECS component.
    TStateTreeExternalDataHandle<FInventoryEquippedWeaponsComponent> InventoryEquippedWeaponsHandle;

private:
    /// @brief Gets the available weapon modes from the active weapon.
    ///
    /// @param ActiveWeaponGlobalEntityId
    ///      GlobalEntityID of the Entity's active weapon
    /// @returns
    ///      List of available firing modes. Data is pulled from the weapon configuration and should
    ///      not be saved.
    const TArray<FWeaponFiringMode>& GetWeaponModes(const FGuid& ActiveWeaponGlobalEntityId) const;
};
