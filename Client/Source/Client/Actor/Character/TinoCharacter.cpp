#include "Actor/Character/TinoCharacter.h"
#include "Actor/Controller/TinoController.h"
#include "Actor/Character/CharacterAnimInstance.h"
#include "Actor/Accessory/AccessoryItem.h"

#include "MenuUI/DialogUIWidget.h"
#include "MenuUI/InGameUIWidget.h"
#include "MenuUI/InGameTimerWidget.h"
#include "MenuUI/LoginUIWidget.h"
#include "MenuUI/CreateAccountsWidget.h"
#include "MenuUI/LobbyUIWidget.h"
#include "MenuUI/StoreUIWidget.h"
#include "MenuUI/InventoryUIWidget.h"
#include "MenuUI/NoticeUIWidget.h"
#include "MenuUI/CouponDialogWidget.h"

#include "Component/InventoryComponent.h"
#include "ClientGameMode.h"
#include "Data/ItemData.h"
#include "Global.h"

#include "Network/Network.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimMontage.h"



ATinoCharacter::ATinoCharacter()
	:MaxDiveTime(3.f),
	MaxTumbledTime(1.0f),
	MaxGrabTime(3.0f),
	GrabCoolTime(1.0f),
	GrabbedSpeed(100.f),
	GrabbedRotationSpeed(FRotator(0.f, 0.f, 108.f)),
	DetectDist(100.f),
	DetectRadius(50.f),
	DetectAngle(60.f),
	TargetInterval(50.f),
	OriginalSpeed(400.f),
	InterTime(0.03f),
	StopTime(0.3f),
	MovementState(EMovementState::EMS_Normal),
	Department(EDepartment::EDepartment_Game),
	Target(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
	UHelpers::CreateComponent<USpringArmComponent>(this, &SpringArm, "SpringArm", GetCapsuleComponent());
	UHelpers::CreateComponent<UCameraComponent>(this, &Camera, "Camera", SpringArm);
	UHelpers::CreateActorComponent<UInventoryComponent>(this, &InventoryComponent, "Inventory");

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = true;

	OriginalRotationSpeed = GetCharacterMovement()->RotationRate;
}

void ATinoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATinoCharacter::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATinoCharacter::OnMoveRight);
	PlayerInputComponent->BindAxis("HorizonLock", this, &ATinoCharacter::OnHorizonLock);
	PlayerInputComponent->BindAxis("VerticalLock", this, &ATinoCharacter::OnVerticalLock);

	PlayerInputComponent->BindAction("CreateDummy", EInputEvent::IE_Pressed, this, &ATinoCharacter::CreateDummy);
	PlayerInputComponent->BindAction("Align", EInputEvent::IE_Pressed, this, &ATinoCharacter::Align);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ATinoCharacter::Jump);
	PlayerInputComponent->BindAction("Grab", EInputEvent::IE_Pressed, this, &ATinoCharacter::OnGrab);
	PlayerInputComponent->BindAction("Grab", EInputEvent::IE_Released, this, &ATinoCharacter::OffGrab);
	//PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ATinoCharacter::StopJumping);
	PlayerInputComponent->BindAction("Dive", EInputEvent::IE_Pressed, this, &ATinoCharacter::Dive);
}

// Called when the game starts or when spawned
void ATinoCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!!GetController())
	{
		if (GetController()->IsPlayerController())
		{
			PlayerController = GetController<ATinoController>();
			// UI 위젯 생성
			//SetLoginUIInstance();
			//SetCreateAccountsInstance();
			//카메라 각도 제한(마우스 Y축 아래로 제한)
			UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->ViewPitchMax = 0.f;
		}
		else
		{
			GetCharacterMovement()->BrakingDecelerationWalking = 0;
			GetCharacterMovement()->BrakingFrictionFactor = 0;
			GetCharacterMovement()->GravityScale = 0.0;
		}
	}

}

void ATinoCharacter::EndPlay(EEndPlayReason::Type Reason)
{
	if (bIsControlledPlayer)
	{
		Network::GetNetwork()->release();
	}

}

