.. image:: https://img.shields.io/badge/rsttst-testable-brightgreen.svg
   :target: https://github.com/willemt/rsttst

.. image:: http://badges.github.io/stability-badges/dist/experimental.svg
   :target: http://github.com/badges/stability-badges

.. image:: https://travis-ci.org/willemt/peardb.png
   :target: https://travis-ci.org/willemt/peardb


What?
=====
PearDB is a durable HTTP Key-Value pair database. It uses `LMDB <http://symas.com/mdb/>`_ for storing data, and `H2O <https://github.com/h2o/h2o>`_ for HTTP.

Persistent connections and pipelining are built-in.

Goals
=====

* Speed
* Low latency
* Durability - An HTTP response means the write is on disk
* Simplicity outside (RESTful inteface)
* Simplicity inside (succinct codebase)
* HTTP caching - Because the CRUD is RESTful you could hypothetically use an HTTP reverse proxy cache to scale out reads. You could use multiple caches to create an eventually consistent database

Example usage
=============

All examples below make use of the excellent `httpie <https://github.com/jakubroztocil/httpie>`_

Starting the server
-------------------

.. code-block:: bash

   build/pear -d -p 8888
   echo daemonizing...

.. code-block:: bash

   daemonizing...

Get
---
You obtain a value by GET'ng the key.

In this case the key is "x":

.. code-block:: bash

   http -h --ignore-stdin 127.0.0.1:8888/x/

But you get a 404 if it doesn't exist:

.. code-block:: bash
   :class: dotted

   HTTP/1.1 404 NOT FOUND
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.0.0
   Connection: keep-alive
   content-length: 0

Put
---
We use PUT for creating or updating a key-value pair. PUTs are `durable <https://en.wikipedia.org/wiki/ACID#Durability>`_ - we only respond when change has been made to disk.

.. code-block:: bash

   echo "MY VALUE" | http -h PUT 127.0.0.1:8888/x/

.. code-block:: bash
   :class: dotted

   HTTP/1.1 200 OK
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.0.0
   Connection: keep-alive
   transfer-encoding: chunked

PUTs have an immediate change to future GETs. There is full `isolation <https://en.wikipedia.org/wiki/ACID#Isolation>`_, and therefore no `dirty reads <http://en.wikipedia.org/wiki/Isolation_(database_systems)#Dirty_reads>`_.

Now we can finally retrieve our data via a GET:

.. code-block:: bash

   http --ignore-stdin 127.0.0.1:8888/x/

.. code-block:: bash

   MY VALUE


Delete
------
DELETEs are durable - we only respond when change has been made to disk.

.. code-block:: bash

   http -h --ignore-stdin DELETE 127.0.0.1:8888/x/

.. code-block:: bash
   :class: dotted

   HTTP/1.1 200 OK
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.0.0
   Connection: keep-alive
   transfer-encoding: chunked

The key doesn't exist anymore:

.. code-block:: bash

   http -h --ignore-stdin 127.0.0.1:8888/x/

.. code-block:: bash
   :class: dotted

   HTTP/1.1 404 NOT FOUND
   Date: ..., ... .... ........ GMT 
   Server: h2o/1.0.0
   Connection: keep-alive
   content-length: 0

Building
========

Ubuntu
------
$ sudo apt-get install libuv

$ make libh2o

$ make

OSX
---
$ brew install --HEAD libuv

$ make libh2o

$ make
