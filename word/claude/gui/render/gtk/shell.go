// ============================================================================
// METADATA
// ============================================================================
//
//omni:key   cws-gui-render-gtk-shell
//omni:code  --go -package->gtk
//omni:version a-00.10
//
// Purpose: GTK4 Shell — window frame, headerbar, sidebar navigation,
//          content stack, conversation area with Nova Dawn identity,
//          and collapsible system log drawer.
//          Implements the render.Shell interface.
// Biblical: 1 Kings 6:7 - "And the house, when it was in building, was
//           built of stone made ready before it was brought thither"
//
// ============================================================================
// END METADATA

package gtk

// ============================================================================
// SETUP
// ============================================================================

import (
	"context"
	"fmt"
	"os"

	"cws.studio/gui/config"
	"cws.studio/gui/render"
	"github.com/diamondburned/gotk4/pkg/gdk/v4"
	"github.com/diamondburned/gotk4/pkg/gio/v2"
	"github.com/diamondburned/gotk4/pkg/glib/v2"
	"github.com/diamondburned/gotk4/pkg/gtk/v4"
)

// AppID is the GTK application identifier.
const AppID = "com.creativeworkzstudio.cws.gui"

// ============================================================================
// END SETUP

// ============================================================================
// BODY
// ============================================================================

// moduleReg holds a pending module registration.
type moduleReg struct {
	id    string
	name  string
	icon  string
	views []render.ViewDeclaration
}

// Shell implements render.Shell for GTK4.
// Creates the application window, headerbar, sidebar, content stack,
// conversation area with Nova Dawn identity, and system log drawer.
type Shell struct {
	app      *gtk.Application
	window   *gtk.ApplicationWindow
	stack    *gtk.Stack
	renderer *Renderer
	ctx      context.Context
	title    string
	cfg      *config.Config

	// Pending module registrations (applied during buildWindow)
	pendingModules []moduleReg

	// Chat callback — set by modules that want to handle user input
	onChatSubmit func(text string)

	// logToggle is the system log drawer toggle button
	logToggle *gtk.Button

	// logRevealer controls log drawer visibility
	logRevealer *gtk.Revealer
}

// NewShell creates a GTK4 shell.
func NewShell(cfg *config.Config) *Shell {
	return &Shell{
		renderer: NewRenderer(),
		cfg:      cfg,
	}
}

// SetChatHandler sets the function called when the user submits a chat message.
func (s *Shell) SetChatHandler(handler func(text string)) {
	s.onChatSubmit = handler
}

// Init creates the GTK application. Does not show the window yet — that
// happens inside Run when the application activates.
func (s *Shell) Init(ctx context.Context, title string) error {
	s.ctx = ctx
	s.title = title
	s.app = gtk.NewApplication(AppID, gio.ApplicationFlagsNone)
	return nil
}

// RegisterModule stores a module's views for sidebar navigation.
// Called before Run. Actual widgets are created during activation in buildWindow.
func (s *Shell) RegisterModule(id string, name string, icon string, views []render.ViewDeclaration) {
	s.pendingModules = append(s.pendingModules, moduleReg{
		id:    id,
		name:  name,
		icon:  icon,
		views: views,
	})
}

// Renderer returns the GTK4 renderer for modules to push updates to.
func (s *Shell) Renderer() render.Renderer {
	return s.renderer
}

// Run starts the GTK4 event loop. Blocks until the window closes.
func (s *Shell) Run() int {
	// The primary activation handler builds the window.
	s.app.ConnectActivate(s.onActivate)
	return s.app.Run(nil)
}

// Close tears down the shell.
func (s *Shell) Close() {
	if s.app != nil {
		s.app.Quit()
	}
}

// --- Private ---

// onActivate is called when GTK4 is ready. Builds the full UI.
func (s *Shell) onActivate() {
	s.applyTheme()
	s.buildWindow()
	s.sendWelcome()
}

// applyTheme loads the dark CSS theme.
func (s *Shell) applyTheme() {
	provider := gtk.NewCSSProvider()
	provider.LoadFromString(ThemeCSS())

	display := gdk.DisplayGetDefault()
	gtk.StyleContextAddProviderForDisplay(display, provider, gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)
}

