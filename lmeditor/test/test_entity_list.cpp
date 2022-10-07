#include "../src/controller/entity_list/entity_list_controller.h"
#include <catch2/catch.hpp>
#include <lmng/hierarchy.h>
#include <lmng/logging.h>
#include <model/selection.h>
#include <spdlog/spdlog.h>

TEST_CASE("Entity list controller")
{
    entt::registry registry;
    lmng::connect_component_logging(registry);
    lmng::hierarchy_system hierarchy_system{registry};
    auto selected_view = registry.view<lmeditor::selected>();

    lmeditor::entity_list_controller controller{registry};
    lmtk::input_state input_state;

    auto parent = registry.create();
    auto child = registry.create();

    registry.emplace<lmng::name>(parent, "Parent");
    registry.emplace<lmng::name>(child, "Child");

    registry.emplace<lmng::parent>(child, parent);

    auto other = registry.create();
    registry.emplace<lmng::name>(other, "Other");

    REQUIRE(selected_view.empty());

    SPDLOG_INFO("Test: Selecting first entity in list.");
    controller.handle(
      lmtk::key_down_event{input_state, lmpl::key_code::Enter}, nullptr);

    REQUIRE(!selected_view.empty());

    for (auto i : lm::range(2))
    {
        SPDLOG_INFO("Test: Moving selection down one.");
        controller.handle(
          lmtk::key_down_event{input_state, lmpl::key_code::K}, nullptr);

        SPDLOG_INFO("Test: Selecting entity {}", i + 1);

        controller.handle(
          lmtk::key_down_event{input_state, lmpl::key_code::Enter}, nullptr);

        REQUIRE(!selected_view.empty());
    }

    REQUIRE(
      controller.selected_entity_index ==
      registry.view<lmng::name>().size() - 1);

    SPDLOG_INFO("Test: Move selection down already at bottom");
    auto old_selection = selected_view[0];

    controller.handle(
      lmtk::key_down_event{input_state, lmpl::key_code::K}, nullptr);

    controller.handle(
      lmtk::key_down_event{input_state, lmpl::key_code::Enter}, nullptr);

    REQUIRE(old_selection == selected_view[0]);

    SPDLOG_INFO("Test: Removing an entity with last entity selected");
    registry.destroy(selected_view[0]);

    REQUIRE(
      controller.selected_entity_index ==
      registry.view<lmng::name>().size() - 1);

    SPDLOG_INFO("Test: selecting highlighted entity");
    controller.handle(
      lmtk::key_down_event{input_state, lmpl::key_code::Enter}, nullptr);

    REQUIRE(!selected_view.empty());

    SPDLOG_INFO("Test: Creating an entity with the last entity selected");
    auto new_entity = registry.create();
    registry.emplace<lmng::name>(new_entity, "New entity");

    REQUIRE(!selected_view.empty());
}
