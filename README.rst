.. image:: https://img.shields.io/badge/rsttst-testable-brightgreen.svg
   :target: https://github.com/willemt/rsttst

.. image:: http://badges.github.io/stability-badges/dist/experimental.svg
   :target: http://github.com/badges/stability-badges

.. image:: https://travis-ci.org/willemt/pearldb.png
   :target: https://travis-ci.org/willemt/pearldb


What?
=====
PearlDB is a durable HTTP Key-Value pair database. It uses `LMDB <http://symas.com/mdb/>`_ for storing data, and `H2O <https://github.com/h2o/h2o>`_ for HTTP.

PearlDB is completely written in C.

Persistent connections and pipelining are built-in.

PearlDB uses `bmon <https://github.com/willemt/bmon>`_ to batch LMDB writes.

Goals
=====

* Speed
* Low latency
* Durability - An HTTP response means the write is on disk
* Simplicity outside (RESTful inteface)
* Simplicity inside (succinct codebase)
* HTTP caching - Because the CRUD is RESTful you could hypothetically use an HTTP reverse proxy cache to scale out reads. You could use multiple caches to create an eventually consistent database

Ubuntu Quick Start
==================

.. code-block:: bash
   :class: ignore

   sudo add-apt-repository -y ppa:willemt/pearldb
   sudo apt-get update
   sudo apt-get install pearldb

Example usage
=============

All examples below make use of the excellent `httpie <https://github.com/jakubroztocil/httpie>`_

Starting
--------

.. code-block:: bash

   sudo build/pearl --daemonize --port 80 --db_size 1
   echo daemonizing...

.. code-block:: bash

   daemonizing...

Get
---
We obtain a value by GET'ng the key.

In this case the key is "x". But we get a 404 if it doesn't exist.

.. code-block:: bash

   http -h --ignore-stdin 127.0.0.1/x/

.. code-block:: bash
   :class: dotted

   HTTP/1.1 404 NOT FOUND
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive
   content-length: 0

You MUST specify a path.

.. code-block:: bash

   http -h --ignore-stdin 127.0.0.1/ | head -n 1

.. code-block:: bash
   :class: dotted

   HTTP/1.1 400 BAD PATH

Put
---
We use PUT for creating or updating a key value pair. PUTs are `durable <https://en.wikipedia.org/wiki/ACID#Durability>`_ - we only respond when change has been made to disk.

.. code-block:: bash

   echo "MY VALUE" | http -h PUT 127.0.0.1/x/

.. code-block:: bash
   :class: dotted

   HTTP/1.1 200 OK
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive
   transfer-encoding: chunked

PUTs have an immediate change on the resource. There is full `isolation <https://en.wikipedia.org/wiki/ACID#Isolation>`_, and therefore no `dirty reads <http://en.wikipedia.org/wiki/Isolation_(database_systems)#Dirty_reads>`_.

Now we can finally retrieve our data via a GET:

.. code-block:: bash

   http --ignore-stdin 127.0.0.1/x/

.. code-block:: bash

   MY VALUE

The slash at the end is optional.

.. code-block:: bash

   http --ignore-stdin 127.0.0.1/x

.. code-block:: bash

   MY VALUE

The user must specify the capacity of the database upfront. PearlDB does not support automatic resizing. A PUT will fail if it would put the database over capacity.

.. code-block:: bash

   head -c 1000000 /dev/urandom | base64 > tmp_file
   du -h tmp_file | awk '{ print $1 }'
   cat tmp_file | http -h PUT 127.0.0.1/1/
   rm tmp_file

.. code-block:: bash
   :class: dotted

   1.3M
   HTTP/1.1 400 NOT ENOUGH SPACE
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive
   content-length: 0

You can't PUT under nested resources.

.. code-block:: bash

   echo 'DATA' | http -h PUT 127.0.0.1/x/nested_resource/

.. code-block:: bash
   :class: dotted

   HTTP/1.1 400 BAD PATH
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive
   content-length: 0

Put without a key
-----------------
If you want PearlDB to generate a key for you, just use POST.

.. code-block:: bash

   echo "MY POSTED VALUE" | http -h POST 127.0.0.1/ > posted.txt
   cat posted.txt

.. code-block:: bash
   :class: dotted

   HTTP/1.1 200 OK
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive
   location: ...
   transfer-encoding: chunked

The Location header in the response has the URI of the newly created resource. The URI is the URL safe base64 encoded UUID4.

.. code-block:: bash

   http --ignore-stdin -b GET 127.0.0.1$(grep location: posted.txt | sed -e 's/location: //' | tr -d '\r\n')

.. code-block:: bash
   :class: dotted

   MY POSTED VALUE

Providing a URL (ie. key) with POST doesn't make sense, and will result in a 400.

.. code-block:: bash

   echo "MY POSTED VALUE" | http -h POST 127.0.0.1/xxxx/

.. code-block:: bash
   :class: dotted

   HTTP/1.1 400 BAD
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive
   content-length: 0

Get keys
--------
You can get the keys that match a prefix by using the /key/XXX/ nested resource.

.. code-block:: bash

   echo '' | http PUT 127.0.0.1/1/ > /dev/null
   echo '' | http PUT 127.0.0.1/199/ > /dev/null
   echo '' | http PUT 127.0.0.1/102/ > /dev/null
   echo '' | http PUT 127.0.0.1/2/ > /dev/null
   http GET 127.0.0.1/key/1/

