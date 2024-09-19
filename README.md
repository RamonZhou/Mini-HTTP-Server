# Mini HTTP Server

Implementation of a simple HTTP server listening at port 80 on local machine. It supports:

- multiple TCP connections
- GET and POST methods
- multiple content types such as `text/html`, `image/jpeg`, `text/plain`, `application/octet-stream`, forms, etc.
- easy way to configure more paths because of functional programming
- flexibility since we used a thread pool