#!/usr/bin/python3
import os

# Hello world python program


handler = {}
page = ''
if 'HTTP_COOKIE' in os.environ:
    cookies = os.environ['HTTP_COOKIE']
    cookies = cookies.split(';')

    for cookie in cookies:
        cookie = cookie.split('=')
        handler[cookie[0]] = cookie[1]

for k in handler:
    page += "<p> <span style='font-weight: bold'>" + k + "</span>: " + handler[k] + " </p> </br>"
    print ('Set-Cookie:' + k + '=' + handler[k])

print ("Content-type:text/html\r\n\r\n")
print (page)