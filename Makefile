MAKEFLAGS += --silent --always

DOCKER = docker compose -f docker-compose.yaml $(if ${CI},,-f docker-compose.dev.yaml)
DOCKER_RUN = ${DOCKER} run --rm --remove-orphans
DOCKER_EXEC = ${DOCKER} exec

build:
	${DOCKER} build

clients/php/tests:
	${DOCKER_RUN} client-php

clients/php/tests/%:
	${DOCKER_RUN} client-php tests/${*}

server/tests:
	$(if ${CI},${DOCKER_RUN},${DOCKER_EXEC}) server ctest --test-dir build/

#
# Dev
dev/status:
	${DOCKER} ps

dev/up:
	${DOCKER} build
	${DOCKER} up --watch server

dev/down:
	${DOCKER} down --remove-orphans

dev/logs:
	${DOCKER} logs -f

dev/server/bash:
	${DOCKER_EXEC} server bash

dev/client/php/bash:
	${DOCKER_EXEC} client-php sh
