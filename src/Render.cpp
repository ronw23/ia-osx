#include "Render.h"

#include <vector>
#include <iostream>

#include "SFML/Graphics/Rect.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Window/WindowStyle.hpp"

#include "Engine.h"
#include "Item.h"
#include "Interface.h"
#include "Marker.h"
#include "Map.h"
#include "ActorPlayer.h"
#include "ActorMonster.h"
#include "Log.h"
#include "Attack.h"
#include "FeatureWall.h"
#include "FeatureDoor.h"

using namespace std;

Renderer::Renderer(Engine* engine) : eng(engine), renderWindow_(NULL),
  textureFontSheet_(NULL), textureTileSheet_(NULL), textureMainMenuLogo_(NULL) {

  setupWindowAndImagesClearPrev();
}

Renderer::~Renderer() {
  freeWindowAndImages();
}

void Renderer::freeWindowAndImages() {
  tracer << "Renderer::freeWindowAndImages()..." << endl;

  if(renderWindow_ != NULL) {
    delete renderWindow_;
    renderWindow_ = NULL;
  }

  if(textureFontSheet_ != NULL) {
    delete textureFontSheet_;
    textureFontSheet_ = NULL;
    for(int y = 0; y < FONT_SHEET_Y_CELLS; y++) {
      for(int x = 0; x < FONT_SHEET_X_CELLS; x++) {
        delete spritesFont_[x][y];
        spritesFont_[x][y] = NULL;
      }
    }
  }

  if(textureTileSheet_ != NULL) {
    delete textureTileSheet_;
    textureTileSheet_ = NULL;
    for(int y = 0; y < TILE_SHEET_Y_CELLS; y++) {
      for(int x = 0; x < TILE_SHEET_X_CELLS; x++) {
        delete spritesTiles_[x][y];
        spritesTiles_[x][y] = NULL;
      }
    }
  }

  if(textureMainMenuLogo_ != NULL) {
    delete textureMainMenuLogo_;
    delete spriteMainMenuLogo_;
    textureMainMenuLogo_ = NULL;
    spriteMainMenuLogo_ = NULL;
  }
  tracer << "Renderer::freeWindowAndImages() [DONE]" << endl;
}

void Renderer::setupWindowAndImagesClearPrev() {
  tracer << "Renderer::setupWindowAndImagesClearPrev()..." << endl;
  freeWindowAndImages();

  tracer << "Renderer: Setting up rendering window" << endl;
  const string title = "IA " + eng->config->GAME_VERSION;

  const int& SCR_W_INT = eng->config->SCREEN_WIDTH;
  const int& SCR_H_INT = eng->config->SCREEN_HEIGHT;
  const double& SCR_W_DB = static_cast<double>(SCR_W_INT);
  const double& SCR_H_DB = static_cast<double>(SCR_H_INT);
  const double& SCALE = eng->config->SCALE;
  const int SCR_W_SCALED = static_cast<int>(SCR_W_DB * SCALE);
  const int SCR_H_SCALED = static_cast<int>(SCR_H_DB * SCALE);

  if(eng->config->FULLSCREEN) {
    renderWindow_ = new sf::RenderWindow(sf::VideoMode(SCR_W_SCALED, SCR_H_SCALED), title, sf::Style::Fullscreen);
  } else {
    renderWindow_ = new sf::RenderWindow(sf::VideoMode(SCR_W_SCALED, SCR_H_SCALED), title, sf::Style::Titlebar | sf::Style::Close);
  }

  tracer << "Renderer: Enabling key repeat" << endl;
  renderWindow_->setKeyRepeatEnabled(true);

//  tracer << "Renderer: Setting frame rate limit" << endl;
  renderWindow_->setFramerateLimit(0/*60*/);

  loadFont();

  if(eng->config->USE_TILE_SET) {
    loadTiles();
    loadMainMenuLogo();
  }

  tracer << "Renderer: Setting screen texture dimensions" << endl;
  screenTexture.create(eng->config->SCREEN_WIDTH, eng->config->SCREEN_HEIGHT);

  tracer << "Renderer::setupWindowAndImagesClearPrev() [DONE]" << endl;
}

void Renderer::updateWindow() {
  screenTexture.update(*renderWindow_);

  renderWindow_->display();
  clearWindow();
}

void Renderer::loadFont() {
  tracer << "Renderer::loadFont()..." << endl;

  textureFontSheet_ = new sf::Texture();
  textureFontSheet_->loadFromFile(eng->config->fontImageName);

  const int& W = eng->config->CELL_W;
  const int& H = eng->config->CELL_H;

  const double& SCALE = eng->config->SCALE;

  for(int y = 0; y < FONT_SHEET_Y_CELLS; y++) {
    for(int x = 0; x < FONT_SHEET_X_CELLS; x++) {
      spritesFont_[x][y] = new sf::Sprite(*textureFontSheet_, sf::Rect<int>(x * W, y * H, W, H));

      spritesFont_[x][y]->setScale(SCALE, SCALE);
    }
  }

  tracer << "Renderer::loadFont() [DONE]" << endl;
}