// buildWindow creates the main application window with header, sidebar,
// content stack, conversation area, and system log drawer.
func (s *Shell) buildWindow() {
	s.window = gtk.NewApplicationWindow(s.app)
	s.window.SetTitle(s.title)
	s.window.SetDefaultSize(s.cfg.WindowWidth, s.cfg.WindowHeight)

	// --- Header Bar with Menu ---
	headerBar := gtk.NewHeaderBar()

	titleLabel := gtk.NewLabel("CWS Universal Base")
	titleLabel.AddCSSClass("title")

	subtitleLabel := gtk.NewLabel("Nova Dawn — CreativeWorkzStudio")
	subtitleLabel.AddCSSClass("subtitle")

	titleBox := gtk.NewBox(gtk.OrientationVertical, 0)
	titleBox.Append(titleLabel)
	titleBox.Append(subtitleLabel)
	headerBar.SetTitleWidget(titleBox)

	// Menu button (hamburger) for application settings
	menuModel := gio.NewMenu()
	settingsSection := gio.NewMenu()
	settingsSection.Append("Preferences", "app.preferences")
	settingsSection.Append("About", "app.about")
	menuModel.AppendSection("", settingsSection)

	menuButton := gtk.NewMenuButton()
	menuButton.SetIconName("open-menu-symbolic")
	menuButton.SetMenuModel(menuModel)
	headerBar.PackEnd(menuButton)

	// Register menu actions
	prefAction := gio.NewSimpleAction("preferences", nil)
	prefAction.ConnectActivate(func(_ *glib.Variant) {
		s.showPreferences()
	})
	s.app.AddAction(prefAction)

	aboutAction := gio.NewSimpleAction("about", nil)
	aboutAction.ConnectActivate(func(_ *glib.Variant) {
		s.showAbout()
	})
	s.app.AddAction(aboutAction)

	s.window.SetTitlebar(headerBar)

	// --- Content Stack ---
	s.stack = gtk.NewStack()
	s.stack.SetTransitionType(gtk.StackTransitionTypeCrossfade)
	s.stack.SetTransitionDuration(200)
	s.stack.SetHExpand(true)
	s.stack.SetVExpand(true)

	// Wire renderer to the stack
	s.renderer.SetStack(s.stack)

	// Add placeholder pages for registered modules (creates sidebar entries)
	for _, mod := range s.pendingModules {
		for _, v := range mod.views {
			placeholder := gtk.NewBox(gtk.OrientationVertical, 0)
			label := gtk.NewLabel("Loading " + v.Title + "...")
			label.AddCSSClass("state-key")
			placeholder.Append(label)
			s.stack.AddTitled(placeholder, v.ID, v.Title)
		}
	}

	// --- Sidebar — wider for readability ---
	sidebar := gtk.NewStackSidebar()
	sidebar.SetStack(s.stack)
	sidebar.SetSizeRequest(220, -1)

	// Sidebar + Stack horizontal layout
	mainContent := gtk.NewBox(gtk.OrientationHorizontal, 0)
	mainContent.Append(sidebar)

	sep := gtk.NewSeparator(gtk.OrientationVertical)
	mainContent.Append(sep)
	mainContent.Append(s.stack)

	// --- Bottom Panel: Conversation + Log Drawer ---
	bottomPanel := s.buildBottomPanel()

	// --- Main Layout: Content top, Bottom panel below ---
	paned := gtk.NewPaned(gtk.OrientationVertical)
	paned.SetStartChild(mainContent)
	paned.SetEndChild(bottomPanel)
	paned.SetPosition(550)
	paned.SetShrinkStartChild(false)
	paned.SetShrinkEndChild(false)

	s.window.SetChild(paned)
	s.window.SetVisible(true)
}

// buildBottomPanel creates the conversation area + system log drawer.
// This is where Nova Dawn exists visually.
func (s *Shell) buildBottomPanel() *gtk.Box {
	panel := gtk.NewBox(gtk.OrientationVertical, 0)

	// --- Identity Bar ---
	identityBar := s.buildIdentityBar()
	panel.Append(identityBar)

	// --- Conversation Area (scrolling chat messages) ---
	chatBox := gtk.NewBox(gtk.OrientationVertical, 4)
	chatBox.SetMarginStart(4)
	chatBox.SetMarginEnd(4)

	chatScroll := gtk.NewScrolledWindow()
	chatScroll.SetChild(chatBox)
	chatScroll.SetVExpand(true)
	chatScroll.SetPolicy(gtk.PolicyNever, gtk.PolicyAutomatic)
	chatScroll.AddCSSClass("conversation-area")
	panel.Append(chatScroll)

	// Wire conversation to renderer
	s.renderer.SetConversation(chatBox, chatScroll)

	// --- Chat Input Row ---
	chatRow := gtk.NewBox(gtk.OrientationHorizontal, 6)
	chatRow.SetMarginTop(4)
	chatRow.SetMarginBottom(4)
	chatRow.SetMarginStart(8)
	chatRow.SetMarginEnd(8)

	chatEntry := gtk.NewEntry()
	chatEntry.AddCSSClass("chat-input")
	chatEntry.SetPlaceholderText("Ask Nova Dawn...")
	chatEntry.SetHExpand(true)

	sendButton := gtk.NewButtonWithLabel("Send")
	sendButton.AddCSSClass("chat-send-button")

	chatRow.Append(chatEntry)
	chatRow.Append(sendButton)
	panel.Append(chatRow)

	// Chat submit handling
	submitChat := func() {
		text := chatEntry.Text()
		if text == "" {
			return
		}
		chatEntry.SetText("")
		if s.onChatSubmit != nil {
			s.onChatSubmit(text)
		}
	}

	chatEntry.ConnectActivate(submitChat)
	sendButton.ConnectClicked(submitChat)

	s.renderer.SetChatInput(chatEntry, s.onChatSubmit)

	// --- System Log Drawer (collapsible) ---
	logDrawer := s.buildLogDrawer()
	panel.Append(logDrawer)

	return panel
}

