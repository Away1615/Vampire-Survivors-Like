#pragma once
#include "../../Foundation/GECollisible.h"
#include <string>

class GETile : public GECollisible {

public:
	GETile(const std::string& filename, GECollisionLayer collisionLayer)
		: GECollisible(filename, collisionLayer) {
	}

	void setCollisionSize(float width, float height) { setAabbCollider(width, height); }

	~GETile() = default;
};