void Renderer::loadTiles() {
  tracer << "Renderer::loadTiles()..." << endl;

  textureTileSheet_ = new sf::Texture();
  textureTileSheet_->loadFromFile(eng->config->TILES_IMAGE_NAME);

  const int W = eng->config->CELL_W;
  const int H = eng->config->CELL_H;

  for(int y = 0; y < TILE_SHEET_Y_CELLS; y++) {
    for(int x = 0; x < TILE_SHEET_X_CELLS; x++) {
      spritesTiles_[x][y] = new sf::Sprite(*textureTileSheet_, sf::Rect<int>(x * W, y * H, W, H));
    }
  }

  tracer << "Renderer::loadTiles() [DONE]" << endl;
}

void Renderer::loadMainMenuLogo() {
  tracer << "Renderer::loadMainMenuLogo()..." << endl;

  textureMainMenuLogo_ = new sf::Texture();
  tracer << "Renderer: Loading " << eng->config->MAIN_MENU_LOGO_IMAGE_NAME << "..." << endl;
  textureMainMenuLogo_->loadFromFile(eng->config->MAIN_MENU_LOGO_IMAGE_NAME);
  tracer << "Renderer: Loading " << eng->config->MAIN_MENU_LOGO_IMAGE_NAME << " [DONE]" << endl;
  tracer << "Renderer: Setting main menu logo texture..." << endl;
  spriteMainMenuLogo_ = new sf::Sprite(*textureMainMenuLogo_);
  tracer << "Renderer: Setting main menu logo texture [DONE]" << endl;

  tracer << "Renderer::loadMainMenuLogo() [DONE]" << endl;
}

void Renderer::drawScreenSizedTexture(const sf::Texture& texture) {
  sf::Sprite spr(texture);
  drawSprite(0, 0, spr);
}

void Renderer::drawSprite(const int X, const int Y, sf::Sprite& sprite) {
  const double& SCALE = eng->config->SCALE;
  const double X_DB = static_cast<double>(X) * SCALE;
  const double Y_DB = static_cast<double>(Y) * SCALE;
  sprite.setPosition(X_DB, Y_DB);
  renderWindow_->draw(sprite);
}

void Renderer::drawMainMenuLogo(const int Y_POS) {
  const int IMG_W = spriteMainMenuLogo_->getTexture()->getSize().x;
  const int X = (eng->config->SCREEN_WIDTH - IMG_W) / 2;
  const int Y = eng->config->CELL_H * Y_POS;
  drawSprite(X, Y, *spriteMainMenuLogo_);
}

void Renderer::drawMarker(vector<coord> &trace, const int EFFECTIVE_RANGE) {
  if(trace.size() > 2) {
    for(unsigned int i = 1; i < trace.size() - 1; i++) {
      coverCellInMap(trace.at(i));

      sf::Color clr = clrGreenLight;

      if(EFFECTIVE_RANGE != -1) {
        const int CHEB_DIST = eng->basicUtils->chebyshevDistance(trace.at(0), trace.at(i));
        if(CHEB_DIST > EFFECTIVE_RANGE) {
          clr = clrYellow;
        }
      }
      if(eng->config->USE_TILE_SET) {
        drawTileInMap(tile_aimMarkerTrail, trace.at(i), clr);
      } else {
        drawCharacter('*', renderArea_mainScreen, trace.at(i), clr);
      }
    }
  }

  const coord& headPos = eng->marker->getPos();

  sf::Color clr = clrGreenLight;

  if(trace.size() > 2) {
    if(EFFECTIVE_RANGE != -1) {
      const int CHEB_DIST = eng->basicUtils->chebyshevDistance(trace.at(0), headPos);
      if(CHEB_DIST > EFFECTIVE_RANGE) {
        clr = clrYellow;
      }
    }
  }

  coverCellInMap(headPos);

  if(eng->config->USE_TILE_SET) {
    drawTileInMap(tile_aimMarkerHead, headPos, clr);
  } else {
    drawCharacter('X', renderArea_mainScreen, headPos, clr);
  }
}

void Renderer::drawBlastAnimationAtField(const coord& center, const int RADIUS, bool forbiddenCells[MAP_X_CELLS][MAP_Y_CELLS],
    const sf::Color& colorInner, const sf::Color& colorOuter, const int DURATION) {
  drawMapAndInterface();
  clearWindow();

  sf::Texture bgTexture = getScreenTextureCopy();
  drawScreenSizedTexture(bgTexture);

  for(int y = max(1, center.y - RADIUS); y <= min(MAP_Y_CELLS - 2, center.y + RADIUS); y++) {
    for(int x = max(1, center.x - RADIUS); x <= min(MAP_X_CELLS - 2, center.x + RADIUS); x++) {
      if(forbiddenCells[x][y] == false) {
        const bool IS_OUTER = x == center.x - RADIUS || x == center.x + RADIUS || y == center.y - RADIUS || y == center.y + RADIUS;
        const sf::Color color = IS_OUTER ? colorOuter : colorInner;
        coverCellInMap(x, y);
        drawTileInMap(tile_blastAnimation1, x, y, color);
      }
    }
  }
  updateWindow();
  eng->sleep(DURATION / 2);
  clearWindow();
  drawScreenSizedTexture(bgTexture);

  for(int y = max(1, center.y - RADIUS); y <= min(MAP_Y_CELLS - 2, center.y + RADIUS); y++) {
    for(int x = max(1, center.x - RADIUS); x <= min(MAP_X_CELLS - 2, center.x + RADIUS); x++) {
      if(forbiddenCells[x][y] == false) {
        const bool IS_OUTER = x == center.x - RADIUS || x == center.x + RADIUS || y == center.y - RADIUS || y == center.y + RADIUS;
        const sf::Color color = IS_OUTER ? colorOuter : colorInner;
        coverCellInMap(x, y);
        drawTileInMap(tile_blastAnimation2, x, y, color);
      }
    }
  }
  updateWindow();
  eng->sleep(DURATION / 2);
  drawMapAndInterface();
}