// Called every frame
void ATinoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();

		float PitchClamp = FMath::ClampAngle(Rotation.Pitch, -45.f, 30.f);
		FRotator RotationControl(PitchClamp, Rotation.Yaw, Rotation.Roll);
		SleepEx(0, true);

		if (MovementState == EMovementState::EMS_Grabbing && Target != nullptr)
		{
			FVector TargetLocation = Target->GetActorLocation();

			if (bIsControlledPlayer == true)
			{
				if (CurrentInterTime >= InterTime)
				{
					SetActorLocation(TargetLocation - GetActorForwardVector() * TargetInterval);
				}
			}
			else
				SetActorLocation(TargetLocation - GetActorForwardVector() * TargetInterval);
		}

		if (Network::GetNetwork()->bIsConnected)
		{
			if (GetController())
			{
				if (!GetController()->IsPlayerController())
				{
					PlayerInterpolation(DeltaTime);
					//서버랑 연결 돼 있을 때만 상대 캐릭터 보간하려
					//Update GroundSpeedd (22-04-05)
					//GroundSpeedd = ServerStoreGroundSpeed;
					//Update Interpolation (23-09-27)
					//GetCharacterMovement()->Velocity = ServerCharMovingSpeed;
					//SetActorLocation(FMath::Lerp(GetActorLocation(), ServerLocateLerp, 0.5));
					//SetActorRotation(FMath::Lerp(GetTransform().GetRotation(), ServerRotateLerp, 0.5));
				}
				else {
					if (Network::GetNetwork()->bGameIsStart)
					{
						auto pos = GetTransform().GetLocation();
						auto rot = GetTransform().GetRotation();
						ServerSyncElapsedTime += DeltaTime;
						if (ServerSyncDeltaTime < ServerSyncElapsedTime)
						{
							if (!bIsSpactateModeEnabled)
								send_move_packet(Network::GetNetwork()->s_socket, Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->bIsAir, pos.X, pos.Y, pos.Z, rot, GetVelocity().Size2D(), GetCharacterMovement()->Velocity);
							ServerSyncElapsedTime = 0.0f;
						}

						float CharXYVelocity = ((ACharacter::GetCharacterMovement()->Velocity) * FVector(1.f, 1.f, 0.f)).Size();
					}
				}
			}
		}

		PlayTumbleMontage(DeltaTime);

		// 10/04 가만히 있을 때 충돌하지 안흔 부분을 해결하기 위한 코드 추가
		FHitResult OutHit;
		GetCharacterMovement()->SafeMoveUpdatedComponent(FVector(0.f, 0.f, 0.01f), GetActorRotation(), true, OutHit);
		GetCharacterMovement()->SafeMoveUpdatedComponent(FVector(0.f, 0.f, -0.01f), GetActorRotation(), true, OutHit);

		if (bIsSpactateModeEnabled)
		{
			if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Flying)
			{
				GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Flying;
			}
		}
		//이전 프레임 속도를 업데이트해줌(다음프레임기준 이전프레임 속도)
		PreviousVelocity = GetVelocity();
	}
}
bool ATinoCharacter::CanTumble(float DeltaTime)
{
	bool ret = true;

	ret &= GetCharacterMovement()->IsFalling();
	//ret &= (GetVelocity().Z != FMath::IsNearlyZero());

	if (ret && MaxTumbledTime > CurrentTumbledTime) CurrentTumbledTime += DeltaTime;
	bCanTumbled = (CurrentTumbledTime >= MaxTumbledTime);

	return ret;
}


void ATinoCharacter::PlayTumbleMontage(float DeltaTime)
{
	CanTumble(DeltaTime);

	if (bCanTumbled && !GetCharacterMovement()->IsFalling())
	{
		if (TumbleMontage)
		{
			CurrentTumbledTime = 0.f;
			bCanTumbled = false;
			SendAnimPacket(3);
			PlayAnimMontage(TumbleMontage);
			//SetMovementState(EMovementState::EMS_Tumbled);
		}
		else
			CLog::Log("Asset TumbleMontage is Invalid");
	}
	else if (!bCanTumbled && !GetCharacterMovement()->IsFalling())
	{
		CurrentTumbledTime = 0.f;
	}
}

