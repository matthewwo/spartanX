//
//  proxy.swift
//  spartanX
//
//  Created by yuuji on 6/9/16.
//  Copyright © 2016 yuuji. All rights reserved.
//

import Foundation

#if swift(>=3)
func proxy_demo() {
    
    do {
        let server = try SXStreamServer(port: 8080,
                                        domain: SXSocketDomains.INET,
                                        maxGuest: 50,
                                        backlog: 50,
                                        bufsize: 819200,
                                        handler: { (queue, data) -> Bool in
                                            
            if var queue = queue as? SXStreamQueue {
                
                /* read our data by segment */
                
                /* NSDataSegment is a helper struct that allows you
                 * to read partial of the NSData by sepearator,
                 * whenever you call next(), it will proceed to
                 * look for the location of next sepearator and
                 * return the subdata in between the previous and
                 * next sepearator, see Helper.swift
                 */
                var segments = DataSegment(core: data, sepearatorBytes: [0x0d, 0x0a]) /* trim data by \r\n */
                print(String(data: data, encoding: .utf8))
                let i = segments.next()
                
                print("\(data.length) , \(i?.length)")
                /* cannot find \r\n */
                if i == nil || i!.length == data.length {
                    queue.socket.send(data: data, flags: 0)
                    return true
                }
                
          
                
                /* get the domain name */
                
                /* Notice that this demo is NOT a complete
                 * proxy server, in order to complete the
                 * server, we also need to check if the domain
                 * name changes. If so, close() the client,
                 * initialize another client to connect with
                 * another host
                 */
                var domain: String?
                segments.findall() { (data) -> Bool in
                    var ssegments = DataSegment(core: data, sepearatorBytes: [0x20]) /* trim the subdata by space */
                    ssegments.next() /* skip the verb */
                    
                    let uri = ssegments.next() /* get the uri */
                    guard let suri = String(data: uri!, encoding: .utf8)
                        else {
                            /* redirect the raw data */
                            queue.socket.send(data: data, flags: 0)
                            return false /* we're done with the payload */
                    }
                    
                    domain = suri
                    return false
                }
                
                print(domain)
                /* end get domain name */
                
                /* if our queue was not bind with any existing client
                 * normally a queue (which is an abstraction of a connection)
                 * does not need to bind a client, but since this is a proxy 
                 * server, it will make sence to bind a client and enstablish 
                 * a stable connection to the host that our user what to connect.
                 */
                if queue.binded.count == 0 {
                    if let domain = domain {

                        var client: SXRuntimeObject
                        
                        /* create a establish a connection to the dest server */
                        /* btw, you should handle the error in real world */
                        client = try! SXStreamClient(hostname: domain, service: "http", handler: { (object, data) -> Bool in
                            queue.socket.send(data: data, flags: 0)
                            return true
                        })
                        
                        /* bind the client to our queue */
                        queue.bindobj(obj: &client)
                        (client as! SXStreamClient).start(DispatchQueue.global(), initialPayload: data)
                        print(data.length)

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
#else
func proxy_demo() {
    
    do {
        let server = try SXStreamServer(port: 8080,
                                        domain: SXSocketDomains.INET,
                                        maxGuest: 50,
                                        backlog: 50,
                                        bufsize: 819200,
                                        handler: { (queue, data) -> Bool in
                                            
            if var queue = queue as? SXStreamQueue {
                
                /* read our data by segment */
                
                /* NSDataSegment is a helper struct that allows you
                 * to read partial of the NSData by sepearator,
                 * whenever you call next(), it will proceed to
                 * look for the location of next sepearator and
                 * return the subdata in between the previous and
                 * next sepearator, see Helper.swift
                 */
                var segments = NSDataSegment(core: data, sepearatorBytes: [0x0d, 0x0a]) /* trim data by \r\n */
                
                let i = segments.next()
                
                /* cannot find \r\n */
                if i == data.length || i == nil {
                    queue.socket.send(data: data, flags: 0)
                    return true
                }
                
                /* get the domain name */
                
                /* Notice that this demo is NOT a complete
                 * proxy server, in order to complete the
                 * server, we also need to check if the domain
                 * name changes. If so, close() the client,
                 * initialize another client to connect with
                 * another host
                 */
                var domain: String?
                segments.findall() { (data) -> Bool in
                    var ssegments = NSDataSegment(core: data, sepearatorBytes: [0x20]) /* trim the subdata by space */
                    ssegments.next() /* skip the verb */
                    let uri = ssegments.next() /* get the uri */
                    guard let suri = String(data: uri!, encoding: NSUTF8StringEncoding)
                        else {
                            /* redirect the raw data */
                            queue.socket.send(data: data as! NSMutableData, flags: 0)
                            return false /* we're done with the payload */
                    }
                    
                    domain = suri
                    return false
                }
                /* end get domain name */
                
                /* if our queue was not bind with any existing client
                 * normally a queue (which is an abstraction of a connection)
                 * does not need to bind a client, but since this is a proxy
                 * server, it will make sence to bind a client and enstablish
                 * a stable connection to the host that our user what to connect.
                 */
                if queue.binded.count == 0 {
                    if let domain = domain {
                        
                        var client: SXRuntimeObject
                        
                        /* create a establish a connection to the dest server */
                        /* btw, you should handle the error in real world */
                        client = try! SXStreamClient(hostname: domain, service: "http", handler: { (object, data) -> Bool in
                            queue.socket.send(data: data, flags: 0)
                            return true
                        })
                        
                        /* bind the client to our queue */
                        queue.bindobj(&client)
                        (client as! SXStreamClient).start(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), initialPayload: data)
                        
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
        
        /* different ways to start */
        //    server.start(DISPATCH_QUEUE_PRIORITY_DEFAULT, operateQueue: DISPATCH_QUEUE_PRIORITY_HIGH)
        //    server.start()
        
        /* most complex one, allows you to assign dispatch_priority in runtime
         * the listening queue block will only call once when the server starts
         * , but the operateQueue will call everytime when a connection accepted.
         */
        /* first argument := the listening dispatch_queue_t */
        server.start({ () -> dispatch_queue_t in
            return dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0)
            }, operatingQueue: { () -> dispatch_queue_t in
                return dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0)
        })
        
    } catch {
        print(error)
    }
    
}#endif
