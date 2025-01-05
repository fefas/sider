MAKEFLAGS += --silent --always

DOCKER = docker compose
DOCKER_BUILD = ${DOCKER} build
DOCKER_RUN = ${DOCKER} run --rm --remove-orphans
DOCKER_LOGS = ${DOCKER} logs -f

build:
	${DOCKER_BUILD}

up:
	${DOCKER} up --watch server

down:
	${DOCKER} down --remove-orphans

logs:
	${DOCKER_LOGS}

%/logs:
	${DOCKER_LOGS} ${*}

clients/php/tests:
	${DOCKER_RUN} client-php ./vendor/bin/phpunit

server/tests:
	${DOCKER_RUN} server ctest --test-dir build/

server/bash:
	${DOCKER_RUN} server bash