MAKEFLAGS += --silent --always

DOCKER = docker compose
DOCKER_RUN = ${DOCKER} run --rm --remove-orphans

build:
	${DOCKER} build

clients/php/tests: path ?= tests/
clients/php/tests:
	${DOCKER_RUN} client-php ./vendor/bin/phpunit ${path}

clients/php/tests/%:
	make clients/php/tests path=tests/${*}

server/tests:
	${DOCKER_RUN} server ./build/sider-tests
	# ${DOCKER_RUN} server ctest --test-dir build/

#
# Dev
dev/setup:
	make dev/down
	mkdir -p ./server/build
	rm -r ./server/build/* 2>/dev/null || true
	make build dev/up

dev/up:
	${DOCKER} up --watch server

dev/down:
	${DOCKER} down --remove-orphans

dev/logs:
	${DOCKER} logs -f

dev/tests:
	make build
	make server/tests
	make clients/php/tests

dev/server/bash:
	${DOCKER_RUN} server bash

dev/client/php/bash:
	${DOCKER_RUN} client-php sh
