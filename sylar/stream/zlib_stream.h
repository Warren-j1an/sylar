#pragma once

#include <memory>
#include <vector>
#include <zlib.h>

#include "stream.h"

namespace sylar {
class ZlibStream : public Stream {
public:
    typedef std::shared_ptr<ZlibStream> ptr;

    enum Type {
        ZLIB,
        DEFLATE,
        GZIP
    };

    enum Strategy {
        DEFAULT     = Z_DEFAULT_STRATEGY,
        FILTERED    = Z_FILTERED,
        HUFFMAN     = Z_HUFFMAN_ONLY,
        FIXED       = Z_FIXED,
        RLE         = Z_RLE
    };

    enum CompressionLevel {
        NO_COMPRESSION      = Z_NO_COMPRESSION,
        BEST_SPEED          = Z_BEST_SPEED,
        BEST_COMPRESSION    = Z_BEST_COMPRESSION,
        DEFAULT_COMPRESSION = Z_DEFAULT_COMPRESSION
    };

    static ZlibStream::ptr CreateGzip(bool encoder, uint32_t buffer_size = 4096);
    static ZlibStream::ptr CreateZlib(bool encoder, uint32_t buffer_size = 4096);
    static ZlibStream::ptr CreateDeflate(bool encoder, uint32_t buffer_size = 4096);
    static ZlibStream::ptr Create(bool encoder, uint32_t buffer_size = 4096,
            Type type = DEFLATE, CompressionLevel level = DEFAULT_COMPRESSION,
            int window_bits = 15, int memlevel = 8, Strategy strategy = DEFAULT);

    ZlibStream(bool encoder, uint32_t buffer_size = 4096);
    ~ZlibStream();

    virtual int read(void* buffer, size_t length) override;
    virtual int read(ByteArray::ptr ba, size_t length) override;
    virtual int write(const void* buffer, size_t length) override;
    virtual int write(ByteArray::ptr ba, size_t length) override;
    virtual void close() override;

    int flush();
    bool isFree() const { return m_free; }
    void setFree(bool v) { m_free = v; }
    bool isEncoder() const { return m_encoder; }
    void setEncoder(bool v) { m_encoder = v; }
    std::vector<iovec>& getBuffers() { return m_buffers; }
    std::string getResult() const;
    ByteArray::ptr getByteArray();

private:
    int init(Type type = DEFLATE, CompressionLevel level = DEFAULT_COMPRESSION,
        int window_bits = 15, int memlevel = 8, Strategy strategy = DEFAULT);
    int encode(const iovec* v, const uint64_t& size, bool finish);
    int decode(const iovec* v, const uint64_t& size, bool finish);

private:
    z_stream m_zstream;
    uint32_t m_bufferSize;
    bool m_encoder;
    bool m_free;
    std::vector<iovec> m_buffers;
};
}