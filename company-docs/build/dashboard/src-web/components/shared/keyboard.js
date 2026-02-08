/**
 * Keyboard Shortcuts — global hotkey handler.
 *
 * Shortcuts:
 *   Ctrl+B        → Navigate to Build
 *   Ctrl+L        → Navigate to Validate (Lint)
 *   Ctrl+W        → Navigate to Validate (Witness)
 *   Ctrl+,        → Navigate to Config
 *   Ctrl+`        → Toggle output panel
 *   Ctrl+\        → Toggle sidebar
 *   Escape        → Cancel running process
 */
import { router } from '../../scripts/router.js';
import { cancelRunning } from '../shared/runner.js';
export function initKeyboardShortcuts() {
    document.addEventListener('keydown', (e) => {
        // Don't intercept when typing in inputs.
        const tag = e.target.tagName;
        if (tag === 'INPUT' || tag === 'TEXTAREA' || tag === 'SELECT')
            return;
        const ctrl = e.ctrlKey || e.metaKey;
        if (ctrl && e.key === 'b') {
            e.preventDefault();
            router.navigate('#/build');
        }
        else if (ctrl && e.key === 'l') {
            e.preventDefault();
            router.navigate('#/validate');
        }
        else if (ctrl && e.key === 'w') {
            e.preventDefault();
            router.navigate('#/validate');
        }
        else if (ctrl && e.key === ',') {
            e.preventDefault();
            router.navigate('#/config');
        }
        else if (ctrl && e.key === '`') {
            e.preventDefault();
            const panel = document.querySelector('.output-panel');
            panel?.classList.toggle('collapsed');
            const arrow = panel?.querySelector('.output-toggle');
            if (arrow) {
                arrow.textContent = panel?.classList.contains('collapsed') ? '\u25B2' : '\u25BC';
            }
        }
        else if (ctrl && e.key === '\\') {
            e.preventDefault();
            const sidebar = document.querySelector('.sidebar');
            sidebar?.classList.toggle('collapsed');
        }
        else if (e.key === 'Escape') {
            cancelRunning();
        }
    });
}
