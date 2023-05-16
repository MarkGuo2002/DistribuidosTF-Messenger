from spyne import Application, rpc, ServiceBase, Unicode
from spyne.protocol.soap import Soap11
from spyne.server.wsgi import WsgiApplication

def reduce_spaces(string):
    # Function to reduce consecutive spaces to a single space
    return ' '.join(string.split())

class StringService(ServiceBase):
    @rpc(Unicode, _returns=Unicode)
    def reduce_spaces_server(self, string):
        return reduce_spaces(string)

# Create the Spyne application
app = Application([StringService], 'string_service',
                  in_protocol=Soap11(validator='lxml'),
                  out_protocol=Soap11())

# Create a WSGI application from the Spyne application
wsgi_app = WsgiApplication(app)

# Run the server
if __name__ == '__main__':
    from wsgiref.simple_server import make_server

    server = make_server('localhost', 8000, wsgi_app)
    server.serve_forever()
