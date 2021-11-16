// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreboardWidget.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

void UScoreboardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateEntryButton->OnClicked.AddDynamic(this, &UScoreboardWidget::CreateNewEntry);
	ReloadListButton->OnClicked.AddDynamic(this, &UScoreboardWidget::ReloadEntryList);

	ReloadEntryList();
}

void UScoreboardWidget::ReloadEntryList()
{
	EntryList->ClearChildren();

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("GET");
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetURL(TEXT("https://localhost:7156/scores"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UScoreboardWidget::OnReloadCompleted);
	HttpRequest->ProcessRequest();
}

void UScoreboardWidget::CreateNewEntry()
{
	FScoreboardEntryPOST ScoreboardEntry;

	if (PlayerNameTextBox->GetText().IsEmptyOrWhitespace() || PlayerScoreTextBox->GetText().IsEmptyOrWhitespace() || !PlayerScoreTextBox->GetText().IsNumeric()) return;
	
	ScoreboardEntry.Name = PlayerNameTextBox->GetText().ToString();
	ScoreboardEntry.Score = FCString::Atoi(*PlayerScoreTextBox->GetText().ToString());

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", "application/json");

	FString JsonString;
	
	FJsonObjectConverter::UStructToJsonObjectString(ScoreboardEntry, JsonString);

	UE_LOG(LogTemp, Warning, TEXT("%s"), *JsonString);

	HttpRequest->SetContentAsString(JsonString);
	HttpRequest->SetURL(TEXT("https://localhost:7156/scores"));
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UScoreboardWidget::OnCreateEntryCompleted);
	HttpRequest->ProcessRequest();
}

void UScoreboardWidget::OnReloadCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	TArray<FScoreboardEntryGET> data;
	FJsonObjectConverter::JsonArrayStringToUStruct(Response->GetContentAsString(), &data);

	if (data.Num())
	{
		data.Sort([](const FScoreboardEntryGET& One, const FScoreboardEntryGET& Two)
		{
			return One.Score > Two.Score;
		});
		
		for (FScoreboardEntryGET Entry : data)
		{
			// Spawn ListEntryWidget
			UListEntryWidget* LEntry = CreateWidget<UListEntryWidget>(GetWorld(), ListEntryWidget);
			if (!ensure(LEntry != nullptr)) return;

			LEntry->PlayerName->SetText(FText::FromString(Entry.Name));
			LEntry->PlayerScore->SetText(FText::FromString(FString::FromInt(Entry.Score)));
			
			EntryList->AddChild(LEntry);
		}
	}
}

void UScoreboardWidget::OnCreateEntryCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("%d"), bWasSuccessful);
	ReloadEntryList();
}

UScoreboardWidget::UScoreboardWidget(const FObjectInitializer& Initializer) : UUserWidget(Initializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> EntryWidgetBPClass(TEXT("/Game/Widgets/BW_ListEntry"));
	if (!ensure(EntryWidgetBPClass.Class != nullptr)) return;

	ListEntryWidget = EntryWidgetBPClass.Class;
}
