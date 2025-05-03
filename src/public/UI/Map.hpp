#include "Base/Core.h"
#include "Base/GameInstance.h"
#include "UI/Display.hpp"
#include "Events/AllPurposeEvent.h"
#include "Events/MapClickEventData.hpp"

class ContextMenu;
class Player;


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
class Map : public Display
{
	AUTOBODY(Map, Display)

public:

	Map(std::string Name, Vector2 Pos);
	Map(int X, int Y);

	virtual ~Map() override;

	virtual void Tick(float DeltaTime) override;
	virtual void Draw() override;
	void Init();

	void AddObjectToDraw(std::weak_ptr<IObject> Object);

	std::shared_ptr<EventDispatcher> MapEventDispatcher;



	bool IsDragging = false;

	Vector2 LastMousePosition = { 0,0 };

protected:
	virtual void OnKeyStroke(KeyboardKey Key, Vector2 MousePos) override;
	virtual void OnMouseButtonPressed(MouseButton Key, Vector2 MousePos) override;



private:

	Matrix GetViewProjectionMatrix() const;
	Matrix GetOpenGLProjectionMatrix() const;

	void LoadRessources();

	Shader shader;
	int locMVP;
	Matrix mvp = { 1,0,0,0,
				   0,1,0,0,
				   0,0,1,0,
				   0,0,0,1};

	unsigned int vaoAfrica, vboAfrica;
	unsigned int vaoEurope, vboEurope;
	unsigned int vaoNA, vboNA;
	unsigned int vaoSA, vboSA;
	unsigned int vaoAsia, vboAsia;
	unsigned int vaoOceania, vboOceania;
	unsigned int vaoAntarctica, vboAntarctica;
	unsigned int vaoSevenSeas, vboSevenSeas;

	const std::string PlayerIconPath = (GameInstance::GetInstance()->g_WorkingDirectory + "/resources/imgs/PlayerMap.png");
	const std::string ShipIconPath = (GameInstance::GetInstance()->g_WorkingDirectory + "/resources/imgs/ShipIcon.png");

	Texture2D PlayerIcon;
	Texture2D ShipIcon;

	// Texture Ressources
	TextureResource* MapBorder;
	SharedTexture2D BorderTexture;

	Rectangle BorderRect;

	Color ColorLookupState[5] = {{0,255,255,255} , GREEN, RED, BLUE, GRAY};
	Color ColorLookupInteractivity[3] = { BLANK, YELLOW, PURPLE};


	std::vector<std::pair<std::weak_ptr<Entity>, std::pair<ObjectType, ObjectState>>> ObjectsToDraw;
	std::shared_ptr<Player> TrackedPlayer = nullptr;

	std::vector<size_t> IndicesPendingKill;
	long double ZoomLevel = 1.f;

	Vector2 MapOffset = { 0, 0 };			// World-space offset
	Vector2 CameraWorldPosition = { 0, 0 };			// World Space Position of The Map

	std::weak_ptr<IObject> FocusedUnit;
	std::weak_ptr<IObject> HoveredUnit;

	Vector2 ConvertWorldToScreenPos(Vector2 VectorToConver) const;
	Vector2 ConvertScreenPosToWorld(Vector2 VectorToConver) const;

	inline Vector2 ConvertTextureSizeToWorldSize(TextureResource* UsedTexture, Vector2 SizeInMeters);

	void LoadBuffer(unsigned int& VAO, unsigned int& VBO, const std::vector<float>* PointArray) const;
	void RenderOpenGLBuffer(unsigned int& VAO, const std::vector<float>* PointArray, const std::vector<unsigned int>* Offset, const std::vector<unsigned int>* Counts) const;

	// Like wtf think of a better name haha
	Vector2 ConvertMouseScreenPosToMapScreenPos(Vector2 MouseAbsolutePos);

	std::shared_ptr<AllPurposeEvent> MapClickEvent;
	std::shared_ptr<MapClickEventData> ClickDataPayload;

	std::shared_ptr<ContextMenu> RightClickMenu;

};