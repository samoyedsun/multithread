#ifndef DATA_PACKET
#define DATA_PACKET
#include <stdlib.h>
#include <string.h>
#include <iostream>

class DataPacket
{
    public:
        DataPacket();
        DataPacket(int size);
        ~DataPacket();
	    template <typename T>
        DataPacket& operator << (const T &val)
        {
            void *buf_ptr = NULL;
            int new_size = sizeof(val);
            if (0 == this->block_count_)
            {
                if (NULL == (buf_ptr = malloc(new_size)))
                {
                    std::cout << "memory allocation fail!"  << std::endl;
                    return *this;
                }
                memcpy(buf_ptr, (void *)&val, new_size);
            }
            else
            {
                int buf_size = new_size;
                new_size += this->block_count_;
                if (NULL == (buf_ptr = realloc(this->data_block_, new_size)))
                {
                    std::cout << "memory allocation fail!"  << std::endl;
                    return *this;
                }
                memcpy((char *)buf_ptr + this->block_count_, (void *)&val, buf_size);
            }
            this->data_block_ = this->pos_ptr_ = buf_ptr;
            this->block_count_ = new_size;
            std::cout << "new size:" << new_size << std::endl;
            return *this;
        }

        DataPacket& operator << (const std::string &val)
        {
            if (true == val.empty() || 0 == this->block_count_)
                return *this;
            void *buf_ptr = NULL;
            int16_t new_size = val.size();
            int16_t buf_size = new_size;
            new_size += sizeof(new_size);
            new_size += this->block_count_;
            if (NULL == (buf_ptr = realloc(this->data_block_, new_size)))
            {
                std::cout << "memory allocation fail!"  << std::endl;
                return *this;
            }
            int buf_count = this->block_count_;
            memcpy((char *)buf_ptr + buf_count, (void *)&buf_size, sizeof(buf_size));
            buf_count += sizeof(buf_size);
            memcpy((char *)buf_ptr + buf_count, (void *)val.c_str(), buf_size);
            this->data_block_ = this->pos_ptr_ = buf_ptr;
            this->block_count_ = new_size;
            return *this;
        }

        template <typename T>
        DataPacket& operator >> (T &val)
        {
            if (0 >= this->block_count_)
                return *this;
            std::cout << "block_count_:" << this->block_count_ << std::endl;
            int ele_size = sizeof(val);
            std::cout << "ele_size:" << ele_size << std::endl;
            char *buf_ptr =  (char *)this->pos_ptr_;
            memcpy((void *)&val, buf_ptr, ele_size);
            this->pos_ptr_ = buf_ptr + ele_size;
            this->block_count_ -= ele_size;
            std::cout << "ele_val:" << val << std::endl;
            return *this;
        }

        DataPacket& operator >> (std::string &val)
        {
            if (0 >= this->block_count_)
                return *this;
            int16_t buf_size = 0;
            char *buf_ptr =  (char *)this->pos_ptr_;
            memcpy((void *)&buf_size, buf_ptr, sizeof(buf_size));
            buf_ptr += sizeof(buf_size);
            char buf_str[buf_size + 1];
            memset(buf_str, 0, buf_size+1);
            memcpy((void *)buf_str, buf_ptr, buf_size);
            val = buf_str;
            this->pos_ptr_ = buf_ptr + buf_size;
            this->block_count_ -= buf_size + sizeof(buf_ptr);
            return *this;
        }

        void* get_data_ptr();
        int get_data_size();

    private:
        void *data_block_;
        int  block_count_;

        void *pos_ptr_;
};

#endif
