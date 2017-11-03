#!/bin/bash

TOKEN='46234234278:ssdaduhjsdfhg238yrieflksdjfhjsdfh'
URL='https://my_cool_domain.com'
PORT='8443'

echo "Bot token is $TOKEN"
echo "Base url is $URL"
echo "Port is $PORT"

#echo Generated command is:

CMD="-F \"${URL}:${PORT}/${TOKEN}\" -F \"certificate=@public.pem\" https://api.telegram.org/bot${TOKEN}/setWebhook"

echo "$CMD"
echo
echo Executing curl...
echo
curl $CMD
