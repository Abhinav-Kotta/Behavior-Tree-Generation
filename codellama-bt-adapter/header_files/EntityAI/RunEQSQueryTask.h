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
//--| TODO: Add description.
//--|
//--|====================================================================|--
#pragma once

// MilVerse
#include "AI/MilVerseStateTreeTask.h"
#include "Components/EntityStateComponent.h"
#include "EntityAI/MoveTaskDataComponent.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "RunEQSQueryTask.generated.h"

/// @brief Instance data for the FRunEQSQueryTaskSingleVector task.
USTRUCT()
struct FRunEQSQueryTaskSingleVectorInstanceData
{
    GENERATED_BODY()

    /// @brief Pointer to the EQS Query asset to use.
    UPROPERTY(EditAnywhere, Category = Parameter)
    UEnvQuery* EQSQuery = nullptr;

    /// @brief Shared pointer to the promise that is generated when executing an EQS Query.
    TSharedPtr<TPromise<FVector>> Promise;

    /// @brief Shared pointer to the future that is generated when executing an EQS Query.
    TSharedPtr<TFuture<FVector>> Future;

    /// @brief EEnvQueryRunMode Enum which defines the kind of result that gets outputed, Random
    /// return within Top 25%, Random return within Top 5% Or single best matching item (Highest
    /// Value).
    UPROPERTY(EditAnywhere, Category = Parameter)
    TEnumAsByte<EEnvQueryRunMode::Type> EnvQueryRunMode;

    /// @brief EQS Results as a single vector, See EEnvQueryRunMode for a more detailed description
    /// of the output.
    UPROPERTY(EditAnywhere, Category = Output)
    FVector EQSResults = FVector::ZeroVector;
};

/// @brief Run EQS Query Task returning the result as a single vector (Location in UE Coordinates)
USTRUCT(meta = (MilVerseEntityLevel))
struct SIMULATIONBEHAVIORS_API FRunEQSQueryTaskSingleVector : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FRunEQSQueryTaskSingleVectorInstanceData;

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

    /// @brief Callback for when the EQS Query is finished.
    /// @param Result EQS Query Result
    /// @param Promise to get the data from EQS.
    void OnEQSQueryFinishedSingleVector(TSharedPtr<FEnvQueryResult> Result,
        TSharedPtr<TPromise<FVector>> Promise) const;
};

/// @brief Instance data for the FRunEQSQueryTaskSingleActor task.
USTRUCT()
struct FRunEQSQueryTaskSingleActorInstanceData
{
    GENERATED_BODY()

    /// @brief Pointer to the EQS Query asset to use.
    UPROPERTY(EditAnywhere, Category = Parameter)
    UEnvQuery* EQSQuery = nullptr;

    /// @brief Shared pointer to the promise that is generated when executing an EQS Query.
    TSharedPtr<TPromise<AActor*>> Promise;

    /// @brief Shared pointer to the future that is generated when executing an EQS Query.
    TSharedPtr<TFuture<AActor*>> Future;

    /// @brief EEnvQueryRunMode Enum which defines the kind of result that gets outputed, Random
    /// return within Top 25%, Random return within Top 5% Or single best matching item (Highest
    /// Value).
    UPROPERTY(EditAnywhere, Category = Parameter)
    TEnumAsByte<EEnvQueryRunMode::Type> EnvQueryRunMode;

    /// @brief EQS Results as a single actor pointer, See EEnvQueryRunMode for a more detailed
    /// description of the output.
    UPROPERTY(EditAnywhere, Category = Output)
    AActor* EQSResults = nullptr;
};

/// @brief Run EQS Query Task returning the result as a single actor pointer.
USTRUCT(meta = (MilVerseEntityLevel))
struct FRunEQSQueryTaskSingleActor : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FRunEQSQueryTaskSingleActorInstanceData;

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

    /// @brief Callback for when the EQS Query is finished.
    /// @param Result EQS Query Result
    /// @param  Promise to get the data from EQS.
    void OnEQSQueryFinishedSingleActor(TSharedPtr<FEnvQueryResult> Result,
        TSharedPtr<TPromise<AActor*>> Promise) const;
};

/// @brief Instance data for the FRunEQSQueryTaskMultiVector task.
USTRUCT()
struct FRunEQSQueryTaskMultiVectorInstanceData
{
    GENERATED_BODY()

    /// @brief Pointer to the EQS Query asset to use.
    UPROPERTY(EditAnywhere, Category = Parameter)
    UEnvQuery* EQSQuery = nullptr;

    /// @brief Shared pointer to the promise that is generated when executing an EQS Query.
    TSharedPtr<TPromise<TArray<FVector>>> Promise;

