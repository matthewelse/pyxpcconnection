from pygattosx import XpcConnection
from threading import Event

class Connection(XpcConnection):
    def __init__(self, target, event):
        super(Connection, self).__init__(target)
        self.event = event

    def handler(self, event):
        print(event)
        self.event.set()

e = Event()

conn = Connection('com.apple.blued', e)

d = {
    'kCBMsgId': 1, 
    'kCBMsgArgs': {
        'kCBMsgArgAlert': 1,
        'kCBMsgArgName': 'node'
    }
}


conn.sendMessage(d)

e.wait()


