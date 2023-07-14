import socket
import select
import os

# To avoid thundering-herd problem. This requires kernel 4.5+.
EPOLLEXCLUSIVE = 1<<28

def child_process(epoll, sock, id):
  while True:
    events = epoll.poll()
    for fd, event in events:
      if fd == sock.fileno():
        try:
            conn, addr = sock.accept()
            print(f'Connected by {addr} pid: {id}')
            conn.close()
        except BlockingIOError:
            print("BlockingIOError")
        except Exception as e:
            print(e)

if __name__ == '__main__':

    # Create and bind socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(('127.0.0.1', 8080))
    sock.listen(5)
    sock.setblocking(False)

    # Create epoll instance
    epoll = select.epoll()
    epoll.register(sock.fileno(), select.EPOLLIN | EPOLLEXCLUSIVE)

    # Fork child processes
    for i in range(4):
        pid = os.fork()
        if pid == 0:
            child_process(epoll, sock, i)

    while True:
        pass
