# Multi-stage build for Lua Script Engine
# Supports both ARM64 and AMD64 architectures

FROM --platform=$BUILDPLATFORM debian:bookworm-slim AS builder

ARG TARGETPLATFORM
ARG BUILDPLATFORM

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    pkg-config \
    liblua5.4-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY CMakeLists.txt .
COPY src/ src/

RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF .. && \
    make -j$(nproc)

# Runtime stage
FROM debian:bookworm-slim AS runtime

RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 \
    liblua5.4-0 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /app/build/lua_engine /app/
COPY scripts/ /app/scripts/
COPY engine.conf /app/

CMD ["./lua_engine", "scripts"]
