#include "Base/Core.h"
#include "Base/GameInstance.h"
#include "UI/Display.hpp"

class Player;


// TODO: Move this enums somewhere more fitting and correct
enum ObjectType
{
	Submarine,
	Ship
};

enum ObjectState
{
	Friendly,
	Enemy,
	Neutral,
	Unknown
};



DECLARE_CLASS(Map, Display)

public:

	Map(std::string Name, Vector2 Pos);
	Map(int X, int Y);

	virtual void Tick(float DeltaTime) override;
	virtual void Draw() override;
	void Init();

	void AddObjectToDraw(std::weak_ptr<IObject> Object);



	bool IsDragging = false;

	Vector2 LastMousePosition = { 0,0 };
	std::weak_ptr<Player> TrackedPlayer;


private:

	Matrix GetViewProjectionMatrix() const;

	void LoadRessources();

	const std::string PlayerIconPath = (GameInstance::GetInstance()->WorkingDirectory + "/resources/imgs/PlayerMap.png");
	const std::string ShipIconPath = (GameInstance::GetInstance()->WorkingDirectory + "/resources/imgs/ShipIcon.png");

	Texture2D PlayerIcon;
	Texture2D ShipIcon;

	Color ColorLookup[4] = { GREEN, RED, YELLOW, GRAY };

	std::vector<std::pair<std::weak_ptr<IObject>, std::pair<ObjectType, ObjectState>>> ObjectsToDraw;
	std::vector<size_t> IndicesPendingKill;
	float ZoomLevel = 1.f;

	Vector2 MapOffset = { 0, 0 };			// World-space offset
	Vector2 CameraWorldPosition = { 0, 0 };			// World Space Position of The Map

	Vector2 ConvertWorldToScreenPos(Vector2 VectorToConver) const;



END_CLASS