-- ============================================================================
-- METADATA - CPI-SI WezTerm Integration
-- ============================================================================
-- Key: wezterm-cpisi-integration
-- Purpose: Full WezTerm integration for CPI-SI state machine display
-- Biblical: Proverbs 25:11 - "A word fitly spoken is like apples of gold"
-- Authors: Nova Dawn
-- Version: 2.0.0
-- Created: 2025-12-20
--
-- Features:
--   - State-aware tab titles (format-tab-title)
--   - State-aware right status (update-status)
--   - Dynamic background based on command state
--   - CPI-SI key bindings (Leader+s for state, Leader+Shift+A for anchor)
--
-- User Variables (set via OSC 1337 by statusline):
--   SM_POSITION, SM_MEANING, SM_COMMAND, SM_OPERATOR, SM_ANCHOR, SM_X, SM_Y, SM_Z
--
-- ============================================================================

local wezterm = require("wezterm")
local M = {}

-- ============================================================================
-- SETUP - State Configuration
-- ============================================================================

M.states = {
    HALT = {
        foreground = "#ff6b6b",
        tab_bg = "#2a1a1a",
        tab_fg = "#ff6b6b",
        icon = "⬡",
        accent = "#ff4757",
    },
    AWAIT = {
        foreground = "#ffd93d",
        tab_bg = "#2a2a1a",
        tab_fg = "#ffd93d",
        icon = "◇",
        accent = "#feca57",
    },
    PROCEED = {
        foreground = "#6bcb77",
        tab_bg = "#1a2a1a",
        tab_fg = "#6bcb77",
        icon = "▷",
        accent = "#26de81",
    },
}

M.default_state = M.states.AWAIT

-- ============================================================================
-- BODY - Helpers
-- ============================================================================

function M.get_user_var(pane, name, default)
    local vars = pane:get_user_vars()
    local value = vars[name]
    if value and value ~= "" then
        return value
    end
    return default
end

-- ============================================================================
-- BODY - Event Handlers
-- ============================================================================

function M.format_tab_title(tab, tabs, panes, config, hover, max_width)
    local pane = tab.active_pane
    local position = M.get_user_var(pane, "SM_POSITION", "")
    local command = M.get_user_var(pane, "SM_COMMAND", "")
    local operator = M.get_user_var(pane, "SM_OPERATOR", "|")

    -- No CPI-SI state - enhanced default
    if position == "" then
        local title = tostring(tab.tab_index + 1) .. ": "
        title = title .. (tab.active_pane.title ~= "" and tab.active_pane.title or "shell")
        if tab.active_pane.is_zoomed then title = title .. " [Z]" end
        return title
    end

    -- CPI-SI state present
    local state = M.states[command] or M.default_state
    local title = string.format("%s %s %s %s", state.icon, position, operator, command)

    if #title > max_width - 2 then
        title = string.sub(title, 1, max_width - 3) .. "…"
    end

    return {
        { Background = { Color = state.tab_bg } },
        { Foreground = { Color = state.tab_fg } },
        { Text = " " .. title .. " " },
    }
end

function M.update_right_status(window, pane)
    local elements = {}
    local meaning = M.get_user_var(pane, "SM_MEANING", "")
    local anchor = M.get_user_var(pane, "SM_ANCHOR", "")
    local command = M.get_user_var(pane, "SM_COMMAND", "")
    local z = M.get_user_var(pane, "SM_Z", "")

    if meaning ~= "" then
        local state = M.states[command] or M.default_state
        table.insert(elements, { Foreground = { Color = state.foreground } })
        table.insert(elements, { Text = meaning })
        table.insert(elements, { Foreground = { Color = "#888888" } })
        table.insert(elements, { Text = " @ " })
        table.insert(elements, { Foreground = { Color = "#6ec1e4" } })
        table.insert(elements, { Text = anchor })
        if z ~= "" then
            table.insert(elements, { Foreground = { Color = "#888888" } })
            table.insert(elements, { Text = " │ Z:" })
            table.insert(elements, { Foreground = { Color = state.foreground } })
            table.insert(elements, { Text = z })
        end
        table.insert(elements, { Foreground = { Color = "#888888" } })
        table.insert(elements, { Text = " │ " })
    end

    -- Workspace + Time
    table.insert(elements, { Foreground = { Color = "#888888" } })
    table.insert(elements, { Text = window:active_workspace() })
    table.insert(elements, { Text = " │ " })
    table.insert(elements, { Foreground = { Color = "#aaaaaa" } })
    table.insert(elements, { Text = wezterm.strftime("%H:%M") })

    -- Battery
    for _, b in ipairs(wezterm.battery_info()) do
        local bat_color = b.state_of_charge < 0.2 and "#ff6b6b" or (b.state_of_charge < 0.5 and "#ffd93d" or "#6bcb77")
        table.insert(elements, { Foreground = { Color = "#888888" } })
        table.insert(elements, { Text = " │ " })
        table.insert(elements, { Foreground = { Color = bat_color } })
        table.insert(elements, { Text = string.format("🔋%.0f%%", b.state_of_charge * 100) })
    end

    table.insert(elements, { Text = " " })
    window:set_right_status(wezterm.format(elements))
end

-- ============================================================================
-- BODY - Key Bindings
-- ============================================================================

function M.build_keys()
    return {
        -- Leader + s: Show CPI-SI state
        {
            key = "s",
            mods = "LEADER",
            action = wezterm.action_callback(function(window, pane)
                local position = M.get_user_var(pane, "SM_POSITION", "Unknown")
                local meaning = M.get_user_var(pane, "SM_MEANING", "No state")
                local command = M.get_user_var(pane, "SM_COMMAND", "N/A")
                local anchor = M.get_user_var(pane, "SM_ANCHOR", "Genesis 1:1")
                window:toast_notification("CPI-SI State",
                    string.format("%s\n%s @ %s\nCommand: %s", position, meaning, anchor, command), nil, 4000)
            end),
        },
        -- Leader + Shift + A: Show anchor
        {
            key = "A",
            mods = "LEADER|SHIFT",
            action = wezterm.action_callback(function(window, pane)
                window:toast_notification("⚓ Anchor",
                    "\"In the beginning God created the heaven and the earth.\"\n— Genesis 1:1", nil, 5000)
            end),
        },
    }
end

-- ============================================================================
-- BODY - Background Support (Optional)
-- ============================================================================

-- Set these paths to enable state-aware backgrounds
M.backgrounds = {
    HALT = nil,    -- e.g., wezterm.home_dir .. "/.config/wezterm/bg/halt.png"
    AWAIT = nil,
    PROCEED = nil,
}

function M.get_background(command)
    local path = M.backgrounds[command]
    if path then
        return {{ source = { File = path }, hsb = { brightness = 0.02 }, opacity = 0.15 }}
    end
    return nil
end

-- ============================================================================
-- BODY - Apply Configuration
-- ============================================================================

function M.apply(config)
    -- User variables are enabled by default in WezTerm
    config.status_update_interval = 1000

    -- Add keys
    local keys = M.build_keys()
    if config.keys then
        for _, k in ipairs(keys) do table.insert(config.keys, k) end
    else
        config.keys = keys
    end

    return config
end

function M.register_events()
    wezterm.on("format-tab-title", M.format_tab_title)
    wezterm.on("update-right-status", M.update_right_status)
end

function M.setup(config)
    M.apply(config)
    M.register_events()
    return config
end

-- ============================================================================
-- CLOSING
-- ============================================================================

return M
