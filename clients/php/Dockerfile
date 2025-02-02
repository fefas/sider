FROM php:8.4.2-cli-alpine3.21

ENV XDEBUG_MODE coverage

RUN apk add --no-cache \
        linux-headers \
        $PHPIZE_DEPS && \
    pecl install \
        xdebug && \
    docker-php-ext-install \
        sockets && \
    docker-php-ext-enable \
        xdebug

COPY --from=composer:2.8.4 /usr/bin/composer /usr/local/bin/composer

WORKDIR /var/www/sider-client

COPY composer.* .
RUN composer install
COPY . .

ENTRYPOINT ["./vendor/bin/phpunit"]
