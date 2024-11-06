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
//--| FEnemySituationEvaluator class declaration.
//--|
//--|====================================================================|--
#pragma once

#include "CoreMinimal.h"

#include "AI/MilVerseStateTreeEvaluator.h"
#include "CommonAI/CommonTypes.h"
#include "Components/Engagement/AssignedFiringSectorComponent.h"
#include "Components/Engagement/CombatPowerComponent.h"
#include "Components/EntityInfoComponent.h"
#include "Components/EntityStateComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/Sensing/ShotAtDetectionComponent.h"
#include "Components/UnitIdentifierComponent.h"
#include "SimTimer.h"

#include "EnemySituationEvaluator.generated.h"

/// @brief Contains all the weapons that can be used to fire at the specific
/// platform type
USTRUCT(BlueprintType)
struct FPlatformTypeWeapons
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Parameter")
    /// @brief The platform type (e.g. Vehicle, Person, etc.)
    PlatformTypes PlatformType = PlatformTypes::Vehicle;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    /// @brief The weapon template names that can be used to fire at the platform type
    TArray<FName> CompatibleWeapons;
};

/// @brief Instance data for `FEnemySituationEvaluator`.
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(BlueprintType)
struct FEnemySituationEvaluatorInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Parameter")
    /// @brief If true, only elements within engagement distance will be considered.
    bool bWithinEngagementDistance = true;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    /// @brief percentage of maximum range to use
    float WeaponEngagementZonePercentage = 0.0f;

    /// @brief Time delay before notifying that the enemy situation has changed when it's due to
    /// being shot at.
    UPROPERTY(EditAnywhere, Category = "Parameter")
    float TimeUntilShotAtDetectionUpdate = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    /// @brief The distance at which an enemy is considered "very close" (meters).
    float CloseRangeDistanceM = 10.f;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    /// @brief If true, only elements within my assigned sector of fire will be considered.
    bool bOnlyInSector = true;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    /// @brief If true, fire at enemy if stunned.
    bool bIsAggressive = false;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    /// @brief If true, fire at enemy no matter that platform type of the enemy.
    bool bSkipPlatformTypeCheck = false;

    UPROPERTY(EditAnywhere, Category = "Parameter")
    /// @brief The different platform types and the weapons that can be fired at them
    TArray<FPlatformTypeWeapons> PlatformTypeWeapons;

    UPROPERTY(EditAnywhere, Category = "Output")
    /// @brief A prioritized array of targets
    FEnemySituation EnemySituation;

    UPROPERTY()
    /// @brief Max engagement distance for entity.
    float MaxEngagementDistance = 0.0;

    /// @brief The time remaining, in seconds, until we update the prioritized list of threats.
    float TimeRemainingBeforNextThreatListUpdate = -1.0f;

    /// @brief Used to track time between frames for a system.
    SimTimer SimClock;
};

/// @brief Evaluates potential targets and sorts them based upon priority
///
///
///
/// @ingroup SimulationBehaviors-Module
/// @ingroup EntityAI
USTRUCT(meta = (MilVerseEntityLevel))
struct FEnemySituationEvaluator : public FMilVerseStateTreeEvaluator
{
    GENERATED_BODY()

    /// @brief Alias for this condition's instance data type.
    using FInstanceDataType = FEnemySituationEvaluatorInstanceData;

    /// @brief Constructor
    FEnemySituationEvaluator() = default;

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
    /// @sa FEnemySituationEvaluatorInstanceData
    virtual const UStruct* GetInstanceDataType() const override
    {
        return FInstanceDataType::StaticStruct();
    }

    /**
     * Called when StateTree is started.
     * @param Context Reference to current execution context.
     */
    virtual void TreeStart(FStateTreeExecutionContext& Context) const override
    {
    }

    /**
     * Called when StateTree is stopped.
     * @param Context Reference to current execution context.
     */
    virtual void TreeStop(FStateTreeExecutionContext& Context) const override
    {
    }

