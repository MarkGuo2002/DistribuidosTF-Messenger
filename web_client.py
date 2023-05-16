import subprocess
import sys
import PySimpleGUI as sg
from enum import Enum
import argparse
import socket
import threading
from zeep import Client


def readSocket(sock):
    """Reads a message from the socket until the NULL character is found, and returns it as a string"""
    acc = ''
    while True:
        msg = sock.recv(1)
        if (msg == b'\0'):
            break
        acc += msg.decode()
    return acc

import socket

def findAvailablePort():
    # Create a socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Bind to port 0, which allows the operating system to assign an available port
    sock.bind(('localhost', 0))
    _, port = sock.getsockname()
    sock.close()
    return port


def getIpAddress():
    # Create a temporary socket
    temp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # Connect the temporary socket to a remote server (doesn't matter which server)
        temp_sock.connect(("8.8.8.8", 80))
        ip_address = temp_sock.getsockname()[0]
    finally:
        temp_sock.close()

    return ip_address


def clientListen(clnt_socket, window):
    #now we  will create a function that will be passed to a thread to run a server
        try:
            window['_CLIENT_'].print('waiting for a connection')
            accepted_sock, client_address = clnt_socket.accept()
            print('connection from', client_address)
            while True:
                window['_SERVER_'].print("s> HERE'S THREAD WAITING FOR SERVER MSG")
                data = readSocket(accepted_sock)
                if data == 'SEND_MESSAGE':
                    window['_SERVER_'].print("s> SERVER SENT SEND_MESSAGE")
                    alias_sender = readSocket(accepted_sock)
                    message_id = readSocket(accepted_sock)
                    message_content = readSocket(accepted_sock)
                    window['_SERVER_'].print(f"s> MESSAGE {message_id} FROM {alias_sender}\n{message_content}")

        except:
            window['_CLIENT_'].print('Entered exception')
            

        finally:
            accepted_sock.close()
    
    
#window['_CLIENT_'].print('no data from', client_address)

