// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/InteractableInterface.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Objects/ObjectBase.h"
#include "Components/PostProcessComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/UnGrabbableInterface.h"
#include "PuzzleNameD/PuzzleNameD.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Interactable, ECollisionResponse::ECR_Block);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetMesh(), FName("head"));
	Camera->bUsePawnControlRotation = true;

	PostProcess = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess"));
	PostProcess->SetupAttachment(Camera);
	PostProcess->bEnabled = false;
	PostProcess->Settings.WhiteTemp = 4957.0f;
	PostProcess->Settings.bOverride_WhiteTemp = true;
	PostProcess->Settings.WhiteTint = 0.43f;
	PostProcess->Settings.bOverride_WhiteTint = true;
	PostProcess->Settings.ColorSaturation = FVector4(0.56f, 0.56f, 0.56f, 1.0f);
	PostProcess->Settings.bOverride_ColorSaturation = true;
	PostProcess->Settings.ColorContrast = FVector4(0.5f, 0.5f, 0.5f, 1.0f);
	PostProcess->Settings.bOverride_ColorContrast = true;
	PostProcess->Settings.ColorGamma = FVector4(0.5f, 0.5f, 0.5f, 1.0f);
	PostProcess->Settings.bOverride_ColorGamma = true;
	PostProcess->Settings.VignetteIntensity = 1.3f;
	PostProcess->Settings.bOverride_VignetteIntensity = true;
	PostProcess->Settings.FilmGrainIntensity = 0.5f;
	PostProcess->Settings.bOverride_FilmGrainIntensity = true;

	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 0.2f;
	GetCharacterMovement()->Mass = 300.0f;
	GetCharacterMovement()->JumpZVelocity = 630.0f;

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	PhysicsHandle->bSoftAngularConstraint = false;
	PhysicsHandle->bInterpolateTarget = false;
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MainContext, 0);
		}
	}

	TArray<AActor*> Interactables;
	UGameplayStatics::GetAllActorsOfClass(this, AObjectBase::StaticClass(), Interactables);
	for (AActor* Interactable : Interactables)
	{
		AObjectBase* InteractableObject = Cast<AObjectBase>(Interactable);
		if (InteractableObject)
		{
			InteractableObjects.Emplace(InteractableObject);
		}
	}
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Camera == nullptr || PhysicsHandle == nullptr) return;

	if (PhysicsHandle->GrabbedComponent)
	{
		FVector Start = Camera->GetComponentLocation();
		FVector End = Start + Camera->GetComponentRotation().Vector() * Length;
		FVector Location;
		FRotator Rotation;
		PhysicsHandle->GetTargetLocationAndRotation(Location, Rotation);
		Rotation.Yaw = Camera->GetComponentRotation().Yaw - YawWhenGrabbed;
		Rotation.Roll = RollWhenSnapped;
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("%s"), *Rotation.ToString()));
		PhysicsHandle->SetTargetLocationAndRotation(End, Rotation);
	}
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(GrabAction, ETriggerEvent::Triggered, this, &AMainCharacter::TryGrab);
		EnhancedInputComponent->BindAction(DuplicateAction, ETriggerEvent::Triggered, this, &AMainCharacter::TryDuplicate);
		EnhancedInputComponent->BindAction(DeleteAction, ETriggerEvent::Triggered, this, &AMainCharacter::TryDelete);
		EnhancedInputComponent->BindAction(ClearAction, ETriggerEvent::Triggered, this, &AMainCharacter::TryClear);
		EnhancedInputComponent->BindAction(ExamineAction, ETriggerEvent::Triggered, this, &AMainCharacter::Examine);
		EnhancedInputComponent->BindAction(SnapLeftAction, ETriggerEvent::Triggered, this, &AMainCharacter::SnapLeft);
		EnhancedInputComponent->BindAction(SnapRightAction, ETriggerEvent::Triggered, this, &AMainCharacter::SnapRight);
		EnhancedInputComponent->BindAction(SnapUpAction, ETriggerEvent::Triggered, this, &AMainCharacter::SnapUp);
		EnhancedInputComponent->BindAction(SnapDownAction, ETriggerEvent::Triggered, this, &AMainCharacter::SnapDown);
	}
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AMainCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

void AMainCharacter::TryGrab()
{
	if (PostProcess->bEnabled) return;

	if (PhysicsHandle->GrabbedComponent)
	{
		Release();
	}
	else
	{
		Grab();
	}
}

void AMainCharacter::Grab()
{
	if (Camera == nullptr || PhysicsHandle == nullptr) return;

	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetComponentRotation().Vector() * ArmLength;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility
	);

	AActor* Interactable = HitResult.GetActor();
	UPrimitiveComponent* Target = HitResult.GetComponent();
	IInteractableInterface* TargetObject = Cast<IInteractableInterface>(Interactable);
	if (TargetObject && !Interactable->Implements<UUnGrabbableInterface>() && Target)
	{
		PhysicsHandle->GrabComponentAtLocationWithRotation(Target, NAME_None, Target->GetComponentLocation(), Target->GetComponentRotation());
		YawWhenGrabbed = Camera->GetComponentRotation().Yaw - Target->GetComponentRotation().Yaw;
		Length = (Target->GetComponentLocation() - Camera->GetComponentLocation()).Size();
		TargetObject->SetIsGrabbed(true);
		if (BasedMovement.MovementBase == PhysicsHandle->GrabbedComponent)
		{
			Release();
		}
	}
}

