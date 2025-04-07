#include "Entities/Player.hpp"

void Player::Tick(float Deltatime)
{

}

void Player::Accel(float Amount)
{
	SetEntityLocation(Vector2Add(GetEntityLocation(), Vector2Scale(GetFacingVector(), Amount)));
}