    /// @brief Shared pointer to the future that is generated when executing an EQS Query.
    TSharedPtr<TFuture<TArray<FVector>>> Future;

    /// @brief EQS Results from EQS Query Pointer, returned as an array of FVector's.
    UPROPERTY(EditAnywhere, Category = Output)
    TArray<FVector> EQSResults;
};

/// @brief Run EQS Query task that returns the results as an array of all matching vectors
/// (Locations in UE Coordinates).
USTRUCT(meta = (MilVerseEntityLevel))
struct FRunEQSQueryTaskMultiVector : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FRunEQSQueryTaskMultiVectorInstanceData;

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

    /// @brief Callback for when the EQS Query is finished.
    /// @param Result EQS Query Result
    /// @param  Promise to get the data from EQS.
    void OnEQSQueryFinishedMultiVector(TSharedPtr<FEnvQueryResult> Result,
        TSharedPtr<TPromise<TArray<FVector>>> Promise) const;
};

/// @brief Instance data for the FRunEQSQueryTaskMultiActor task.
USTRUCT()
struct FRunEQSQueryTaskMultiActorInstanceData
{
    GENERATED_BODY()

    /// @brief Pointer to the EQS Query asset to use.
    UPROPERTY(EditAnywhere, Category = Parameter)
    UEnvQuery* EQSQuery = nullptr;

    /// @brief Shared pointer to the promise that is generated when executing an EQS Query.
    TSharedPtr<TPromise<TArray<AActor*>>> Promise;

    /// @brief Shared pointer to the future that is generated when executing an EQS Query.
    TSharedPtr<TFuture<TArray<AActor*>>> Future;

    /// @brief EQS Results from EQS Query Pointer, returned as an array of actor pointers.
    UPROPERTY(EditAnywhere, Category = Output)
    TArray<AActor*> EQSResults;
};

/// @brief EQS Query task that returns the result as an array of actor pointers.
USTRUCT(meta = (MilVerseEntityLevel))
struct FRunEQSQueryTaskMultiActor : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FRunEQSQueryTaskMultiActorInstanceData;

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

    /// @brief Callback for when the EQS Query is finished.
    /// @param Result EQS Query Result
    /// @param  Promise to get the data from EQS.
    void OnEQSQueryFinishedMultiActor(TSharedPtr<FEnvQueryResult> Result,
        TSharedPtr<TPromise<TArray<AActor*>>> Promise) const;
};

/// @brief Instance data for the FRunEQSQueryTaskMoveTo task.
USTRUCT()
struct FRunEQSQueryTaskMoveToInstanceData
{
    GENERATED_BODY()

    /// @brief Pointer to the EQS Query asset to use.
    UPROPERTY(EditAnywhere, Category = Parameter)
    UEnvQuery* EQSQuery = nullptr;

    /// @brief Shared pointer to the promise that is generated when executing an EQS Query.
    TSharedPtr<TPromise<FVector>> Promise;

    /// @brief Shared pointer to the future that is generated when executing an EQS Query.
    TSharedPtr<TFuture<FVector>> Future;

    /// @brief EEnvQueryRunMode Enum which defines the kind of result that gets outputed, Random
    /// return within Top 25%, Random return within Top 5% Or single best matching item (Highest
    /// Value).
    UPROPERTY(EditAnywhere, Category = Parameter)
    TEnumAsByte<EEnvQueryRunMode::Type> EnvQueryRunMode;

    /// @brief EQS Results to be stored in the instance data for retreving them to construct a
    /// route.
    FVector EQSResults = FVector::ZeroVector;

    /// @brief Array of WayPoints to be taken in by a route planning task.
    UPROPERTY(VisibleAnywhere, Category = Output)
    TArray<FRoutePoint> WayPoints;
};

USTRUCT(meta = (MilVerseEntityLevel))
struct SIMULATIONBEHAVIORS_API FRunEQSQueryTaskMoveTo : public FMilVerseStateTreeTask
{
    GENERATED_BODY()

    /// @brief Alias for this task's instance data type.
    using FInstanceDataType = FRunEQSQueryTaskMoveToInstanceData;

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

    /// @brief Callback for when the EQS Query is finished.
    /// @param Result EQS Query Result
    /// @param  Promise to get the data from EQS.
    void OnEQSQueryFinishedSingleVector(TSharedPtr<FEnvQueryResult> Result,
        TSharedPtr<TPromise<FVector>> Promise) const;

    /// @brief State Tree Handle for the @ref FEntityStateComponent
    TStateTreeExternalDataHandle<FEntityStateComponent> EntityStateComponentHandle;
};
