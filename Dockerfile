# Builds every project in the repo and runs them.
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential cmake libeigen3-dev \
        python3 python3-numpy python3-pandas python3-matplotlib \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /lab
COPY . .

# Windows checkouts can leave CRLF line endings.
RUN sed -i 's/\r$//' run_all_projects.sh

RUN for dir in */; do \
        if [ -f "${dir}CMakeLists.txt" ]; then \
            echo "Building ${dir}"; \
            cmake -S "${dir}" -B "${dir}build" -DCMAKE_BUILD_TYPE=Release \
                  -DEIGEN_INCLUDE_DIR=/usr/include/eigen3 && \
            cmake --build "${dir}build" -j 2 || exit 1; \
        fi; \
    done

ENTRYPOINT ["bash", "run_all_projects.sh"]
