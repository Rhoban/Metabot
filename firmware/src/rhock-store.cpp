#include <terminal.h>
#include <rhock/vm.h>
#include <rhock/obj.h>
#include <rhock/store.h>
#include <flash_write.h>

#define PAGE_SIZE   1024
#define PAGES       20
#define FLASH_SIZE  (PAGES*PAGE_SIZE)

// Allocate the appropriate zone in the flash
const char rhock_progs[FLASH_SIZE] 
__attribute__((section("rhock_progs"), aligned(PAGE_SIZE))) = {0};

/**
 * Returns the number of pages that are needed to contain size
 */
static uint32_t page_round(uint32_t size)
{
    uint32_t pages;
    if ((size % PAGE_SIZE) == 0) {
        pages = size / PAGE_SIZE;
    } else {
        pages = (size/PAGE_SIZE)+1;
    }
    return pages;
}

/**
 * Tell if a page is free
 */
static bool page_is_free(uint32_t n)
{
    return (
           rhock_progs[n*PAGE_SIZE+0]==0
        && rhock_progs[n*PAGE_SIZE+1]==0
        && rhock_progs[n*PAGE_SIZE+2]==0
        && rhock_progs[n*PAGE_SIZE+3]==0
    );
}

/**
 * Tells how many free pages are following
 */
static int page_free_size(uint32_t n)
{
    int c = 0;
    while (n<PAGES && page_is_free(n)) {
        c++;
        n++;
    }

    return c;
}

/**
 * If a page contains an obj, tell how many pages are reserved
 */
static uint32_t page_obj_size(uint32_t n)
{
    int size = rhock_obj_guess_size((uint8_t*)&rhock_progs[n*PAGE_SIZE]);
    return page_round(size);
}

/**
 * Free a page (writes 0 over it)
 */
static void page_free(uint32_t n)
{
    if (n < PAGES) {
        char buffer[PAGE_SIZE] = {0};
        flash_write((int)&rhock_progs[n*PAGE_SIZE], buffer, PAGE_SIZE);
    }
}

/**
 * Current upload page, buffer, position & size
 */
uint32_t current_page;          // Current page being written
uint32_t current_page_position; // Current position in the page
char current_buffer[PAGE_SIZE]; // Current buffer to store data before persiting it to the ram
uint32_t current_position;      // Current position in that buffer
uint32_t current_size = 0;      // Overall size of the target current obj

static void dismiss()
{
    if (current_size > 0) {
        int count = page_round(current_size);
        for (int k=0; k<count; k++) {
            if (!page_is_free(k)) {
                page_free(k);
            }
        }
    }
    current_size = 0;
}

char rhock_store_alloc(uint32_t size)
{
    if (!size) {
        return 0;
    }
    dismiss();

    // Compute the number of required pages
    uint32_t pages = page_round(size);

    uint32_t page = 0;
    bool found = false;
    while (page<PAGES && !found) {
        if (page_is_free(page)) {
            // If the page is free, let's check if there is 
            uint32_t available = page_free_size(page);
            if (available >= pages) {
                // We found the right place
                found = true;
            } else {
                // Skipping the available pages
                page += available;
            }
        } else {
            // If the page is allocated, skipping objects in it
            page += page_obj_size(page);
        }
    }

    if (found) {
        current_page = page;
        current_page_position = page;
        current_position = 0;
        current_size = size;
        return 1;
    } else {
        return 0;
    }
}

void persist_current_data()
{
    flash_write((int)&rhock_progs[current_page_position*PAGE_SIZE], current_buffer, PAGE_SIZE);
    current_page_position++;
}

char rhock_store_append(char *data, uint32_t size)
{
    if (current_position+size > current_size) {
        dismiss();
        return 0;
    } else {
        for (uint32_t k=0; k<size; k++) {
            current_buffer[current_position%PAGE_SIZE] = data[k];
            if (((current_position+1)%PAGE_SIZE) == 0) {
                // We are going to overflow the buffer, so let's persist data to
                // the current page
                persist_current_data();
            }
            current_position++;
        }
        return 1;
    }
}