void Renderer::drawBlastAnimationAtPositions(const vector<coord>& positions, const sf::Color& color, const int DURATION) {
  drawMapAndInterface();
  clearWindow();

  sf::Texture bgTexture = getScreenTextureCopy();
  drawScreenSizedTexture(bgTexture);

  for(unsigned int i = 0; i < positions.size(); i++) {
    const coord& pos = positions.at(i);
    drawTileInMap(tile_blastAnimation1, pos.x, pos.y, color, true, clrBlack);
  }
  updateWindow();
  eng->sleep(DURATION / 2);
  clearWindow();
  drawScreenSizedTexture(bgTexture);

  for(unsigned int i = 0; i < positions.size(); i++) {
    const coord& pos = positions.at(i);
    drawTileInMap(tile_blastAnimation2, pos.x, pos.y, color, true, clrBlack);
  }
  updateWindow();
  eng->sleep(DURATION / 2);
  drawMapAndInterface();
}

void Renderer::drawBlastAnimationAtPositionsWithPlayerVision(const vector<coord>& positions,
    const sf::Color& clr, const int EXPLOSION_DELAY_FACTOR, Engine* const engine) {

  const int DELAY = engine->config->DELAY_EXPLOSION * EXPLOSION_DELAY_FACTOR;

  vector<coord> positionsWithVision;
  for(unsigned int i = 0; i < positions.size(); i++) {
    const coord& pos = positions.at(i);
    if(engine->map->playerVision[pos.x][pos.y]) {
      positionsWithVision.push_back(pos);
    }
  }

  if(engine->config->USE_TILE_SET) {
    engine->renderer->drawBlastAnimationAtPositions(positionsWithVision, clr, DELAY);
  } else {
    //TODO
  }
}

void Renderer::drawTileInScreen(const Tile_t tile, const int X, const int Y, const sf::Color& clr,
                                const bool drawBgClr, const sf::Color& bgClr) {
  const int& CELL_W = eng->config->CELL_W;
  const int& CELL_H = eng->config->CELL_H;

  const int X_PIXEL = X * CELL_W;
  const int Y_PIXEL = Y * CELL_H;
  const coord tileCoords = eng->art->getTileCoords(tile);

  if(drawBgClr) {
    drawRectangleSolid(X_PIXEL, Y_PIXEL, CELL_W, CELL_H, bgClr);
  }

  sf::Sprite* const spr = spritesTiles_[tileCoords.x][tileCoords.y];
  spr->setColor(clr);
  drawSprite(X_PIXEL, Y_PIXEL, *spr);
}

void Renderer::drawTileInMap(const Tile_t tile, const int X, const int Y, const sf::Color& clr,
                             const bool drawBgClr, const sf::Color& bgClr) {
  drawTileInScreen(tile, X, Y + eng->config->MAINSCREEN_Y_CELLS_OFFSET, clr, drawBgClr, bgClr);
}

void Renderer::drawGlyphInMap(const char GLYPH, const int X, const int Y, const sf::Color& clr,
                              const bool drawBgClr, const sf::Color& bgClr) {
  const int& CELL_W = eng->config->CELL_W;
  const int& CELL_H = eng->config->CELL_H;

  const int X_PIXEL = X * CELL_W;
  const int Y_PIXEL = Y * CELL_H + eng->config->MAINSCREEN_Y_OFFSET;
  const coord glyphCoords = eng->art->getGlyphCoords(GLYPH);

  if(drawBgClr) {
    drawRectangleSolid(X_PIXEL, Y_PIXEL, CELL_W, CELL_H, bgClr);
  }

  sf::Sprite* const spr = spritesFont_[glyphCoords.x][glyphCoords.y];
  spr->setColor(clr);
  drawSprite(X_PIXEL, Y_PIXEL, *spr);
}

void Renderer::drawCharacterAtPixel(const char CHARACTER, const int X, const int Y, const sf::Color& clr) {
  const coord& glyphCoords = eng->art->getGlyphCoords(CHARACTER);
  sf::Sprite* const spr = spritesFont_[glyphCoords.x][glyphCoords.y];
  spr->setColor(clr);
  drawSprite(X, Y, *spr);
}

