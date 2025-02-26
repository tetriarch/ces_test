#include "components/components.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"
#include "spell_loader.hpp"

void indent(std::ostream& out, u32 depth) {

	for(u32 i = 0; i < depth * 1; i++) {
		out << "\t";
	}
	out << ">";
}



void print(std::ostream& out, const EntityPtr& e, u32 depth) {

	indent(out, depth);

	out << e->getName() << " (" << e.get() << ")" << " [" << typeid(e).name() << "]" << std::endl;

	for(auto&& comp : e->getComponents()) {
		indent(out, depth);
		out << comp->describe() << " " << comp.get() << std::endl;
	}

	for(auto&& child : e->getChildren()) {
		print(out, child, depth + 1);
	}
}



int main(int argc, char const* argv[]) {

	// parse spells
	SpellLoader spellLoader;
	auto spells = spellLoader.load("assets/data/spells.json");
	if(!spells) {
		std::cout << spellLoader.getError(spells.error()) << std::endl;
		return 1;
	}

	// hardcoded scene
	EntityPtr scene = Entity::create("scene");
	EntityPtr player = Entity::create("player");
	EntityPtr wolf = Entity::create("wolf");

	auto playerTag = std::make_shared<TagComponent>();
	playerTag->setTag(TagType::PLAYER);

	auto wolfTag = std::make_shared<TagComponent>();
	wolfTag->setTag(TagType::ENEMY);

	auto playerLife = std::make_shared<LifeComponent>();
	playerLife->setLife({100, 100});

	auto playerMana = std::make_shared<ManaComponent>();
	playerMana->setMana({200, 200});

	auto wolfLife = std::make_shared<LifeComponent>();
	wolfLife->setLife({200, 200});

	auto wolfMana = std::make_shared<ManaComponent>();
	wolfMana->setMana({0, 0});

	player->addComponent(std::make_shared<SpellBookComponent>());
	player->addComponent(playerTag);
	player->addComponent(playerLife);
	player->addComponent(playerMana);

	wolf->addComponent(wolfLife);
	wolf->addComponent(wolfMana);

	scene->addChild(player);
	scene->addChild(wolf);

	// quick spellBook test
	auto playerSpellBook = player->getComponent<SpellBookComponent>();
	playerSpellBook->addSpell(std::make_shared<SpellData>(spells.value()[0]));
	playerSpellBook->addSpell(std::make_shared<SpellData>(spells.value()[1]));
	playerSpellBook->addSpell(std::make_shared<SpellData>(spells.value()[2]));

	playerSpellBook->castSpell(std::make_shared<SpellData>(spells.value()[0]), player, Vec2(0, 0));
	print(std::cout, scene, 0);

	return 0;
}
