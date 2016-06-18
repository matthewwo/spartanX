//
//  proxy.swift
//  spartanX
//
//  Created by yuuji on 6/9/16.
//  Copyright © 2016 yuuji. All rights reserved.
//

import Foundation

func proxy_demo() {
    
    do {
        let server = try SXStreamServer(port: 8080,
                                        domain: SXSocketDomains.INET,
                                        maxGuest: 50,
                                        backlog: 50,
                                        bufsize: 819200,
                                        handler: { (queue, data) -> Bool in
                                            
            if var queue = queue as? SXStreamQueue {

                
                /* \r\n  */
                var separator: [UInt8] = [0x0d, 0x0a]
                
                /* read our data by segment */
                
                /* DataReader is a helper struct that allows you
                 * to read partial of the NSData by sepearator,
                 * whenever you call next(), it will proceed to
                 * look for the location of next sepearator and
                 * return the subdata in between the previous and
                 * next sepearator, see Algorithm.swift
                 */
                var reader = DataReader(fromData: data)
                let requestLine = reader.nextSegmentOfData(separatedBy: &separator)
                
                if requestLine == nil || requestLine?.count == data.count {
                    queue.socket.send(data: data, flags: 0)
                    return true
                }
                
                /* Notice that this demo is NOT a complete
                 * proxy server, in order to complete the
                 * server, we also need to check if the domain
                 * name changes. If so, close() the client,
                 * initialize another client to connect with
                 * another host. To do that, subclass SXQueue
                 * and add a `var hostname: String` property to 
                 * track changes
                 */
                
                /* get the hostname line 
                 * e.g. `Host: www.example.com`
                 */
                guard let hostnameLine = reader.nextSegmentOfData(separatedBy: &separator) else {
                    queue.socket.send(data: data, flags: 0)
                    return false
                }
                
                /* use our reader to reader the line separate by space(0x20) */
                var hostnameReader = DataReader(fromData: hostnameLine)
                guard let hostnameBytes = hostnameReader.segmentOfData(separatedBy: [0x20], atIndex: 1) else {
                    queue.socket.send(data: data, flags: 0)
                    return false
                }
                
                let hostname = String(data: hostnameBytes, encoding: .ascii)
                print(hostname)
                /* if our queue was not bind with any existing client
                 * normally a queue (which is an abstraction of a connection)
                 * does not need to bind a client, but since this is a proxy 
                 * server, it will make sence to bind a client and enstablish 
                 * a stable connection to the host that our user what to connect.
                 */
                if queue.binded.count == 0 {
                    if let domain = hostname {

                        var client: SXRuntimeObject
                        
                        /* create a establish a connection to the dest server */
                        /* btw, you should handle the error in real world */
                        client = try! SXStreamClient(hostname: domain, service: "http", handler: { (object, data) -> Bool in
                            queue.socket.send(data: data, flags: 0)
                            return true
                        })
                        
                        /* bind the client to our queue */
                        queue.bind(obj: &client)
                        #if swift(>=3)
                        (client as! SXStreamClient).start(DispatchQueue.global(), initialPayload: data)
                        #else
                        (client as! SXStreamClient).start(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), initialPayload: data)
                        #endif
                    }
                } else {
                    /* we have our client ready */
                    if let c = queue.binded[0] as? SXStreamClient {
                        c.send(data: data, flags: 0)
                    }
                }
            }
            return true
        })
        
        /* most complex one, allows you to assign dispatch_priority in runtime
         * the listening queue block will only call once when the server starts
         * , but the operateQueue will call everytime when a connection accepted.
         */
        server.start()
        
    } catch {
        print(error)
    }
    
}

