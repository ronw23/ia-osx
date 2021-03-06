#ifndef ITEM_EXPLOSIVE_H
#define ITEM_EXPLOSIVE_H

#include "StatusEffects.h"
#include "Item.h"

class Engine;

class Explosive : public Item {
public:
  Explosive(ItemDefinition* const itemDefinition) : Item(itemDefinition) {}
  virtual ~Explosive();

  virtual bool activateDefault(Actor* const actor, Engine* const engine) {
    (void)actor;
    setPlayerExplosive(engine);
    return true;
  }
  virtual string getDefaultActivationLabel() const {
    return "Ignite";
  }

  virtual sf::Color getInterfaceClr() const {
    return clrRedLight;
  }

  virtual void setPlayerExplosive(Engine* const engine) const;

protected:
};

class Dynamite: public Explosive {
public:
  Dynamite(ItemDefinition* const itemDefinition) : Explosive(itemDefinition) {
  }
  ~Dynamite() {
  }

  void setPlayerExplosive(Engine* const engine) const;

private:
};

class Molotov: public Explosive {
public:
  Molotov(ItemDefinition* const itemDefinition) : Explosive(itemDefinition) {
  }
  ~Molotov() {
  }

  void setPlayerExplosive(Engine* const engine) const;

private:
};

class Flare: public Explosive {
public:
  Flare(ItemDefinition* const itemDefinition) : Explosive(itemDefinition) {
  }
  ~Flare() {
  }

  void setPlayerExplosive(Engine* const engine) const;

private:
};

#endif
