FROM gcc

RUN apt-get update && apt-get install -y \
        gdb \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /work
