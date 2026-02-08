import { defineConfig } from 'vite';
import { resolve } from 'path';

export default defineConfig({
  build: {
    outDir: '../server/static',
    emptyOutDir: true,
  },
  resolve: {
    alias: {
      '@': resolve(__dirname, 'src'),
    },
  },
  server: {
    port: 5173,
    proxy: {
      '/api': {
        target: 'http://localhost:8470',
        changeOrigin: true,
      },
      '/ws': {
        target: 'ws://localhost:8470',
        ws: true,
      },
    },
  },
});
