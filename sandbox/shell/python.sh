#! /bin/sh


use_python () {
  # pyenv
  export PYENV_ROOT="${HOME}/.pyenv"
  if [ -d "${PYENV_ROOT}" ]; then
    export PATH="${PYENV_ROOT}/bin:${PATH}"
    eval "$(pyenv init --path)"
  fi

  # poetry
  export PATH="${HOME}/.local/bin:${PATH}"

  # switch python version
  export PYENV_VERSION="$1"

  # env
  python3 --version
  poetry --version
}
