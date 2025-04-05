#include "Entities/Player.hpp"

void Player::Tick(float Deltatime)
{

}

void Player::Accel(float Amount)
{
	Position += Vector2Scale(FacingVector, Amount);
}
