#include "asset_manager.hpp"
#include "components/components.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"
#include "scene.hpp"
#include "scene_loader.hpp"
#include "spell_loader.hpp"

void indent(std::ostream& out, u32 depth) {

	for(u32 i = 0; i < depth * 1; i++) {
		out << "\t";
	}
	out << ">";
}

void print(std::ostream& out, const EntityPtr& e, u32 depth) {

	indent(out, depth);

	out << e->name() << " (" << e.get() << ")" << " [" << typeid(e).name() << "]" << std::endl;

	for(auto&& comp : e->components()) {
		indent(out, depth);
		out << comp->describe() << " " << comp.get() << std::endl;
	}

	for(auto&& child : e->children()) {
		print(out, child, depth + 1);
	}
}

int main(int argc, char const* argv[]) {

	// init
	AssetManager am("assets");
	am.registerLoader<SpellData>(std::make_shared<SpellLoader>());
	am.registerLoader<Scene>(std::make_shared<SceneLoader>());

	auto scene = am.load<Scene>("scenes/level_1.json");
	if(!scene) {
		return 1;
	}

	// auto fireBall = am.load<SpellData>("spells/fireball.json");
	// auto iceLance = am.load<SpellData>("spells/ice_lance.json");
	// auto zap = am.load<SpellData>("spells/zap.json");

	// // hardcoded scene
	// EntityPtr player = Entity::create("player");
	// EntityPtr wolf = Entity::create("wolf");

	// auto playerTag = std::make_shared<TagComponent>();
	// playerTag->setTag(TagType::PLAYER);

	// auto wolfTag = std::make_shared<TagComponent>();
	// wolfTag->setTag(TagType::ENEMY);

	// auto playerLife = std::make_shared<LifeComponent>();
	// playerLife->setLife({100, 100});

	// auto playerMana = std::make_shared<ManaComponent>();
	// playerMana->setMana({200, 200});

	// auto wolfLife = std::make_shared<LifeComponent>();
	// wolfLife->setLife({200, 200});

	// auto wolfMana = std::make_shared<ManaComponent>();
	// wolfMana->setMana({0, 0});

	// player->addComponent(std::make_shared<SpellBookComponent>());
	// player->addComponent(playerTag);
	// player->addComponent(playerLife);
	// player->addComponent(playerMana);

	// wolf->addComponent(wolfLife);
	// wolf->addComponent(wolfMana);

	// scene->addChild(player);
	// scene->addChild(wolf);

	// auto spellBook = player->component<SpellBookComponent>();
	// spellBook->addSpell(fireBall);
	// spellBook->addSpell(iceLance);
	// spellBook->addSpell(zap);
	// spellBook->castSpell(fireBall, player, Vec2(0, 0));

	print(std::cout, scene, 0);

	return 0;
}
