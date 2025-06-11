FROM ubuntu:22.04
WORKDIR /aos
COPY . .
RUN apt-get update && \
    apt-get install -y build-essential git curl pkg-config \
    libcurl4-openssl-dev libncurses-dev python3 python3-pip nodejs npm \
    firecracker && \
    pip3 install -r requirements.txt && \
    npm --prefix ui install && npm --prefix ui run build && \
    make -C bare_metal_os kernel.bin && make all
EXPOSE 8080 5000
CMD ["python3", "scripts/branch_ui.py"]
