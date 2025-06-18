# AOS — A Minimal Multiboot OS + Host Toolkit

AOS combines a simple x86_64 multiboot kernel with a rich host-side toolkit (Python services + React UI). Clone, build, boot in QEMU, and start hacking!

---

## 🏗️ Architecture

- **bare_metal_os/**: NASM + GCC-built bootloader (`stage1.asm` + GRUB’s `stage2`) + a small C kernel (`kernel.c`) with your interpreter.
- **src/**: Shared C code (syscalls, logging, policy, etc.).
- **scripts/**: Python/FastAPI + helper scripts (AI backend, orchestration, QEMU smoke test).
- **ui/**: React/Vite-based front-end.
- **.github/**: CI which currently builds host & UI.

See [docs/system_architecture.md](docs/system_architecture.md) for the big picture.

---

## ⚙️ Prerequisites

- **Build tools**: `nasm`, `gcc`, `ld`, `grub-mkrescue` (from `grub-pc-bin`), `xorriso`  
- **Emulator**: `qemu-system-x86_64`  
- **Host tools**: Python 3.8+, `pip`, Node 18+, `npm`  
- **Optional**: Docker (for demo), Redis (for RQ)

---

## 🚀 Getting Started

```bash
# 1. Clone
git clone https://github.com/RedactedCoder23/AOS.git
cd AOS

# 2. Install dependencies
./scripts/bootstrap.sh  # installs system packages, Python libs and UI deps

# 3. Build everything + ISO
make all iso

# 4. Boot in QEMU
./scripts/qemu_smoke.sh x86_64
```

🔧 Development
Bare-metal: cd bare_metal_os && make

Host binary: make host → installs /usr/local/bin/aos-host

UI: cd ui && npm run dev

Tests:

pytest for Python

make test for C units/integration

📦 Packaging
We use pyproject.toml + setup.cfg for the Python CLI (aos, aos-audit), and standard npm for the UI.

📝 CI / GitHub Actions
We now build & smoke-test the ISO under .github/workflows/ci.yml.

Happy hacking!
