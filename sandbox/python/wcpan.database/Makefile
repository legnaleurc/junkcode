.PHONY: all clean upload test install

all:
	python setup.py sdist bdist_wheel

upload: clean all
	twine upload ./dist/*

clean:
	rm -rf ./dist ./build ./*.egg-info

test:
	python -m compileall wcpan

install:
	pip install -e .
