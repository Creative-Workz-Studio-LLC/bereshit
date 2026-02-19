// ============================================================================
// CWS Studio — Frontend Application
// ============================================================================
//
// Vanilla JS — no framework, no build pipeline.
// Fetches from server API and renders views.
//
// ============================================================================

// ============================================================================
// SETUP
// ============================================================================

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

// Relative base — works both standalone (/) and behind reverse proxy (/studio/).
// "./" resolves relative to the page's URL, so fetch("./api/health") becomes
// "/api/health" standalone or "/studio/api/health" when proxied.
const API = ".";

const SEVERITY_ICONS = {
  error: "\u2716",  // ✖
  warn:  "\u26A0",  // ⚠
  info:  "\u2139",  // ℹ
};

/** Seed templates, loaded from server on first Create view open. */
let seedsCache = null;

/** Formats, loaded from server on first Formats view open. */
let formatsCache = null;

// ---------------------------------------------------------------------------
// DOM References
// ---------------------------------------------------------------------------

const $ = (sel) => document.querySelector(sel);
const $$ = (sel) => document.querySelectorAll(sel);

// ============================================================================
// BODY
// ============================================================================

// ---------------------------------------------------------------------------
// Navigation
// ---------------------------------------------------------------------------

function initNav() {
  const navItems = $$(".nav-item");
  const views = $$(".view");

  navItems.forEach((item) => {
    item.addEventListener("click", (e) => {
      e.preventDefault();
      const viewName = item.dataset.view;

      // Update nav active state
      navItems.forEach((n) => n.classList.remove("active"));
      item.classList.add("active");

      // Show the target view
      views.forEach((v) => v.classList.remove("active"));
      const target = $(`#view-${viewName}`);
      if (target) target.classList.add("active");

      // Load data on first visit
      if (viewName === "formats" && !formatsCache) loadFormats();
      if (viewName === "create" && !seedsCache) loadSeeds();
    });
  });
}

// ---------------------------------------------------------------------------
// Health Check
// ---------------------------------------------------------------------------

async function checkHealth() {
  const dot = $("#health-dot");
  try {
    const res = await fetch(`${API}/api/health`);
    if (res.ok) {
      dot.classList.remove("offline");
      dot.classList.add("online");
    } else {
      dot.classList.remove("online");
      dot.classList.add("offline");
    }
  } catch {
    dot.classList.remove("online");
    dot.classList.add("offline");
  }
}

// ---------------------------------------------------------------------------
// Toast Notifications
// ---------------------------------------------------------------------------

function ensureToastContainer() {
  let container = $(".toast-container");
  if (!container) {
    container = document.createElement("div");
    container.className = "toast-container";
    document.body.appendChild(container);
  }
  return container;
}

function toast(message, type = "success") {
  const container = ensureToastContainer();
  const el = document.createElement("div");
  el.className = `toast toast-${type}`;
  el.textContent = message;
  container.appendChild(el);
  setTimeout(() => el.remove(), 4000);
}

// ---------------------------------------------------------------------------
// Formats View
// ---------------------------------------------------------------------------

async function loadFormats() {
  const grid = $("#formats-grid");
  const loading = $("#formats-loading");

  loading.style.display = "flex";
  grid.innerHTML = "";

  try {
    const res = await fetch(`${API}/api/formats`);
    const data = await res.json();
    formatsCache = data.formats;

    for (const fmt of data.formats) {
      const card = document.createElement("div");
      card.className = "format-card";
      card.innerHTML = `
        <div class="format-name">${esc(fmt.name)}</div>
        <div class="format-desc">${esc(fmt.description)}</div>
        <div class="format-extensions">
          ${fmt.extensions.map((e) => `<span class="ext-tag">${esc(e)}</span>`).join("")}
        </div>
      `;
      grid.appendChild(card);
    }
  } catch (err) {
    grid.innerHTML = `<p style="color:var(--color-error)">Failed to load formats: ${esc(String(err))}</p>`;
  } finally {
    loading.style.display = "none";
  }
}

// ---------------------------------------------------------------------------
// Create View
// ---------------------------------------------------------------------------

async function loadSeeds() {
  try {
    const res = await fetch(`${API}/api/seeds`);
    seedsCache = await res.json();
  } catch {
    seedsCache = { code: [], data: [] };
  }
}

