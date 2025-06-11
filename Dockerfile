FROM ubuntu:24.04
RUN apt-get update && \
    apt-get install -y build-essential clang python3 python3-pip \
    nodejs npm firecracker && \
    npm install -g serve

WORKDIR /aos
COPY . .

RUN cd bare_metal_os && make all
RUN pip3 install -r requirements.txt
RUN cd ui && npm install && npm run build

EXPOSE 5000 8080
CMD ["bash", "-lc", "scripts/branch_ui.py & cd ui && serve -s build"]
