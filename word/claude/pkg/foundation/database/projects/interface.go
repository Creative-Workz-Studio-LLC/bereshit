// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cpisi-db-projects-interface
// Purpose: Repository interface for work tracking domain
// Biblical: Luke 14:28 - "Which of you, intending to build a tower, sitteth
//   not down first, and counteth the cost?"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-08
//
// ═══════════════════════════════════════════════════════════════════════════

package projects

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"context"
	"time"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Types
// ═══════════════════════════════════════════════════════════════════════════

// Project represents a tracked project
type Project struct {
	ID            int64
	Name          string
	Description   string
	Priority      string
	Status        string
	WorkspacePath string
	StartedAt     *time.Time
	TargetDate    *time.Time
	CompletedAt   *time.Time
}

// Milestone represents a project milestone
type Milestone struct {
	ID          int64
	ProjectID   int64
	Name        string
	Description string
	TargetDate  *time.Time
	CompletedAt *time.Time
	Status      string
}

// WorkItem represents a single task/item in a project
type WorkItem struct {
	ID          int64
	ProjectID   int64
	MilestoneID *int64
	Title       string
	Description string
	Status      string
	Priority    string
	AssignedTo  string
	CompletedAt *time.Time
}

// SessionProject links a session to a project
type SessionProject struct {
	ID              int64
	SessionID       string
	ProjectID       int64
	WorkDescription string
	SessionDate     time.Time
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Repository Interface
// ═══════════════════════════════════════════════════════════════════════════

// Repository defines the contract for project tracking persistence
type Repository interface {
	// Project operations
	CreateProject(ctx context.Context, project *Project) error
	GetProject(ctx context.Context, id int64) (*Project, error)
	GetActiveProjects(ctx context.Context) ([]Project, error)
	UpdateProject(ctx context.Context, project *Project) error

	// Milestone operations
	CreateMilestone(ctx context.Context, milestone *Milestone) error
	GetProjectMilestones(ctx context.Context, projectID int64) ([]Milestone, error)

	// Work item operations
	CreateWorkItem(ctx context.Context, item *WorkItem) error
	GetProjectWorkItems(ctx context.Context, projectID int64) ([]WorkItem, error)

	// Session-project linking
	LinkSessionToProject(ctx context.Context, link *SessionProject) error
	GetProjectSessions(ctx context.Context, projectID int64, limit int) ([]SessionProject, error)

	// Raw SQL
	Exec(ctx context.Context, query string, args ...interface{}) (int64, error)
	Query(ctx context.Context, query string, args ...interface{}) ([]map[string]interface{}, error)

	// Lifecycle
	Close() error
	Migrate(ctx context.Context) error
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
