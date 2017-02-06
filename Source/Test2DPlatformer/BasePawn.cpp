// Fill out your copyright notice in the Description page of Project Settings.

#include "Test2DPlatformer.h"
#include "BasePawn.h"


// Sets default values
ABasePawn::ABasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABasePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABasePawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	LastTimeMult = TimeMult;
	ActualDeltaTime = DeltaTime * TimeRate;
	ComputedDeltaTime = FMath::Min(ActualDeltaTime, 0.01666667f * (TimeRate + 0.008333334f));
	TimeMult = ComputedDeltaTime / 0.01666667f;

}

// Called to bind functionality to input
void ABasePawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

bool ABasePawn::MoveH(float moveH, OnCollidePtr onCollide) {
	SubPixelCounter.X += moveH;
	int32 dx = FMath::RoundToInt(SubPixelCounter.X);
	if (dx != 0) {
		int8 sign = FMath::Sign(dx);
		SubPixelCounter.X -= sign;
		while (dx != 0) {
			AActor *entity = CollideFirst(/*GameTags.Solid, */GetActorLocation().X + sign, GetActorLocation().Z);
			if (entity != nullptr)
			{
				SubPixelCounter.X = 0.0f;
				if (onCollide != nullptr)
                {
                    onCollide(entity);
                }
				return true;
			}
			FVector location = GetActorLocation();
			location.X += sign;
			SetActorLocation(location);
			dx -= sign;
		}
	}
	return false;
}

bool ABasePawn::MoveV(float moveV, OnCollidePtr onCollide = nullptr) {
	AActor *entity;
	SubPixelCounter.Z += moveV;
	int32 dy = FMath::RoundToInt(SubPixelCounter.Z);
	if (dy < 0)
	{
		SubPixelCounter.Z -= dy;
		while (dy != 0)
		{
			entity = CollideFirst(/*GameTags.Solid, */GetActorLocation().X, GetActorLocation().Z - 1.0f);
			if (entity != nullptr)
			{
				SubPixelCounter.Z = 0.0f;
				if (onCollide != nullptr)
                {
                    onCollide(entity);
                }
				return true;
			}
			FVector location = GetActorLocation();
			location.Z += -1;
			SetActorLocation(location);
			dy -= -1;
		}
	}
	else if (dy > 0)
	{
		SubPixelCounter.Z -= dy;
		while (dy != 0)
		{
			entity = CollideFirst(/*GameTags.Solid, */GetActorLocation().X, GetActorLocation().Z + 1.0f);
			if (entity != nullptr)
			{
				SubPixelCounter.Z = 0.0f;
				if (onCollide != nullptr)
                {
                    onCollide(entity);
                }
				return true;
			}
			if (!IgnoreJumpThrus && ((entity = CollideFirst(/*GameTags.JumpThru, */GetActorLocation().X, GetActorLocation().Z + 1.0f)) != nullptr))
			{
				SubPixelCounter.Z = 0.0f;
				if (onCollide != nullptr)
                {
                    onCollide(entity);
                }
				return true;
			}
			FVector location = GetActorLocation();
			location.Z += 1.0f;
			SetActorLocation(location);
			dy--;
		}
	}
	return false;
}

AActor *ABasePawn::CollideFirst(float x, float z) {
	AActor *res = NULL;
	FVector originalLocation = GetActorLocation();
	FVector newLocation = GetActorLocation();
	newLocation.X = x;
	newLocation.Z = z;
	SetActorLocation(newLocation);

	TArray<AActor *> actors;
	WorldCollisionBoxComponent->GetOverlappingActors(actors, NULL);
	for (auto CompIt = actors.CreateIterator(); CompIt; ++CompIt) {
		//UE_LOG(LogTemp, Warning, TEXT("Overlapping actor."));
		AActor *OverlappingActor = *CompIt;
		if (OverlappingActor != this) {
			UE_LOG(LogTemp, Warning, TEXT("CollideFirst: Actor: %s"), *AActor::GetDebugName(OverlappingActor));
			res = OverlappingActor;
		}
	}

	SetActorLocation(originalLocation);
	return res;
}

bool ABasePawn::CollideCheck(AActor *other, float x, float z) {
    AActor *res = nullptr;
    FVector originalLocation = GetActorLocation();
    FVector newLocation = GetActorLocation();
    newLocation.X = x;
    newLocation.Z = z;
    SetActorLocation(newLocation);

    TArray<AActor *> actors;
    WorldCollisionBoxComponent->GetOverlappingActors(actors, NULL);
    for (auto CompIt = actors.CreateIterator(); CompIt; ++CompIt) {
        //UE_LOG(LogTemp, Warning, TEXT("Overlapping actor."));
        AActor *OverlappingActor = *CompIt;
        if (OverlappingActor == other) {
            UE_LOG(LogTemp, Warning, TEXT("CollideCheck: Actor: %s"), *AActor::GetDebugName(OverlappingActor));
            res = OverlappingActor;
        }
    }

    SetActorLocation(originalLocation);
    return res != nullptr;
}

float ABasePawn::Approach(float start, float end, float shift)
{
	if (start < end)
	{
		return FMath::Min(start + shift, end);
	}
	else
	{
		return FMath::Max(start - shift, end);
	}
}

FVector ABasePawn::Approach(FVector val, FVector target, float maxMove)
{
    if ((maxMove == 0.0f) || (val == target))
    {
        return val;
    }
    FVector vector = target - val;
    if (vector.Size() < maxMove)
    {
        return target;
    }
    vector.Normalize();
    return (val + (vector * maxMove));
}


void ABasePawn::Move(FVector amount, OnCollidePtr onCollideH, OnCollidePtr onCollideV)
{
    MoveH(amount.X/*, onCollideH*/);
    MoveV(amount.Z/*, onCollideV*/);
}

void ABasePawn::MoveTowards(FVector target, float maxAmount/*, Action<Solid> onCollideH = null, Action<Solid> onCollideV = null*/)
{
    FVector vector = Approach(ActualPosition(), target, maxAmount);
    Move(vector - ActualPosition()/*, null, null*/);
}

FVector ABasePawn::ActualPosition()
{
    FVector location = GetActorLocation();
    return FVector(location.X + SubPixelCounter.X, location.Y + SubPixelCounter.Y, location.Z + SubPixelCounter.Z);
}