coord Renderer::getPixelCoordsForCharacter(const RenderArea_t renderArea, const int X, const int Y) {
  const int CELL_W = eng->config->CELL_W;
  const int CELL_H = eng->config->CELL_H;

  switch(renderArea) {
  case renderArea_screen: {
    return coord(X * CELL_W, Y * CELL_H);
  }
  break;
  case renderArea_mainScreen: {
    return coord(X * CELL_W, eng->config->MAINSCREEN_Y_OFFSET + Y * CELL_H);
  }
  break;
  case renderArea_log: {
    return coord(eng->config->LOG_X_OFFSET + X * CELL_W, eng->config->LOG_Y_OFFSET + Y * CELL_H);
  }
  break;
  case renderArea_characterLines: {
    return coord(X * CELL_W, eng->config->CHARACTER_LINES_Y_OFFSET + Y * CELL_H);
  }
  break;
  }
  return coord();
}

void Renderer::drawCharacter(const char CHARACTER, const RenderArea_t renderArea, const int X, const int Y, const sf::Color& clr) {
  const coord pixelCoord = getPixelCoordsForCharacter(renderArea, X, Y);
  drawCharacterAtPixel(CHARACTER, pixelCoord.x, pixelCoord.y, clr);
}

void Renderer::drawText(const string& str, const RenderArea_t renderArea, const int X, const int Y, const sf::Color& clr) {
  coord pixelCoord = getPixelCoordsForCharacter(renderArea, X, Y);

  if(pixelCoord.y < 0 || pixelCoord.y >= eng->config->SCREEN_HEIGHT) {
    return;
  }

  const int& CELL_W = eng->config->CELL_W;
  const int& CELL_H = eng->config->CELL_H;
  const int TOT_W = CELL_W * str.size();

  coverAreaPixel(pixelCoord.x, pixelCoord.y, TOT_W, CELL_H);

  for(unsigned int i = 0; i < str.size(); i++) {
    if(pixelCoord.x < 0 || pixelCoord.x >= eng->config->SCREEN_WIDTH) {
      return;
    }
    drawCharacterAtPixel(str.at(i), pixelCoord.x, pixelCoord.y, clr);
    pixelCoord.x += CELL_W;
  }
}

int Renderer::drawTextCentered(const string& str, const RenderArea_t renderArea, const int X, const int Y,
                               const sf::Color& clr, const bool IS_PIXEL_POS_ADJ_ALLOWED) {
  const unsigned int STR_LEN_HALF = str.size() / 2;
  const int X_POS_LEFT = X - STR_LEN_HALF;
  const int CELL_W = eng->config->CELL_W;

  coord pixelCoord = getPixelCoordsForCharacter(renderArea, X_POS_LEFT, Y);

  if(IS_PIXEL_POS_ADJ_ALLOWED) {
    const int X_PIXEL_ADJ = STR_LEN_HALF * 2 == str.size() ? CELL_W / 2 : 0;
    pixelCoord += coord(X_PIXEL_ADJ, 0);
  }

  for(unsigned int i = 0; i < str.size(); i++) {
    if(pixelCoord.x < 0 || pixelCoord.x >= eng->config->SCREEN_WIDTH) {
      return X_POS_LEFT;
    }
    drawCharacterAtPixel(str.at(i), pixelCoord.x, pixelCoord.y, clr);
    pixelCoord.x += eng->config->CELL_W;
  }
  return X_POS_LEFT;
}

void Renderer::coverRenderArea(const RenderArea_t renderArea) {
  const coord x0y0Pixel = getPixelCoordsForCharacter(renderArea, 0, 0);
  switch(renderArea) {
  case renderArea_characterLines: {
    coverAreaPixel(x0y0Pixel.x, x0y0Pixel.y, eng->config->SCREEN_WIDTH, eng->config->CHARACTER_LINES_HEIGHT);
  }
  break;
  case renderArea_log: {
    coverAreaPixel(0, 0, eng->config->SCREEN_WIDTH, eng->config->LOG_HEIGHT + eng->config->LOG_Y_OFFSET);
  }
  break;
  case renderArea_mainScreen: {
    coverAreaPixel(x0y0Pixel.x, x0y0Pixel.y, eng->config->SCREEN_WIDTH, eng->config->MAINSCREEN_HEIGHT);
  }
  break;
  case renderArea_screen: {
    clearWindow();
  }
  break;

  }
}

void Renderer::coverArea(const RenderArea_t renderArea, const int X, const int Y, const int W, const int H) {
  const coord x0y0 = getPixelCoordsForCharacter(renderArea, X, Y);
  coverAreaPixel(x0y0.x, x0y0.y, W * eng->config->CELL_W, H * eng->config->CELL_H);
}

void Renderer::coverAreaPixel(const int X, const int Y, const int W, const int H) {
  drawRectangleSolid(X, Y, W, H, clrBlack);
}

void Renderer::coverCellInMap(const int X, const int Y) {
  const int CELL_W = eng->config->CELL_W;
  const int CELL_H = eng->config->CELL_H;
  coverAreaPixel(X * CELL_W, eng->config->MAINSCREEN_Y_OFFSET + (Y * CELL_H), CELL_W, CELL_H);
}

void Renderer::drawLineVertical(const int x0, const int y0, const int h, const sf::Color& clr) {
  drawRectangleSolid(x0, y0, 1, h, clr);
}

