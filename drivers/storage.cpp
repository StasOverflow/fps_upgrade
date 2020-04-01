#include <string.h>
#include <stm32f2xx.h>
#include <stm32f2xx_conf.h>
#include <system/system.h>
#include "crc.h"

#include "storage.h"

/****
 *  The start address of the first storage page
 */
#define STORAGE_PAGE_QUAN           2
#define STORAGE_START               0x8008000

/****
 *  The storage page size
 */
#define STORAGE_PAGE_SIZE           0x0004000

static const uint16_t flash_page_addr[STORAGE_PAGE_QUAN] = {
        FLASH_Sector_2,
        FLASH_Sector_3,
};

/****
 *  Block structure
 */
typedef struct __attribute__((__packed__))
{
    struct __attribute__((__packed__))
    {
        uint32_t    value;
        uint16_t    param;

    } data;

    uint16_t    crc;

} blk_t;

/****
 *  Header structure
 */
typedef struct __attribute__((__packed__))
{
    struct __attribute__((__packed__))
    {
        uint32_t    magic;
        int16_t     epoch;

    } data;

    uint16_t    crc;

} hdr_t;

/****
 *  Storage structure
 */
typedef struct
{
    hdr_t hdr;
    blk_t blk[];

} storage_t;

/****
 *  The array of storages
 */
static storage_t *storages[2] =
{
    (storage_t *)(STORAGE_START + STORAGE_PAGE_SIZE * 0),
    (storage_t *)(STORAGE_START + STORAGE_PAGE_SIZE * 1),
};


/****
 *  'Save all' callback
 */
static void (*save_callback)(void);

/****
 *  Active storage slot
 */
static uint8_t  active_slot;

/****
 *  Check the block for empty condition
 */
static
uint8_t is_blk_empty(blk_t *blk)
{
    for(uint32_t i = 0; i < sizeof(blk_t); i++)
    {
        if(*((uint8_t *)blk + i) != 0xFF)
        {
            return 0;
        }
    }

    return 1;
}

/****
 *  Read the header of storage
 */
static
int16_t header_read(uint8_t slot)
{
    uint8_t     *pdata;
    uint16_t    size;
    uint16_t    crc;

    // Check header crc
    pdata = (uint8_t *)&storages[slot]->hdr.data;
    size = sizeof(storages[slot]->hdr.data);
    crc = storages[slot]->hdr.crc;

    if(crc_8_dallas(pdata, size) == crc)
    {
        return storages[slot]->hdr.data.epoch;
    }

    return -1;
}

/****
 *  Write the header to storage
 */
static
void header_write(uint8_t slot, int16_t epoch)
{
    hdr_t hdr =
    {
        .data =
        {
            .magic = 'S' << 0 | 'T' << 8 | 'R' << 16 | 'G' << 24,
            .epoch = epoch
        },

        .crc = crc_8_dallas((uint8_t *)&hdr.data, sizeof(hdr.data))
    };

    // Write data
    FLASH_Unlock();
    FLASH_ProgramWord((uint32_t)&storages[slot]->hdr.data.magic, hdr.data.magic);
    FLASH_ProgramHalfWord((uint32_t)&storages[slot]->hdr.data.epoch, hdr.data.epoch);
    FLASH_ProgramHalfWord((uint32_t)&storages[slot]->hdr.crc, hdr.crc);
    FLASH_Lock();
}

/****
 *  Read param from storage
 */
uint32_t storage_read(uint16_t param)
{
    uint32_t    value = 0xFFFFFFFF;
    uint16_t    blk_cnt;
    uint8_t     *pdata;
    uint16_t    size;
    uint16_t    crc;

    // Compute the total number of blocks
    blk_cnt = (STORAGE_PAGE_SIZE - sizeof(hdr_t)) / sizeof(blk_t);

    // Loop through all blocks
    for(uint16_t i = 0; i < blk_cnt; i++)
    {
        // Check for target param
        if(storages[active_slot]->blk[i].data.param == param)
        {
            // Check block crc
            pdata = (uint8_t *)&storages[active_slot]->blk[i].data;
            size = sizeof(storages[active_slot]->blk[i].data);
            crc = storages[active_slot]->blk[i].crc;

            if(crc_8_dallas(pdata, size) == crc)
            {
                value = storages[active_slot]->blk[i].data.value;
            }
        }
    }

    return value;
}
#include "app_data.h"
/****
 *  Write param to storage
 */