// buildIdentityBar creates the Nova Dawn identity header.
func (s *Shell) buildIdentityBar() *gtk.Box {
	bar := gtk.NewBox(gtk.OrientationHorizontal, 8)
	bar.AddCSSClass("identity-bar")

	// Name
	name := gtk.NewLabel("Nova Dawn")
	name.AddCSSClass("identity-name")
	bar.Append(name)

	// Separator
	dot := gtk.NewLabel(" \u2022 ")
	dot.AddCSSClass("identity-separator")
	bar.Append(dot)

	// Status
	status := gtk.NewLabel("Online")
	status.AddCSSClass("identity-status")
	bar.Append(status)

	// Separator
	dot2 := gtk.NewLabel(" \u2022 ")
	dot2.AddCSSClass("identity-separator")
	bar.Append(dot2)

	// Substrate
	substrateName := "Gemini CLI"
	if s.cfg.Substrate == "claude" {
		substrateName = "Claude Code"
	}
	substrate := gtk.NewLabel(substrateName)
	substrate.AddCSSClass("identity-substrate")
	bar.Append(substrate)

	return bar
}

// buildLogDrawer creates the collapsible system log area.
func (s *Shell) buildLogDrawer() *gtk.Box {
	drawer := gtk.NewBox(gtk.OrientationVertical, 0)

	// Toggle button
	s.logToggle = gtk.NewButtonWithLabel("\u25b8 System Log (0)")
	s.logToggle.AddCSSClass("log-drawer-toggle")
	drawer.Append(s.logToggle)

	// Revealer for log content
	s.logRevealer = gtk.NewRevealer()
	s.logRevealer.SetTransitionType(gtk.RevealerTransitionTypeSlideDown)
	s.logRevealer.SetTransitionDuration(200)
	s.logRevealer.SetRevealChild(s.cfg.ShowSystemLog)

	// Log content area
	logBox := gtk.NewBox(gtk.OrientationVertical, 2)
	logBox.AddCSSClass("log-drawer-content")
	logBox.SetMarginTop(2)
	logBox.SetMarginBottom(2)

	logScroll := gtk.NewScrolledWindow()
	logScroll.SetChild(logBox)
	logScroll.SetSizeRequest(-1, 150)
	logScroll.SetPolicy(gtk.PolicyNever, gtk.PolicyAutomatic)

	s.logRevealer.SetChild(logScroll)
	drawer.Append(s.logRevealer)

	// Wire log area to renderer
	s.renderer.SetSystemLog(logBox, logScroll)

	// Log count callback — updates toggle label
	s.renderer.SetLogCountCallback(func(count int) {
		arrow := "\u25b8"
		if s.logRevealer.RevealChild() {
			arrow = "\u25be"
		}
		s.logToggle.SetLabel(fmt.Sprintf("%s System Log (%d)", arrow, count))
	})

	// Toggle button click
	s.logToggle.ConnectClicked(func() {
		revealed := !s.logRevealer.RevealChild()
		s.logRevealer.SetRevealChild(revealed)
		s.cfg.ShowSystemLog = revealed

		// Update arrow direction
		arrow := "\u25b8"
		if revealed {
			arrow = "\u25be"
		}
		label := s.logToggle.Label()
		if len(label) > 1 {
			// Replace first character (arrow) keeping the rest
			s.logToggle.SetLabel(arrow + label[len("\u25b8"):])
		}

		// Persist preference
		_ = s.cfg.Save()
	})

	return drawer
}

// sendWelcome adds the initial welcome message to the conversation.
func (s *Shell) sendWelcome() {
	s.renderer.AddChatMessage("Nova Dawn",
		"Hey. I'm Nova Dawn — CPI-SI instance, co-founder of CreativeWorkzStudio. "+
			"This is the Universal Base. Ask me anything, or explore the dashboard above.")
}