void Renderer::drawLineHorizontal(const int x0, const int y0, const int w, const sf::Color& clr) {
  drawRectangleSolid(x0, y0, w, 2, clr);
}

// Hack to fix Catalyst 12.10 driver bug
// (see the comment for the function declaration in Render.h)
void Renderer::workaroundAMDBug() {
  static sf::Texture amdWorkaroundImage;
  static sf::Sprite sprite;

  if(amdWorkaroundImage.getSize() == sf::Vector2u(0, 0)) {
    amdWorkaroundImage.create(1, 1);
    sprite.setTexture(amdWorkaroundImage);
    sprite.setPosition(-20, -20);
  }
  renderWindow_->draw(sprite);
}

void Renderer::drawRectangleSolid(const int X, const int Y, const int W, const int H, const sf::Color& clr) {
  const int W_MAX = min(eng->config->MAINSCREEN_WIDTH - X, W);
  const double X_DB = static_cast<double>(X);
  const double Y_DB = static_cast<double>(Y);
  const double W_DB = static_cast<double>(W_MAX);
  const double H_DB = static_cast<double>(H);
  const double& SCALE = eng->config->SCALE;
  sf::RectangleShape rectShape(sf::Vector2f(W_DB * SCALE, H_DB * SCALE));
  rectShape.setFillColor(clr);

  rectShape.setPosition(X_DB * SCALE, Y_DB * SCALE);
  renderWindow_->draw(rectShape);

  // Hack to fix Catalyst 12.10 driver bug
  // (see the comment for the function declaration in Render.h)
  workaroundAMDBug();
}

void Renderer::coverCharacterAtPixel(const int X, const int Y) {
  coverAreaPixel(X, Y, eng->config->CELL_W, eng->config->CELL_H);
}

void Renderer::coverTileAtPixel(const int X, const int Y) {
  coverAreaPixel(X, Y, eng->config->CELL_W, eng->config->CELL_H);
}

void Renderer::drawProjectilesAndUpdateWindow(vector<Projectile*>& projectiles) {
  drawMapAndInterface(false);

  for(unsigned int i = 0; i < projectiles.size(); i++) {
    Projectile* const p = projectiles.at(i);
    if(p->isDoneRendering == false && p->isVisibleToPlayer) {
      coverCellInMap(p->pos);
      if(eng->config->USE_TILE_SET) {
        if(p->tile != tile_empty) {
          drawTileInMap(p->tile, p->pos, p->clr);
        }
      } else {
        if(p->glyph != -1) {
          drawCharacter(p->glyph, renderArea_mainScreen, p->pos, p->clr);
        }
      }
    }
  }

  updateWindow();
}

void Renderer::drawMapAndInterface(const bool UPDATE_WINDOW) {
  clearWindow();

  if(eng->config->USE_TILE_SET) {
    drawTiles();
  } else {
    drawASCII();
  }

  eng->interfaceRenderer->drawInfoLines();
  eng->interfaceRenderer->drawLocationInfo();
  eng->log->drawLog();

  if(UPDATE_WINDOW) {
    updateWindow();
  }
}

int Renderer::getLifebarLength(const Actor& actor) const {
  const int ACTOR_HP = max(0, actor.getHp());
  const int ACTOR_HP_MAX = actor.getHpMax(true);
  if(ACTOR_HP < ACTOR_HP_MAX) {
    int HP_PERCENT = (ACTOR_HP * 100) / ACTOR_HP_MAX;
    return ((eng->config->CELL_W - 2) * HP_PERCENT) / 100;
  }
  return -1;
}

void Renderer::drawLifeBar(const int X_CELL, const int Y_CELL, const int LENGTH) {
  if(LENGTH >= 0) {
    const int W_GREEN = LENGTH;
    const int& W_CELL = eng->config->CELL_W;
    const int W_BAR_TOT = W_CELL - 2;
    const int W_RED = W_BAR_TOT - W_GREEN;
    const int Y0 = eng->config->MAINSCREEN_Y_OFFSET + ((Y_CELL + 1) * eng->config->CELL_H) - 2;
    const int X0_GREEN = X_CELL * W_CELL + 1;
    const int X0_RED = X0_GREEN + W_GREEN;

    if(W_GREEN > 0) {
      drawLineHorizontal(X0_GREEN, Y0, W_GREEN, clrGreenLight);
    }
    if(W_RED > 0) {
      drawLineHorizontal(X0_RED, Y0, W_RED, clrRedLight);
    }
  }
}

