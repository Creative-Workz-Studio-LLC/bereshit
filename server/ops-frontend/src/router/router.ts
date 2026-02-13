/**
 * Hash-based router. Simple, no dependencies.
 *
 * Routes:
 *   #/           → Overview
 *   #/deploy     → Deploy
 *   #/logs       → Logs
 *   #/services   → Services
 *   #/dns        → DNS
 *   #/backups    → Backups
 */

export interface Route {
  path: string;
  label: string;
  render: () => HTMLElement;
}

let routes: Route[] = [];
let container: HTMLElement | null = null;
let currentCleanup: (() => void) | null = null;

export function registerRoutes(routeList: Route[], target: HTMLElement): void {
  routes = routeList;
  container = target;

  window.addEventListener('hashchange', navigate);
  navigate(); // Initial route
}

export function navigate(): void {
  if (!container) return;

  const hash = window.location.hash || '#/';
  const path = hash.slice(1) || '/';

  const route = routes.find(r => r.path === path) ?? routes[0];
  if (!route) return;

  // Cleanup previous page
  if (currentCleanup) {
    currentCleanup();
    currentCleanup = null;
  }

  // Render new page
  container.innerHTML = '';
  const element = route.render();
  container.appendChild(element);

  // Store cleanup if the element provides one
  if ('cleanup' in element && typeof element.cleanup === 'function') {
    currentCleanup = element.cleanup as () => void;
  }

  // Update active nav link
  document.querySelectorAll('.sidebar-nav a').forEach(a => {
    const href = a.getAttribute('href') ?? '';
    a.classList.toggle('active', href === hash || (hash === '' && href === '#/'));
  });
}

export function getRoutes(): Route[] {
  return routes;
}
