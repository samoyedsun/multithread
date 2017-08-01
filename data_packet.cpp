#include "data_packet.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>

DataPacket::DataPacket()
{
    this->data_block_ = this->pos_ptr_ = NULL;
    this->block_count_  = 0;
}

DataPacket::DataPacket(int size)
{
    void *buf_ptr = malloc(size);
    if (buf_ptr != NULL)
    {
        this->data_block_ = this->pos_ptr_ = buf_ptr;
        this->block_count_ = size;
    }
}

DataPacket::~DataPacket()
{
    if (this->data_block_ != NULL)
        free(this->data_block_);
}

void* DataPacket::get_data_ptr()
{
    return this->data_block_;
}

int DataPacket::get_data_size()
{
    return this->block_count_;
}
