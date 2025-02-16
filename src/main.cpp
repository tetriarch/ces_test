#include "components/components.hpp"
#include "entity.hpp"
#include "spell_manager.hpp"
#include "spell_loader.hpp"

//TODO: Combat exchange
//TODO: Figure out how to tag castSpellComponent to know which spell to cast, 
// I am thinking that component could have list of actions it can do and those actions would be provided by some ActionManager 
// which would hold std::unordered_map of std::function that perform action

void indent(std::ostream& out, u32 depth) {

	for(u32 i = 0; i < depth * 1; ++i) {
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

	for(auto& s : spells.value()) {
		std::cout << s.describe() << std::endl;
	}

	// hardcoded scene
	EntityPtr scene = Entity::create("scene");
	EntityPtr player = Entity::create("player");
	EntityPtr wolf = Entity::create("wolf");

	SpellManager sm(scene);

	auto playerTag = std::make_shared<Tag>();
	playerTag->setTag(TagType::PLAYER);

	auto wolfTag = std::make_shared<Tag>();
	wolfTag->setTag(TagType::ENEMY);

	auto playerResources = std::make_shared<LifeMana>();
	playerResources->setLife(100);
	playerResources->setMana(200);

	auto wolfResources = std::make_shared<LifeMana>();
	wolfResources->setLife(200);
	wolfResources->setMana(0);

	player->addComponent(std::make_shared<SpellBook>(&sm));
	player->addComponent(std::make_shared<Attack>());
	player->addComponent(playerTag);
	player->addComponent(playerResources);

	wolf->addComponent(std::make_shared<Attack>());
	wolf->addComponent(std::make_shared<Ability>());
	wolf->addComponent(wolfTag);
	wolf->addComponent(wolfResources);

	scene->addChild(player);
	scene->addChild(wolf);

	// print(std::cout, scene, 0);
	// char x;
	// while(std::cin >> x) {
	// 	if(x == 'x') {
	// 		break;
	// 	}
	// 	print(std::cout, scene, 0);
	// }

	//TODO: Add combat

	return 0;
}
