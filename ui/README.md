# Web UI

This directory contains the sources for the experimental web interface for AOS.
The code under `src/` is bundled with [Parcel](https://parceljs.org) and
produces static assets under `dist/`.

## Setup

```bash
cd ui
npm install
```

### Development server

```bash
npm start
```

### Build for release

```bash
npm run build
```

The Python backends will serve files from `dist/` if present, falling back to the
`src/` directory during development.
