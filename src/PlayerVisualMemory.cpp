#include "PlayerVisualMemory.h"

#include "Engine.h"
#include "Map.h"

void PlayerVisualMemory::updateVisualMemory() {
	if(eng->config->USE_TILE_SET) {
		for(int x = 0; x < MAP_X_CELLS; x++) {
			for(int y = 0; y < MAP_Y_CELLS; y++) {
				eng->map->playerVisualMemoryTiles[x][y] = eng->renderer->renderArrayActorsOmittedTiles[x][y];
			}
		}
	} else {
		for(int x = 0; x < MAP_X_CELLS; x++) {
			for(int y = 0; y < MAP_Y_CELLS; y++) {
				eng->map->playerVisualMemory[x][y] = eng->renderer->renderArrayActorsOmitted[x][y];
			}
		}
	}

}

