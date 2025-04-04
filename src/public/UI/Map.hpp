#include "Base/Core.h"
#include "Base/GameInstance.h"
#include "UI/Display.hpp"

class Player;

enum ObjectType
{
	Submarine,
	Ship
};

DECLARE_CLASS(Map, Display)

public:

	Map(Vector2 Pos);
	Map(int X, int Y);

	virtual void Tick(float DeltaTime) override;
	virtual void Draw() override;
	void Init();

	void AddObjectToDraw(std::weak_ptr<IObject> Object);

	Texture2D PlayerIcon;
	Texture2D ShipIcon;

	bool IsDragging = false;

	Vector2 LastMousePosition = { 0,0 };
	std::weak_ptr<Player> TrackedPlayer;


private:

	std::vector<std::pair<std::weak_ptr<IObject>, ObjectType>> ObjectsToDraw;
	float ZoomLevel = 1.f;

	Vector2 MapOffset = { 0, 0 };			// World-space offset
	Vector2 CameraWorldPosition = { 0, 0 };			// World Space Position of The Map

	Vector2 ConvertWorldToScreenPos(Vector2 VectorToConver) const;



END_CLASS