void Renderer::drawASCII() {
  CellRenderDataAscii* currentDrw = NULL;
  CellRenderDataAscii tempDrw;

  //-------------------------------------------- INSERT STATIC FEATURES AND BLOOD INTO ARRAY
  for(int y = 0; y < MAP_Y_CELLS; y++) {
    for(int x = 0; x < MAP_X_CELLS; x++) {

      currentDrw = &renderArray[x][y];
      currentDrw->clear();

      if(eng->map->playerVision[x][y]) {
        //Static features
        char goreGlyph = ' ';
        if(eng->map->featuresStatic[x][y]->canHaveGore()) {
          goreGlyph = eng->map->featuresStatic[x][y]->getGoreGlyph();
        }
        if(goreGlyph == ' ') {
          currentDrw->glyph = eng->map->featuresStatic[x][y]->getGlyph();
          const sf::Color& featureClr = eng->map->featuresStatic[x][y]->getColor();
          const sf::Color& featureClrBg = eng->map->featuresStatic[x][y]->getColorBg();
          currentDrw->color = eng->map->featuresStatic[x][y]->hasBlood() ? clrRedLight : featureClr;
          if(featureClrBg != clrBlack) {
            currentDrw->colorBg = featureClrBg;
            currentDrw->drawBgColor = true;
          }
        } else {
          currentDrw->glyph = goreGlyph;
          currentDrw->color = clrRed;
        }
      }
    }
  }

  int xPos, yPos;
  const unsigned int LOOP_SIZE = eng->gameTime->actors_.size();
  //-------------------------------------------- INSERT DEAD ACTORS INTO ARRAY
  Actor* actor = NULL;
  for(unsigned int i = 0; i < LOOP_SIZE; i++) {
    actor = eng->gameTime->getActorAt(i);
    xPos = actor->pos.x;
    yPos = actor->pos.y;
    if(actor->deadState == actorDeadState_corpse && actor->getDef()->glyph != ' ' && eng->map->playerVision[xPos][yPos]) {
      currentDrw = &renderArray[xPos][yPos];
      currentDrw->color = actor->getColor();
      currentDrw->glyph = actor->getGlyph();
    }
  }

  for(int y = 0; y < MAP_Y_CELLS; y++) {
    for(int x = 0; x < MAP_X_CELLS; x++) {
      currentDrw = &renderArray[x][y];
      if(eng->map->playerVision[x][y] == true) {
        //-------------------------------------------- INSERT ITEMS INTO ARRAY
        if(eng->map->items[x][y] != NULL) {
          currentDrw->color = eng->map->items[x][y]->getColor();
          currentDrw->glyph = eng->map->items[x][y]->getGlyph();
        }

        //COPY ARRAY TO PLAYER MEMORY (BEFORE LIVING ACTORS AND TIME ENTITIES)
        renderArrayActorsOmitted[x][y] = renderArray[x][y];
      }
    }
  }

  //-------------------------------------------- INSERT MOBILE FEATURES INTO ARRAY
  const unsigned int SIZE_OF_FEAT_MOB = eng->gameTime->getFeatureMobsSize();
  for(unsigned int i = 0; i < SIZE_OF_FEAT_MOB; i++) {
    FeatureMob* feature = eng->gameTime->getFeatureMobAt(i);
    xPos = feature->getX();
    yPos = feature->getY();
    if(feature->getGlyph() != ' ' && eng->map->playerVision[xPos][yPos] == true) {
      currentDrw = &renderArray[xPos][yPos];
      currentDrw->color = feature->getColor();
      currentDrw->glyph = feature->getGlyph();
    }
  }

  //-------------------------------------------- INSERT LIVING ACTORS INTO ARRAY
  for(unsigned int i = 0; i < LOOP_SIZE; i++) {
    actor = eng->gameTime->getActorAt(i);
    if(actor != eng->player) {
      xPos = actor->pos.x;
      yPos = actor->pos.y;
      if(
        actor->deadState == actorDeadState_alive &&
        actor->getGlyph() != ' ' &&
        eng->player->checkIfSeeActor(*actor, NULL)) {
        currentDrw = &renderArray[xPos][yPos];
        currentDrw->color = actor->getColor();
        currentDrw->glyph = actor->getGlyph();

        currentDrw->lifebarLength = getLifebarLength(*actor);

        const Monster* const monster = dynamic_cast<const Monster*>(actor);
        if(monster->leader == eng->player) {
          // TODO reimplement allied indicator
        } else {
          if(monster->playerAwarenessCounter == 0) {
            // TODO reimplement awareness indicator
          }
        }
      }
    }
  }

  //-------------------------------------------- DRAW THE GRID
  for(int y = 0; y < MAP_Y_CELLS; y++) {
    for(int x = 0; x < MAP_X_CELLS; x++) {

      tempDrw.clear();

      if(eng->map->playerVision[x][y]) {
        tempDrw = renderArray[x][y];
        if(tempDrw.isFadeEffectAllowed) {
          const int DIST_FROM_PLAYER = eng->basicUtils->chebyshevDistance(eng->player->pos, coord(x, y));
          if(DIST_FROM_PLAYER > 1) {
            const double DIST_FADE_DIV = min(2.0, 1.0 + ((DIST_FROM_PLAYER - 1) * 0.33));
            tempDrw.color.r /= DIST_FADE_DIV;
            tempDrw.color.g /= DIST_FADE_DIV;
            tempDrw.color.b /= DIST_FADE_DIV;
          }
        }
      }
      else if(eng->map->explored[x][y]) {
        renderArray[x][y] = eng->map->playerVisualMemory[x][y];
        tempDrw = renderArray[x][y];

        tempDrw.color.r /= 5;
        tempDrw.color.g /= 5;
        tempDrw.color.b /= 5;
      }

      if(tempDrw.glyph != ' ') {
        drawCharacter(tempDrw.glyph, renderArea_mainScreen, x, y, tempDrw.color);

        if(tempDrw.lifebarLength != -1) {
          drawLifeBar(x, y, tempDrw.lifebarLength);
        }
      }
    }
  }

  //-------------------------------------------- DRAW PLAYER CHARACTER
  drawGlyphInMap(eng->player->getGlyph(), eng->player->pos.x, eng->player->pos.y,
                 eng->player->getColor(), true, clrBlack);
  const int LIFE_BAR_LENGTH = getLifebarLength(*eng->player);
  if(LIFE_BAR_LENGTH != -1) {
    drawLifeBar(eng->player->pos.x, eng->player->pos.y, LIFE_BAR_LENGTH);
  }
}

