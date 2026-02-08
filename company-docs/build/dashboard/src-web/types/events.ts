/**
 * Event types for the dashboard.
 */

/** Menu action event from native menu or in-app menu bar. */
export interface MenuActionEvent {
  action: string;
}

/** Navigation event from router. */
export interface NavigationEvent {
  route: string;
  params?: Record<string, string>;
}

/** State change event from reactive store. */
export interface StateChangeEvent {
  key: string;
  value: unknown;
}
