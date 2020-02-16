FROM ubuntu

# Install package dependencies.
RUN apt-get update && apt-get install -y \
    cmake \
    g++-7 \
    git \
    libwayland-dev \
    libx11-xcb-dev \
    libxcb-randr0-dev \
    libxcb-util0-dev \
    pkg-config \
    python3-pip \
    wget && \
    pip3 install conan

RUN mkdir /app

COPY conan-config /app/conan
RUN conan config install /app/conan/common
RUN conan config install /app/conan/linux

ENV BUILD_DIR /app/build

COPY conanfile.py /app
RUN conan install /app -if $BUILD_DIR --build missing

COPY . /app

WORKDIR $BUILD_DIR
