#include "util.h"
#include "zlib_stream.h"

#include <cstdlib>
#include <stdexcept>

namespace sylar {
ZlibStream::ptr ZlibStream::CreateGzip(bool encoder, uint32_t buffer_size) {

}

ZlibStream::ptr ZlibStream::CreateZlib(bool encoder, uint32_t buffer_size ) {

}

ZlibStream::ptr ZlibStream::CreateDeflate(bool encoder, uint32_t buffer_size) {

}

ZlibStream::ptr ZlibStream::Create(bool encoder, uint32_t buffer_size, Type type,
        CompressionLevel level, int window_bits, int memlevel, Strategy strategy) {

}

ZlibStream::ZlibStream(bool encoder, uint32_t buffer_size)
    : m_bufferSize(buffer_size), m_encoder(encoder), m_free(true) {}

ZlibStream::~ZlibStream() {
    if (m_free) {
        for (iovec& buffer : m_buffers) {
            free(buffer.iov_base);
            buffer.iov_base = nullptr;
        }
    }
    if (m_encoder) {
        deflateEnd(&m_zstream);
    } else {
        inflateEnd(&m_zstream);
    }
}

int ZlibStream::read(void* buffer, size_t length) {
    throw std::logic_error("ZlibStream::read is invalid");
}

int ZlibStream::read(ByteArray::ptr ba, size_t length) {
    throw std::logic_error("ZlibStream::read is invalid");
}

int ZlibStream::write(const void* buffer, size_t length) {
    iovec iov;
    iov.iov_base = (void*)buffer;
    iov.iov_len = length;
    if (m_encoder) {
        return encode(&iov, 1, false);
    } else {
        return decode(&iov, 1, false);
    }
}

int ZlibStream::write(ByteArray::ptr ba, size_t length) {
    std::vector<iovec> iovs;
    ba->getReadBuffers(iovs, length);
    if (m_encoder) {
        return encode(&iovs[0], iovs.size(), false);
    } else {
        return decode(&iovs[0], iovs.size(), false);
    }
}

void ZlibStream::close() {
    flush();
}

int ZlibStream::init(Type type, CompressionLevel level, int window_bits,
        int memlevel, Strategy strategy) {
    SYLAR_ASSERT((window_bits >= 8 && window_bits <= 15));
    SYLAR_ASSERT((memlevel >= 1 && memlevel <= 9));

    memset(&m_zstream, 0, sizeof(m_zstream));
    m_zstream.zalloc = Z_NULL;
    m_zstream.zfree = Z_NULL;
    m_zstream.opaque = Z_NULL;

    switch(type) {
        case DEFLATE:
            window_bits = -window_bits;
            break;
        case GZIP:
            window_bits += 16;
            break;
        case ZLIB:
        default:
            break;
    }

    if (m_encoder) {
        return deflateInit2(&m_zstream, level, Z_DEFLATED, window_bits, memlevel, strategy);
    } else {
        return inflateInit2(&m_zstream, window_bits);
    }
}

int ZlibStream::encode(const iovec* v, const uint64_t& size, bool finish) {
    int ret = 0;
    int flush = 0;
    for (uint64_t i = 0; i < size; ++i) {
        m_zstream.avail_in = v[i].iov_len;
        m_zstream.next_in = (Bytef*)v[i].iov_base;
        flush = finish ? (i == size - 1 ? Z_FINISH : Z_NO_FLUSH) : Z_NO_FLUSH;
        iovec* iov = nullptr;
        do {
            if (!m_buffers.empty() && m_buffers.back().iov_len != m_bufferSize) {
                iov = &m_buffers.back();
            } else {
                iovec ivc;
                ivc.iov_base = malloc(m_bufferSize);
                ivc.iov_len = 0;
                m_buffers.push_back(ivc);
                iov = &m_buffers.back();
            }

            m_zstream.avail_out = m_bufferSize - iov->iov_len;
            m_zstream.next_out = (Bytef*)iov->iov_base + iov->iov_len;
            ret = deflate(&m_zstream, flush);
            if(ret == Z_STREAM_ERROR) {
                return ret;
            }
            iov->iov_len = m_bufferSize - m_zstream.avail_out;
        } while (m_zstream.avail_out == 0);
    }
    if (flush == Z_FINISH) {
        deflateEnd(&m_zstream);
    }
    return Z_OK;
}

int ZlibStream::decode(const iovec* v, const uint64_t& size, bool finish) {

}

int ZlibStream::flush() {

}
}