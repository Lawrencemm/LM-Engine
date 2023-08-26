#pragma once
#include <cstddef>
#include <string>

class Resource {
public:
    Resource(const unsigned char* start, const size_t len) : resource_data(start), data_len(len) {}

    const char * const data() const { return (char*)resource_data; }
    const size_t size() const { return data_len; }

    const char *begin() const { return (char*)resource_data; }
    const char *end() const { return (char*)resource_data + data_len; }

    std::string toString() { return std::string((char*)data(), size()); }

private:
    const unsigned char* resource_data;
    const size_t data_len;

};
#define LOAD_RESOURCE(varname, RESOURCE)   \
    extern const unsigned char _resource_##RESOURCE[]; extern const size_t _resource_##RESOURCE##_len;   \
    static auto varname = Resource(_resource_##RESOURCE, _resource_##RESOURCE##_len);
