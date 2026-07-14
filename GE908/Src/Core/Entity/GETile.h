#pragma once
#include "../../Foundation/GECollisible.h"

class GETile : public GECollisible {

public:
	GETile(const Image& texture, GECollisionLayer collisionLayer)
		: GECollisible(texture, collisionLayer) {
	}

	void setCollisionSize(float width, float height) { setAabbCollider(width, height); }

	~GETile() = default;
};
