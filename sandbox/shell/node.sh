#! /bin/sh


use_node () {
  export NVM_DIR="${HOME}/.nvm"

  . "${NVM_DIR}/nvm.sh"

  nvm use "$1"
}
