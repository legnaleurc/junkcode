RM := rm -rf
PYTHON := poetry run -- python3
BLACK := poetry run -- black

PKG_FILES := pyproject.toml
PKG_LOCK := poetry.lock
ENV_DIR := .venv
ENV_LOCK := $(ENV_DIR)/pyvenv.cfg

MODULE_LIST := app

.PHONY: all format lint purge test venv

all: venv

format: venv
	$(BLACK) $(MODULE_LIST)

lint: venv
	$(BLACK) --check $(MODULE_LIST)

purge:
	$(RM) -rf $(ENV_DIR)

test: venv
	$(PYTHON) -m compileall $(MODULE_LIST)

venv: $(ENV_LOCK)

$(ENV_LOCK): $(PKG_LOCK)
	poetry install
	touch $@

$(PKG_LOCK): $(PKG_FILES)
	poetry lock --no-update
	touch $@
