# encoding=utf-8
import json
import js2py
from ws4py.client.threadedclient import WebSocketClient
import openpyxl
import time

LoginKey = js2py.eval_js('''
function s4() {
return Math.floor((1 + Math.random()) * 0x10000).toString(16).substring(1);
};
function guid() {
return s4() + s4() + '-' + s4() + '-' + s4() + '-' + s4() + '-' + s4() + s4() + s4();
}
var a = guid()  // 在这里是需要调用的
''')
global msgs
msgs=[]
class CG_Client(WebSocketClient):

    def opened(self):
        req = '连接成功'
        print(req)

    def closed(self, code, reason=None):
        print("Closed down:", code, reason)

    def received_message(self, resp):
        global f
        global msgs
        print("消息:")
        if resp:
            try:
                msg=resp.data.decode('ascii')
                print(msg)
                if msg.strip()!="":
                    msgs.append(msg)
                    f = open("data.txt",'w')  
                    f.write("\n".join(msgs)) 
                    f.close()
            except Exception as e:
                print("error:",e)


if __name__ == '__main__':
    ws = None
    try:
        ws = CG_Client('wss://cloud.alientek.com/session/8e37c8d5393344f082565bb0b2aa9cec/org/1591/connection/3316?token='+LoginKey)
        ws.connect()
        ws.run_forever()
    except KeyboardInterrupt:
        f.close()
        ws.close()