.. code-block:: bash
   :class: dotted

   1
   102
   199

Without a prefix you get all keys.

.. code-block:: bash

   http GET 127.0.0.1/key/

.. code-block:: bash
   :class: dotted

   1
   102
   199
   2
   ...

Existence
---------
To check for existence use the HEAD method. This is great, because you don't waste bandwidth sending the document body.

.. code-block:: bash

   http -h --ignore-stdin HEAD 127.0.0.1/x/

.. code-block:: bash
   :class: dotted

   HTTP/1.1 200 OK
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive

Delete
------
DELETEs are durable - we only respond when the change has been made to disk.

.. code-block:: bash

   http -h --ignore-stdin DELETE 127.0.0.1/x/

.. code-block:: bash
   :class: dotted

   HTTP/1.1 200 OK
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive
   transfer-encoding: chunked

Of course, after a DELETE the key doesn't exist anymore:

.. code-block:: bash

   http -h --ignore-stdin 127.0.0.1/x/

.. code-block:: bash
   :class: dotted

   HTTP/1.1 404 NOT FOUND
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive
   content-length: 0

Compare and Swap (CAS)
----------------------
A form of `opportunistic concurrency control <http://en.wikipedia.org/wiki/Optimistic_concurrency_control>`_ is available through `ETags <http://en.wikipedia.org/wiki/HTTP_ETag>`_.

When the client provides the Prefers: ETag header on a GET request we generate an ETag. A client can then use the `If-Match <https://msdn.microsoft.com/en-us/library/dd541480.aspx>`_ header with the ETag to perform a conditional update, (ie. a CAS operation). If the ETag has changed then the PUT operation will fail. CAS operations are great because there is no locking; if a CAS operation fails for one client that means it has succeeded for another, ie. there has been progress.

Imagine two clients trying to update the same key. Client 1 requests an ETag. The ETag is provided via the etag header.

.. code-block:: bash

   echo 'SWEET DATA' | http -h --ignore-stdin PUT 127.0.0.1/x/ > /dev/null
   http -h --ignore-stdin GET 127.0.0.1/x/ Prefers:ETag > etag.txt
   cat etag.txt

.. code-block:: bash
   :class: dotted

   HTTP/1.1 200 OK
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive
   etag: ...
   transfer-encoding: chunked

If client 1 requests an ETag again, the same ETag is sent:

.. code-block:: bash

   http -h --ignore-stdin GET 127.0.0.1/x/ Prefers:ETag > etag2.txt
   cat etag2.txt
   diff <(grep etag etag.txt) <(grep etag etag2.txt)

.. code-block:: bash
   :class: dotted

   HTTP/1.1 200 OK
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive
   etag: ...
   transfer-encoding: chunked

Client 2 does a PUT on x. This will invalidate the ETag.

.. code-block:: bash

   echo 'SURPRISE' | http -h PUT 127.0.0.1/x/

.. code-block:: bash
   :class: dotted

   HTTP/1.1 200 OK
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive
   transfer-encoding: chunked

Client 1 uses a conditional PUT to update "x" using the If-Match tag. Because the ETag was invalidated, we don't commit, and respond with 412 Precondition Failed.

.. code-block:: bash

   echo 'MY NEW VALUE BASED OFF OLD VALUE' | http -h PUT 127.0.0.1/x/ If-Match:$(grep etag: etag.txt | sed -e 's/etag: //' | tr -d '\r\n')

.. code-block:: bash
   :class: dotted

   HTTP/1.1 412 BAD ETAG
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive
   content-length: 0

Once this happens we can retry the PUT after we do a new GET.

.. code-block:: bash

   http -h GET 127.0.0.1/x/ Prefers:ETag > etag3.txt
   cat etag3.txt

.. code-block:: bash
   :class: dotted

   HTTP/1.1 200 OK
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive
   etag: ...
   transfer-encoding: chunked

The PUT will succeed because the ETag is still valid.

.. code-block:: bash

   echo 'NEW VALUE' | http -h PUT 127.0.0.1/x/ If-Match:$(grep etag: etag3.txt | sed -e 's/etag: //' | tr -d '\r\n')

.. code-block:: bash
   :class: dotted

   HTTP/1.1 200 OK
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive
   transfer-encoding: chunked

However, if we use the ETag again it will fail.

.. code-block:: bash

   echo 'NEW VALUE2' | http -h PUT 127.0.0.1/x/ If-Match:$(grep etag: etag3.txt | sed -e 's/etag: //' | tr -d '\r\n')

.. code-block:: bash
   :class: dotted

   HTTP/1.1 412 BAD ETAG
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.3.1
   Connection: keep-alive
   content-length: 0

Notes about ETags:

- On reboots, PearlDB loses all ETag information
- On launch PearlDB generates a random ETag prefix
- ETags are expected to have a short life (ie. < 1 day)

Shutting down
-------------

.. code-block:: bash

   cat /var/run/pearl.pid | sudo xargs kill -9
   echo shutdown

.. code-block:: bash
   :class: dotted

   shutdown


Building
========

$ make libuv

$ make libh2o

$ make libck

$ make