void ATinoCharacter::Align()
{
	GetController()->SetControlRotation(GetActorForwardVector().Rotation());
}

void ATinoCharacter::PlayerInterpolation(float DeltaTime)
{
	if (MovementState == EMovementState::EMS_Grabbing)
		return;
	CurrentInterTime += DeltaTime;

	//플레이어 정지시간 측정
	if (FMath::Floor((PreviousVelocity.Length() - GetVelocity().Length())) == 0)
		CurrentStopTime += DeltaTime;

	//기준 시간 초과시 플레이어는 안움직이는 상태이다
	//11.15 지금 보간을 velocity로 해주는게 아니고 SetLocation으로 하는거라 무조건 StopTime에 들어옴.
	if (CurrentStopTime >= StopTime)
	{
		CurrentStopTime -= StopTime;
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		PreviousVelocity = FVector::ZeroVector;
	}

	//네트워크 한프레임 차이에서 발생하는 위치 차이를 기준으로
	//이동방향을 재설정해준다.
	AddMovementInput(GetVelocity());

	//InterTime마다 보간속도를구함(이전 프레임 위치 - 현재 위치) 
	if (CurrentInterTime >= InterTime)
	{
		CurrentInterTime -= InterTime;
		InterVelocity = (PreviousLocation - GetActorLocation()) / InterTime;
	}

	// 보간 주기만 큼 나눠주면 속도가된다
	if (InterVelocity.IsNearlyZero() == false && InterVelocity.Length() <= GetCharacterMovement()->MaxWalkSpeed)
	{
		SetActorLocation(GetActorLocation() + InterVelocity * DeltaTime);
	}
}

void ATinoCharacter::SetNetworkLocation(const FVector& Location)
{
	PreviousLocation = GetActorLocation();
	SetActorLocation(Location);
}

void ATinoCharacter::MakeAndShowHUD()
{
	auto InGameWidgetInstance = GetController<ATinoController>()->InGameUIInstance;
	auto InGameUITimerInstance = GetController<ATinoController>()->InGameUITimerInstance;


	InGameWidgetInstance->AddToViewport();

	InGameUITimerInstance->SetInGameUI(InGameWidgetInstance);
	InGameUITimerInstance->AddToViewport();

	InGameWidgetInstance->InGameTimer = InGameUITimerInstance;

}

void ATinoCharacter::MakeAndShowLoginOK(const double GradeValue)
{
	float GValue = StaticCast<float>(GradeValue);
	auto TinoController = GetController<ATinoController>();

	if (!!TinoController)
	{
		SetGrade(GValue);

		TinoController->ChangeMenuWidget(TinoController->LobbyUIInstance);
		auto LobbyUI = TinoController->LobbyUIInstance;
		LobbyUI->Grade = GValue;

		auto StoreUI = TinoController->StoreUIInstance;
		StoreUI->Grade = GValue;

		auto InventoryUI = TinoController->InventoryUIInstance;
		InventoryUI->Grade = GValue;
	}
}

void ATinoCharacter::MakeAndShowLoginFail()
{
	auto LoginUIWidget = GetController<ATinoController>()->LoginUIInstance;
	LoginUIWidget->UIAlertMessage();
}

void ATinoCharacter::MakeAndShowCreateAccountsSignUpOK()
{
	auto CreateAccountWidget = GetController<ATinoController>()->CreateAccountsUIInstance;
	CreateAccountWidget->ShowCreateAccountResult(true);
}

void ATinoCharacter::MakeAndShowCreateAccountsSignUpFail()
{
	auto CreateAccountWidget = GetController<ATinoController>()->CreateAccountsUIInstance;
	CreateAccountWidget->ShowCreateAccountResult(false);
}

void ATinoCharacter::MakeAndShowLevelStartCountdown()
{
	auto InGameWidget = GetController<ATinoController>()->InGameUIInstance;
	InGameWidget->LevelStartCountdown();
}

void ATinoCharacter::MakeAndShowInGameLevelStart()
{
	auto InGameWidget = GetController<ATinoController>()->InGameUIInstance;
	InGameWidget->LevelStart();
	bDebugInterVelocity = true;
}

