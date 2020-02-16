#pragma once

#include <lmlib/enumerate.h>
#include <range/v3/action/sort.hpp>
#include <range/v3/view/enumerate.hpp>

namespace lmtk
{
class table
{
  public:
    template <typename range_type>
    table(range_type &range, lm::point2i position = {0, 0})
    {
        std::vector<int> column_widths{};
        std::vector<int> row_heights{};
        for (auto [i, row] : ranges::view::enumerate(range))
        {
            row_heights.emplace_back(0);
            for (auto [j, col] : ranges::view::enumerate(row))
            {
                column_widths.emplace_back(0);
                auto col_size = col.get_size();
                column_widths[j] = std::max(column_widths[j], col_size.width);
                row_heights[i] = std::max(row_heights[i], col_size.height);
            }
        }

        int ypos{0};
        for (auto [i, row] : ranges::view::enumerate(range))
        {
            if (i)
                ypos += row_heights[i - 1] + row_padding;

            int xpos{0};
            for (auto [j, col] : ranges::view::enumerate(row))
            {
                if (j)
                    xpos += column_widths[j - 1] + column_padding;

                col.set_position(position + lm::point2i{xpos, ypos});
            }
        }
    }

  private:
    int row_padding{25}, column_padding{25};
};
} // namespace lmtk
