-- ============================================================================
-- METADATA - CPI-SI WezTerm Integration
-- ============================================================================
-- Key: claude-global-pkg-wezterm-cpisi-lua
-- Purpose: WezTerm Lua configuration for CPI-SI state machine display
-- Biblical: Proverbs 25:11 - "A word fitly spoken is like apples of gold"
-- Authors: Nova Dawn
-- Version: 1.0.0
-- Created: 2025-12-20
--
-- Installation:
--   1. Copy this file to your WezTerm config directory
--   2. In your wezterm.lua, add: require("cpisi-wezterm").apply(config)
--   3. Or copy specific functions into your existing config
--
-- User Variables (set via OSC 1337 by statusline):
--   SM_POSITION - State machine position (e.g., "PRESENT-PRESENT")
--   SM_MEANING  - Position meaning (e.g., "Origin (Genesis 1:1)")
--   SM_COMMAND  - Current command (HALT, AWAIT, PROCEED)
--   SM_OPERATOR - Current operator (|, @, ->, etc.)
--   SM_ANCHOR   - Kingdom anchor (Genesis 1:1)
--   SM_X, SM_Y  - X/Y coordinates
--   SM_Z        - Z depth
--
-- ============================================================================

local wezterm = require("wezterm")
local M = {}

-- ============================================================================
-- SETUP - Command Colors
-- ============================================================================

-- Colors matching the Go implementation
M.command_colors = {
    HALT = {
        foreground = "#ff6b6b",
        background = "#2a1a1a",
        icon = "⬡",
    },
    AWAIT = {
        foreground = "#ffd93d",
        background = "#2a2a1a",
        icon = "◇",
    },
    PROCEED = {
        foreground = "#6bcb77",
        background = "#1a2a1a",
        icon = "▷",
    },
}

-- ============================================================================
-- BODY - Event Handlers
-- ============================================================================

-- Helper: Get user variable with fallback
local function get_user_var(pane, name, default)
    local value = pane:get_user_vars()[name]
    return value or default
end

-- Helper: Get command colors
local function get_cmd_colors(command)
    return M.command_colors[command] or M.command_colors["AWAIT"]
end

-- format-tab-title: Custom tab title from state machine
-- Shows: [icon] POSITION | COMMAND
function M.format_tab_title(tab, tabs, panes, config, hover, max_width)
    local pane = tab.active_pane
    local position = get_user_var(pane, "SM_POSITION", "")
    local command = get_user_var(pane, "SM_COMMAND", "")
    local operator = get_user_var(pane, "SM_OPERATOR", "|")

    -- If no CPI-SI state, fall back to default
    if position == "" then
        return nil -- Let WezTerm use default
    end

    local colors = get_cmd_colors(command)

    -- Build title: icon POSITION op COMMAND
    local title = string.format("%s %s %s %s", colors.icon, position, operator, command)

    -- Truncate if needed
    if #title > max_width then
        title = string.sub(title, 1, max_width - 1) .. "…"
    end

    return {
        { Background = { Color = colors.background } },
        { Foreground = { Color = colors.foreground } },
        { Text = " " .. title .. " " },
    }
end

-- update-status: Right status bar showing state machine info
-- Shows: MEANING @ ANCHOR | Z: depth
function M.update_status(window, pane)
    local meaning = get_user_var(pane, "SM_MEANING", "")
    local anchor = get_user_var(pane, "SM_ANCHOR", "")
    local command = get_user_var(pane, "SM_COMMAND", "AWAIT")
    local z = get_user_var(pane, "SM_Z", "0.0")

    -- If no state, show nothing
    if meaning == "" then
        window:set_right_status("")
        return
    end

    local colors = get_cmd_colors(command)

    -- Build status: MEANING @ ANCHOR | Z: depth
    window:set_right_status(wezterm.format({
        { Foreground = { Color = colors.foreground } },
        { Text = meaning },
        { Foreground = { Color = "#888888" } },
        { Text = " @ " },
        { Foreground = { Color = "#6ec1e4" } },
        { Text = anchor },
        { Foreground = { Color = "#888888" } },
        { Text = " | Z: " },
        { Foreground = { Color = colors.foreground } },
        { Text = z },
        { Text = " " },
    }))
end

-- ============================================================================
-- APPLY - Configuration Application
-- ============================================================================

-- Apply CPI-SI integration to WezTerm config
-- Usage in wezterm.lua:
--   local cpisi = require("cpisi-wezterm")
--   cpisi.apply(config)
function M.apply(config)
    -- Use our format-tab-title
    wezterm.on("format-tab-title", M.format_tab_title)

    -- Use our update-status
    wezterm.on("update-status", M.update_status)

    -- Enable user variables
    config.enable_user_variables = true

    -- Tab bar settings for state machine display
    config.use_fancy_tab_bar = false
    config.tab_bar_at_bottom = true
    config.hide_tab_bar_if_only_one_tab = false

    -- Show right status
    config.status_update_interval = 1000

    return config
end

-- ============================================================================
-- CLOSING
-- ============================================================================

return M
