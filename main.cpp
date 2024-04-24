#include <array>
#include <cstdint>
#include <cstdio>
#include <format>
#include <random>
#include <string>
#include <vector>

#include <Windows.h>
#include <dwmapi.h>

const static std::array<std::uint32_t, 7> colors = {
        0xE70000,
        0xE77000,
        0xE7D800,
        0x00E71D,
        0x0077E7,
        0x7E00E7,
        0xC900E7
};

bool is_empty_wchar(wchar_t ch) {
    return ch == L'\0' || ch == L' ';
}

bool is_empty(
        std::uint16_t cell_x,
        std::uint16_t cell_y,
        std::uint16_t width,
        const std::vector<wchar_t>& screen
        ) {
    size_t left_coord = (cell_y * width) + (2 * cell_x);

    if(is_empty_wchar(screen[left_coord])) {
        if((cell_x + 1) * 2 <= width) {
            size_t right_coord = left_coord + 1;
            return is_empty_wchar(screen[right_coord]);
        } else return true;
    } else return false;
}

bool in_circle(
        std::uint16_t cell_x,
        std::uint16_t cell_y,
        std::uint16_t center_x,
        std::uint16_t center_y,
        std::uint32_t rad) {
    std::uint32_t h_distance = (cell_x >= center_x) ? cell_x - center_x : center_x - cell_x;
    std::uint32_t v_distance = (cell_y >= center_y) ? cell_y - center_y : center_y - cell_y;
    std::uint32_t dist_sq = (h_distance * h_distance) + (v_distance * v_distance);

    if(dist_sq < (rad * rad)) return true;

    return false;
}

bool new_in_circle(
        std::uint16_t cell_x,
        std::uint16_t cell_y,
        std::uint16_t center_x,
        std::uint16_t center_y,
        std::uint32_t rad,
        std::uint16_t ncells_w,
        const std::vector<bool>& already) {
    if(already[(cell_y * ncells_w) + cell_x]) return false;

    return in_circle(cell_x, cell_y, center_x, center_y, rad);
}

std::wstring color_radius(
        std::uint32_t r,
        std::uint32_t color,
        std::uint16_t cx,
        std::uint16_t cy,
        std::uint16_t ncells_w,
        std::uint16_t ncells_h,
        std::uint16_t width,
        std::vector<bool>& colored,
        const std::vector<wchar_t>& screen) {
    std::uint8_t red = (color >> 16) & 0xFF;
    std::uint8_t green = (color >> 8) & 0xFF;
    std::uint8_t blue = (color >> 0) & 0xFF;

    std::vector<wchar_t> builder;

    std::uint16_t rt = (r > cy) ? 0 : cy - r;
    std::uint16_t rb = ((cy + r) >= ncells_h) ? (ncells_h - 1) : cy + r;
    std::uint16_t rl = (r > cx) ? 0 : cx - r;
    std::uint16_t rr = ((cx + r) >= ncells_w) ? (ncells_w - 1) : cx + r;

    for(std::uint16_t cell_y = rt; cell_y <= rb; cell_y++) {
        for (std::uint16_t cell_x = rl; cell_x <= rr; cell_x++) {
            if(is_empty(cell_x, cell_y, width, screen)) continue;
            if(!new_in_circle(cell_x, cell_y, cx, cy, r, ncells_w, colored)) continue;

            std::wstring go_here_set_color = std::format(
                    L"\033[{};{}H\033[48;2;{};{};{}m\033[38;2;0;0;0m",
                    1 + cell_y,
                    1 + (cell_x * 2),
                    red,
                    green,
                    blue
                    );
            builder.insert(builder.end(), go_here_set_color.begin(), go_here_set_color.end());

            uint32_t start_offs = (cell_y * static_cast<uint32_t>(width)) + (cell_x * 2);
            uint32_t end_offs = start_offs;
            while(cell_x <= rr &&
            !is_empty(cell_x, cell_y, width, screen) &&
            new_in_circle(cell_x, cell_y, cx, cy, r, ncells_w, colored)) {
                colored[(cell_y * ncells_w) + cell_x] = true;
                cell_x++;
                // handle odd widths
                if(cell_x * 2 > width) end_offs += 1;
                else end_offs += 2;
            }
            builder.insert(builder.end(), screen.begin() + start_offs, screen.begin() + end_offs);
        }
    }

    return { builder.data(), builder.size() };
}

