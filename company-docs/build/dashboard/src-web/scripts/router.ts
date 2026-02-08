/**
 * Client-Side Router — hash-based routing for the dashboard.
 *
 * Routes: #/build, #/inspect, #/validate, #/analysis, #/utility,
 * #/config, #/config/<section>, etc.
 */

export type RouteHandler = () => HTMLElement;

interface Route {
  pattern: string;
  handler: RouteHandler;
}

class Router {
  private routes: Route[] = [];
  private contentEl: HTMLElement | null = null;
  private currentRoute: string = '';

  /** Set the content container element. */
  setContainer(el: HTMLElement): void {
    this.contentEl = el;
  }

  /** Register a route. */
  on(pattern: string, handler: RouteHandler): void {
    this.routes.push({ pattern, handler });
  }

  /** Start listening for hash changes. */
  start(): void {
    window.addEventListener('hashchange', () => this.handleRoute());
    this.handleRoute();
  }

  /** Navigate to a route programmatically. */
  navigate(route: string): void {
    window.location.hash = route;
  }

  /** Get the current route. */
  getCurrentRoute(): string {
    return this.currentRoute;
  }

  private handleRoute(): void {
    const hash = window.location.hash || '#/build';
    this.currentRoute = hash;

    if (!this.contentEl) return;

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
      this.navigate(this.routes[0]!.pattern);
    }
  }
}

/** Singleton router instance. */
export const router = new Router();
