#pragma once
struct dec_data;
namespace XDft {
    namespace impl {
        class SharedMemoryImpl;
        class WsjtSharedMemory
        {
        public:
            WsjtSharedMemory(const std::string &, bool useJt9);
            WsjtSharedMemory();
            ~WsjtSharedMemory();
            std::string GetSharedMemoryKey()const;
            struct dec_data *GetWsjtCommonBlock();
            void Close();
            bool CreateWsjtSharedMem();
            void Spy();
        private:
            std::shared_ptr<SharedMemoryImpl> m_impl;
        };
    }
}

