#pragma once

#include "Base/Core.h"
#include "Rendering/RenderBufferArrayLine.hpp"
#include "Base/Texture2DWrap.hpp"
#include "UI/Display.hpp"
#include "Events/AllPurposeEvent.h"
#include "Events/MapClickEventData.hpp"

constexpr float WORLD_HALF_WIDTH = 20037508.34f;
constexpr float WORLD_HALF_HEIGHT = 10018754.17f;
constexpr float WORLD_WIDTH = WORLD_HALF_WIDTH * 2.0f;
constexpr float WORLD_HEIGHT = WORLD_HALF_HEIGHT * 2.0f;

class ContextMenu;
class Entity;
class EventDispatcher;
struct TextureResource;
class SubmarinePlayer;


// TODO: Move this enums somewhere more fitting and correct
enum class ObjectType
{
	Submarine,
	Ship
};

enum class ObjectState
{
	EPlayer,
	EFriendly,
	EEnemy,
	ENeutral,
	EUnknown
};

enum class InteractionState
{
	None,
	Hovered,
	Active
};

// World Units: 1 Unit 1 Meter
// TODO: we need to refactor this here a lot since we allocate a lot of memory
// and most of the stuff here could be somehow created and managed over the factory
class Map : public Display
{
	AUTOBODY(Map, Display)

public:

	Map(std::string Name, Sonar::Vector2 Pos);
	Map(int X, int Y);

	virtual ~Map() override;

	virtual void Tick(float DeltaTime) override;
	virtual void Initialize() override;
	virtual void Draw() override;
	void Init();

	void AddObjectToDraw(std::weak_ptr<IObject> inObject);

	std::shared_ptr<EventDispatcher> MapEventDispatcher;



	bool IsDragging = false;

	Sonar::Vector2 LastMousePosition = { 0,0 };

protected:
	virtual void OnKeyStroke(Sonar::KeyboardKey Key, Sonar::Vector2 MousePos) override;
	virtual void OnMouseButtonPressed(Sonar::MouseButton Key, Sonar::Vector2 MousePos) override;



private:
	static constexpr long double MinMapZoom = 0.00005L;
	static constexpr long double MaxMapZoom = 10.0L;

	Sonar::Matrix GetViewProjectionMatrix() const;
	Sonar::Matrix GetOpenGLProjectionMatrix() const;
	long double GetMinimumZoomLevel() const;
	void ClampZoomToWorldBounds();
	void ClampCameraToWorldBounds();

	Sonar::Vector2 MarkedPos = { 0,0 };

	void LoadRessources();

	std::vector<RenderBufferArrayLine> Continents;
	Sonar::Shader shader;
	int locMVP;
	int locWorldOffsetX;
	Sonar::Matrix mvp = { 1,0,0,0,
				   0,1,0,0,
				   0,0,1,0,
				   0,0,0,1};

	SharedTexture2D PlayerIcon;
	SharedTexture2D ShipIcon;

	// Texture Ressources
	TextureResource* MapBorder = nullptr;
	SharedTexture2D BorderTexture;

	Sonar::Rectangle BorderRect;

	Sonar::Color ColorLookupState[5] = {{0,255,255,255} , Sonar::GREEN, Sonar::RED, Sonar::BLUE, Sonar::GRAY};
	Sonar::Color ColorLookupInteractivity[3] = { Sonar::BLANK, Sonar::YELLOW, Sonar::PURPLE};


	std::vector<std::pair<std::weak_ptr<Entity>, std::pair<ObjectType, ObjectState>>> ObjectsToDraw;
	std::weak_ptr<SubmarinePlayer> TrackedPlayer;

	std::vector<size_t> IndicesPendingKill;
	long double ZoomLevel = 1.f;

	Sonar::Vector2 MapOffset = { 0, 0 };			// World-space offset
	Sonar::Vector2 CameraWorldPosition = { 0, 0 };			// World Space Position of The Map

	std::weak_ptr<IObject> FocusedUnit;
	std::weak_ptr<IObject> HoveredUnit;

	Sonar::Vector2 ConvertWorldToScreenPos(Sonar::Vector2 VectorToConver) const;
	Sonar::Vector2 ConvertScreenPosToWorld(Sonar::Vector2 VectorToConver) const;

	inline Sonar::Vector2 ConvertTextureSizeToWorldSize(TextureResource* UsedTexture, Sonar::Vector2 SizeInMeters);

	// Like wtf think of a better name haha
	Sonar::Vector2 ConvertMouseScreenPosToMapScreenPos(Sonar::Vector2 MouseAbsolutePos);

	std::shared_ptr<AllPurposeEvent> MapClickEvent;
	std::shared_ptr<MapClickEventData> ClickDataPayload;

	SoftObjectPath<ContextMenu> RightClickMenu;
	SoftObjectPath<ContextMenu> SpeedMenu;

};