function initCreateForm() {
  const typeSelect = $("#create-type");
  const formatSelect = $("#create-format");
  const roleGroup = $("#role-group");
  const destInput = $("#create-dest");
  const btnPreview = $("#btn-preview");
  const btnCreate = $("#btn-create");
  const form = $("#create-form");

  // Type change → populate format options
  typeSelect.addEventListener("change", () => {
    const type = typeSelect.value;
    formatSelect.innerHTML = '<option value="">Select format...</option>';
    formatSelect.disabled = !type;
    roleGroup.style.display = "none";

    if (!type || !seedsCache) return;

    const seeds = seedsCache[type] || [];
    // Deduplicate formats
    const seen = new Set();
    for (const seed of seeds) {
      if (seen.has(seed.format)) continue;
      seen.add(seed.format);
      const opt = document.createElement("option");
      opt.value = seed.format;
      opt.textContent = `${seed.format} — ${seed.description}`;
      formatSelect.appendChild(opt);
    }

    // Show role selector for code type
    if (type === "code") {
      roleGroup.style.display = "";
    }

    updateCreateButtons();
  });

  // Format change → update buttons
  formatSelect.addEventListener("change", updateCreateButtons);
  destInput.addEventListener("input", updateCreateButtons);

  function updateCreateButtons() {
    const ready = typeSelect.value && formatSelect.value && destInput.value.trim();
    btnPreview.disabled = !ready;
    btnCreate.disabled = !ready;
  }

  // Preview
  btnPreview.addEventListener("click", async () => {
    const body = gatherCreateBody();
    body.dryRun = true;

    btnPreview.disabled = true;
    btnPreview.textContent = "Loading...";

    try {
      const res = await fetch(`${API}/api/create`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(body),
      });
      const data = await res.json();

      showCreateOutput("Preview", data.preview || data.output || data.error || "No output", null);
    } catch (err) {
      showCreateOutput("Error", String(err), null);
    } finally {
      btnPreview.disabled = false;
      btnPreview.textContent = "Preview";
    }
  });

  // Create
  form.addEventListener("submit", async (e) => {
    e.preventDefault();
    const body = gatherCreateBody();

    btnCreate.disabled = true;
    btnCreate.textContent = "Creating...";

    try {
      const res = await fetch(`${API}/api/create`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(body),
      });
      const data = await res.json();

      if (data.success) {
        toast(`Created: ${data.path}`);
        showCreateOutput(
          "Created",
          data.output || `File created at ${data.path}`,
          data.verification,
        );
      } else {
        toast(data.error || "Creation failed", "error");
        showCreateOutput("Error", data.error || "Unknown error", null);
      }
    } catch (err) {
      toast(String(err), "error");
    } finally {
      btnCreate.disabled = false;
      btnCreate.textContent = "Create File";
    }
  });

  // Close output panel
  $("#btn-close-output").addEventListener("click", () => {
    $("#create-output").style.display = "none";
  });
}

function gatherCreateBody() {
  return {
    type: $("#create-type").value,
    format: $("#create-format").value,
    dest: $("#create-dest").value.trim(),
    role: $("#create-role")?.value || undefined,
    key: $("#create-key").value.trim() || undefined,
    title: $("#create-title").value.trim() || undefined,
    purpose: $("#create-purpose").value.trim() || undefined,
    version: $("#create-version").value.trim() || "a-01.00",
  };
}

function showCreateOutput(title, content, verification) {
  const panel = $("#create-output");
  const titleEl = $("#output-title");
  const contentEl = $("#output-content");
  const verPanel = $("#output-verification");

  titleEl.textContent = title;

  // Strip ANSI escape codes from terminal output
  contentEl.textContent = stripAnsi(content);
  panel.style.display = "";

  if (verification) {
    verPanel.style.display = "";
    verPanel.innerHTML = renderVerificationSummary(verification);
  } else {
    verPanel.style.display = "none";
  }
}

// ---------------------------------------------------------------------------
// Validate View
// ---------------------------------------------------------------------------

function initValidateView() {
  const pathInput = $("#validate-path");
  const btnLint = $("#btn-lint");
  const verboseCheck = $("#validate-verbose");
  const errorsCheck = $("#validate-errors-only");

  btnLint.addEventListener("click", async () => {
    const path = pathInput.value.trim();
    if (!path) return;

    btnLint.disabled = true;
    btnLint.textContent = "Linting...";
    $("#lint-loading").style.display = "flex";
    $("#lint-totals").style.display = "none";
    $("#lint-results").innerHTML = "";

    try {
      const res = await fetch(`${API}/api/lint`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          path,
          verbose: verboseCheck.checked,
          errorsOnly: errorsCheck.checked,
        }),
      });
      const data = await res.json();

      if (data.error) {
        toast(data.error, "error");
        return;
      }

      renderLintResults(data);
    } catch (err) {
      toast(String(err), "error");
    } finally {
      btnLint.disabled = false;
      btnLint.textContent = "Lint";
      $("#lint-loading").style.display = "none";
    }
  });

  // Enter key triggers lint
  pathInput.addEventListener("keydown", (e) => {
    if (e.key === "Enter") btnLint.click();
  });
}