void ATinoCharacter::MakeAndShowInGameLevelClear()
{
	auto InGameWidget = GetController<ATinoController>()->InGameUIInstance;
	InGameWidget->LevelClear();
}

void ATinoCharacter::MakeAndShowInGameShowResult()
{
	auto InGameWidget = GetController<ATinoController>()->InGameUIInstance;
	InGameWidget->ShowResultUI();
}

void ATinoCharacter::MakeAndShowInGameClearCountdown()
{
	auto InGameWidget = GetController<ATinoController>()->InGameUIInstance;
	InGameWidget->LevelClearCountdown();
}

void ATinoCharacter::MakeAndShowDialogInLobby()
{
	auto DialogWidget = GetController<ATinoController>()->DialogUIInstance;
	DialogWidget->AddToViewport();
	DialogWidget->RenderDisconnectNetworkWindow();
}

void ATinoCharacter::MakeAndShowDialogInGame()
{
	if (true == Network::GetNetwork()->bGameEndFlag) return;
	auto DialogWidget = GetController<ATinoController>()->DialogUIInstance;
	DialogWidget->AddToViewport();
	DialogWidget->RenderDisconnectNetworkWindow();
}

void ATinoCharacter::MakeAndShowLobbyRankSystem(rankInfo rank[])
{
	auto TinoController = GetController<ATinoController>();
	if (!!TinoController)
	{
		auto LobbyUI = TinoController->LobbyUIInstance;
		LobbyUI->GetRankData(rank);
		LobbyUI->UpdataRankData();
	}
}

void ATinoCharacter::MakeAndShowChangePoint(int AfterPoint)
{
	auto TinoController = GetController<ATinoController>();

	if (!!TinoController)
	{
		auto StoreUI = TinoController->StoreUIInstance;
		StoreUI->ChangePoint(AfterPoint);
	}
}

void ATinoCharacter::MakeAndShowNotice(FString notice)
{
	auto TinoController = GetController<ATinoController>();
	if (!!TinoController)
	{
		auto NoticeUI = TinoController->NoticeUIInstance;
		NoticeUI->Notice = notice;
		NoticeUI->SetVisibleNotice();
	}
}

void ATinoCharacter::MakeAndShowCouponDialog(bool bCheckUsingCoupon)
{
	auto TinoController = GetController<ATinoController>();
	if (!!TinoController)
	{
		auto CouponDialog = TinoController->CouponDialogUIInstance;
		CouponDialog->AddToViewport();
		if (bCheckUsingCoupon == true)
		{

			CouponDialog->SuccessUsingCoupon();
		}
		else
		{
			CouponDialog->FailedUsingCoupon();
		}
	}
}

void ATinoCharacter::UpdateEquippedAccessoryUI()
{
	auto TinoController = GetController<ATinoController>();
	if (!!TinoController)
		TinoController->InventoryUIInstance->UpdateEquippedAccessory();
}

void ATinoCharacter::UpdateInventoryPointAndGrade()
{
	auto TinoController = GetController<ATinoController>();
	if (!!TinoController)
		TinoController->InventoryUIInstance->UpdateGradeAndPoint();
}

void ATinoCharacter::RemoveStoreDialog()
{
	auto TinoController = GetController<ATinoController>();
	if (!!TinoController)
		TinoController->RemoveDialogUI();
}

void ATinoCharacter::UpdataPointInLobby(int point)
{
	auto TinoController = GetController<ATinoController>();

	SetPoint(point);

	if (!!TinoController)
	{
		auto LobbyUI = TinoController->LobbyUIInstance;
		LobbyUI->Point = point;

		auto StoreUI = TinoController->StoreUIInstance;
		StoreUI->Point = point;

		auto InventoryUI = TinoController->InventoryUIInstance;
		InventoryUI->Point = point;
	}
}

TArray<FInventoryItem> ATinoCharacter::GetInventoryContents()
{
	return InventoryComponent->GetInventoryContents();
}

void ATinoCharacter::AddItemToInventory(const FItemData& Data)
{
	InventoryComponent->AddItem(Data);
}

