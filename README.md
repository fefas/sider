# Sider, my memory database implementation

This project is a memory database implementation in C++ as a personal side
project to get back to the language after a long time being a PHP developer.

The idea initially popped up to my mind while reading the book _Designing Data
Intensive Applications_ as Database implementations look pretty interesting
problems to get into and solve. However, it was when my team had to implement
a _Circuit breaker_ using Redis that I started to wonder how a Rater data type
could implement in Redis that could fit the circuit breaker use case.

And here it is, **Sider**.

To follow the development, you can check the [project page] or the [issues].

### Clients

So far, there is only a PHP client implemented.

> It is still not published in packagist. For now, you can take a look in the >
`clients/php` directory. Let's see when I will finally publish it :)

## How it works

Sider is a basic memory database that uses a key-value store to store different
kind of data idenfied by a pair `scope` and `key`. It utilises C++
`unordered_map` to store different types of data type.

Sider listens to the port `7963` and assumes it is running on a safe network as
it does not provide any authentication layer.

### Build and run

For running the project on local, you need to have Docker >= 2.32 installed.

1. To start the server, run the following commands in one terminal:

   ```base
   $ make build dev/up
   ```

2. To run the tests, run the following commands in another terminal:

   ```bash
   $ make server/tests clients/php/tests
   ```

### Supported commands

1. `GET scope key [partition]`

    Return value stored in the entry identified by `key` inside of the `scope`.
    Depending on the entry's data type associated with the entry, a `partition`
    may be required in order to retrieve the value.

2. `CLEAR scope [key]`
   
   Remove entry identified by `key` inside of the `scope`. If only `scope` is
   provided, removes all entries inside of the scope.

3. `KEEP scope key value [ttl]`

   Keep the `value` stored associated with the provided `scope:key`. If `ttl` is
   provided, the entry will be cleared after its expiration.

4. `COUNT scope key step [ttl]`

   Increment by `step` the counter identified by `key` inside of the `scope`. If
   `ttl` is provided, the `step` is decremented after its expiration and the
   entry is removed in case the counter comes back to 0.

5. `RATE scope key step partition [ttl]`

   Increment by `step` the rater `partition` identified by `key` inside of the
   `scope`.  If `ttl` is provided, the `step` is decremented after its
   expiration and the entry is removed in case the rater comes back to 0.

6. `QUEUE scope key value`

   Add `value` to the queue identified by `key` inside of the `scope`.

7. `DEQUEUE scope key`

   Remove oldest value from the queue identified by `key` inside of the `scope`.

### Communication Protocol

Currently, a custom protocol is implemented for the communication between
server and clients and it is as follows:

| Checksum | Version | Content Type | Content Length | Arguments    |
|----------|---------|--------------|----------------|--------------|
| 4 bytes  | 2 bytes | 2 bytes      | 4 bytes        | X bytes      |

Where the arguments part and its format depends on the content type provided
which determines the command to be executed:

| Command   | Value in Decimal : Hex |
|-----------|------------------------|
| `CLEAR`   | `20` : `00 14`         |
| `GET`     | `21` : `00 15`         |
| `KEEP`    | `22` : `00 16`         |
| `COUNT`   | `23` : `00 17`         |
| `RATE`    | `24` : `00 18`         |
| `QUEUE`   | `30` : `00 1e`         |
| `DEQUEUE` | `31` : `00 1f`         |



[project page]: https://github.com/users/fefas/projects/1
[issues]: https://github.com/fefas/sider/issues
