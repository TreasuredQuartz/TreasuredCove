#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"

class IPlanetEditorModule;

class IPlanetEditor :
	public FAssetEditorToolkit
{
public:
	/** Delegate to be called when the tabs are being registered **/
	DECLARE_EVENT_OneParam(IPlanetEditorModule, FRegisterTabSpawnersEvent, const TSharedRef<class FTabManager>&);
	virtual FRegisterTabSpawnersEvent& OnRegisterTabSpawners() { return RegisterTabSpawnersEvent; };

	/** Delegate to be called when the tabs are being unregistered **/
	DECLARE_EVENT_OneParam(IPlanetEditorModule, FUnregisterTabSpawnersEvent, const TSharedRef<class FTabManager>&);
	virtual FUnregisterTabSpawnersEvent& OnUnregisterTabSpawners() { return UnregisterTabSpawnersEvent; };

	/** Delegate to be called when this IMaterialEditor is about to be destroyed **/
	DECLARE_EVENT(IPlanetEditorModule, FPlanetEditorClosedEvent);
	virtual FPlanetEditorClosedEvent& OnPlanetEditorClosed() { return PlanetEditorClosedEvent; };

	/** Ripped from Material editor...
	 * Gets the name of the material or material function that we are editing
	 */
	virtual FText GetOriginalObjectName() const { return FText::GetEmpty(); }
private:
	FPlanetEditorClosedEvent PlanetEditorClosedEvent;
	FRegisterTabSpawnersEvent RegisterTabSpawnersEvent;
	FUnregisterTabSpawnersEvent UnregisterTabSpawnersEvent;
};

