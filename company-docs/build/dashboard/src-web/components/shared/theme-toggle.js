/**
 * Theme Toggle — dark/light/system theme switcher.
 *
 * Reads from and writes to the preferences system.
 * Applies [data-theme] attribute on <html> for CSS variable overrides.
 */
import { getAdapter } from '../../scripts/adapter.js';
import { store, STATE } from '../../scripts/state.js';
/** Apply theme to the document. */
export function applyTheme(choice) {
    const root = document.documentElement;
    if (choice === 'system') {
        const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
        root.setAttribute('data-theme', prefersDark ? 'dark' : 'light');
    }
    else {
        root.setAttribute('data-theme', choice);
    }
}
/** Get the current theme from preferences, defaulting to 'light'. */
export function getCurrentTheme() {
    const prefs = store.get(STATE.PREFERENCES) ?? {};
    return prefs.theme ?? 'light';
}
/** Save theme to preferences. */
async function saveTheme(choice) {
    const adapter = getAdapter();
    try {
        const prefs = await adapter.readPreferences();
        prefs.theme = choice;
        await adapter.writePreferences(prefs);
        store.set(STATE.PREFERENCES, prefs);
    }
    catch {
        // Silently fail — theme still works locally.
    }
}
/** Create a theme toggle button for the sidebar. */
export function createThemeToggle() {
    const wrapper = document.createElement('div');
    wrapper.style.cssText = 'padding: var(--space-3) var(--space-4); border-top: 1px solid rgba(255,255,255,0.1);';
    const btn = document.createElement('button');
    btn.style.cssText = `
    background: rgba(255,255,255,0.08); border: none;
    color: rgba(255,255,255,0.7); padding: var(--space-2) var(--space-3);
    border-radius: var(--border-radius); cursor: pointer;
    font-family: var(--font-heading); font-size: var(--text-xs);
    width: 100%; text-align: left;
    transition: background var(--transition-fast);
  `;
    btn.addEventListener('mouseenter', () => { btn.style.background = 'rgba(255,255,255,0.12)'; });
    btn.addEventListener('mouseleave', () => { btn.style.background = 'rgba(255,255,255,0.08)'; });
    const choices = ['light', 'dark', 'system'];
    let currentIndex = choices.indexOf(getCurrentTheme());
    if (currentIndex < 0)
        currentIndex = 0;
    function updateLabel() {
        const labels = {
            light: 'Theme: Light',
            dark: 'Theme: Dark',
            system: 'Theme: System',
        };
        btn.textContent = labels[choices[currentIndex]];
    }
    btn.addEventListener('click', () => {
        currentIndex = (currentIndex + 1) % choices.length;
        const choice = choices[currentIndex];
        applyTheme(choice);
        saveTheme(choice);
        updateLabel();
    });
    updateLabel();
    wrapper.appendChild(btn);
    // Listen for system theme changes.
    window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', () => {
        if (choices[currentIndex] === 'system') {
            applyTheme('system');
        }
    });
    return wrapper;
}
/** Initialize theme from preferences on startup. */
export async function initTheme() {
    try {
        const adapter = getAdapter();
        const prefs = await adapter.readPreferences();
        store.set(STATE.PREFERENCES, prefs);
        const theme = prefs.theme ?? 'light';
        applyTheme(theme);
    }
    catch {
        applyTheme('light');
    }
}
