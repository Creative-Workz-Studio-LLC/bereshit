// ============================================================================
// METADATA - CPI-SI Dashboard GTK4 System Data Panel
// ============================================================================
//
// Purpose: File browser for system/data/ directory tree
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ============================================================================

package panels

// ============================================================================
// SETUP
// ============================================================================

import (
	"fmt"
	"path/filepath"
	"strings"

	"cws.studio/pkg/dashboard"
	"github.com/diamondburned/gotk4/pkg/gtk/v4"
)

// ============================================================================
// BODY
// ============================================================================

// SystemData provides a file browser for the system/data directory.
type SystemData struct {
	box *gtk.Box
	svc *dashboard.DashboardService

	// Navigation
	pathLabel *gtk.Label
	fileList  *gtk.Box

	// Content viewer
	contentView   *gtk.TextView
	contentBuffer *gtk.TextBuffer

	currentPath string
}

// NewSystemData creates the system data browser panel.
func NewSystemData() *SystemData {
	s := &SystemData{
		currentPath: "",
	}
	s.build()
	return s
}

// SetService provides the dashboard service for data access.
func (s *SystemData) SetService(svc *dashboard.DashboardService) {
	s.svc = svc
	s.navigateTo("")
}

// Widget returns the root widget.
func (s *SystemData) Widget() gtk.Widgetter {
	return s.box
}

// build constructs the system data browser layout.
func (s *SystemData) build() {
	s.box = gtk.NewBox(gtk.OrientationVertical, 0)
	s.box.SetMarginTop(12)
	s.box.SetMarginBottom(12)
	s.box.SetMarginStart(12)
	s.box.SetMarginEnd(12)

	// Path bar
	pathBar := gtk.NewBox(gtk.OrientationHorizontal, 8)
	pathBar.SetMarginBottom(8)

	pathTitle := gtk.NewLabel("Path:")
	pathTitle.AddCSSClass("state-key")
	pathBar.Append(pathTitle)

	s.pathLabel = gtk.NewLabel("system/data/")
	s.pathLabel.SetXAlign(0)
	s.pathLabel.SetHExpand(true)
	s.pathLabel.AddCSSClass("state-value")
	pathBar.Append(s.pathLabel)

	upBtn := gtk.NewButtonWithLabel("Up")
	upBtn.ConnectClicked(func() {
		if s.currentPath != "" {
			parent := filepath.Dir(s.currentPath)
			if parent == "." {
				parent = ""
			}
			s.navigateTo(parent)
		}
	})
	pathBar.Append(upBtn)

	s.box.Append(pathBar)

	sep := gtk.NewSeparator(gtk.OrientationHorizontal)
	s.box.Append(sep)

	// Split: file list left, content viewer right
	paned := gtk.NewPaned(gtk.OrientationHorizontal)
	paned.SetVExpand(true)
	paned.SetHExpand(true)

	// File list
	listScroll := gtk.NewScrolledWindow()
	listScroll.SetPolicy(gtk.PolicyNever, gtk.PolicyAutomatic)
	listScroll.SetSizeRequest(250, -1)

	s.fileList = gtk.NewBox(gtk.OrientationVertical, 2)
	s.fileList.SetMarginTop(4)
	s.fileList.SetMarginStart(4)
	s.fileList.SetMarginEnd(4)

	emptyLabel := gtk.NewLabel("No data available")
	emptyLabel.AddCSSClass("state-label")
	emptyLabel.SetXAlign(0)
	s.fileList.Append(emptyLabel)

	listScroll.SetChild(s.fileList)
	paned.SetStartChild(listScroll)

	// Content viewer
	s.contentBuffer = gtk.NewTextBuffer(nil)
	s.contentView = gtk.NewTextViewWithBuffer(s.contentBuffer)
	s.contentView.SetEditable(false)
	s.contentView.SetCursorVisible(false)
	s.contentView.SetMonospace(true)
	s.contentView.SetWrapMode(gtk.WrapWordChar)
	s.contentView.SetTopMargin(8)
	s.contentView.SetBottomMargin(8)
	s.contentView.SetLeftMargin(8)
	s.contentView.SetRightMargin(8)

	contentScroll := gtk.NewScrolledWindow()
	contentScroll.SetPolicy(gtk.PolicyAutomatic, gtk.PolicyAutomatic)
	contentScroll.SetChild(s.contentView)
	paned.SetEndChild(contentScroll)

	paned.SetPosition(250)
	s.box.Append(paned)
}

// navigateTo changes the current directory and refreshes the file list.
func (s *SystemData) navigateTo(path string) {
	if s.svc == nil {
		return
	}

	s.currentPath = path
	displayPath := "system/data/"
	if path != "" {
		displayPath += path + "/"
	}
	s.pathLabel.SetText(displayPath)

	entry, err := s.svc.SystemData(path)
	if err != nil {
		s.contentBuffer.SetText(fmt.Sprintf("Error: %v", err))
		return
	}

	// Clear file list
	for child := s.fileList.FirstChild(); child != nil; child = s.fileList.FirstChild() {
		s.fileList.Remove(child)
	}

	children := entry.Children
	if len(children) == 0 {
		empty := gtk.NewLabel("(empty directory)")
		empty.AddCSSClass("state-label")
		empty.SetXAlign(0)
		s.fileList.Append(empty)
		return
	}

	for _, child := range children {
		childCopy := child
		row := gtk.NewBox(gtk.OrientationHorizontal, 4)

		icon := "  "
		if child.IsDir {
			icon = "/ "
		}

		iconLabel := gtk.NewLabel(icon)
		row.Append(iconLabel)

		nameBtn := gtk.NewButtonWithLabel(child.Name)
		nameBtn.SetHasFrame(false)
		nameBtn.SetHExpand(true)

		nameBtn.ConnectClicked(func() {
			entryPath := childCopy.Name
			if s.currentPath != "" {
				entryPath = s.currentPath + "/" + childCopy.Name
			}

			if childCopy.IsDir {
				s.navigateTo(entryPath)
			} else {
				s.viewFile(entryPath)
			}
		})
		row.Append(nameBtn)

		if !child.IsDir {
			sizeLabel := gtk.NewLabel(formatFileSize(child.Size))
			sizeLabel.AddCSSClass("state-key")
			row.Append(sizeLabel)
		}

		s.fileList.Append(row)
	}
}

// viewFile reads and displays a file's contents.
func (s *SystemData) viewFile(path string) {
	if s.svc == nil {
		return
	}

	entry, err := s.svc.SystemData(path)
	if err != nil {
		s.contentBuffer.SetText(fmt.Sprintf("Error reading %s: %v", path, err))
		return
	}

	content := entry.Content
	// Truncate very large files
	if len(content) > 50000 {
		content = content[:50000] + "\n\n... (truncated)"
	}

	s.contentBuffer.SetText(content)
}

// formatFileSize returns a human-readable file size.
func formatFileSize(bytes int64) string {
	switch {
	case bytes >= 1<<20:
		return fmt.Sprintf("%.1fM", float64(bytes)/float64(1<<20))
	case bytes >= 1<<10:
		return fmt.Sprintf("%.1fK", float64(bytes)/float64(1<<10))
	default:
		return fmt.Sprintf("%dB", bytes)
	}
}

// formatPath returns the last path component for display.
func formatPath(path string) string {
	parts := strings.Split(path, "/")
	if len(parts) > 0 {
		return parts[len(parts)-1]
	}
	return path
}

// ============================================================================
// CLOSING
// ============================================================================
