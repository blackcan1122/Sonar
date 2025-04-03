#include "Base/Core.h"
#include "Base/GameInstance.h"
#include "UI/Display.hpp"


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


	Vector2 CenterMap;
	Vector2 CenterPlayer;

private:

	std::vector<std::pair<std::weak_ptr<IObject>, ObjectType>> ObjectsToDraw;


END_CLASS