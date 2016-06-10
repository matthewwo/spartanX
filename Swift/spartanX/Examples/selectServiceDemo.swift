
//  Created by yuuji on 6/9/16.
//  Copyright © 2016 yuuji. All rights reserved.
//

import Foundation


func selectService_demo() {
    
    /* This server recieve raw bytes from
     * the client who connect to it, if 
     * the payload is "Apple", the entire
     * connectiom will forward to the class Apple
     * and if the payload is "BurgerKing", the 
     * entire connection will be handle by the class
     * BurgerKing.
     */
    let dispatch_center = Dispatch()
    let server = try! SXStreamServer(port: 5000, domain: .INET, maxGuest: 50, backlog: 50, dataDelegate: dispatch_center)
    server.start()
}

class BurgerKing: SXRuntimeDataDelegate {
    func didReceiveError(object: SXRuntimeObject, err: ErrorType) {}
    
    func didReceiveData(object: SXRuntimeObject, data: NSMutableData) -> Bool {
        let queue = object as! SXStreamQueue
        print("Received");
        queue.socket.send("Your burger is ready.".dataUsingEncoding(NSASCIIStringEncoding)!, flags: 0)

        return true
    }
}

class Apple: SXRuntimeDataDelegate, SXRuntimeStreamObjectDelegate {
    
    func objectWillKill(object: SXRuntimeObject) {}
    func objectDidChangeStatus(object: SXRuntimeObject, status: SXStatus) {}
    
    /* this will never call in this case since the connect part is handled by Dispatch() class*/
    func objectDidConnect(object: SXRuntimeObject, withSocket: SXRemoteSocket) {}
    func objectDidDisconnect(object: SXRuntimeObject, withSocket: SXRemoteSocket) {
        print("That guy is gone")
    }
    
    func didReceiveError(object: SXRuntimeObject, err: ErrorType) {}
    
    func didReceiveData(object: SXRuntimeObject, data: NSMutableData) -> Bool {
        let queue = object as! SXStreamQueue
        print("Received");
        queue.socket.send("Sorry, nothing for you.".dataUsingEncoding(NSASCIIStringEncoding)!, flags: 0)
        
        return true
    }
}

class Dispatch: SXRuntimeDataDelegate {
    
    var apple = Apple()
    var bk = BurgerKing()
    
    func didReceiveError(object: SXRuntimeObject, err: ErrorType) {}
    
    func didReceiveData(object: SXRuntimeObject, data: NSMutableData) -> Bool {
        let queue = object as! SXStreamQueue
        print("Received");
        if let payload_string = String(data: data, encoding: NSASCIIStringEncoding) {
            
           
            if payload_string.hasSuffix("BurgerKing") {
                
                /* this guy want burger */
                queue.setDataDelegate(bk)
                
            } else if payload_string.hasSuffix("Apple") {
                /* now everything from this particular guy will
                 * handle by `apple` instead of the dispatch class
                 * this feature is useful for implementing a SOCKS
                 * server where you need to support different SOCKS
                 * version
                 */
                queue.setDataDelegate(apple)
                queue.delegate = apple /* for `objectDidDisconnect` */
            } else {
                /* Otherwise, Ask the client to make a chose */
                queue.socket.send("Please type 'Apple' or 'BurgerKing'".dataUsingEncoding(NSUTF8StringEncoding)!, flags: 0)
            }
            
            
        }
        
        return true
    }
}