//
//  Algorithm.swift
//  spartanX
//
//  Created by yuuji on 6/18/16.
//  Copyright © 2016 yuuji. All rights reserved.
//

import Foundation

#if swift(>=3)
public struct DataReader {
    public var origin: Data
    public var currentOffset: Int = 0

    init(fromData data: Data) {
        self.origin = data
    }
}

extension DataReader {
    public mutating func rangeOfNextSegmentOfData(spearatedBy data: Data) -> Range<Data.Index>? {
        var bytes = data.bytes
        return rangeOfNextSegmentOfData(separatedBy: &bytes)
    }
    
    public mutating func rangeOfNextSegmentOfData(separatedBy bytes: [UInt8]) -> Range<Data.Index>? {
        var bytes = bytes
        return rangeOfNextSegmentOfData(separatedBy: &bytes)
    }
    public mutating func rangeOfNextSegmentOfData(separatedBy bytes: inout [UInt8]) -> Range<Data.Index>? {
        guard let endpoint = origin.findBytes(bytes: &bytes,
                                              offset: currentOffset,
                                              len: bytes.count) else {
                                                return nil
        }
        let begin = origin.index(origin.startIndex, offsetBy: currentOffset)
        let end = origin.index(begin, offsetBy: endpoint - currentOffset)
        currentOffset = endpoint + bytes.count
        return Range(begin ..< end)
    }
}
extension DataReader {

    public mutating func segmentOfData(separatedBy bytes: [UInt8], atIndex count: Int) -> Data? {
        var bytes = bytes
        return segmentOfData(separatedBy: &bytes, atIndex: count)
    }
    
    public mutating func segmentOfData(separatedBy data: Data, atIndex count: Int) -> Data? {
        var bytes = data.bytes
        return segmentOfData(separatedBy: &bytes, atIndex: count)
    }
    
    public mutating func segmentOfData(separatedBy bytes: inout [UInt8], atIndex count: Int) -> Data? {
        var holder: Range<Data.Index>?
        var i = 0
        
        repeat {
            holder = rangeOfNextSegmentOfData(separatedBy: &bytes)
            i += 1
        } while i <= count && holder != nil

        if holder == nil {
            return nil
        }
      
        return origin.subdata(in: holder!)
    }
}

extension DataReader {

    public mutating func nextSegmentOfData(separatedBy data: Data) -> Data? {
        var bytes = data.bytes
        return nextSegmentOfData(separatedBy: &bytes)
    }
    
    public mutating func nextSegmentOfData(separatedBy bytes: [UInt8]) -> Data? {
        var bytes = bytes
        return nextSegmentOfData(separatedBy: &bytes)
    }
    
    public mutating func nextSegmentOfData(separatedBy bytes: inout [UInt8]) -> Data? {
        if let range = rangeOfNextSegmentOfData(separatedBy: &bytes) {
            return origin.subdata(in: range)
        }
        return nil
    }
}

extension DataReader {
    
    public mutating func forallSegments(separatedBy data: Data, handler: (Data) -> Bool) {
        var bytes = data.bytes
        return forallSegments(separatedBy: &bytes, handler: handler)
    }
    
    public mutating func forallSegments(separatedBy bytes: [UInt8], handler: (Data) -> Bool) {
        var bytes = bytes
        return forallSegments(separatedBy: &bytes, handler: handler)
    }
    
    public mutating func forallSegments(separatedBy bytes: inout [UInt8], handler: (Data) -> Bool) {
        var data = nextSegmentOfData(separatedBy: &bytes)
        while data != nil {
            if !handler(data!) {
                break
            }
            data = nextSegmentOfData(separatedBy: &bytes)
        }
    }

}
#endif
