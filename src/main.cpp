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

	print(std::cout, scene, 0);
}
