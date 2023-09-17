#include "bytearray.h"
#include "util_endian.h"
#include "log.h"

#include <cmath>
#include <iomanip>

namespace sylar {
static Logger::ptr g_logger = SYLAR_LOG_NAME("root");

ByteArray::Node::Node(size_t s) {
    ptr = new char[s];
    next = nullptr;
    size = s;
}

ByteArray::Node::Node() {
    ptr = nullptr;
    next = nullptr;
    size = 0;
}

ByteArray::Node::~Node() {
    if (ptr) {
        delete[] ptr;
    }
}

ByteArray::ByteArray(size_t base_szie) : m_baseSize(base_szie), m_position(0)
    , m_capacity(base_szie), m_size(0), m_endian(SYLAR_BIG_ENDIAN)
    , m_root(new Node(base_szie)), m_cur(m_root) {}

ByteArray::~ByteArray() {
    Node* tmp = m_root;
    while(tmp) {
        m_cur = tmp;
        tmp = tmp->next;
        delete m_cur;
    }
}

void ByteArray::writeFint8(int8_t value) {
    write(&value, sizeof(value));
}

void ByteArray::writeFuint8(uint8_t value) {
    write(&value, sizeof(value));
}

void ByteArray::writeFint16(int16_t value) {
    if (m_endian != SYLAR_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFuint16(uint16_t value) {
    if (m_endian != SYLAR_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFint32(int32_t value) {
    if (m_endian != SYLAR_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFuint32(uint32_t value) {
    if (m_endian != SYLAR_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFint64(int64_t value) {
    if (m_endian != SYLAR_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFuint64(uint64_t value) {
    if (m_endian != SYLAR_BYTE_ORDER) {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeInt32(int32_t value) {
    writeUint32(EncodeZigzag32(value));
}

void ByteArray::writeUint32(uint32_t value) {
    uint8_t temp[5];
    uint8_t i = 0;
    while (value >= 0x80) {
        temp[i++] = (value & 0x7F) | 0x80;
        value  = value >> 7;
    }
    temp[i++] = value;
    write(temp, i);
}

void ByteArray::writeInt64(int64_t value) {
    writeUint64(EncodeZigzag64(value));
}

void ByteArray::writeUint64(uint64_t value) {
    uint8_t temp[19];
    uint8_t i = 0;
    while (value >= 0x80) {
        temp[i++] = (value & 0x7F) | 0x80;
        value = value >> 7;
    }
    temp[i++] = value;
    write(temp, i);
}

void ByteArray::writeFloat(float value) {
    uint32_t v;
    memcpy(&v, &value, sizeof(value));
    writeFuint32(v);
}

void ByteArray::writeDouble(double value) {
    uint64_t v;
    memcpy(&v, &value, sizeof(value));
    writeFuint64(v);
}

void ByteArray::writeStringF16(const std::string& value) {
    writeFuint16(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringF32(const std::string& value) {
    writeFuint32(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringF64(const std::string& value) {
    writeFuint64(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringVint(const std::string& value) {
    writeUint64(value.size());
    write(value.c_str(), value.size());
}

void ByteArray::writeStringWithoutLength(const std::string& value) {
    write(value.c_str(), value.size());
}

int8_t ByteArray::readFint8() {
    int8_t v;
    read(&v, sizeof(v));
    return v;
}

uint8_t ByteArray::readFuint8() {
    uint8_t v;
    read(&v, sizeof(v));
    return v;
}

int16_t ByteArray::readFint16() {
    int16_t v;
    read(&v, sizeof(v));
    if (m_endian == SYLAR_BYTE_ORDER) {
        return v;
    } else {
        return byteswap(v);
    }
}

uint16_t ByteArray::readFuint16() {
    uint16_t v;
    read(&v, sizeof(v));
    if (m_endian == SYLAR_BYTE_ORDER) {
        return v;
    } else {
        return byteswap(v);
    }
}

int32_t ByteArray::readFint32() {
    int32_t v;
    read(&v, sizeof(v));
    if (m_endian == SYLAR_BYTE_ORDER) {
        return v;
    } else {
        return byteswap(v);
    }
}

uint32_t ByteArray::readFuint32() {
    uint32_t v;
    read(&v, sizeof(v));
    if (m_endian == SYLAR_BYTE_ORDER) {
        return v;
    } else {
        return byteswap(v);
    }
}

int64_t ByteArray::readFint64() {
    int64_t v;
    read(&v, sizeof(v));
    if (m_endian == SYLAR_BYTE_ORDER) {
        return v;
    } else {
        return byteswap(v);
    }
}

uint64_t ByteArray::readFuint64() {
    uint64_t v;
    read(&v, sizeof(v));
    if (m_endian == SYLAR_BYTE_ORDER) {
        return v;
    } else {
        return byteswap(v);
    }
}

int32_t ByteArray::readInt32() {
    return DecodeZigzag32(readUint32());
}

uint32_t ByteArray::readUint32() {
    uint32_t result = 0;
    for (int i = 0; i < 32; i += 7) {
        uint8_t b = readFuint8();
        if (b < 0x80) {
            result |= ((uint32_t)b) << i;
            break;
        } else {
            result |= (uint32_t)(b & 0x7F) << i;
        }
    }
    return result;
}

int64_t ByteArray::readInt64() {
    return DecodeZigzag64(readUint64());
}

uint64_t ByteArray::readUint64() {
    uint64_t result = 0;
    for (int i = 0; i  < 64; i += 7) {
        uint8_t b = readFuint8();
        if (b < 0x80) {
            result |= ((uint64_t)b) << i;
            break;
        } else {
            result |= ((uint64_t)b & 0x7F) << i;
        }
    }
    return result;
}

float ByteArray::readFloat() {
    uint32_t v = readFuint32();
    float value;
    memcpy(&value, &v, sizeof(v));
    return value;
}

double ByteArray::readDouble() {
    uint64_t v = readFuint64();
    double value;
    memcpy(&value, &v, sizeof(v));
    return value;
}

std::string ByteArray::readStringF16() {
    uint16_t len = readFuint16();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

std::string ByteArray::readStringF32() {
    uint32_t len = readFuint32();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

std::string ByteArray::readStringF64() {
    uint64_t len = readFuint64();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

std::string ByteArray::readStringVint() {
    uint64_t len = readFuint64();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

void ByteArray::clear() {
    m_position = m_size = 0;
    m_capacity = m_baseSize;
    Node* tmp = m_root->next;
    while(tmp) {
        m_cur = tmp;
        tmp = tmp->next;
        delete m_cur;
    }
    m_cur = m_root;
    m_root->next = NULL;
}

void ByteArray::write(const void* buf, size_t size) {
    if (size == 0) {
        return;
    }
    addCapacity(size);
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;
    while (size) {
        if (ncap >= size) {
            memcpy(m_cur->ptr + npos, (const char*)buf + bpos, size);
            if (m_cur->size == (npos + size)) {
                m_cur = m_cur->next;
            }
            m_position += size;
            bpos += size;
            size = 0;
        } else {
            memcpy(m_cur->ptr + npos, (const char*)buf + bpos, ncap);
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            m_cur = m_cur->next;
            ncap = m_cur->size;
            npos = 0;
        }
    }
    if (m_position > m_size) {
        m_size = m_position;
    }
}

void ByteArray::read(void* buf, size_t size) {
    if (size > getReadSize()) {
        throw std::out_of_range("read not enough range");
    }
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;
    while (size) {
        if (ncap >= size) {
            memcpy((char*)buf + bpos, m_cur->ptr + npos, size);
            if (m_cur->size == (npos + size)) {
                m_cur = m_cur->next;
            }
            m_position += size;
            bpos += size;
            size = 0;
        } else {
            memcpy((char*)buf + bpos, m_cur->ptr + npos, ncap);
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            m_cur = m_cur->next;
            ncap = m_cur->size;
            npos = 0;
        }
    }
}

void ByteArray::read(void* buf, size_t size, size_t position) const {
    if (size > (m_size - position)) {
        throw std::out_of_range("read not enough range");
    }
    Node* cur = m_root;
    size_t count = position / m_baseSize;
    while (count--) {
        cur = cur->next;
    }
    size_t npos = position % m_baseSize;
    size_t ncap = cur->size - npos;
    size_t bpos = 0;
    while (size) {
        if (ncap >= size) {
            memcpy((char*)buf + bpos, cur->ptr + npos, size);
            if (cur->size == (npos + size)) {
                cur = cur->next;
            }
            position += size;
            bpos += size;
            size = 0;
        } else {
            memcpy((char*)buf + bpos, cur->ptr + npos, ncap);
            position += ncap;
            bpos += ncap;
            size -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
    }
}

void ByteArray::setPosition(size_t v) {
    if (v > m_capacity) {
        throw std::out_of_range("set position out of range");
    }
    m_position = v;
    if (m_position > m_size) {
        m_size = m_position;
    }
    m_cur = m_root;
    while (v > m_cur->size) {
        v -= m_cur->size;
        m_cur = m_cur->next;
    }
    if (v == m_cur->size) {
        m_cur = m_cur->next;
    }
}

bool ByteArray::writeToFile(const std::string& name) const {
    std::ofstream ofs;
    ofs.open(name, std::ios::trunc | std::ios::binary);
    if (!ofs) {
        SYLAR_LOG_ERROR(g_logger) << "writeToFile name=" << name << " error , errno=" << errno
            << " errstr=" << strerror(errno);
        return false;
    }
    size_t read_size = getReadSize();
    size_t pos = m_position;
    Node* cur = m_cur;
    while (read_size > 0) {
        size_t diff = pos % m_baseSize;
        size_t len = (read_size > m_baseSize ? m_baseSize : read_size) - diff;
        ofs.write(cur->ptr + diff, len);
        cur = cur->next;
        pos += len;
        read_size -= len;
    }
    return true;
}

bool ByteArray::readFromFile(const std::string& name) {
    std::ifstream ifs;
    ifs.open(name, std::ios::binary);
    if (!ifs) {
        SYLAR_LOG_ERROR(g_logger) << "readFromFile name=" << name << " error , errno=" << errno
            << " errstr=" << strerror(errno);
        return false;
    }
    std::shared_ptr<char> buff(new char[m_baseSize], [](char* ptr){ delete[] ptr; });
    while (!ifs.eof()) {
        ifs.read(buff.get(), m_baseSize);
        write(buff.get(), ifs.gcount());
    }
    return true;
}

bool ByteArray::isLittleEndian() const {
    return m_endian == SYLAR_LITTLE_ENDIAN;
}

void ByteArray::setIsLittleEndian(bool val) {
    if (val) {
        m_endian = SYLAR_LITTLE_ENDIAN;
    } else {
        m_endian = SYLAR_BIG_ENDIAN;
    }
}

std::string ByteArray::toString() const {
    std::string str;
    str.resize(getReadSize());
    if (str.empty()) {
        return str;
    }
    read(&str[0], str.size(), m_position);
    return str;
}

std::string ByteArray::toHexString() const {
    std::string str = toString();
    std::stringstream ss;
    for(size_t i = 0; i < str.size(); ++i) {
        if (i > 0 && i % 31 == 0) {
            ss << std::endl;
        }
        ss << std::setw(2) << std::setfill('0') << std::hex
           << (int)(uint8_t)str[i] << " ";
    }
    return ss.str();
}

uint64_t ByteArray::getReadBuffers(std::vector<iovec>& buffers, uint64_t len) const {
    len = len > getReadSize() ? getReadSize() : len;
    size_t size = len;
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    Node* cur = m_cur;
    struct iovec iov;
    while (len) {
        if (ncap >= len) {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = len;
            len = 0;
        } else {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = ncap;
            len -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
        buffers.push_back(iov);
    }
    return size;
}

uint64_t ByteArray::getReadBuffers(std::vector<iovec>& buffers,
    uint64_t len, uint64_t position) const {
    len = len > getReadSize() ? getReadSize() : len;
    size_t size = len;
    size_t count = position / m_baseSize;
    size_t npos = position % m_baseSize;
    Node* cur = m_root;
    while (count--) {
        cur = cur->next;
    }
    size_t ncap = cur->size - npos;
    struct iovec iov;
    while (len) {
        if (ncap >= len) {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = len;
            len = 0;
        } else {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = ncap;
            len -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
        buffers.push_back(iov);
    }
    return size;
}

uint64_t ByteArray::getWriteBuffers(std::vector<iovec>& buffers, uint64_t len) {
    addCapacity(len);
    size_t size = len;
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    struct iovec iov;
    Node* cur = m_cur;
    while (len) {
        if (ncap >= len) {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = len;
            len = 0;
        } else {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = ncap;
            len -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
        buffers.push_back(iov);
    }
    return size;
}

void ByteArray::addCapacity(size_t size) {
    size_t old_size = getCapacity();
    if (old_size >= size) {
        return;
    }
    size = size - old_size;
    size_t count = std::ceil(1.0 * size / m_baseSize);
    Node* node = m_root;
    while (node->next) {
        node = node->next;
    }
    Node* cur = nullptr;
    for (size_t i = 0; i < count; ++i) {
        node->next = new Node(m_baseSize);
        if (!cur) {
            cur = node->next;
        }
        node = node->next;
        m_capacity += m_baseSize;
    }
    if (old_size == 0) {
        m_cur = cur;
    }
}

uint32_t ByteArray::EncodeZigzag32(const int32_t& v) {
    return (v << 1) ^ (v >> 31);
}

uint64_t ByteArray::EncodeZigzag64(const int64_t& v) {
    return (v << 1) ^ (v >> 63);
}

int32_t ByteArray::DecodeZigzag32(const uint32_t& v) {
    return (v >> 1) ^ -(v & 1);
}

int64_t ByteArray::DecodeZigzag64(const uint64_t& v) {
    return (v >> 1) ^ -(v & 1);
}
}