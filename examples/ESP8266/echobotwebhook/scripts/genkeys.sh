#!/bin/bash

openssl req -newkey rsa:1024 -sha256 -nodes -keyout private.key -x509 -days 3650 -out public.pem
openssl x509 -in public.pem -outform DER -out public.dig
openssl rsa -in private.key -out private.dig -outform DER

xxd -i public.dig       | sed 's/.*{//' | sed 's/\};//' | sed 's/unsigned.*//' > "x509.h"
xxd -i private.dig      | sed 's/.*{//' | sed 's/\};//' | sed 's/unsigned.*//' > "key.h"

