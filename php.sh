# export GATEWAY_INTERFACE="CGI/1.1"
# export SCRIPT_FILENAME="/Users/ymarji/Desktop/Webserver/public/test.php"
# export REQUEST_METHOD="POST"
# export REDIRECT_STATUS=200
# export SERVER_PROTOCOL="HTTP/1.1"
# export REMOTE_HOST="127.0.0.1"
# export CONTENT_LENGHT=3
# # export HTTP_ACCEPT="text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"
# export CONTENT_TYPE="application/x-www-form-urlencoded"
# export BODY="t=1"
# exec echo "$BODY" | php-cgi


echo "POST /path/script.cgi HTTP/1.0\r\nFrom: frog@jmarshall.com\r\nUser-Agent: HTTPTool/1.0\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n" | telnet 127.0.0.1 8080

