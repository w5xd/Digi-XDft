#pragma once
struct dec_data;
namespace XDft {
    namespace impl {
        class SharedMemoryImpl
        {
        public:
            SharedMemoryImpl(const std::string &key, bool useJt9);
            ~SharedMemoryImpl();

            std::string GetSharedMemoryKey()const;
            void Close();
            bool CreateWsjtSharedMem();
            void Spy();
            struct dec_data* GetWsjtCommonBlock() const { return m_mapViewOfFile; }
        protected:
            struct dec_data *m_mapViewOfFile;
            HANDLE m_SharedMem;
            // name to identify shared memory on the system.
            const std::string m_sharedMemoryKey;
            const bool m_useJt9;
        };
    }
}

