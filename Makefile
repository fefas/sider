MAKEFLAGS += --silent --always

DOCKER = docker compose
DOCKER_BUILD = ${DOCKER} build
DOCKER_RUN = ${DOCKER} run --rm --remove-orphans

build:
	${DOCKER_BUILD}

up:
	${DOCKER} up server

down:
	${DOCKER} down --remove-orphans

clients/php/tests:
	${DOCKER_RUN} client-php ./vendor/bin/phpunit

server/tests:
	TARGET=tests ${DOCKER_RUN} server ./tests/run --gtest_color=yes