void AMainCharacter::Grab(UPrimitiveComponent* Target)
{
	if (PhysicsHandle == nullptr || Target == nullptr) return;

	PhysicsHandle->GrabComponentAtLocationWithRotation(Target, NAME_None, Target->GetComponentLocation(), Target->GetComponentRotation());
	YawWhenGrabbed = Camera->GetComponentRotation().Yaw - Target->GetComponentRotation().Yaw;
	IInteractableInterface* TargetObject = Cast<IInteractableInterface>(Target->GetOwner());
	if (TargetObject)
	{
		TargetObject->SetIsGrabbed(true);
	}
}

void AMainCharacter::Release()
{
	if (PhysicsHandle == nullptr) return;

	UPrimitiveComponent* Component = PhysicsHandle->GrabbedComponent;
	if (Component == nullptr || Component->GetOwner() == nullptr) return;

	TArray<AActor*> OverlappingActors;
	Component->GetOverlappingActors(OverlappingActors, AObjectBase::StaticClass());
	Component->GetOverlappingActors(OverlappingActors, AMainCharacter::StaticClass());
	if (OverlappingActors.Num() > 0) return;

	IInteractableInterface* GrabbedActor = Cast<IInteractableInterface>(Component->GetOwner());
	if (GrabbedActor)
	{
		GrabbedActor->SetCollisionResponse(ECollisionResponse::ECR_Block);
		GrabbedActor->OnPlace();
		GrabbedActor->SetIsGrabbed(false);
	}
	PhysicsHandle->ReleaseComponent();
	YawWhenGrabbed = 0.0f;
	RollWhenSnapped = 0.0f;
}

void AMainCharacter::TryDuplicate()
{
	Duplicate();
}

void AMainCharacter::Duplicate()
{
	if (PostProcess->bEnabled || PhysicsHandle == nullptr || PhysicsHandle->GrabbedComponent) return;

	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetComponentRotation().Vector() * ArmLength;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility
	);

	AActor* Interactable = HitResult.GetActor();
	IInteractableInterface* TargetObject = Cast<IInteractableInterface>(Interactable);
	if (TargetObject && TargetObject->IsDuplicatable())
	{
		Length = DuplicateLength;
		FActorSpawnParameters SpawnParams;
		if (Interactable->GetOwner())
		{
			SpawnParams.Owner = Interactable->GetOwner();
		}
		else
		{
			SpawnParams.Owner = Interactable;
		}
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
			Interactable->GetClass(), 
			Start + Camera->GetComponentRotation().Vector() * DuplicateLength, 
			Interactable->GetActorRotation(), 
			SpawnParams
			);
		IInteractableInterface* SpawnedObject = Cast<IInteractableInterface>(SpawnedActor);
		if (SpawnedActor && SpawnedObject)
		{
			SpawnedObject->OnPreview();
			UPrimitiveComponent* TargetComponent = Cast<UPrimitiveComponent>(SpawnedActor->GetRootComponent());
			if (TargetComponent)
			{
				Grab(TargetComponent);
				SpawnedObject->SetCollisionResponse(ECollisionResponse::ECR_Overlap);
				InteractableObjects.Emplace(SpawnedObject);
			}
		}
	}
}

void AMainCharacter::TryDelete()
{
	Delete();
}

void AMainCharacter::Delete()
{
	if (PostProcess->bEnabled || PhysicsHandle == nullptr || PhysicsHandle->GrabbedComponent) return;

	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetComponentRotation().Vector() * ArmLength;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility
	);

	AActor* Interactable = HitResult.GetActor();
	IInteractableInterface* TargetObject = Cast<IInteractableInterface>(Interactable);
	if (TargetObject && Interactable->GetOwner())
	{
		if (TargetObject)
		{
			InteractableObjects.Remove(TargetObject);
			TargetObject->OnDisappear();
		}
	}
}

void AMainCharacter::TryClear()
{
	Clear();
}

void AMainCharacter::Clear()
{
	if (PostProcess->bEnabled || PhysicsHandle == nullptr || PhysicsHandle->GrabbedComponent) return;

	FHitResult HitResult;
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetComponentRotation().Vector() * ArmLength;
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility
	);

	AActor* Interactable = HitResult.GetActor();
	if (Interactable && Interactable->Implements<UInteractableInterface>())
	{
		TArray<AActor*> TargetActors;
		UGameplayStatics::GetAllActorsOfClass(this, Interactable->GetClass(), TargetActors);
		for (AActor* TargetActor : TargetActors)
		{
			if (TargetActor && TargetActor->GetOwner())
			{
				IInteractableInterface* TargetObject = Cast<IInteractableInterface>(TargetActor);
				if (TargetObject)
				{
					InteractableObjects.Remove(TargetObject);
					TargetObject->OnDisappear();
				}
			}
		}
	}
}

