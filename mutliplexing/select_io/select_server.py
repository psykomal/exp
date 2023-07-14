import socket
import select
import os

def child_process(epoll, sock):
  while True:
    events = epoll.poll()
    for fd, event in events:
      if fd == sock.fileno():
        conn, addr = sock.accept()
        print('Connected by', addr)
        conn.close()

if __name__ == '__main__':

    # Create and bind socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(('127.0.0.1', 8080))
    sock.listen(5)
    sock.setblocking(False)

    # Create epoll instance
    epoll = select.epoll()
    epoll.register(sock.fileno(), select.EPOLLIN)

    # Fork child processes
    for i in range(4):
        pid = os.fork()
        if pid == 0:
            child_process(epoll, sock)

    while True:
        pass