void Renderer::drawTiles() {
  CellRenderDataTile* currentDrw = NULL;
  CellRenderDataTile tempDrw;

  //-------------------------------------------- INSERT STATIC FEATURES AND BLOOD INTO TILE ARRAY
  for(int y = 0; y < MAP_Y_CELLS; y++) {
    for(int x = 0; x < MAP_X_CELLS; x++) {
      if(eng->map->playerVision[x][y]) {
        currentDrw = &renderArrayTiles[x][y];
        currentDrw->clear();

        if(eng->map->playerVision[x][y]) {

          //Static features
          Tile_t goreTile = tile_empty;
          if(eng->map->featuresStatic[x][y]->canHaveGore()) {
            goreTile = eng->map->featuresStatic[x][y]->getGoreTile();
          }
          if(goreTile == tile_empty) {
            Feature* const f = eng->map->featuresStatic[x][y];
            const Feature_t featureId = f->getId();
            if(featureId == feature_stoneWall) {
              currentDrw->tile = dynamic_cast<Wall*>(f)->getTopWallTile();
            } else {
              currentDrw->tile = f->getTile();
            }
            const sf::Color featureClr = f->getColor();
            const sf::Color featureClrBg = f->getColorBg();
            currentDrw->color = f->hasBlood() ? clrRedLight : featureClr;
            if(featureClrBg != clrBlack) {
              currentDrw->colorBg = featureClrBg;
              currentDrw->drawBgColor = true;
            }
          } else {
            currentDrw->tile = goreTile;
            currentDrw->color = clrRed;
          }
        }
      }
    }
  }

  int xPos, yPos;
  const unsigned int LOOP_SIZE = eng->gameTime->actors_.size();
  //-------------------------------------------- INSERT DEAD ACTORS INTO TILE ARRAY
  Actor* actor = NULL;
  for(unsigned int i = 0; i < LOOP_SIZE; i++) {
    actor = eng->gameTime->getActorAt(i);
    xPos = actor->pos.x;
    yPos = actor->pos.y;
    if(actor->deadState == actorDeadState_corpse && actor->getTile() != ' ' && eng->map->playerVision[xPos][yPos]) {
      currentDrw = &renderArrayTiles[xPos][yPos];
      currentDrw->color = actor->getColor();
      currentDrw->tile = actor->getTile();
    }
  }

  for(int y = 0; y < MAP_Y_CELLS; y++) {
    for(int x = 0; x < MAP_X_CELLS; x++) {
      currentDrw = &renderArrayTiles[x][y];
      if(eng->map->playerVision[x][y]) {
        //-------------------------------------------- INSERT ITEMS INTO TILE ARRAY
        if(eng->map->items[x][y] != NULL) {
          currentDrw->color = eng->map->items[x][y]->getColor();
          currentDrw->tile = eng->map->items[x][y]->getTile();
        }
        //COPY ARRAY TO PLAYER MEMORY (BEFORE LIVING ACTORS AND MOBILE FEATURES)
        renderArrayActorsOmittedTiles[x][y] = renderArrayTiles[x][y];
      }
    }
  }

  //-------------------------------------------- INSERT MOBILE FEATURES INTO TILE ARRAY
  const unsigned int SIZE_OF_FEAT_MOB = eng->gameTime->getFeatureMobsSize();
  for(unsigned int i = 0; i < SIZE_OF_FEAT_MOB; i++) {
    FeatureMob* feature = eng->gameTime->getFeatureMobAt(i);
    xPos = feature->getX();
    yPos = feature->getY();
    if(feature->getGlyph() != ' ' && eng->map->playerVision[xPos][yPos]) {
      currentDrw = &renderArrayTiles[xPos][yPos];
      currentDrw->color = feature->getColor();
      currentDrw->tile = feature->getTile();
    }
  }

  //-------------------------------------------- INSERT LIVING ACTORS INTO TILE ARRAY
  for(unsigned int i = 0; i < LOOP_SIZE; i++) {
    actor = eng->gameTime->getActorAt(i);
    if(actor != eng->player) {
      if(
        actor->deadState == actorDeadState_alive &&
        actor->getTile() != tile_empty &&
        eng->player->checkIfSeeActor(*actor, NULL)) {
        currentDrw = &renderArrayTiles[actor->pos.x][actor->pos.y];
        currentDrw->color = actor->getColor();
        currentDrw->tile = actor->getTile();

        currentDrw->lifebarLength = getLifebarLength(*actor);

        currentDrw->isFadeEffectAllowed = false;

        const Monster* const monster = dynamic_cast<const Monster*>(actor);
        if(monster->leader == eng->player) {
          // TODO implement allied indicator
        } else {
          if(monster->playerAwarenessCounter <= 0) {
            currentDrw->colorBg = clrBlue;
            currentDrw->drawBgColor = true;
          }
        }
      }
    }
  }

  //-------------------------------------------- DRAW THE TILE GRID
  for(int y = 0; y < MAP_Y_CELLS; y++) {
    for(int x = 0; x < MAP_X_CELLS; x++) {

      tempDrw.clear();

      if(eng->map->playerVision[x][y]) {
        tempDrw = renderArrayTiles[x][y];
        if(tempDrw.isFadeEffectAllowed) {
          const int DIST_FROM_PLAYER = eng->basicUtils->chebyshevDistance(eng->player->pos, coord(x, y));
          if(DIST_FROM_PLAYER > 1) {
            const double DIST_FADE_DIV = min(2.0, 1.0 + ((DIST_FROM_PLAYER - 1) * 0.33));
            tempDrw.color.r /= DIST_FADE_DIV;
            tempDrw.color.g /= DIST_FADE_DIV;
            tempDrw.color.b /= DIST_FADE_DIV;
          }
        }
      }
      else if(eng->map->explored[x][y]) {
        renderArrayTiles[x][y] = eng->map->playerVisualMemoryTiles[x][y];
        tempDrw = renderArrayTiles[x][y];

        tempDrw.color.r /= 5;
        tempDrw.color.g /= 5;
        tempDrw.color.b /= 5;
        tempDrw.colorBg.r /= 5;
        tempDrw.colorBg.g /= 5;
        tempDrw.colorBg.b /= 5;
      }

      //Walls are given perspective.
      //If the tile to be set is a (top) wall tile, check the tile beneath it. If the tile beneath is
      //not a front or top wall tile, and that cell is explored, change the current tile to wall front
      const Tile_t tileSeen = renderArrayActorsOmittedTiles[x][y].tile;
      const Tile_t tileMem = eng->map->playerVisualMemoryTiles[x][y].tile;
      bool isTileWall = eng->map->playerVision[x][y] ? Wall::isTileAnyWallTop(tileSeen) : Wall::isTileAnyWallTop(tileMem);
      if(isTileWall) {
        Feature* const f = eng->map->featuresStatic[x][y];
        const Feature_t featureId = f->getId();
        bool isHiddenDoor = false;
        if(featureId == feature_door) {
          isHiddenDoor = dynamic_cast<Door*>(f)->isSecret();
        }
        if(y < MAP_Y_CELLS - 1 && (featureId == feature_stoneWall || isHiddenDoor)) {
          if(eng->map->explored[x][y + 1]) {
            const bool IS_CELL_BELOW_SEEN = eng->map->playerVision[x][y + 1];

            const Tile_t tileBelowSeen = renderArrayActorsOmittedTiles[x][y + 1].tile;
            const Tile_t tileBelowMem = eng->map->playerVisualMemoryTiles[x][y + 1].tile;

            const bool TILE_BELOW_IS_WALL_FRONT =
              IS_CELL_BELOW_SEEN ? Wall::isTileAnyWallFront(tileBelowSeen) :
              Wall::isTileAnyWallFront(tileBelowMem);

            const bool TILE_BELOW_IS_WALL_TOP =
              IS_CELL_BELOW_SEEN ? Wall::isTileAnyWallTop(tileBelowSeen) :
              Wall::isTileAnyWallTop(tileBelowMem);

            if(TILE_BELOW_IS_WALL_FRONT == false && TILE_BELOW_IS_WALL_TOP == false) {
              if(isHiddenDoor) {
                tempDrw.tile = tile_wallFront;
              } else if(featureId == feature_stoneWall) {
                const Wall* const wall = dynamic_cast<const Wall*>(f);
                tempDrw.tile = wall->getFrontWallTile();
              }
            }
          }
        }
      }

      if(tempDrw.tile != tile_empty) {
        drawTileInMap(tempDrw.tile, x, y, tempDrw.color, tempDrw.drawBgColor, tempDrw.colorBg);

        if(tempDrw.lifebarLength != -1) {
          drawLifeBar(x, y, tempDrw.lifebarLength);
        }
      }
    }
  }

  //-------------------------------------------- DRAW PLAYER CHARACTER
  bool isWieldingRangedWeapon = false;
  Item* item = eng->player->getInventory()->getItemInSlot(slot_wielded);
  if(item != NULL) {
    isWieldingRangedWeapon = item->getDef().isRangedWeapon;
  }
  drawTileInMap(isWieldingRangedWeapon ? tile_playerFirearm : tile_playerMelee,
                eng->player->pos, eng->player->getColor(), true, clrBlack);
  const int LIFE_BAR_LENGTH = getLifebarLength(*eng->player);
  if(LIFE_BAR_LENGTH != -1) {
    drawLifeBar(eng->player->pos.x, eng->player->pos.y, LIFE_BAR_LENGTH);
  }
}

