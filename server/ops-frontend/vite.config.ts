import { defineConfig } from 'vite';
import { resolve } from 'path';

export default defineConfig({
  build: {
    outDir: '../cmd/cws-ops/static',
    emptyOutDir: true,
  },
  resolve: {
    alias: {
      '@': resolve(__dirname, 'src'),
    },
  },
  server: {
    port: 4848,
    proxy: {
      '/api': {
        target: 'http://localhost:4847',
        changeOrigin: true,
      },
      '/ws': {
        target: 'ws://localhost:4847',
        ws: true,
      },
    },
  },
});
