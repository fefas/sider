MAKEFLAGS += --silent --always

DOCKER = docker compose
DOCKER_BUILD = ${DOCKER} build
DOCKER_RUN = ${DOCKER} run --build --rm --remove-orphans

build:
	${DOCKER_BUILD}

up:
	${DOCKER} up --build

clients/php:
	${DOCKER_BUILD} client-php

tests:
	${DOCKER_RUN} client-php

sh:
	${DOCKER_RUN} --entrypoint=sh server
