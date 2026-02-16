//omni:code --go -library
//omni:key B-L3-database-projects-sqlite
//omni:version a-01.00
// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cpisi-db-projects-sqlite
// Purpose: SQLite implementation of projects Repository interface
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
	"database/sql"
	"embed"
	"fmt"

	l2db "creativeworkzstudio.com/bereshit/L2-platform/hybrid/database"
)

//go:embed schema/*.sql
var schemaFS embed.FS

// SQLiteRepository implements projects.Repository using SQLite
type SQLiteRepository struct {
	db *sql.DB
}

// Compile-time interface check
var _ Repository = (*SQLiteRepository)(nil)

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Constructor & Lifecycle
// ═══════════════════════════════════════════════════════════════════════════

// NewSQLiteRepository creates a new projects SQLite repository
func NewSQLiteRepository(dbPath string) (*SQLiteRepository, error) {
	db, err := l2db.OpenDefault(dbPath)
	if err != nil {
		return nil, fmt.Errorf("open projects db: %w", err)
	}
	return &SQLiteRepository{db: db}, nil
}

// Close releases database resources
func (r *SQLiteRepository) Close() error {
	return r.db.Close()
}

// Migrate runs the schema migrations
func (r *SQLiteRepository) Migrate(ctx context.Context) error {
	schema, err := schemaFS.ReadFile("schema/001_initial.sql")
	if err != nil {
		return fmt.Errorf("read projects schema: %w", err)
	}
	_, err = r.db.ExecContext(ctx, string(schema))
	if err != nil {
		return fmt.Errorf("execute projects schema: %w", err)
	}
	return nil
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Project Operations
// ═══════════════════════════════════════════════════════════════════════════

// CreateProject creates a new project
func (r *SQLiteRepository) CreateProject(ctx context.Context, project *Project) error {
	query := `
		INSERT INTO projects (
			name, description, priority, status,
			workspace_path, started_at, target_date
		) VALUES (?, ?, ?, ?, ?, ?, ?)
	`
	result, err := r.db.ExecContext(ctx, query,
		project.Name, project.Description, project.Priority, project.Status,
		project.WorkspacePath, project.StartedAt, project.TargetDate,
	)
	if err != nil {
		return err
	}
	id, err := result.LastInsertId()
	if err != nil {
		return err
	}
	project.ID = id
	return nil
}

// GetProject retrieves a project by ID
func (r *SQLiteRepository) GetProject(ctx context.Context, id int64) (*Project, error) {
	query := `
		SELECT id, name, description, priority, status,
			workspace_path, started_at, target_date, completed_at
		FROM projects WHERE id = ?
	`
	row := r.db.QueryRowContext(ctx, query, id)

	var p Project
	var description, workspacePath sql.NullString
	var startedAt, targetDate, completedAt sql.NullTime

	err := row.Scan(
		&p.ID, &p.Name, &description, &p.Priority, &p.Status,
		&workspacePath, &startedAt, &targetDate, &completedAt,
	)
	if err == sql.ErrNoRows {
		return nil, nil
	}
	if err != nil {
		return nil, err
	}

	if description.Valid {
		p.Description = description.String
	}
	if workspacePath.Valid {
		p.WorkspacePath = workspacePath.String
	}
	if startedAt.Valid {
		p.StartedAt = &startedAt.Time
	}
	if targetDate.Valid {
		p.TargetDate = &targetDate.Time
	}
	if completedAt.Valid {
		p.CompletedAt = &completedAt.Time
	}
	return &p, nil
}

// GetActiveProjects returns all active projects
func (r *SQLiteRepository) GetActiveProjects(ctx context.Context) ([]Project, error) {
	query := `
		SELECT id, name, description, priority, status,
			workspace_path, started_at, target_date, completed_at
		FROM projects
		WHERE status = 'active'
		ORDER BY priority ASC, name ASC
	`
	rows, err := r.db.QueryContext(ctx, query)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var projects []Project
	for rows.Next() {
		var p Project
		var description, workspacePath sql.NullString
		var startedAt, targetDate, completedAt sql.NullTime

		err := rows.Scan(
			&p.ID, &p.Name, &description, &p.Priority, &p.Status,
			&workspacePath, &startedAt, &targetDate, &completedAt,
		)
		if err != nil {
			return nil, err
		}
		if description.Valid {
			p.Description = description.String
		}
		if workspacePath.Valid {
			p.WorkspacePath = workspacePath.String
		}
		if startedAt.Valid {
			p.StartedAt = &startedAt.Time
		}
		if targetDate.Valid {
			p.TargetDate = &targetDate.Time
		}
		if completedAt.Valid {
			p.CompletedAt = &completedAt.Time
		}
		projects = append(projects, p)
	}
	return projects, rows.Err()
}

// UpdateProject updates an existing project
func (r *SQLiteRepository) UpdateProject(ctx context.Context, project *Project) error {
	query := `
		UPDATE projects SET
			name = ?, description = ?, priority = ?, status = ?,
			workspace_path = ?, started_at = ?, target_date = ?,
			completed_at = ?, updated_at = CURRENT_TIMESTAMP
		WHERE id = ?
	`
	_, err := r.db.ExecContext(ctx, query,
		project.Name, project.Description, project.Priority, project.Status,
		project.WorkspacePath, project.StartedAt, project.TargetDate,
		project.CompletedAt, project.ID,
	)
	return err
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Milestone Operations
// ═══════════════════════════════════════════════════════════════════════════

// CreateMilestone creates a milestone for a project
func (r *SQLiteRepository) CreateMilestone(ctx context.Context, milestone *Milestone) error {
	query := `
		INSERT INTO milestones (project_id, name, description, target_date, status)
		VALUES (?, ?, ?, ?, ?)
	`
	result, err := r.db.ExecContext(ctx, query,
		milestone.ProjectID, milestone.Name, milestone.Description,
		milestone.TargetDate, milestone.Status,
	)
	if err != nil {
		return err
	}
	id, err := result.LastInsertId()
	if err != nil {
		return err
	}
	milestone.ID = id
	return nil
}

// GetProjectMilestones returns milestones for a project
func (r *SQLiteRepository) GetProjectMilestones(ctx context.Context, projectID int64) ([]Milestone, error) {
	query := `
		SELECT id, project_id, name, description, target_date, completed_at, status
		FROM milestones
		WHERE project_id = ?
		ORDER BY target_date ASC
	`
	rows, err := r.db.QueryContext(ctx, query, projectID)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var milestones []Milestone
	for rows.Next() {
		var m Milestone
		var description sql.NullString
		var targetDate, completedAt sql.NullTime

		err := rows.Scan(
			&m.ID, &m.ProjectID, &m.Name, &description,
			&targetDate, &completedAt, &m.Status,
		)
		if err != nil {
			return nil, err
		}
		if description.Valid {
			m.Description = description.String
		}
		if targetDate.Valid {
			m.TargetDate = &targetDate.Time
		}
		if completedAt.Valid {
			m.CompletedAt = &completedAt.Time
		}
		milestones = append(milestones, m)
	}
	return milestones, rows.Err()
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Work Item Operations
// ═══════════════════════════════════════════════════════════════════════════

// CreateWorkItem creates a work item in a project
func (r *SQLiteRepository) CreateWorkItem(ctx context.Context, item *WorkItem) error {
	query := `
		INSERT INTO work_items (
			project_id, milestone_id, title, description,
			status, priority, assigned_to
		) VALUES (?, ?, ?, ?, ?, ?, ?)
	`
	result, err := r.db.ExecContext(ctx, query,
		item.ProjectID, item.MilestoneID, item.Title, item.Description,
		item.Status, item.Priority, item.AssignedTo,
	)
	if err != nil {
		return err
	}
	id, err := result.LastInsertId()
	if err != nil {
		return err
	}
	item.ID = id
	return nil
}

// GetProjectWorkItems returns work items for a project
func (r *SQLiteRepository) GetProjectWorkItems(ctx context.Context, projectID int64) ([]WorkItem, error) {
	query := `
		SELECT id, project_id, milestone_id, title, description,
			status, priority, assigned_to, completed_at
		FROM work_items
		WHERE project_id = ?
		ORDER BY priority ASC, id ASC
	`
	rows, err := r.db.QueryContext(ctx, query, projectID)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var items []WorkItem
	for rows.Next() {
		var w WorkItem
		var milestoneID sql.NullInt64
		var description, assignedTo sql.NullString
		var completedAt sql.NullTime

		err := rows.Scan(
			&w.ID, &w.ProjectID, &milestoneID, &w.Title, &description,
			&w.Status, &w.Priority, &assignedTo, &completedAt,
		)
		if err != nil {
			return nil, err
		}
		if milestoneID.Valid {
			mid := milestoneID.Int64
			w.MilestoneID = &mid
		}
		if description.Valid {
			w.Description = description.String
		}
		if assignedTo.Valid {
			w.AssignedTo = assignedTo.String
		}
		if completedAt.Valid {
			w.CompletedAt = &completedAt.Time
		}
		items = append(items, w)
	}
	return items, rows.Err()
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Session-Project Linking
// ═══════════════════════════════════════════════════════════════════════════

// LinkSessionToProject links a session to a project
func (r *SQLiteRepository) LinkSessionToProject(ctx context.Context, link *SessionProject) error {
	query := `
		INSERT INTO session_projects (session_id, project_id, work_description, session_date)
		VALUES (?, ?, ?, ?)
		ON CONFLICT(session_id, project_id) DO UPDATE SET
			work_description = excluded.work_description
	`
	_, err := r.db.ExecContext(ctx, query,
		link.SessionID, link.ProjectID, link.WorkDescription, link.SessionDate,
	)
	return err
}

// GetProjectSessions returns sessions linked to a project
func (r *SQLiteRepository) GetProjectSessions(ctx context.Context, projectID int64, limit int) ([]SessionProject, error) {
	query := `
		SELECT id, session_id, project_id, work_description, session_date
		FROM session_projects
		WHERE project_id = ?
		ORDER BY session_date DESC
		LIMIT ?
	`
	rows, err := r.db.QueryContext(ctx, query, projectID, limit)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var links []SessionProject
	for rows.Next() {
		var sp SessionProject
		var workDesc sql.NullString
		err := rows.Scan(&sp.ID, &sp.SessionID, &sp.ProjectID, &workDesc, &sp.SessionDate)
		if err != nil {
			return nil, err
		}
		if workDesc.Valid {
			sp.WorkDescription = workDesc.String
		}
		links = append(links, sp)
	}
	return links, rows.Err()
}

// ═══════════════════════════════════════════════════════════════════════════
// BODY - Raw SQL Operations
// ═══════════════════════════════════════════════════════════════════════════

// Exec executes a raw SQL statement and returns rows affected
func (r *SQLiteRepository) Exec(ctx context.Context, query string, args ...interface{}) (int64, error) {
	result, err := r.db.ExecContext(ctx, query, args...)
	if err != nil {
		return 0, fmt.Errorf("exec: %w", err)
	}
	rowsAffected, err := result.RowsAffected()
	if err != nil {
		return 0, fmt.Errorf("rows affected: %w", err)
	}
	return rowsAffected, nil
}

// Query executes a raw SQL query and returns rows as maps
func (r *SQLiteRepository) Query(ctx context.Context, query string, args ...interface{}) ([]map[string]interface{}, error) {
	rows, err := r.db.QueryContext(ctx, query, args...)
	if err != nil {
		return nil, fmt.Errorf("query: %w", err)
	}
	defer rows.Close()

	columns, err := rows.Columns()
	if err != nil {
		return nil, fmt.Errorf("columns: %w", err)
	}

	var results []map[string]interface{}
	for rows.Next() {
		values := make([]interface{}, len(columns))
		valuePtrs := make([]interface{}, len(columns))
		for i := range values {
			valuePtrs[i] = &values[i]
		}
		if err := rows.Scan(valuePtrs...); err != nil {
			return nil, fmt.Errorf("scan: %w", err)
		}
		row := make(map[string]interface{})
		for i, col := range columns {
			row[col] = values[i]
		}
		results = append(results, row)
	}
	return results, rows.Err()
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
