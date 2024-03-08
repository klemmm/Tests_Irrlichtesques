#include "ship.h"

void Ship::process(float delta) {
    setPosition(getPosition() + _velocity * delta);
}