FItemData ATinoCharacter::GetItemDataFromItemCode(const int64& ItemCode)
{
	auto GameMode = Cast<AClientGameMode>(GetWorld()->GetAuthGameMode());
	auto Data = GameMode->GetItemData(ItemCode);
	if (Data == nullptr) return FItemData();

	FItemData ItemData;
	ItemData.ItemCode = ItemCode;
	ItemData.EquipType = Data->EquipType;
	ItemData.TextData = Data->TextData;
	ItemData.NumericData = Data->NumericData;
	ItemData.AssetData = Data->AssetData;
	ItemData.SellValue = Data->SellValue;

	return ItemData;
}

void ATinoCharacter::SetInventoryFromInventoryFlag(const long long& EquippedItems)
{
	int64 IC = StaticCast<int64>(EquippedItems);
	InventoryComponent->ClearInventory();

	for (int64 i = 0; i < 64; ++i)
	{
		int Value = (EquippedItems >> i) & 1;

		if (Value != 0)
		{
			int64 ItemCode = i;
			InventoryComponent->AddItem(GetItemDataFromItemCode(ItemCode));
		}
	}

}

FItemData ATinoCharacter::GetShopItemDataFromItemCode(const int64& ItemCode)
{
	auto GameMode = Cast<AClientGameMode>(GetWorld()->GetAuthGameMode());
	auto Data = GameMode->GetShopProductData(ItemCode);
	if (Data == nullptr) return FItemData();

	FItemData ItemData;
	ItemData.ItemCode = ItemCode;
	ItemData.EquipType = Data->EquipType;
	ItemData.TextData = Data->TextData;
	ItemData.NumericData = Data->NumericData;
	ItemData.AssetData = Data->AssetData;
	ItemData.SellValue = Data->SellValue;

	return ItemData;
}

void ATinoCharacter::GetShopData(UPARAM(REF) TArray<int>& iOut)
{

	auto GameMode = Cast<AClientGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->GetShopProductTable(iOut);
}

void ATinoCharacter::SetDepartmentClothes(int department)
{
	EDepartment EnumValue = static_cast<EDepartment>(department);

	if (EDepartment::EDepartment_None < EnumValue && EDepartment::EDepartment_MAX > EnumValue)
	{
		//Staff 전용
		//if (EDepartment::EDepartment_Staff == EnumValue) WearAccessory(1);

		auto DynamicMaterialInstance = GetMesh()->CreateDynamicMaterialInstance(0);
		auto DepartmentTexture = GetTinoDepartTexture(static_cast<EDepartment>(department));
		if (DepartmentTexture == nullptr)
		{
			CLog::Log("Fail to Get Department Texture!");
			return;
		}
		DynamicMaterialInstance->SetTextureParameterValue(TEXT("Department"), DepartmentTexture);
		if (DynamicMaterialInstance)
			GetMesh()->SetMaterial(0, DynamicMaterialInstance);
	}
	else
	{
		CLog::Print("Department value is Invalid. Check Server", 1, 10.f, FColor::Red);
	}
}


bool ATinoCharacter::SendAnimPacket(int32 AnimType)
{
	if (!!GetController())
	{
		if (GetController()->IsPlayerController())
		{
			send_action_packet(Network::GetNetwork()->s_socket, static_cast<int>(AnimType));
			return true;
		}
	}
	else
	{
		CLog::Log("Controller is nullptr");
	}
	return false;

}

void ATinoCharacter::DiveBegin()
{
}

void ATinoCharacter::DiveEnd()
{
	bIsDiving = false;
}

void ATinoCharacter::OnAccelEffect()
{
	if (bIsControlledPlayer)
	{
		//비네트 값을 조절해 가속 이펙트를 킴
		Camera->PostProcessSettings.bOverride_VignetteIntensity = true;
		Camera->PostProcessSettings.VignetteIntensity = CustomVignetteIntensity;
		ASoundManager::GetSoundManager()->PlaySFX2D(ESFXType::ESFXType_ObstacleAccel);
	}
}

void ATinoCharacter::OffAccelEffect()
{
	Camera->PostProcessSettings.bOverride_VignetteIntensity = false;
}

void ATinoCharacter::SetOriginalSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
}