function renderLintResults(data) {
  const { results, totals } = data;

  // Totals bar
  const totalsBar = $("#lint-totals");
  totalsBar.style.display = "flex";
  $("#total-files").textContent = totals.files;
  $("#total-errors").textContent = totals.errors;
  $("#total-warnings").textContent = totals.warnings;
  $("#total-info").textContent = totals.info;

  // Results cards
  const container = $("#lint-results");
  container.innerHTML = "";

  if (results.length === 0) {
    container.innerHTML = '<p style="color:var(--text-secondary);padding:var(--space-md)">No findings to display. All files clean.</p>';
    return;
  }

  for (const summary of results) {
    const card = document.createElement("div");
    card.className = "result-card";

    // Header with file name and badges
    const badges = [];
    if (summary.errors > 0) badges.push(`<span class="badge badge-error">${summary.errors} error${summary.errors > 1 ? "s" : ""}</span>`);
    if (summary.warnings > 0) badges.push(`<span class="badge badge-warn">${summary.warnings} warn</span>`);
    if (summary.infos > 0) badges.push(`<span class="badge badge-info">${summary.infos} info</span>`);
    if (summary.health) {
      const score = summary.health.total;
      badges.push(`<span class="badge badge-health">${score}%</span>`);
    }

    const header = document.createElement("div");
    header.className = "result-header";
    header.innerHTML = `
      <span class="result-file">${esc(summary.file)}</span>
      <span class="result-badges">${badges.join("")}</span>
    `;

    // Details (expandable)
    const details = document.createElement("div");
    details.className = "result-details";

    for (const r of summary.results) {
      const finding = document.createElement("div");
      finding.className = "finding";
      finding.innerHTML = `
        <span class="finding-severity ${r.severity}">${SEVERITY_ICONS[r.severity] || ""}</span>
        <span class="finding-line">${r.line ? "L" + r.line : ""}</span>
        <span class="finding-rule">${esc(r.rule)}</span>
        <span class="finding-message">${esc(r.message)}</span>
      `;
      details.appendChild(finding);
    }

    // Health bar
    if (summary.health) {
      const healthContainer = document.createElement("div");
      healthContainer.className = "health-bar-container";
      const score = summary.health.total;
      const color = score >= 80 ? "var(--color-success)" : score >= 50 ? "var(--color-warn)" : "var(--color-error)";
      healthContainer.innerHTML = `
        <div class="health-label">
          <span>Health Score</span>
          <span>${score}% (${summary.health.passCount}/${summary.health.totalActions} checks)</span>
        </div>
        <div class="health-bar">
          <div class="health-fill" style="width:${score}%;background:${color}"></div>
        </div>
      `;
      details.appendChild(healthContainer);
    }

    // Toggle details on header click
    header.addEventListener("click", () => {
      details.classList.toggle("open");
    });

    card.appendChild(header);
    card.appendChild(details);
    container.appendChild(card);
  }
}

function renderVerificationSummary(summary) {
  const score = summary.health ? summary.health.total : null;
  const color = score >= 80 ? "var(--color-success)" : score >= 50 ? "var(--color-warn)" : "var(--color-error)";

  let html = '<div style="font-size:var(--font-size-sm)">';
  html += `<strong>Verification:</strong> `;
  html += `${summary.errors} errors, ${summary.warnings} warnings, ${summary.infos} info`;

  if (score !== null) {
    html += `<div class="health-bar" style="margin-top:var(--space-sm)">`;
    html += `<div class="health-fill" style="width:${score}%;background:${color}"></div>`;
    html += `</div>`;
    html += `<span style="color:var(--text-secondary)">Health: ${score}%</span>`;
  }

  html += "</div>";
  return html;
}

// ---------------------------------------------------------------------------
// Utilities
// ---------------------------------------------------------------------------

/** Escape HTML entities. */
function esc(str) {
  const div = document.createElement("div");
  div.textContent = str;
  return div.innerHTML;
}

/** Strip ANSI escape codes. */
function stripAnsi(str) {
  return str.replace(/\x1b\[[0-9;]*m/g, "");
}

// ============================================================================
// CLOSING
// ============================================================================

// ---------------------------------------------------------------------------
// Initialize
// ---------------------------------------------------------------------------

document.addEventListener("DOMContentLoaded", () => {
  initNav();
  initCreateForm();
  initValidateView();

  // Initial data loads
  checkHealth();
  loadSeeds();

  // Periodic health check
  setInterval(checkHealth, 30_000);
});

// "Except the LORD build the house, they labour in vain." — Psalm 127:1
// ============================================================================
