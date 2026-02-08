/**
 * Adapter Interface — abstraction layer between frontend and backend.
 *
 * Two implementations: TauriAdapter (native IPC) and WebAdapter (HTTP + WebSocket).
 * Frontend imports only this interface — never backend-specific code.
 */
export {};