void ATinoCharacter::Dive()
{
	if (DiveMontage && CanDive())
	{
		bIsDiving = true;
		SendAnimPacket(2);
		PlayAnimMontage(DiveMontage);
		SetMovementState(EMovementState::EMS_Dive);
		//GetWorldTimerManager().SetTimer(DiveTimer,this,&ATinoCharacter::DiveEnd, MaxDiveTime, false);
	}
}

bool ATinoCharacter::GetIsAirForNetwork()
{
	auto AnimInstance = GetTinoAnimInstance();
	if (!!AnimInstance)
	{
		return AnimInstance->bIsAirForNetwork;
	}
	return false;
}

void ATinoCharacter::SetIsAirForNetwork(bool val)
{
	auto AnimInstance = GetTinoAnimInstance();
	if (!!AnimInstance)
	{
		AnimInstance->bIsAirForNetwork = val;
	}
}

void ATinoCharacter::SetCollisionProfileName(const FName& CollisionName)
{
	GetCapsuleComponent()->SetCollisionProfileName(CollisionName);
}

UCharacterAnimInstance* ATinoCharacter::GetTinoAnimInstance()
{
	return Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());
}

void ATinoCharacter::SetAccessoryFromEquippedFlag(const long long& EquippedItems)
{
	int64 IC = StaticCast<int64>(EquippedItems);
	InventoryComponent->ClearEquippedInfo();

	for (int64 i = 0; i < 64; ++i)
	{
		int Value = (EquippedItems >> i) & 1;

		if (Value != 0)
		{
			int64 ItemCode = i;
			//InventoryComponent->AddItem(GetItemDataFromItemCode(ItemCode),true);

			WearAccessory(ItemCode);
		}
	}
}

void ATinoCharacter::WearAllAccessory()
{
	//인벤토리 컴포넌트에서 아이템 정보를 가져와서 스폰한다. 
	for (auto item : GetInventoryContents())
	{
		auto Accessory = InventoryComponent->GetInstnace(item.ItemInfo.ItemCode);
		if (Accessory == nullptr)
		{
			AAccessoryItem::Spawn< AAccessoryItem>(GetWorld(), item.ItemInfo.AssetData.BPClass, this);
			Accessory->SetSocketNameWithItemCode(item.ItemInfo.ItemCode);
			InventoryComponent->SetInstnace(item.ItemInfo.ItemCode, Accessory);
		}
		Accessory->Equip();
	}
}

void ATinoCharacter::WearAccessory(const int ItemCode)
{
	//로비에서 한개씩 장착할때 사용하는 함수입니다.
	//또는 테스트용으로 사용가능합니다.
	FItemData Item = GetItemDataFromItemCode(ItemCode);
	auto Accessory = InventoryComponent->GetInstnace(ItemCode);
	if (Accessory == nullptr)
	{
		Accessory = AAccessoryItem::Spawn< AAccessoryItem>(GetWorld(), Item.AssetData.BPClass, this);
		Accessory->SetSocketNameWithItemCode(ItemCode);
		InventoryComponent->SetInstnace(ItemCode, Accessory);
	}
	InventoryComponent->SetEquipped(ItemCode, true);
	Accessory->Equip();
	//int idx = EquipAccessoryContainer.Add(Accessory);
	//EquipAccessoryContainer[idx]->SetItemCode(Item.ItemCode);
	//EquipAccessoryContainer[idx]->Equip();
}

void ATinoCharacter::UnWearAccessory(const int ItemCode)
{
	for (auto item : GetInventoryContents())
	{
		auto Accessory = InventoryComponent->GetInstnace(item.ItemInfo.ItemCode);
		if (Accessory)
			Accessory->UnEquip();
	}
	//auto Accessory = EquipAccessoryContainer.FindByPredicate([&ItemCode](const AAccessoryItem& i) { return i.GetItemCode() == ItemCode; });

	//Accessory->UnEquip();
	//EquipAccessoryContainer.Remove(*Accessory);
	//Accessory->Destroy();
}

void ATinoCharacter::OnMoveForward(float Axis)
{
	if (CanMove())
	{
		FRotator rotator = FRotator(0, GetControlRotation().Yaw, 0);
		FVector dir = FQuat(rotator).GetForwardVector().GetUnsafeNormal2D();

		AddMovementInput(dir, Axis);
	}

}

