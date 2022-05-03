// Fill out your copyright notice in the Description page of Project Settings.


#include "AI_Bot_Controller.h"
#include "AI_Bot_Character.h"
#include "Waypoint.h"
#include "Animation/AnimMontage.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AAI_Bot_Controller::AAI_Bot_Controller()
{
	PrimaryActorTick.bCanEverTick = true;

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));

	SightConfig->SightRadius = AISightRadius;
	SightConfig->LoseSightRadius = AILoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = AIFieldOfView;
	SightConfig->SetMaxAge(AISightAge);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AAI_Bot_Controller::OnPawnDetected);


	GetPerceptionComponent()->ConfigureSense(*SightConfig);

}

void AAI_Bot_Controller::BeginPlay()
{
	Super::BeginPlay();
	

}

void AAI_Bot_Controller::OnPossess(APawn* MyPawn)
{
	Super::OnPossess(MyPawn);
}

void AAI_Bot_Controller::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AAI_Bot_Character* MyCharacter = Cast<AAI_Bot_Character>(GetPawn());
	
	if (DistanceToPlayer > AISightRadius)
	{
		bIsPlayerDetected = false;
	}
	
	//Move to Waypoint
	if (MyCharacter->NextWaypoint != nullptr && bIsPlayerDetected == false)

	{
		MoveToActor(MyCharacter->NextWaypoint, 0.1f);
		
	}
	
	else if (bIsPlayerDetected == true)
	
	{
		AActor* Player = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

		MoveToActor(Player, 50.0f);
		
	}

	
}

FRotator AAI_Bot_Controller::GetControlRotation() const
{
	if (GetPawn() == nullptr)
	{
		return FRotator(0.0f, 0.0f, 0.0f);
	}
	return FRotator(0.0f, GetPawn()->GetActorRotation().Yaw, 0.0f);
}

void AAI_Bot_Controller::OnPawnDetected(const TArray<AActor*>& Detectedpawns)
{
	for (size_t i = 0; i < Detectedpawns.Num(); i++)
	{

		DistanceToPlayer = GetPawn()->GetDistanceTo(Detectedpawns[i]);

	}
	bIsPlayerDetected = true;
}
