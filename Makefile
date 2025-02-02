MAKEFLAGS += --silent --always

DOCKER = docker compose
DOCKER_RUN = ${DOCKER} run --rm --remove-orphans
DOCKER_EXEC = ${DOCKER} exec

build:
	${DOCKER} build

clients/php/tests: path ?= tests/
clients/php/tests:
	${DOCKER_RUN} client-php ./vendor/bin/phpunit ${path}

clients/php/tests/%:
	make clients/php/tests path=tests/${*}

server/tests:
	$(if ${CI},${DOCKER_RUN},${DOCKER_EXEC}) server ctest --test-dir build/

#
# Dev
dev/status:
	${DOCKER} ps

dev/up:
	${DOCKER} up --watch server

dev/down:
	${DOCKER} down --remove-orphans

dev/logs:
	${DOCKER} logs -f

dev/server/bash:
	${DOCKER_EXEC} server bash

dev/client/php/bash:
	${DOCKER_EXEC} client-php sh
