import socket
import select
import os

# To avoid thundering-herd problem. This requires kernel 4.5+.
EPOLLEXCLUSIVE = 1<<28

def child_process(epoll, sock, id):
    while True:
        try:
            epoll.poll()
        except IOError:
            continue
        while True:
            try:
                cd, _ = sock.accept()
                print(f"worker {id}")
            except socket.error:
                break
            except Exception as e:
                print(f"Some other exp {e}")
                break
        cd.close()

if __name__ == '__main__':

    # Create and bind socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(('127.0.0.1', 8080))
    sock.listen(10)
    sock.setblocking(False)


    # Fork child processes
    for i in range(3):
        pid = os.fork()
        if pid == 0:
            # Create epoll instance
            epoll = select.epoll()
            epoll.register(sock, select.EPOLLIN | EPOLLEXCLUSIVE)
            child_process(epoll, sock, i)

    os.wait()
