#include "../src/controller/entity_list/entity_list_controller.h"
#include <catch2/catch.hpp>
#include <lmng/hierarchy.h>
#include <lmng/logging.h>
#include <model/selection.h>

TEST_CASE("Entity list controller")
{
    entt::registry registry;
    lmng::connect_component_logging(registry);
    auto selected_view = registry.view<lmeditor::selected>();

    lmeditor::entity_list_controller controller{registry};
    lmtk::input_state input_state;

    auto parent = registry.create();
    auto child = registry.create();

    registry.assign<lmng::name>(parent, "Parent");
    registry.assign<lmng::name>(child, "Child");

    registry.assign<lmng::parent>(child, parent);

    auto other = registry.create();
    registry.assign<lmng::name>(other, "Other");

    REQUIRE(selected_view.empty());

    controller.handle(lmtk::key_down_event{input_state, lmpl::key_code::Enter});

    REQUIRE(!selected_view.empty());

    for (auto unused : lm::range(2))
    {
        controller.handle(lmtk::key_down_event{input_state, lmpl::key_code::K});

        controller.handle(
          lmtk::key_down_event{input_state, lmpl::key_code::Enter});

        REQUIRE(!selected_view.empty());
    }

    REQUIRE(
      controller.selected_entity_index ==
      registry.view<lmng::name>().size() - 1);

    auto old_selection = selected_view[0];

    controller.handle(lmtk::key_down_event{input_state, lmpl::key_code::K});

    controller.handle(lmtk::key_down_event{input_state, lmpl::key_code::Enter});

    REQUIRE(old_selection == selected_view[0]);

    registry.destroy(selected_view[0]);

    REQUIRE(
      controller.selected_entity_index ==
      registry.view<lmng::name>().size() - 1);

    controller.handle(lmtk::key_down_event{input_state, lmpl::key_code::Enter});

    REQUIRE(!selected_view.empty());

    auto new_entity = registry.create();
    registry.assign<lmng::name>(new_entity, "New entity");

    REQUIRE(!selected_view.empty());
}
