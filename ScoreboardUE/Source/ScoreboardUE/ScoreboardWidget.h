// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ListEntryWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Interfaces/IHttpRequest.h"
#include "ScoreboardWidget.generated.h"

USTRUCT()
struct FScoreboardEntryGET
{
	GENERATED_BODY()

	UPROPERTY()
	int32 id;

	UPROPERTY()
	FString Name;

	UPROPERTY()
	int Score;
};

USTRUCT()
struct FScoreboardEntryPOST
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name;

	UPROPERTY()
	int Score;
};

/**
 * Main widget.
 * Responsible for:
 * - Button functionality.
 * - HTTP Requests (GET and POST).
 * - Updating the scores list.
 */
UCLASS()
class SCOREBOARDUE_API UScoreboardWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UScrollBox* EntryList;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* PlayerNameTextBox;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* PlayerScoreTextBox;

	UPROPERTY(meta = (BindWidget))
	UButton* CreateEntryButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ReloadListButton;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void ReloadEntryList();

	UFUNCTION()
	void CreateNewEntry();
	
	void OnReloadCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnCreateEntryCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	TSubclassOf<UListEntryWidget> ListEntryWidget;

public:
	UScoreboardWidget(const FObjectInitializer& Initializer);
};