void AMainCharacter::Examine()
{
	if (PostProcess == nullptr || PhysicsHandle == nullptr || PhysicsHandle->GrabbedComponent) return;

	PostProcess->bEnabled = !PostProcess->bEnabled;
	PostProcess->bEnabled = PostProcess->bEnabled;
	if (PostProcess->bEnabled)
	{
		for (IInteractableInterface* InteractableObject : InteractableObjects)
		{
			if (InteractableObject && InteractableObject->IsDuplicatable())
			{
				UStaticMeshComponent* TargetMesh = InteractableObject->GetMesh();
				if (TargetMesh)
				{
					TargetMesh->SetScalarParameterValueOnMaterials(FName("EmissiveMultiply"), 10.f);
				}
			}
		}
	}
	else
	{
		for (IInteractableInterface* InteractableObject : InteractableObjects)
		{
			if (InteractableObject)
			{
				UStaticMeshComponent* TargetMesh = InteractableObject->GetMesh();
				if (TargetMesh)
				{
					TargetMesh->SetScalarParameterValueOnMaterials(FName("EmissiveMultiply"), 0.f);
				}
			}
		}
	}
}

void AMainCharacter::SnapLeft()
{
	if (Camera == nullptr || PhysicsHandle == nullptr || PhysicsHandle->GrabbedComponent == nullptr) return;

	FRotator NewRotation = UKismetMathLibrary::ComposeRotators(PhysicsHandle->GrabbedComponent->GetComponentRotation(), FRotator(0, -90, 0));
	PhysicsHandle->SetTargetRotation(NewRotation);
	YawWhenGrabbed = Camera->GetComponentRotation().Yaw - NewRotation.Yaw;
}

void AMainCharacter::SnapRight()
{
	if (Camera == nullptr || PhysicsHandle == nullptr || PhysicsHandle->GrabbedComponent == nullptr) return;

	FRotator NewRotation = UKismetMathLibrary::ComposeRotators(PhysicsHandle->GrabbedComponent->GetComponentRotation(), FRotator(0, 90, 0));
	PhysicsHandle->SetTargetRotation(NewRotation);
	YawWhenGrabbed = Camera->GetComponentRotation().Yaw - NewRotation.Yaw;
}

void AMainCharacter::SnapUp()
{
	if (Camera == nullptr || PhysicsHandle == nullptr || PhysicsHandle->GrabbedComponent == nullptr) return;

	/*
	FRotator UpRotation = FRotator(-90.0f, Camera->GetComponentRotation().Yaw, 0.0f);
	FRotator RestoreRotation = FRotator(0.0f, -Camera->GetComponentRotation().Yaw, 0.0f);
	FRotator DeltaRotation = UKismetMathLibrary::ComposeRotators(RestoreRotation, UpRotation);
	FRotator NewRotation = UKismetMathLibrary::ComposeRotators(PhysicsHandle->GrabbedComponent->GetComponentRotation(), DeltaRotation);
	*/
	FRotator NewRotation = UKismetMathLibrary::ComposeRotators(FRotator(90, 0, 0), PhysicsHandle->GrabbedComponent->GetComponentRotation());
	PhysicsHandle->SetTargetRotation(NewRotation);
	YawWhenGrabbed = Camera->GetComponentRotation().Yaw - NewRotation.Yaw;
	RollWhenSnapped = NewRotation.Roll;
}

void AMainCharacter::SnapDown()
{
	if (Camera == nullptr || PhysicsHandle == nullptr || PhysicsHandle->GrabbedComponent == nullptr) return;
	
	/*
	FRotator DownRotation = FRotator(90.0f, Camera->GetComponentRotation().Yaw, 0.0f);
	FRotator RestoreRotation = FRotator(0.0f, -Camera->GetComponentRotation().Yaw, 0.0f);
	FRotator DeltaRotation = UKismetMathLibrary::ComposeRotators(RestoreRotation, DownRotation);
	FRotator NewRotation = UKismetMathLibrary::ComposeRotators(PhysicsHandle->GrabbedComponent->GetComponentRotation(), DeltaRotation);
	*/
	FRotator NewRotation = UKismetMathLibrary::ComposeRotators(FRotator(-90, 0, 0), PhysicsHandle->GrabbedComponent->GetComponentRotation());
	PhysicsHandle->SetTargetRotation(NewRotation);
	YawWhenGrabbed = Camera->GetComponentRotation().Yaw - NewRotation.Yaw;
	RollWhenSnapped = NewRotation.Roll;
}

void AMainCharacter::ClearAll()
{
	Release();

	for (IInteractableInterface* InteractableObject : InteractableObjects)
	{
		AActor* TargetObject = Cast<AActor>(InteractableObject);
		if (TargetObject && TargetObject->GetOwner())
		{
			TargetObject->Destroy();
		}
	}
}
