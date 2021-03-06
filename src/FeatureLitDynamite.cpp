#include "FeatureLitDynamite.h"

#include "Engine.h"
#include "Explosion.h"
#include "Map.h"
#include "Fov.h"

void LitDynamite::newTurn() {
  turnsLeftToExplosion_--;
  if(turnsLeftToExplosion_ <= 0) {
    eng->explosionMaker->runExplosion(pos_);
    eng->gameTime->eraseFeatureMob(this, true);
  }
}

void LitFlare::newTurn() {
  life_--;
  if(life_ <= 0) {
    eng->gameTime->eraseFeatureMob(this, true);
  }
}

LitFlare::LitFlare(Feature_t id, coord pos, Engine* engine, DynamiteSpawnData* spawnData) :
  FeatureMob(id, pos, engine), life_(spawnData->turnsLeftToExplosion_) {
}

void LitFlare::addLight(bool light[MAP_X_CELLS][MAP_Y_CELLS]) const {
  bool myLight[MAP_X_CELLS][MAP_Y_CELLS];
  eng->basicUtils->resetBoolArray(myLight, false);
  const int RADI = FOV_STANDARD_RADI_INT; //getLightRadius();
  coord x0y0(max(0, pos_.x - RADI), max(0, pos_.y - RADI));
  coord x1y1(min(MAP_X_CELLS - 1, pos_.x + RADI), min(MAP_Y_CELLS - 1, pos_.y + RADI));
  bool visionBlockers[MAP_X_CELLS][MAP_Y_CELLS];
  for(int y = x0y0.y; y <= x1y1.y; y++) {
    for(int x = x0y0.x; x <= x1y1.x; x++) {
      visionBlockers[x][y] = !eng->map->featuresStatic[x][y]->isVisionPassable();
    }
  }

  eng->fov->runFovOnArray(visionBlockers, pos_, myLight, false);
  for(int y = x0y0.y; y <= x1y1.y; y++) {
    for(int x = x0y0.x; x <= x1y1.x; x++) {
      if(myLight[x][y]) {
        light[x][y] = true;
      }
    }
  }
}