void ATinoCharacter::OnMoveRight(float Axis)
{
	if (CanMove())
	{
		FRotator rotator = FRotator(0, GetControlRotation().Yaw, 0);
		FVector dir = FQuat(rotator).GetRightVector().GetUnsafeNormal2D();

		AddMovementInput(dir, Axis);
	}

}

void ATinoCharacter::OnHorizonLock(float Axis)
{
	if (bIsControlledPlayer && PlayerController->bShowMouseCursor == true) return;
	AddControllerYawInput(Axis);
}

void ATinoCharacter::OnVerticalLock(float Axis)
{
	if (bIsControlledPlayer && PlayerController->bShowMouseCursor == true) return;
	AddControllerPitchInput(Axis);
}

void ATinoCharacter::CreateDummy()
{
	GetController<ATinoController>()->CreateDummy();
}

void ATinoCharacter::Jump()
{
	if (bIsControlledPlayer == false || CanJump())
	{
		Super::Jump();
		SendAnimPacket(1);
		ASoundManager::GetSoundManager()->PlaySFXAtLocation(this, ESFXType::ESFXType_Jump, GetActorLocation());
	}

}

void ATinoCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	DiveEnd();
	ASoundManager::GetSoundManager()->PlaySFXAtLocation(this, ESFXType::ESFXType_Land, GetActorLocation());
}

void ATinoCharacter::StopJumping()
{
	Super::StopJumping();
	SetMovementState(EMovementState::EMS_Normal);
}

void ATinoCharacter::PlayTumbleMontage()
{
	PlayAnimMontage(TumbleMontage);
}

void ATinoCharacter::OnGrab()
{
	if (GrabMontage == nullptr || (CanGrab() == false)) return;

	bIsGrabbing = !bIsGrabbing;

	SendAnimPacket(4);
	PlayAnimMontage(GrabMontage);
	//if (GetController() && GetController()->IsPlayerController())
		//CLog::Print("GrabOn");
}

void ATinoCharacter::OffGrab()
{
	SendAnimPacket(5);

	StopAnimMontage(GrabMontage);
	ASoundManager::GetSoundManager()->PlaySFXAtLocation(this, ESFXType::ESFXType_OffGrab, GetActorLocation());
	if (MovementState == EMovementState::EMS_Grabbing)
		SetMovementState(EMovementState::EMS_Normal);

	bIsGrabbing = false;

	//if (Target == nullptr) return;
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//SetTargetGrabbedToNormal();

	bIsGrabCoolTime = true;

	if (GetWorldTimerManager().IsTimerActive(GrabCoolTimer) == false)
	{	//그랩 쿨타임 시작
		GetWorldTimerManager().SetTimer(GrabCoolTimer, [this]()
			{
				bIsGrabCoolTime = false;
			}
		, GrabCoolTime, false);
	}

	//if (GetController() && GetController()->IsPlayerController())
		//CLog::Print("GrabOff");
}

void ATinoCharacter::SetTargetNormalToGrabbed()
{
	if (!!Target)
	{
		auto Other = Cast<ATinoCharacter>(Target);
		Other->SetMovementState(EMovementState::EMS_IsGrabbed);
		Other->GetCharacterMovement()->MaxWalkSpeed = GrabbedSpeed;
		Other->GetCharacterMovement()->RotationRate = GrabbedRotationSpeed;
	}
	else
		CLog::Log("Target is Nullptr");
}

void ATinoCharacter::SetTargetGrabbedToNormal()
{
	if (!!Target)
	{
		//if(GetController() && GetController()->IsPlayerController())
			//CLog::Print("Target Remove");
		auto Other = Cast<ATinoCharacter>(Target);
		Other->SetMovementState(EMovementState::EMS_Normal);
		Other->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
		Other->GetCharacterMovement()->RotationRate = OriginalRotationSpeed;
		Target = nullptr;
	}
	else
		CLog::Log("No targets to delete");
}

void ATinoCharacter::GrabBegin()
{
	//그랩 지속시간 시작
	//if (GetController() && GetController()->IsPlayerController())
	GetWorldTimerManager().SetTimer(GrabTimer, this, &ATinoCharacter::OffGrab, MaxGrabTime, false);
}