class client :

    # ******************** TYPES *********************
    # *
    # * @brief Return codes for the protocol methods
    class RC(Enum) :
        OK = 0
        ERROR = 1
        USER_ERROR = 2

    # ****************** ATTRIBUTES ******************
    _server = None
    _port = -1
    _soap = Client('http://localhost:8000/?wsdl')
    _quit = 0
    _username = None
    _alias = None
    _date = None
    _clnt_server_socket = None
    _clnt_listen_thread = None

    


    # ******************** METHODS *******************
    # *
    # * User is just alias
    # * @param user - User name to register in the system
    # *
    # * @return OK if successful
    # * @return USER_ERROR if the user is already registered
    # * @return ERROR if another error occurred
    @staticmethod
    def register(user, window):
        msg = ''
        #  Write your code here
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        sock.connect((client._server, client._port))
        window['_CLIENT_'].print("Connecting to server: " + client._server + " on port: " + str(client._port) + "...\n", end=" ")
        try:
            msg = b'REGISTER\0'    
            sock.sendall(msg)
            # send user._username, user._alias and user._date to server through socket
            sock.sendall(client._username.encode())
            sock.sendall(b'\0')
            sock.sendall(client._alias.encode())
            sock.sendall(b'\0')
            sock.sendall(client._date.encode())
            sock.sendall(b'\0')
            # read response from server
            msg = sock.recv(1)
            if (msg == b'0'):
                window['_SERVER_'].print("s> REGISTER OK")
                return client.RC.OK
            elif (msg == b'1'):
                window['_SERVER_'].print("s> USERNAME IN USE")
                return client.RC.USER_ERROR
            else:
                window['_SERVER_'].print("s> REGISTER FAIL")
                return client.RC.ERROR
        
        finally:
            #Closing socket
            sock.close()


    # *
    # 	 * @param user - User name to unregister from the system
    # 	 *
    # 	 * @return OK if successful
    # 	 * @return USER_ERROR if the user does not exist
    # 	 * @return ERROR if another error occurred
    @staticmethod
    def  unregister(user, window):
        #  Write your code here
        msg = ''
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((client._server, client._port))
        window['_CLIENT_'].print("Connecting to server: " + client._server + " on port: " + str(client._port) + "...\n", end=" ")
        try:
            msg = b'UNREGISTER\0'    
            sock.sendall(msg)
            # send user._username, user._alias and user._date to server through socket
            sock.sendall(client._alias.encode())
            sock.sendall(b'\0')
            # read response from server
            msg = sock.recv(1)
            if (msg == b'0'): 
                window['_SERVER_'].print("s> UNREGISTER OK")
                client._username = None
                client._alias = None
                client._date = None
                return client.RC.OK
            elif (msg == b'1'):
                window['_SERVER_'].print("s> USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            else:
                window['_SERVER_'].print("s> UNREGISTER FAIL")
                return client.RC.ERROR
        
        finally:
            #Closing socket
            sock.close()


    # *
    # * @param user - User name to connect to the system
    # *
    # * @return OK if successful
    # * @return USER_ERROR if the user does not exist or if it is already connected
    # * @return ERROR if another error occurred
    @staticmethod
    def connect(user, window):
        # find available port
        client._clnt_server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client._clnt_server_socket.bind(('' , 0))
        client._clnt_server_socket.listen(1)
        my_port = client._clnt_server_socket.getsockname()[1]

        window['_CLIENT_'].print("Available IP: ??? on port: " + str(my_port) + "...\n", end=" ")
        
        # sending messages to the server
        msg = ''
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((client._server, client._port))

        client._clnt_listen_thread = threading.Thread(target=clientListen, args=(client._clnt_server_socket, window))
        client._clnt_listen_thread.start()
        try:
            msg = b'CONNECT\0'    
            sock.sendall(msg)
            # send user._username, user._alias and user._date to server through socket
            sock.sendall(client._alias.encode())
            sock.sendall(b'\0')
            sock.sendall(str(my_port).encode())
            sock.sendall(b'\0')
            # read response from server
            msg = sock.recv(1)
            if (msg == b'0'): 
                window['_SERVER_'].print("s> CONNECT OK")
                return client.RC.OK
            elif (msg == b'1'):
                window['_SERVER_'].print("s> CONNECT FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            elif (msg == b'2'):
                window['_SERVER_'].print("s> USER ALREADY CONNECTED")
                return client.RC.USER_ERROR
            else:
                window['_SERVER_'].print("s> CONNECT FAIL")
                return client.RC.ERROR
        
        finally:
            #Closing socket
            sock.close()


    # *
    # * @param user - User name to disconnect from the system
    # *
    # * @return OK if successful
    # * @return USER_ERROR if the user does not exist
    # * @return ERROR if another error occurred
    @staticmethod
    def  disconnect(user, window):
        #  Write your code here

        if client._clnt_server_socket:
            client._clnt_server_socket.close()
        
        msg = ''
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((client._server, client._port))
        try:
            msg = b'DISCONNECT\0'    
            sock.sendall(msg)
            sock.sendall(client._alias.encode())
            sock.sendall(b'\0')
            
            # read response from server
            msg = sock.recv(1)
            if (msg == b'0'): 
                window['_SERVER_'].print("s> DISCONNECT OK")
                return client.RC.OK
            elif (msg == b'1'):
                window['_SERVER_'].print("s> DISCONNECT FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            elif (msg == b'2'):
                window['_SERVER_'].print("s> USER NOT CONNECTED")
                return client.RC.USER_ERROR
            else:
                window['_SERVER_'].print("s> DISCONNECT FAIL")
                return client.RC.ERROR
        
        finally:
            #Closing socket
            sock.close()

    # *
    # * @param user    - Receiver user name
    # * @param message - Message to be sent
    # *
    # * @return OK if the server had successfully delivered the message
    # * @return USER_ERROR if the user is not connected (the message is queued for delivery)
    # * @return ERROR the user does not exist or another error occurred
    @staticmethod
    def  send(user, message, window):
        if len(message) > 256:
            return client.RC.ERROR
        
        message = client._soap.service.reduce_spaces_server(message)

        window['_SERVER_'].print("s> SEND " + user + " " + message + "\n", end="")
        #  Write your code here
        msg = ''
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((client._server, client._port))
        try:
            msg = b'SEND\0'    
            sock.sendall(msg)
            sock.sendall(client._alias.encode())
            sock.sendall(b'\0')
            sock.sendall(user.encode())
            sock.sendall(b'\0')
            sock.sendall(message.encode())
            sock.sendall(b'\0')

            # read response from server
            msg = sock.recv(1)
            if (msg == b'0'): 
                msg_id = readSocket(sock)
                window['_SERVER_'].print("s> SEND OK - MESSAGE: " + msg_id)
                
                return client.RC.OK
            elif (msg == b'1'):
                window['_SERVER_'].print("s> SEND FAIL, USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            else:
                window['_SERVER_'].print("s> SEND FAIL")
                return client.RC.ERROR
        
        finally:
            #Closing socket
            sock.close()


    @staticmethod
    def  connectedUsers(window):
        window['_SERVER_'].print("s> CONNECTED USERS OK")
        #  Write your code here
        return client.RC.ERROR


    @staticmethod
    def window_register():
        layout_register = [[sg.Text('Ful Name:'),sg.Input('Text',key='_REGISTERNAME_', do_not_clear=True, expand_x=True)],
                            [sg.Text('Alias:'),sg.Input('Text',key='_REGISTERALIAS_', do_not_clear=True, expand_x=True)],
                            [sg.Text('Date of birth:'),sg.Input('',key='_REGISTERDATE_', do_not_clear=True, expand_x=True, disabled=True, use_readonly_for_disable=False),
                            sg.CalendarButton("Select Date",close_when_date_chosen=True, target="_REGISTERDATE_", format='%d-%m-%Y',size=(10,1))],
                            [sg.Button('SUBMIT', button_color=('white', 'blue'))]
                            ]

        layout = [[sg.Column(layout_register, element_justification='center', expand_x=True, expand_y=True)]]

        window = sg.Window("REGISTER USER", layout, modal=True)
        choice = None

        while True:
            event, values = window.read()

            if (event in (sg.WINDOW_CLOSED, "-ESCAPE-")):
                break

            if event == "SUBMIT":
                if(values['_REGISTERNAME_'] == 'Text' or values['_REGISTERNAME_'] == '' or values['_REGISTERALIAS_'] == 'Text' or values['_REGISTERALIAS_'] == '' or values['_REGISTERDATE_'] == ''):
                    sg.Popup('Registration error', title='Please fill in the fields to register.', button_type=5, auto_close=True, auto_close_duration=1)
                    continue

                client._username = values['_REGISTERNAME_']
                client._alias = values['_REGISTERALIAS_']
                client._date = values['_REGISTERDATE_']
                break
        window.Close()


    # *
    # * @brief Prints program usage
    @staticmethod
    def usage() :
        print("Usage: python3 py -s <server> -p <port>")


    # *
    # * @brief Parses program execution arguments
    @staticmethod
    def  parseArguments(argv) :
        parser = argparse.ArgumentParser()
        parser.add_argument('-s', type=str, required=True, help='Server IP')
        parser.add_argument('-p', type=int, required=True, help='Server Port')
        args = parser.parse_args()

        if (args.s is None):
            parser.error("Usage: python3 py -s <server> -p <port>")
            return False

        if ((args.p < 1024) or (args.p > 65535)):
            parser.error("Error: Port must be in the range 1024 <= port <= 65535")
            return False

        client._server = args.s
        client._port = args.p

        return True


    def main(argv):

        if (not client.parseArguments(argv)):
            client.usage()
            exit()


        lay_col = [[sg.Button('REGISTER',expand_x=True, expand_y=True),
                sg.Button('UNREGISTER',expand_x=True, expand_y=True),
                sg.Button('CONNECT',expand_x=True, expand_y=True),
                sg.Button('DISCONNECT',expand_x=True, expand_y=True),
                sg.Button('CONNECTED USERS',expand_x=True, expand_y=True)],
                [sg.Text('Dest:'),sg.Input('User',key='_INDEST_', do_not_clear=True, expand_x=True),
                sg.Text('Message:'),sg.Input('Text',key='_IN_', do_not_clear=True, expand_x=True),
                sg.Button('SEND',expand_x=True, expand_y=False)],
                [sg.Text('Attached File:'), sg.In(key='_FILE_', do_not_clear=True, expand_x=True), sg.FileBrowse(),
                sg.Button('SENDATTACH',expand_x=True, expand_y=False)],
                [sg.Multiline(key='_CLIENT_', disabled=True, autoscroll=True, size=(60,15), expand_x=True, expand_y=True),
                sg.Multiline(key='_SERVER_', disabled=True, autoscroll=True, size=(60,15), expand_x=True, expand_y=True)],
                [sg.Button('QUIT', button_color=('white', 'red'))]
            ]


        layout = [[sg.Column(lay_col, element_justification='center', expand_x=True, expand_y=True)]]

        window = sg.Window('Messenger', layout, resizable=True, finalize=True, size=(1000,400))
        window.bind("<Escape>", "-ESCAPE-")


        while True:
            event, values = window.Read()

            if (event in (None, 'QUIT')) or (event in (sg.WINDOW_CLOSED, "-ESCAPE-")):
                sg.Popup('Closing Client APP', title='Closing', button_type=5, auto_close=True, auto_close_duration=1)
                break

            #if (values['_IN_'] == '') and (event != 'REGISTER' and event != 'CONNECTED USERS'):
             #   window['_CLIENT_'].print("c> No text inserted")
             #   continue

            if (client._alias == None or client._username == None or client._alias == 'Text' or client._username == 'Text' or client._date == None) and (event != 'REGISTER'):
                sg.Popup('NOT REGISTERED', title='ERROR', button_type=5, auto_close=True, auto_close_duration=1)
                continue

            if (event == 'REGISTER'):
                client.window_register()

                if (client._alias == None or client._username == None or client._alias == 'Text' or client._username == 'Text' or client._date == None):
                    sg.Popup('NOT REGISTERED', title='ERROR', button_type=5, auto_close=True, auto_close_duration=1)
                    continue

                window['_CLIENT_'].print('c> REGISTER ' + client._alias)
                client.register(client._alias, window)

            elif (event == 'UNREGISTER'):
                window['_CLIENT_'].print('c> UNREGISTER ' + client._alias)
                client.unregister(client._alias, window)


            elif (event == 'CONNECT'):
                window['_CLIENT_'].print('c> CONNECT ' + client._alias)
                client.connect(client._alias, window)


            elif (event == 'DISCONNECT'):
                window['_CLIENT_'].print('c> DISCONNECT ' + client._alias)
                client.disconnect(client._alias, window)


            elif (event == 'SEND'):
                window['_CLIENT_'].print('c> SEND ' + values['_INDEST_'] + " " + values['_IN_'])

                if (values['_INDEST_'] != '' and values['_IN_'] != '' and values['_INDEST_'] != 'User' and values['_IN_'] != 'Text') :
                    client. send(values['_INDEST_'], values['_IN_'], window)
                else :
                    window['_CLIENT_'].print("Syntax error. Insert <destUser> <message>")


            elif (event == 'SENDATTACH'):

                window['_CLIENT_'].print('c> SENDATTACH ' + values['_INDEST_'] + " " + values['_IN_'] + " " + values['_FILE_'])

                if (values['_INDEST_'] != '' and values['_IN_'] != '' and values['_FILE_'] != '') :
                    client.sendAttach(values['_INDEST_'], values['_IN_'], values['_FILE_'], window)
                else :
                    window['_CLIENT_'].print("Syntax error. Insert <destUser> <message> <attachedFile>")


            elif (event == 'CONNECTED USERS'):
                window['_CLIENT_'].print("c> CONNECTEDUSERS")
                client.connectedUsers(window)



            window.Refresh()

        window.Close()


if __name__ == '__main__':
    client.main([])
    print("+++ FINISHED +++")