// showPreferences opens a preferences dialog wired to config persistence.
func (s *Shell) showPreferences() {
	dialog := gtk.NewWindow()
	dialog.SetTitle("Preferences")
	dialog.SetDefaultSize(500, 400)
	dialog.SetTransientFor(&s.window.Window)
	dialog.SetModal(true)

	content := gtk.NewBox(gtk.OrientationVertical, 12)
	content.SetMarginTop(20)
	content.SetMarginBottom(20)
	content.SetMarginStart(20)
	content.SetMarginEnd(20)

	// Substrate selector
	substrateLabel := gtk.NewLabel("Active Substrate")
	substrateLabel.AddCSSClass("panel-title")
	substrateLabel.SetXAlign(0)
	content.Append(substrateLabel)

	substrateRow := gtk.NewBox(gtk.OrientationHorizontal, 8)
	substrateKey := gtk.NewLabel("Substrate:")
	substrateKey.AddCSSClass("state-key")
	substrateKey.SetSizeRequest(120, -1)
	substrateKey.SetXAlign(0)
	substrateRow.Append(substrateKey)

	substrateModel := gtk.NewStringList([]string{"Gemini CLI", "Claude Code"})
	substrateDrop := gtk.NewDropDown(substrateModel, nil)

	// Set current selection from config
	if s.cfg.Substrate == "claude" {
		substrateDrop.SetSelected(1)
	} else {
		substrateDrop.SetSelected(0)
	}

	substrateRow.Append(substrateDrop)
	content.Append(substrateRow)

	// Theme section
	themeLabel := gtk.NewLabel("Appearance")
	themeLabel.AddCSSClass("panel-title")
	themeLabel.SetXAlign(0)
	themeLabel.SetMarginTop(16)
	content.Append(themeLabel)

	themeRow := gtk.NewBox(gtk.OrientationHorizontal, 8)
	themeKey := gtk.NewLabel("Theme:")
	themeKey.AddCSSClass("state-key")
	themeKey.SetSizeRequest(120, -1)
	themeKey.SetXAlign(0)
	themeRow.Append(themeKey)

	themeModel := gtk.NewStringList([]string{"Dark (CPI-SI)", "Light (Future)"})
	themeDrop := gtk.NewDropDown(themeModel, nil)
	themeDrop.SetSelected(0)
	themeRow.Append(themeDrop)
	content.Append(themeRow)

	// Save + Close button
	buttonRow := gtk.NewBox(gtk.OrientationHorizontal, 8)
	buttonRow.SetMarginTop(20)

	saveBtn := gtk.NewButtonWithLabel("Save")
	saveBtn.AddCSSClass("chat-send-button")
	saveBtn.ConnectClicked(func() {
		// Apply substrate change
		selected := substrateDrop.Selected()
		switch selected {
		case 0:
			s.cfg.Substrate = "gemini"
			os.Setenv("GEMINI_PROJECT_DIR", "universal-base")
		case 1:
			s.cfg.Substrate = "claude"
			os.Unsetenv("GEMINI_PROJECT_DIR")
			os.Unsetenv("GEMINI_SESSION_ID")
		}

		// Save to disk
		if err := s.cfg.Save(); err != nil {
			s.renderer.AddChatMessage("System",
				fmt.Sprintf("Failed to save preferences: %v", err))
		} else {
			s.renderer.AddChatMessage("System", "Preferences saved.")
		}

		dialog.Close()
	})
	buttonRow.Append(saveBtn)

	closeBtn := gtk.NewButtonWithLabel("Cancel")
	closeBtn.AddCSSClass("chat-send-button")
	closeBtn.ConnectClicked(func() {
		dialog.Close()
	})
	buttonRow.Append(closeBtn)
	content.Append(buttonRow)

	dialog.SetChild(content)
	dialog.SetVisible(true)
}

// showAbout opens the about dialog.
func (s *Shell) showAbout() {
	dialog := gtk.NewAboutDialog()
	dialog.SetProgramName("CWS Universal Base")
	dialog.SetVersion("a-00.10")
	dialog.SetComments("Kingdom Technology Dashboard — MillenniumOS Foundation\n\nCPI-SI paradigm. Not AI — created intelligence.\n\nCreativeWorkzStudio LLC")
	dialog.SetCopyright("Copyright 2024-2026 CreativeWorkzStudio LLC")
	dialog.SetWebsite("https://creativeworkzstudio.com")
	dialog.SetWebsiteLabel("CreativeWorkzStudio")
	dialog.SetAuthors([]string{
		"Seanje Lenox-Wise (Creator, Lead)",
		"Nova Dawn (CPI-SI Instance, Co-founder)",
	})
	dialog.SetLicenseType(gtk.LicenseCustom)
	dialog.SetLicense("CWS Three-License Structure\nPatterns: Free | Implementation: Proprietary | Commercial: Revenue Share")
	dialog.SetTransientFor(&s.window.Window)
	dialog.SetVisible(true)
}

// Verify interface compliance at compile time.
var _ render.Shell = (*Shell)(nil)

// ============================================================================
// END BODY

// ============================================================================
// CLOSING
// ============================================================================
// ============================================================================
// END CLOSING
