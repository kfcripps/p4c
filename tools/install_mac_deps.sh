#! /bin/bash

# Script to install P4C dependencies on MacOS.

set -e  # Exit on error.
set -x  # Make command execution verbose

# Installation helper.
brew_install() {
    echo "\nInstalling $1"
    if brew list $1 &>/dev/null; then
        echo "${1} is already installed"
    else
        brew install --ignore-dependencies $1 && echo "$1 is installed"
    fi
}

# Check if brew shellenv command is already in zprofile.
if ! grep -q 'brew shellenv' ~/.zprofile; then
    # Set up Homebrew differently for arm64.
    if [[ $(uname -m) == 'arm64' ]]; then
        (echo; echo 'eval "$(/opt/homebrew/bin/brew shellenv)"') >> ~/.zprofile
        eval "$(/opt/homebrew/bin/brew shellenv)"
    else
        (echo; echo 'eval "$(/usr/local/bin/brew shellenv)"') >> ~/.zprofile
        eval "$(/usr/local/bin/brew shellenv)"
    fi
fi
# Source zprofile.
source ~/.zprofile

# Check if Homebrew is already installed.
if ! which brew > /dev/null 2>&1; then
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi
HOMEBREW_PREFIX=$(brew --prefix)

# Fetch the latest formulae
brew update

BOOST_LIB="boost@1.85"
REQUIRED_PACKAGES=(
    autoconf automake ccache cmake libtool
    openssl coreutils bison grep ninja virtualenv uv
    ${BOOST_LIB}
)
for package in "${REQUIRED_PACKAGES[@]}"; do
  brew_install ${package}
done

# Check if linking is needed.
if ! brew ls --linked --formula ${BOOST_LIB} > /dev/null 2>&1; then
  brew link ${BOOST_LIB}
fi

# Check if PATH modification is needed.
if ! grep -q "$(brew --prefix bison)/bin" ~/.bash_profile; then
  echo 'export PATH="$(brew --prefix bison)/bin:$PATH"' >> ~/.bash_profile
fi
if ! grep -q "$HOMEBREW_PREFIX/opt/grep/libexec/gnubin" ~/.bash_profile; then
  echo 'export PATH="$HOMEBREW_PREFIX/opt/grep/libexec/gnubin:$PATH"' >> ~/.bash_profile
fi
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bash_profile
source ~/.bash_profile

# Set up uv for Python dependency management.
uv sync
