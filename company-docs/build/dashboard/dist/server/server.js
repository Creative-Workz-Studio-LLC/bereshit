/**
 * CWS Dashboard — Express Server (Web Mode)
 *
 * Serves the frontend static files and provides API endpoints
 * that wrap the builder CLI. WebSocket for streaming output.
 *
 * Usage: node --loader ts-node/esm server/server.ts
 *   or:  node dist/server/server.js
 */
import express from 'express';
import { createServer } from 'http';
import { WebSocketServer } from 'ws';
import { resolve, dirname } from 'path';
import { fileURLToPath } from 'url';
import { commandRoutes, setWss } from './routes/commands.js';
import { configRoutes } from './routes/config.js';
const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);
const PORT = parseInt(process.env.CWS_DASHBOARD_PORT ?? '3847', 10);
const app = express();
app.use(express.json());
// Serve frontend static files.
// When running from source (server/), __dirname/../src-web works.
// When running from dist (dist/server/), we need ../../src-web.
// Detect by checking if __dirname ends with 'dist/server'.
const webRoot = __dirname.endsWith('dist/server')
    ? resolve(__dirname, '..', '..', 'src-web')
    : resolve(__dirname, '..', 'src-web');
app.use(express.static(webRoot));
// API routes.
app.use('/api/commands', commandRoutes);
app.use('/api/config', configRoutes);
// Fallback to index.html for client-side routing.
app.get('*', (_req, res) => {
    res.sendFile(resolve(webRoot, 'index.html'));
});
// HTTP + WebSocket server.
const server = createServer(app);
const wss = new WebSocketServer({ server, path: '/ws' });
setWss(wss);
wss.on('connection', (ws) => {
    console.log('[WS] Client connected');
    ws.on('close', () => console.log('[WS] Client disconnected'));
});
server.listen(PORT, () => {
    console.log(`[CWS Dashboard] Web server running at http://localhost:${PORT}`);
    console.log(`[CWS Dashboard] Press Ctrl+C to stop`);
});
//# sourceMappingURL=server.js.map