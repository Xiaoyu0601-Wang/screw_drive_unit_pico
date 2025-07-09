#!/bin/bash
set -e  # 遇到错误退出

# === 全局变量配置 ===
INSTALL_DIR="$HOME/pico"
GCC_DIR="gcc-arm-none-eabi-12.3.rel1-x86_64-linux"
GCC_URL="https://developer.arm.com/-/media/Files/downloads/gnu-rm/12.3.rel1/${GCC_DIR}.tar.bz2"
PICO_SDK_REPO="https://github.com/raspberrypi/pico-sdk.git"
PICO_EXAMPLES_REPO="https://github.com/raspberrypi/pico-examples.git"
CUSTOM_PROJECT_REPO="https://github.com/你的用户名/screw_drive_unit_pico.git"
CUSTOM_PROJECT_NAME="screw_drive_unit_pico"
BOARD_TYPE="pico_w"
BUILD_TYPE="Release"

# === 函数定义区 ===

install_dependencies() {
  echo "Installing system dependencies..."
  sudo apt update
  sudo apt install -y cmake gcc g++ make git \
    libusb-1.0-0-dev pkg-config \
    gcc-arm-none-eabi libnewlib-arm-none-eabi
}

install_gcc_toolchain() {
  echo "Installing GCC toolchain..."
  cd "$HOME"
  if [ ! -d "$GCC_DIR" ]; then
    wget -c "$GCC_URL"
    tar -xjf "${GCC_DIR}.tar.bz2"
  else
    echo "GCC toolchain already installed: $GCC_DIR"
  fi
  export PATH="$HOME/$GCC_DIR/bin:$PATH"
}

clone_repositories() {
  echo "Cloning repositories into $INSTALL_DIR..."
  mkdir -p "$INSTALL_DIR"
  cd "$INSTALL_DIR"

  if [ ! -d pico-sdk ]; then
    git clone --depth=1 "$PICO_SDK_REPO"
    cd pico-sdk && git submodule update --init && cd ..
  else
    echo "pico-sdk already exists."
  fi

  if [ ! -d pico-examples ]; then
    git clone --depth=1 "$PICO_EXAMPLES_REPO"
  else
    echo "pico-examples already exists."
  fi

  if [ ! -d "$CUSTOM_PROJECT_NAME" ]; then
    git clone "$CUSTOM_PROJECT_REPO"
  else
    echo "$CUSTOM_PROJECT_NAME already exists."
  fi
}

setup_environment_variables() {
  echo "Setting environment variables..."
  BASHRC="$HOME/.bashrc"
  SDK_EXPORT="export PICO_SDK_PATH=$INSTALL_DIR/pico-sdk"
  GCC_EXPORT="export PATH=$HOME/$GCC_DIR/bin:\$PATH"

  grep -Fxq "$SDK_EXPORT" "$BASHRC" || echo "$SDK_EXPORT" >> "$BASHRC"
  grep -Fxq "$GCC_EXPORT" "$BASHRC" || echo "$GCC_EXPORT" >> "$BASHRC"

  export PICO_SDK_PATH=$INSTALL_DIR/pico-sdk
}

build_example_project() {
  echo "Building pico-examples (blink)..."
  cd "$INSTALL_DIR/pico-examples"
  mkdir -p build && cd build
  cmake .. -DPICO_BOARD="$BOARD_TYPE" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
  make -j$(nproc)
}

build_custom_project() {
  echo "Building custom project: $CUSTOM_PROJECT_NAME..."
  cd "$INSTALL_DIR/$CUSTOM_PROJECT_NAME"
  mkdir -p build && cd build
  cmake .. -DPICO_BOARD="$BOARD_TYPE" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
  make -j$(nproc)
}

show_finish_info() {
  echo
  echo "Install and build complete."
  echo "If you're using Raspberry Pi Pico, plug in the device and copy the .uf2 file:"
  echo "cp blink/blink.uf2 /media/\$USER/RPI-RP2/"
}

# === 主流程 ===

install_dependencies
install_gcc_toolchain
clone_repositories
setup_environment_variables
build_example_project
build_custom_project
show_finish_info