void storage_write(uint16_t param, uint32_t value)
{
    int16_t     next_epoch;
    uint16_t    blk_cnt;

    blk_t blk =
    {
        .data =
        {
            .value = value,
            .param = param
        },

        .crc = crc_8_dallas((uint8_t *)&blk.data, sizeof(blk.data))
    };

    // Compute the total number of blocks
    blk_cnt = (STORAGE_PAGE_SIZE - sizeof(hdr_t)) / sizeof(blk_t);

    // Loop through all blocks
    for(uint16_t i = 0; i < blk_cnt; i++)
    {
        // Check block for empty
        if(is_blk_empty(&storages[active_slot]->blk[i]))
        {
            // Write data
            FLASH_Unlock();
            FLASH_ProgramWord((uint32_t)&storages[active_slot]->blk[i].data.value, blk.data.value);
//            app_data_item_set(APPDATA_DEBUG_FIELD_1, FLASH_WaitForLastOperation());
            FLASH_ProgramHalfWord((uint32_t)&storages[active_slot]->blk[i].data.param, blk.data.param);
            FLASH_ProgramHalfWord((uint32_t)&storages[active_slot]->blk[i].crc, blk.crc);
            FLASH_Lock();

            // Write complete
            return;
        }
    }

    // Calc the next epoch
    next_epoch = header_read(active_slot) + 1;

    // Change the active slot
    if( ++active_slot >= STORAGE_PAGE_QUAN )
    {
        active_slot = 0;
    }

    // Erase next storage
    FLASH_Unlock();
    FLASH_EraseSector(flash_page_addr[active_slot], VOLTAGE_RANGE);
    FLASH_Lock();

    // Write header
    header_write(active_slot, next_epoch);

    // Write data
    FLASH_Unlock();
    FLASH_ProgramWord((uint32_t)&storages[active_slot]->blk[0].data.value, blk.data.value);
    FLASH_ProgramHalfWord((uint32_t)&storages[active_slot]->blk[0].data.param, blk.data.param);
    FLASH_ProgramHalfWord((uint32_t)&storages[active_slot]->blk[0].crc, blk.crc);
    FLASH_Lock();

    // Call the save callback
    if(save_callback)
    {
        save_callback();
    }
}

/****
 *  Initialize the storage
 */
void storage_init(void (*save_callback_function)(void))
{
    int16_t cur;
    int16_t epoch[STORAGE_PAGE_QUAN];

    epoch[0] = header_read(0);
    epoch[1] = header_read(1);

    if((epoch[0] == -1) && (epoch[1] == -1))
    {
        active_slot = 0;

        // Erase the first storage
        FLASH_Unlock();
        FLASH_EraseSector(flash_page_addr[active_slot], VOLTAGE_RANGE);
        FLASH_Lock();

        // Write header
        header_write(active_slot, 0);
    }
    else
    {
        cur = epoch[0];
        active_slot = 0;
        for( int i = 0; i < STORAGE_PAGE_QUAN; i++ )
        {
            if( cur < epoch[i] )
            {
                cur = epoch[i];
                active_slot = i;
            }
        }
    }

    save_callback = save_callback_function;
}


void
storage_proc( void )
{
//    uint16_t        data;


    uint16_t blk_cnt = (STORAGE_PAGE_SIZE - sizeof(hdr_t)) / sizeof(blk_t);
    for(uint16_t i = 0; i < blk_cnt; i++)
    {
        if(is_blk_empty(&storages[0]->blk[i]))
        {
            break;
        }
    }

    for(uint16_t i = 0; i < blk_cnt; i++)
    {
//        data = blk_cnt - i;
        if(is_blk_empty(&storages[1]->blk[i]))
        {
            break;
        }
    }
}
