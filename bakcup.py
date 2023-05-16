    @staticmethod
    def clientListen(ip, port, window):
    #now we  will create a function that will be passed to a thread to run a server
        client._clnt_server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client._clnt_server_socket.bind((ip, port))
        client._clnt_server_socket.listen(1)
        while True:
            window['_CLIENT_'].print('waiting for a connection')
            connection, client_address = client._clnt_server_socket.accept()
            try:
                print('connection from', client_address)
                while True:
                    data = connection.recv(16)
                    if not data:
                        window['_CLIENT_'].print('no data from', client_address)
                        break
                    window['_CLIENT_'].print('received {!r}'.format(data))
                  
                    """
                    data = connection.recv(16)
                    if not data:
                        window['_CLIENT_'].print('no data from', client_address)
                        break
                    window['_CLIENT_'].print('received {!r}'.format(data))
                    
                    if data:
                        print('sending data back to the client')
                        connection.sendall(data)
                    else:
                        print('no data from', client_address)
                        break
                    """
            finally:
                connection.close()




    @staticmethod
    def clientListen(ip, port, window):
    #now we  will create a function that will be passed to a thread to run a server
        client._clnt_listen_is_running = True
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.bind((ip, port))
        sock.listen(1)
        while client._clnt_listen_is_running:
            window['_CLIENT_'].print('waiting for a connection')
            connection, client_address = sock.accept()
            try:
                print('connection from', client_address)
                while client._clnt_listen_is_running:
                    data = connection.recv(16)
                    if not data:
                        window['_CLIENT_'].print('no data from', client_address)
                        break
                    window['_CLIENT_'].print('received {!r}'.format(data))
                    """
                    if data:
                        print('sending data back to the client')
                        connection.sendall(data)
                    else:
                        print('no data from', client_address)
                        break
                    """
            finally:
                connection.close()