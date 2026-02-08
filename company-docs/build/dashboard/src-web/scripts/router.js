/**
 * Client-Side Router — hash-based routing for the dashboard.
 *
 * Routes: #/build, #/inspect, #/validate, #/analysis, #/utility,
 * #/config, #/config/<section>, etc.
 */
class Router {
    routes = [];
    contentEl = null;
    currentRoute = '';
    /** Set the content container element. */
    setContainer(el) {
        this.contentEl = el;
    }
    /** Register a route. */
    on(pattern, handler) {
        this.routes.push({ pattern, handler });
    }
    /** Start listening for hash changes. */
    start() {
        window.addEventListener('hashchange', () => this.handleRoute());
        this.handleRoute();
    }
    /** Navigate to a route programmatically. */
    navigate(route) {
        window.location.hash = route;
    }
    /** Get the current route. */
    getCurrentRoute() {
        return this.currentRoute;
    }
    handleRoute() {
        const hash = window.location.hash || '#/build';
        this.currentRoute = hash;
        if (!this.contentEl)
            return;
        // Find matching route.
        for (const route of this.routes) {
            if (hash === route.pattern || hash.startsWith(route.pattern + '/')) {
                this.contentEl.innerHTML = '';
                const el = route.handler();
                this.contentEl.appendChild(el);
                // Update active nav item.
                document.querySelectorAll('.nav-item').forEach((item) => {
                    const href = item.getAttribute('data-route');
                    item.classList.toggle('active', href === route.pattern);
                });
                return;
            }
        }
        // No match — show default.
        if (this.routes.length > 0) {
            this.navigate(this.routes[0].pattern);
        }
    }
}
/** Singleton router instance. */
export const router = new Router();