std::wstring clear_radius(
        std::uint32_t r,
        std::uint16_t cx,
        std::uint16_t cy,
        std::uint16_t ncells_w,
        std::uint16_t ncells_h,
        std::uint16_t width) {
    std::vector<wchar_t> builder;

    std::uint16_t rt = (r > cy) ? 0 : cy - r;
    std::uint16_t rb = ((cy + r) >= ncells_h) ? (ncells_h - 1) : cy + r;
    std::uint16_t rl = (r > cx) ? 0 : cx - r;
    std::uint16_t rr = ((cx + r) >= ncells_w) ? (ncells_w - 1) : cx + r;

    for(std::uint16_t cell_y = rt; cell_y <= rb; cell_y++) {
        for (std::uint16_t cell_x = rl; cell_x <= rr; cell_x++) {
            if(!in_circle(cell_x, cell_y, cx, cy, r)) continue;

            std::wstring go_here_reset = std::format(
                    L"\033[{};{}H\033[0m",
                    1 + cell_y,
                    1 + (cell_x * 2)
            );
            builder.insert(builder.end(), go_here_reset.begin(), go_here_reset.end());

            uint32_t chars_to_clear = 0;
            while(cell_x <= rr && in_circle(cell_x, cell_y, cx, cy, r)) {
                cell_x++;
                // handle odd widths
                if(cell_x * 2 > width) chars_to_clear += 1;
                else chars_to_clear += 2;
            }
            std::wstring write_spaces = std::format(
                    L" \033[{}b",
                    chars_to_clear - 1
                    );
            builder.insert(builder.end(), write_spaces.begin(), write_spaces.end());
        }
    }

    return { builder.data(), builder.size() };
}

int main() {
    HANDLE conout = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;

    bool i = GetConsoleScreenBufferInfo(conout, &csbi);

    if(i == 0) {
        std::fprintf(stderr, "Encountered an error getting console info: %lu\n", GetLastError());
        std::fprintf(stderr, "Likely not a TTY\n");
        return 1;
    }

    if(csbi.srWindow.Top == csbi.srWindow.Bottom) {
        std::fprintf(stderr, "Console has no height\n");
        std::fprintf(stderr, "Likely not a TTY\n");
        return 1;
    }

    std::uint16_t width = 1 + (csbi.srWindow.Right - csbi.srWindow.Left);
    std::uint16_t height = 1 + (csbi.srWindow.Bottom - csbi.srWindow.Top);

    std::vector<wchar_t> screen;
    screen.reserve(width * height);

    for(std::uint16_t y = 0; y < height; y++) {
        DWORD num_read;
        ReadConsoleOutputCharacterW(
                conout,
                &(screen[y * width]),
                width,
                { .X = 0, .Y = static_cast<SHORT>(y) },
                &num_read
        );
    }

    DWORD written;

    // hide cursor
    std::wstring dectcem = L"\033[?25l";
    WriteConsoleW(conout, dectcem.data(), dectcem.size(), &written, nullptr);

    // color two-wide cells
    std::uint16_t ncell_w = (width + 1) / 2;
    std::uint16_t ncell_h = height;

    std::vector<bool> colored;
    colored.resize(ncell_w * ncell_h, false);

    // pick a center cell
    std::random_device rd;
    std::uniform_int_distribution<std::uint16_t> random_cell_x(0, ncell_w - 1);
    std::uniform_int_distribution<std::uint16_t> random_cell_y(0, ncell_h - 1);
    std::uint16_t startx = random_cell_x(rd);
    std::uint16_t starty = random_cell_y(rd);

    std::uint32_t dist_t = starty;
    std::uint32_t dist_r = ncell_w - startx;
    std::uint32_t dist_b = ncell_h - starty;
    std::uint32_t dist_l = startx;

    std::uint32_t max_v = std::max(dist_t, dist_b);
    std::uint32_t max_h = std::max(dist_l, dist_r);

    std::uint32_t max_r2 = (max_v * max_v) + (max_h * max_h);

    std::uint32_t r = 1;
    std::uint32_t w = 5;
    std::uint32_t inr = 0;
    while(inr * inr <= max_r2) {
        // r is our radius to color
        std::wstring color_cmds = color_radius(
                r,
                colors[r % colors.size()],
                startx,
                starty,
                ncell_w,
                ncell_h,
                width,
                colored,
                screen);

        // inr is our radius to clear
        std::wstring clear_commands;
        if(inr > 0) {
            clear_commands = clear_radius(inr, startx, starty, ncell_w, ncell_h, width);
        }

        std::wstring whole_command = std::format(L"{}{}", color_cmds, clear_commands);

        WriteConsoleW(conout, whole_command.data(), whole_command.size(), &written, nullptr);

        r += 1;
        if(r == 8 || r == 12) w += 1;
        inr = (r > w) ? r - w : 0;

        DwmFlush();
    }

    WriteConsoleW(conout, L"\033c", 2, &written, nullptr);

    return 0;
}
