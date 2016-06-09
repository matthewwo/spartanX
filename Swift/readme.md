## This is a version of spartanX in swift

The Xcode project contains the source code and also examples to build servers using the framework (even though it's not build as framework yet). Screenshot of the example can be find it the examples folder.

spartanX-swift is a async-socket level framework written in swift and build mostly on top of C apis. It is designed for developers to build low-level server-side applications in a relatively easy way.

spartanX provide developers ability of managing each connection precisely by abstracting them using `SXQueue`. A queue represents a single connection and its delegate and dataDelegate are used to manage the lifetime of the connection. Besides of normal connect/disconnect event, spartanX also offer option to suspend/resume all runtime items(server, queue, client). When an object is suspended. Its data handlers will never be called even if the other side is keep sending payloads until it resumes, where losted data is non-recoverable.

See the Xcode project for how to create servers

# something not included in demo but useful

perform DNS look up and get ipaddress

```Swift
var addr: SXSockaddr = try! SXSockaddr.DNSLookup("www.apple.com", service: "http")!
var ip: String = addr.getIpaddress.getIpAddress()
```

with hints:

```Swift
var addr: SXSockaddr = try! SXSockaddr.DNSLookup("www.apple.com", service: "http", hints: [DNSLookupHint.Family(AF_INET), DNSLookupHint.SockType(SOCK_STREAM)]))!
var ip: String = addr.getIpaddress.getIpAddress()
```