    /**
     * Called each frame to update the evaluator.
     * @param Context Reference to current execution context.
     * @param DeltaTime Time since last StateTree tick, or 0 if called during preselection.
     */
    virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;

protected:
    /// @brief Handle for the `FAssignedFiringSectorComponent` ECS component.
    TStateTreeExternalDataHandle<FAssignedFiringSectorComponent> AssignedFiringSectorHandle;

    /// @brief Handle for the `FEntityStateComponent` ECS component.
    TStateTreeExternalDataHandle<FEntityStateComponent> EntityStateHandle;

    /// @brief Handle for the `FSensedEntitiesComponent` ECS component.
    TStateTreeExternalDataHandle<FSensedEntitiesComponent> SensedEntitiesHandle;

    /// @brief Handle for the `FCombatPowerComponent` ECS component.
    TStateTreeExternalDataHandle<FCombatPowerComponent> CombatPowerHandle;

    /// @brief Handle for the @ref FEntityInfoComponent ECS component.
    TStateTreeExternalDataHandle<FEntityInfoComponent> EntityInfoHandle;

    /// @brief Handle for the @ref FInventoryWeaponsComponent ECS component.
    TStateTreeExternalDataHandle<FInventoryWeaponsComponent> InventoryWeaponsHandle;

    /// @brief Handle for the @ref FInventoryComponent ECS Component.
    TStateTreeExternalDataHandle<FInventoryComponent> InventoryHandle;

    /// @brief Optional Handle for the @ref FUnitIdentifierComponent ECS component.
    TOptionalStateTreeExternalDataHandle<FUnitIdentifierComponent> UnitIdentifierHandle;

    /// @brief Handle for the @ref FShotAtDetectionComponent ECS component.
    TOptionalStateTreeExternalDataHandle<FShotAtDetectionComponent> ShotAtDetectionHandle;

private:
    /// @brief Determines if the target is within the firing sector
    /// @param EntityPosition
    ///     The Unreal position of the entity determining targets
    /// @param ConeDirectionNormal
    ///     The cone direction
    /// @param ConeAngleCos
    ///     The angle of the cone
    /// @param Target
    ///     The entity to be evaluated if within firing sector
    /// @return
    ///     True if within firing sector
    bool IsWithinFiringSector(const FVector& EntityPosition,
        const FVector& ConeDirectionNormal,
        const float& ConeAngleCos,
        const FSensedEntityData Target) const;

    /// @brief  Determine if the list of prioritized targets should be updated.
    /// @param PrioritizedThreats
    ///     The current list of threats.
    /// @param InstanceData
    ///     The context entity instance data
    /// @return true if the prioritized list of threats should be updated
    bool ShouldEvaluateTargets(TArray<FEnemySituationThreat>& PrioritizedThreats,
        FInstanceDataType& InstanceData) const;

    /// @brief Update the threats based on qualifying conditions
    ///
    /// @param Context
    ///     The execution context
    /// @param OutPrioritizedThreats
    ///     The container to update the threats in
    /// @param InstanceData
    ///     Access to the Instance data for the current entity
    /// @param EntityPosition
    ///     The unreal position of the context entity
    /// @param WeaponsComponent
    ///     The current weapons available to the entity
    /// @param EntityInfo
    ///     Basic information about the context entity
    /// @param OutPreviousPriorityThreat
    ///     Assgined to the value of the previous top threat, if there are any threats in the
    ///     OutPrioritizedThreats list coming into this method
    /// @return
    ///     true if the OutPrioritizedThreats contained any threats, false otherwise.
    bool DetermineThreats(FStateTreeExecutionContext& Context,
        TArray<FEnemySituationThreat>& OutPrioritizedThreats,
        FInstanceDataType& InstanceData,
        const FVector& EntityPosition,
        const FInventoryWeaponsComponent& WeaponsComponent,
        const FEntityInfoComponent& EntityInfo,
        FEnemySituationThreat& PreviousPriorityThreat) const;
};