void ATinoCharacter::DetectTarget()
{
	FVector start = GetActorLocation();
	FVector end = start + GetActorForwardVector() * DetectDist;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	EObjectTypeQuery Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(Pawn);

	TArray<AActor*> IgnoreActors;
	//자기자신은 충돌검사 X
	IgnoreActors.Add(this);

	FHitResult HitResult;
	bool result = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(),
		start, end, DetectRadius,
		ObjectTypes,
		false,
		IgnoreActors,
		bShowDebugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult,
		true);

	if (result && bIsGrabCoolTime == false)
	{
		Target = Cast<ATinoCharacter>(HitResult.GetActor());
		//float ScalarValue = GetActorForwardVector().Dot(Target->GetActorForwardVector());

		// 이미 잡힌 캐릭터나 잡고있는 캐릭터는 잡기 대상이 될 수 없다(기차 방지)
		EMovementState TargetState = Cast<ATinoCharacter>(Target)->GetMovementState();
		switch (TargetState)
		{
		case EMovementState::EMS_Grabbing:
		case EMovementState::EMS_IsGrabbed:
			Target = nullptr;
			return;
		default:
			break;
		}

		FVector DirVec = Target->GetActorLocation() - GetActorLocation();
		double angle = FMath::Acos(static_cast<double>(GetActorForwardVector().Dot(DirVec.GetUnsafeNormal())));
		angle = FMath::RadiansToDegrees(angle);
		if (angle < DetectAngle / 2.f)
		{
			SetMovementState(EMovementState::EMS_Grabbing);
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SetTargetNormalToGrabbed();
			ASoundManager::GetSoundManager()->PlaySFXAtLocation(this, ESFXType::ESFXType_OnGrab, GetActorLocation());
		}
		else
		{
			CLog::Log("Target is bigger than DetectAngle, Go to Location In DectectAngle");
			Target = nullptr;
		}
		//앞뒤 판정
		/*if (ScalarValue > 0)
		{

		}
		else
		{
			CLog::Log("Target is front, Go to the Target's Back");
			Target = nullptr;
		}*/
	}
}

void ATinoCharacter::DisableInputMode()
{
	DisableInput(GetController<APlayerController>());
}

void ATinoCharacter::EnableInputMode()
{
	EnableInput(GetController<APlayerController>());
}

UTexture* ATinoCharacter::GetTinoDepartTexture(EDepartment DepartmentNumber)
{
	if (DepartmentTextureMap.IsEmpty())
	{
		CLog::Log("DepartmentTextureMap is Empty");
		return nullptr;
	}
	UTexture* Texture = DepartmentTextureMap[DepartmentNumber];

	return Texture;
}

bool ATinoCharacter::CanMove()
{
	bool ret = true;

	switch (MovementState)
	{
	case EMovementState::EMS_Normal:
	case EMovementState::EMS_Fall:
	case EMovementState::EMS_IsGrabbed:
		ret = true;
		break;
	default:
		ret = false;
	}
	return ret;
}

bool ATinoCharacter::CanDive()
{
	bool ret = false;

	switch (MovementState)
	{
	case EMovementState::EMS_Normal:
	case EMovementState::EMS_Fall:
		ret = true;
		break;
	default:
		ret = false;
		break;
	}
	ret &= (bIsDiving == false);
	ret &= (bIsSpactateModeEnabled == false);
	return ret;
}

bool ATinoCharacter::CanGrab()
{
	bool ret = false;

	if (bIsGrabCoolTime == true)
	{
		ret = false;
		return ret;
	}

	switch (MovementState)
	{
	case EMovementState::EMS_Normal:
	case EMovementState::EMS_Fall:
	case EMovementState::EMS_Grabbing:
		ret = true;
		break;
	default:
		ret = false;
		break;
	}

	ret &= (bIsSpactateModeEnabled == false);
	return ret;
}

bool ATinoCharacter::CanJump()
{
	bool ret = true;

	ret &= (CanMove() && GetCharacterMovement()->IsFalling() == false);
	ret &= (bIsSpactateModeEnabled == false);

	return ret;
}