char rhock_store_load()
{
    // If there is still pending chunk, persisting them
    if ((current_position%PAGE_SIZE)!=0) {
        persist_current_data();
    }

    if (current_position != current_size) {
        dismiss();
        return 0;
    }

    char result = rhock_vm_obj_load((uint8_t*)&rhock_progs[current_page*PAGE_SIZE], current_size);
    if (result) {
        current_size = 0;
        return 1;
    } else {
        dismiss();
        return 0;
    }
}

void rhock_store_remove(struct rhock_obj *obj)
{
    int pages = page_round(obj->size);
    int start_page = ((int)obj->start-(int)rhock_progs)/PAGE_SIZE;
    if (start_page >= 0) {
        for (int k=0; k<pages; k++) {
            page_free(start_page+k);
        }
    }
}

TERMINAL_COMMAND(test, "Testing calculus")
{
    // Minimum void binary
    char dummy[38] = {0x00, 0x00, 0x00, 0x00, 0x31, 0xf1, 0x00, 0x26, 
        0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x00, 0x00, 0x00, 0x47, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00
    };
    dummy[3] = atoi(argv[0]);

    struct rhock_obj tmp;
    rhock_obj_load(&tmp, (uint8_t*)dummy, 38);
    uint16_t crc = rhock_obj_crc(&tmp);
    dummy[4] = (crc>>8)&0xff;
    dummy[5] = (crc>>0)&0xff;

    terminal_io()->print("id=");
    terminal_io()->println((int)dummy[3]);
    terminal_io()->print("crc16=");
    terminal_io()->println(rhock_obj_crc(&tmp));

    int r = rhock_store_alloc(sizeof(dummy));
    terminal_io()->print("Result: ");
    terminal_io()->println(r);
    terminal_io()->print("Page: ");
    terminal_io()->println(current_page);

    if (r) {
        r = rhock_store_append(dummy, 38);
        terminal_io()->print("Append result: ");
        terminal_io()->println(r);
        
        if (r) {
            r = rhock_store_load();
            terminal_io()->print("Load result: ");
            terminal_io()->println(r);
        }
    }
}

TERMINAL_COMMAND(store, "Test the store")
{
    uint32_t k = 0;
    terminal_io()->println("k=");
    terminal_io()->println((int)&k);
    terminal_io()->println("rhock_progs=");
    terminal_io()->println((int)&rhock_progs);
    while (k < PAGES) {
        terminal_io()->print(k);
        if (!page_is_free(k)) {
            terminal_io()->println(" is not free");
        } else {
            terminal_io()->println(" is free");
        }
        terminal_io()->print((int)rhock_progs[k*PAGE_SIZE+0]);
        terminal_io()->print(" ");
        terminal_io()->print((int)rhock_progs[k*PAGE_SIZE+1]);
        terminal_io()->print(" ");
        terminal_io()->print((int)rhock_progs[k*PAGE_SIZE+2]);
        terminal_io()->print(" ");
        terminal_io()->print((int)rhock_progs[k*PAGE_SIZE+3]);
        terminal_io()->println();
        k++;
    }
}

void rhock_store_init()
{
    uint32_t k = 0;
    while (k < PAGES) {
        if (!page_is_free(k)) {
            int size = rhock_obj_guess_size((uint8_t*)&rhock_progs[k*PAGE_SIZE]);
            if ((size+k*PAGE_SIZE) < FLASH_SIZE) {
                char result = rhock_vm_obj_load((uint8_t*)&rhock_progs[k*PAGE_SIZE], size);
                if (!result) {
                    page_free(k);
                }
                k += page_round(size);
            } else {
                page_free(k);
                k++;
            }
        } else {
            k++;
        }
    }
}

void rhock_store_reset()
{
    for (int k=0; k<PAGES; k++) {
        if (!page_is_free(k)) {
            page_free(k);
        }
    }
}
