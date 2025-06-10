FROM ubuntu:22.04
WORKDIR /opt/aos
COPY . .
RUN apt-get update && \
    apt-get install -y build-essential git curl pkg-config libcurl4-openssl-dev \
    libncurses-dev python3 python3-pip nodejs npm && \
    pip3 install -r requirements.txt && \
    npm --prefix ui install && \
    make all
CMD ["./build/host_test"]
