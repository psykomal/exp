import socket
import select
import os

SERVER_HOST = 'localhost'
SERVER_PORT = 8080
MAX_CLIENTS = 10

def child_process(client_sockets, master_socket, id):
    
    while True:
        read_sockets, _, _ = select.select(client_sockets, [], [])
        
        for sock in read_sockets:
            if sock == master_socket:
                try:
                    client_socket, addr = master_socket.accept()
                    client_socket.setblocking(False)
                    print(f"New conection {addr} in process {id}")
                    client_sockets.append(client_socket)
                except:
                    break
            else:
                data = sock.recv(1024)
                if not data:
                    sock.close()
                    client_sockets.remove(sock)
                else:
                    sock.send(data)
                    
if __name__ == '__main__':

    master_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    master_socket.bind((SERVER_HOST, SERVER_PORT))
    master_socket.listen(MAX_CLIENTS)
    master_socket.setblocking(False)
    
    client_sockets = [master_socket]
    
    for i in range(100):
        pid = os.fork()
        if pid == 0:
            child_process(client_sockets, master_socket, i)
            
    while True:
        pass
        
