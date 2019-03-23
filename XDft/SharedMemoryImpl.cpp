#include "stdafx.h"
#include "SharedMemoryImpl.h"
#include <boost/uuid/sha1.hpp>
#include <commons.h>
namespace XDft { namespace impl {
    SharedMemoryImpl::SharedMemoryImpl(const std::string &key, bool useJt9)
        :m_mapViewOfFile(0)
        , m_SharedMem(INVALID_HANDLE_VALUE)
        , m_sharedMemoryKey(key)
        , m_useJt9(useJt9)
    {    }

    SharedMemoryImpl::~SharedMemoryImpl()
    {        Close();    }

    void SharedMemoryImpl::Close()
    {
        if (m_SharedMem != INVALID_HANDLE_VALUE)
            ::CloseHandle(m_SharedMem);
        m_SharedMem = INVALID_HANDLE_VALUE;
    }

    void SharedMemoryImpl::Spy()
    {
        if (m_mapViewOfFile)
        {
            auto &sharedMem = m_mapViewOfFile->params;
            int i = 0; // place to stop debugger and see what's in the common block
        }
    }

    std::string SharedMemoryImpl::GetSharedMemoryKey()const { return m_sharedMemoryKey; }

    // The jt9 exe communicates with us through process-to-process
     // shared memory. Win32 requires both processes to ask for 
     // the same shared memory object by name. That is the purpose of the
     // shared memory key.
    bool SharedMemoryImpl::CreateWsjtSharedMem()
    {
        if (m_SharedMem != INVALID_HANDLE_VALUE)
            ::CloseHandle(m_SharedMem);

        std::string memoryName = m_sharedMemoryKey;
        if (m_useJt9)
        {
            // duplicate QT's QSharedMemory name mangling for Windows....
            boost::uuids::detail::sha1 sha1;
            sha1.process_bytes(m_sharedMemoryKey.c_str(), m_sharedMemoryKey.size());
            static const unsigned DIGEST_LENGTH(5);
            unsigned dig[DIGEST_LENGTH];
            sha1.get_digest(dig);

            std::string QTname;
            for (unsigned i = 0; i < DIGEST_LENGTH; i++)
            {
                std::ostringstream oss;
                oss.width(8);
                oss.fill('0');
                oss << std::hex << dig[i];
                QTname += oss.str();
            }
            std::string filteredName;
            for (char c : m_sharedMemoryKey)
            {
                if (isalpha(c))
                    filteredName += c;
            }

            memoryName = "qipc_sharedmemory_" + filteredName + QTname;
        }
        else
            memoryName = m_sharedMemoryKey + "-memory";

        m_SharedMem = ::CreateFileMappingA(INVALID_HANDLE_VALUE,
            NULL,
            PAGE_EXECUTE_READWRITE,
            0,
            sizeof(dec_data),
            memoryName.c_str()
        );
        if (m_SharedMem != INVALID_HANDLE_VALUE)
        {
            m_mapViewOfFile = reinterpret_cast<struct dec_data *>(
                ::MapViewOfFile(m_SharedMem, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0));
            return m_mapViewOfFile != 0;
        }
        return false;
    }
}}