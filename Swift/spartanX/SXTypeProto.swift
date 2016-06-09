
//  Copyright (c) 2016, Yuji
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
//  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  The views and conclusions contained in the software and documentation are those
//  of the authors and should not be interpreted as representing official policies,
//  either expressed or implied, of the FreeBSD Project.
//
//  Created by Yuji on 6/4/16.
//  Copyright © 2016 yuuji. All rights reserved.
//

import Foundation

public protocol SXBindedSocket {
    var address: SXSockaddr {get set}
}

public protocol SXSocketTypeProtocol {
    var bufsize: Int {get set}
    var port: in_port_t? {get set}
}

public protocol SXStreamProtocol : SXSocketTypeProtocol {
    var addr: SXSockaddr? {get set}
}


public protocol SXDGRAMProtocol : SXSocketTypeProtocol {
    var addr: SXSockaddr? {get set}
}


public extension SXBindedSocket where Self : SXLocalSocket {
    
    public func bind() throws {
        var i: Int32 = 0
        
        var yes = true
        if setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, UInt32(sizeof(Int32))) == -1 {
            throw SXSocketError.SetSockOpt(String.errno)
        }
        
        switch address {
        case var .INET(sock):
            i = Darwin.bind(sockfd, UnsafePointer<sockaddr>(getpointer(&sock)), socklen_t(sizeof(sockaddr_in)))
            
        case var .INET6(sock):
            i = Darwin.bind(sockfd, UnsafePointer<sockaddr>(getpointer(&sock)), socklen_t(sizeof(sockaddr_in6)))
        }
        if i == -1 {
            throw SXSocketError.Bind(String.errno)
        }
    }
    
    public func accept(bufsize: Int = 16384) throws -> SXRemoteSocket {
        var addr = sockaddr()
        var socklen = socklen_t()
        let fd = Darwin.accept(sockfd, &addr, &socklen)
        
        return try SXRemoteSocket(fd: fd,
                                  domain: self.domain,
                                  type: self.type,
                                  protocol: self.`protocol`,
                                  addr: addr,
                                  len: socklen,
                                  bufsize: bufsize)
    }
    
    public func listen(backlog: Int) throws {
        if Darwin.listen(sockfd, Int32(backlog)) == -1 {
            throw SXSocketError.Listen(String.errno)
        }
    }
}

public extension SXStreamProtocol where Self : SXSocket {
    
    func receive(size: Int, flags: Int32) throws -> NSMutableData {
        var buffer = [UInt8](count: size, repeatedValue: 0)
        let len = recv(sockfd, &buffer, size, flags)
        if len == -1 {throw SXSocketError.Recv(String.errno)}
        return NSMutableData(bytes: buffer, length: len)
    }
    
    func send(data: NSData, flags: Int32) {
        Darwin.send(sockfd, data.bytes, data.length, flags)
    }
}


public extension SXStreamProtocol where Self : SXLocalSocket {
    public func connect() throws {
        var i: Int32 = 0
        
        switch self.addr! {
        case var .INET(sock):
            
            i = Darwin.connect(sockfd, UnsafePointer<sockaddr>(getpointer(&sock)), socklen_t(sizeof(sockaddr_in)))
            
        case var .INET6(sock):
            i = Darwin.connect(sockfd, UnsafePointer<sockaddr>(getpointer(&sock)), socklen_t(sizeof(sockaddr_in6)))
        }
        
        if i == -1 {
            print(String.errno)
            throw SXSocketError.Connect(String.errno)
        }
    }
}

public extension SXDGRAMProtocol where Self : SXSocket {
    public func recvFrom(addr: SXSockaddr, flags: Int32 = 0) -> NSData {
        var addr_ = addr // since the expression var addr: SXSockaddr is not compatible with Swift 3
        var socklen = addr.socklen
        var buf = [UInt8](count: bufsize, repeatedValue: 0)
        let len = recvfrom(sockfd, &buf, bufsize, flags, UnsafeMutablePointer<sockaddr>(getMutablePointer(&addr_)), &socklen)
        return NSData(bytes: buf, length: len)
    }
    
    public func sendTo(addr: SXSockaddr, data: NSData, flags: Int32 = 0) {
        var addr_ = addr
        sendto(sockfd, data.bytes, data.length, flags, UnsafeMutablePointer<sockaddr>(getMutablePointer(&addr_)), addr_.socklen)
    }
    
    public func boardcast(port: in_port_t, data: NSData, flags: Int32 = 0) throws {
        let addr = try SXSockaddr.boardcastAddr(port)
        sendTo(addr, data: data, flags: flags)
    }
}