import { defineConfig } from 'vite';
import react from '@vitejs/plugin-react';
import helmet from 'helmet';

export default defineConfig({
  plugins: [
    react(),
    {
      name: 'helmet-plugin',
      configureServer(server) {
        server.middlewares.use(helmet());
      },
    },
  ],
  server: {
    proxy: {
      '/api': 'http://localhost:8000',
    },
  },
});
