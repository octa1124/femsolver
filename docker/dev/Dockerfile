FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        clang \
        clang-format \
        clang-tidy \
        cmake \
        curl \
        git \
        ninja-build \
        python3 \
        python3-pip \
        python3-venv \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

RUN python3 -m pip install --no-cache-dir --break-system-packages \
      pytest \
      ruff \
      PyYAML \
      gcovr
